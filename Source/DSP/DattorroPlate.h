#pragma once

#include <cmath>
#include <cstring>
#include <algorithm>
#include <array>

// ============================================================
// Dattorro Plate Reverb Engine
// Based on Jon Dattorro's "Effect Design Part 1" (1997)
// ============================================================

namespace DSP
{

// --- Fractional delay line with linear interpolation ---
class DelayLine
{
public:
    void setMaxDelay(int maxSamples)
    {
        buffer.resize(static_cast<size_t>(maxSamples + 1), 0.0f);
        writePos = 0;
    }

    void clear()
    {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        writePos = 0;
    }

    void push(float sample)
    {
        buffer[static_cast<size_t>(writePos)] = sample;
        writePos = (writePos + 1) % static_cast<int>(buffer.size());
    }

    float read(float delaySamples) const
    {
        float readPos = static_cast<float>(writePos) - delaySamples;
        if (readPos < 0.0f)
            readPos += static_cast<float>(buffer.size());

        const int idx0 = static_cast<int>(readPos);
        const int idx1 = (idx0 + 1) % static_cast<int>(buffer.size());
        const float frac = readPos - static_cast<float>(idx0);

        return (buffer[static_cast<size_t>(idx0)] * (1.0f - frac)) + (buffer[static_cast<size_t>(idx1)] * frac);
    }

    float readNearest(int delaySamples) const
    {
        int readPos = writePos - delaySamples;
        if (readPos < 0)
            readPos += static_cast<int>(buffer.size());
        return buffer[static_cast<size_t>(readPos)];
    }

private:
    std::vector<float> buffer;
    int writePos = 0;
};

// --- Simple allpass filter using a delay line ---
class AllpassFilter
{
public:
    void setMaxDelay(int maxSamples)
    {
        delay.setMaxDelay(maxSamples);
    }

    void clear()
    {
        delay.clear();
    }

    float process(float input, float delaySamples, float coefficient)
    {
        const float delayed = delay.read(delaySamples);
        const float output = (-input * coefficient) + delayed;
        delay.push(input + (delayed * coefficient));
        return output;
    }

private:
    DelayLine delay;
};

// --- One-pole lowpass for damping ---
class OnePole
{
public:
    void clear() { state = 0.0f; }

    void setCoefficient(float coeff) { g = coeff; }

    float process(float input)
    {
        state = (input * (1.0f - g)) + (state * g);
        return state;
    }

private:
    float state = 0.0f;
    float g = 0.0f;
};

// --- Sine LFO ---
class LFO
{
public:
    void setSampleRate(double sr) { sampleRate = sr; }

    void setFrequency(float freq)
    {
        phaseIncrement = freq / static_cast<float>(sampleRate);
    }

    void clear() { phase = 0.0f; }

    float process()
    {
        const float value = std::sin(2.0f * 3.14159265358979f * phase);
        phase += phaseIncrement;
        if (phase >= 1.0f) phase -= 1.0f;
        return value;
    }

private:
    double sampleRate = 44100.0;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
};

// --- Plate mode presets ---
enum class PlateMode : uint8_t
{
    Clean = 0,
    Bright,
    Dark,
    Lush,
    Dense,
    NumModes
};

inline const char* plateModeNames[] = { "Clean", "Bright", "Dark", "Lush", "Dense" };

struct PlatePreset
{
    float delayScale;
    float inputDiffusion1;
    float inputDiffusion2;
    float decayDiffusion1;
    float decayDiffusion2;
    float dampingCoeff;      // higher = more HF damping
    float bassMult;          // bass decay multiplier
    float modDepthScale;
    float modRateScale;
};

inline PlatePreset getPreset(PlateMode mode)
{
    switch (mode)
    {
        case PlateMode::Clean:
            return { 1.0f, 0.750f, 0.625f, 0.700f, 0.500f, 0.0005f, 1.0f, 1.0f, 1.0f };
        case PlateMode::Bright:
            return { 0.85f, 0.780f, 0.650f, 0.720f, 0.520f, 0.0002f, 0.95f, 0.8f, 1.2f };
        case PlateMode::Dark:
            return { 1.15f, 0.720f, 0.600f, 0.680f, 0.480f, 0.003f, 1.1f, 1.2f, 0.7f };
        case PlateMode::Lush:
            return { 1.05f, 0.760f, 0.640f, 0.710f, 0.510f, 0.001f, 1.0f, 2.5f, 0.9f };
        case PlateMode::Dense:
            return { 0.70f, 0.800f, 0.700f, 0.750f, 0.550f, 0.001f, 1.0f, 0.6f, 1.5f };
        case PlateMode::NumModes:
        default:
            return { 1.0f, 0.750f, 0.625f, 0.700f, 0.500f, 0.0005f, 1.0f, 1.0f, 1.0f };
    }
}

// ============================================================
// Main Dattorro Plate Reverb
// ============================================================
class DattorroPlate
{
public:
    void prepare(double sampleRate, int /*maxBlockSize*/)
    {
        sr = sampleRate;
        sampleRateScale = sr / 29761.0; // Dattorro reference rate

        // Input diffusion allpasses
        for (auto& ap : inputAP)
            ap.setMaxDelay(8192);

        // Tank allpasses
        for (auto& ap : tankAP)
            ap.setMaxDelay(8192);

        // Tank delays
        for (auto& dl : tankDelay)
            dl.setMaxDelay(32768);

        // Predelay
        predelayLine.setMaxDelay(static_cast<int>(sr * 0.5) + 1); // max 500ms

        // LFOs
        for (auto& lfo : lfo_)
        {
            lfo.setSampleRate(sr);
            lfo.clear();
        }

        clear();
    }

    void clear()
    {
        for (auto& ap : inputAP) ap.clear();
        for (auto& ap : tankAP) ap.clear();
        for (auto& dl : tankDelay) dl.clear();
        for (auto& f : dampingFilter) f.clear();
        predelayLine.clear();
        for (auto& lfo : lfo_) lfo.clear();
    }

    void setParameters(float decayParam, float sizeParam, float dampingParam,
                       float predelayMs, float widthParam, PlateMode mode)
    {
        decay = std::clamp(decayParam, 0.0f, 0.999f);
        size = std::clamp(sizeParam, 0.1f, 2.0f);
        width = std::clamp(widthParam, 0.0f, 1.0f);
        predelaySamples = static_cast<float>(predelayMs * 0.001 * sr);
        currentPreset = getPreset(mode);

        float dampCoeff = (dampingParam * 0.7f) + currentPreset.dampingCoeff;
        dampCoeff = std::clamp(dampCoeff, 0.0f, 0.999f);
        for (auto& f : dampingFilter)
            f.setCoefficient(dampCoeff);

        // LFO rates
        lfo_[0].setFrequency(0.50f * currentPreset.modRateScale);
        lfo_[1].setFrequency(0.31f * currentPreset.modRateScale);
        lfo_[2].setFrequency(0.37f * currentPreset.modRateScale);
        lfo_[3].setFrequency(0.23f * currentPreset.modRateScale);
    }

    void process(const float* inputL, const float* inputR,
                 float* outputL, float* outputR, int numSamples)
    {
        const float sc = static_cast<float>(sampleRateScale) * size;
        const PlatePreset& p = currentPreset;

        // Dattorro delay lengths (from paper, reference rate 29761 Hz)
        const float inAP1 = 142.0f * sc;
        const float inAP2 = 107.0f * sc;
        const float inAP3 = 379.0f * sc;
        const float inAP4 = 277.0f * sc;

        const float tankAP1delay = 672.0f * sc * p.delayScale;
        const float tankDL1delay = 4453.0f * sc * p.delayScale;
        const float tankAP2delay = 1800.0f * sc * p.delayScale;
        const float tankDL2delay = 3720.0f * sc * p.delayScale;

        const float tankAP3delay = 908.0f * sc * p.delayScale;
        const float tankDL3delay = 4217.0f * sc * p.delayScale;
        const float tankAP4delay = 2656.0f * sc * p.delayScale;
        const float tankDL4delay = 3163.0f * sc * p.delayScale;

        const float modDepth = 12.0f * p.modDepthScale * static_cast<float>(sampleRateScale);

        for (int i = 0; i < numSamples; ++i)
        {
            const float mono = (inputL[i] + inputR[i]) * 0.5f;

            // Predelay
            predelayLine.push(mono);
            const float predelayed = predelayLine.read(predelaySamples);

            // Input diffusion
            float diffused = inputAP[0].process(predelayed, inAP1, p.inputDiffusion1);
            diffused = inputAP[1].process(diffused, inAP2, p.inputDiffusion1);
            diffused = inputAP[2].process(diffused, inAP3, p.inputDiffusion2);
            diffused = inputAP[3].process(diffused, inAP4, p.inputDiffusion2);

            // Read tank feedback from previous iteration
            const float tankFeedL = tankDelay[1].read(tankDL2delay) * decay;
            const float tankFeedR = tankDelay[3].read(tankDL4delay) * decay;

            // --- Left tank path ---
            const float lMod = lfo_[0].process() * modDepth;
            const float rMod = lfo_[1].process() * modDepth;

            const float leftIn = diffused + tankFeedR;
            const float leftAP1 = tankAP[0].process(leftIn, tankAP1delay + lMod, -p.decayDiffusion1);
            tankDelay[0].push(leftAP1);
            const float leftDL1 = tankDelay[0].read(tankDL1delay);
            const float leftDamped = dampingFilter[0].process(leftDL1);
            const float leftAP2 = tankAP[1].process(leftDamped * decay, tankAP2delay, p.decayDiffusion2);
            tankDelay[1].push(leftAP2);

            // --- Right tank path ---
            const float lMod2 = lfo_[2].process() * modDepth;
            const float rMod2 = lfo_[3].process() * modDepth;

            const float rightIn = diffused + tankFeedL;
            const float rightAP1 = tankAP[2].process(rightIn, tankAP3delay + rMod, -p.decayDiffusion1);
            tankDelay[2].push(rightAP1);
            const float rightDL1 = tankDelay[2].read(tankDL3delay);
            const float rightDamped = dampingFilter[1].process(rightDL1);
            const float rightAP2 = tankAP[3].process(rightDamped * decay, tankAP4delay + lMod2 + rMod2, p.decayDiffusion2);
            tankDelay[3].push(rightAP2);

            // --- Output taps (Dattorro multi-tap) ---
            const float sc2 = sc * p.delayScale;
            float outL = tankDelay[2].read(266.0f * sc2)
                       + tankDelay[2].read(2974.0f * sc2)
                       - tankAP[3].process(tankDelay[3].read(1913.0f * sc2), 1.0f, 0.0f)
                       + tankDelay[3].read(1996.0f * sc2)
                       - tankDelay[0].read(1990.0f * sc2)
                       - tankAP[1].process(tankDelay[1].read(187.0f * sc2), 1.0f, 0.0f)
                       - tankDelay[1].read(1066.0f * sc2);

            float outR = tankDelay[0].read(353.0f * sc2)
                       + tankDelay[0].read(3627.0f * sc2)
                       - tankAP[1].process(tankDelay[1].read(1228.0f * sc2), 1.0f, 0.0f)
                       + tankDelay[1].read(2673.0f * sc2)
                       - tankDelay[2].read(2111.0f * sc2)
                       - tankAP[3].process(tankDelay[3].read(335.0f * sc2), 1.0f, 0.0f)
                       - tankDelay[3].read(121.0f * sc2);

            outL *= 0.2f;
            outR *= 0.2f;

            // Stereo width
            const float mid = (outL + outR) * 0.5f;
            float side = (outL - outR) * 0.5f;
            side *= width;
            outputL[i] = mid + side;
            outputR[i] = mid - side;
        }
    }

private:
    double sr = 44100.0;
    double sampleRateScale = 1.0;

    float decay = 0.5f;
    float size = 1.0f;
    float width = 1.0f;
    float predelaySamples = 0.0f;

    PlatePreset currentPreset{};

    // Input diffusion: 4 allpass filters
    std::array<AllpassFilter, 4> inputAP;

    // Tank: 4 allpass filters (2 per path)
    std::array<AllpassFilter, 4> tankAP;

    // Tank: 4 delay lines (2 per path)
    std::array<DelayLine, 4> tankDelay;

    // Damping filters (one per path)
    std::array<OnePole, 2> dampingFilter;

    // Predelay
    DelayLine predelayLine;

    // LFOs for modulation
    std::array<LFO, 4> lfo_;
};

} // namespace DSP
