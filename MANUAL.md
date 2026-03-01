# IvanSound Plate Reverb — User Manual

## Table of Contents

1. [Installation](#installation)
2. [macOS Security (Unsigned Plugin)](#macos-security-unsigned-plugin)
3. [Loading in Ableton Live](#loading-in-ableton-live)
4. [Controls Reference](#controls-reference)
5. [Plate Modes](#plate-modes)
6. [Usage Tips](#usage-tips)
7. [Troubleshooting](#troubleshooting)
8. [Building from Source](#building-from-source)
9. [Uninstallation](#uninstallation)

---

## Installation

### From Release (Pre-built)

1. Download `IvanSound-macOS.zip` from the [Releases page](https://github.com/Salobaka/ivan-reverb/releases).
2. Unzip the archive. You will find `IvanSound.vst3`.
3. Copy it to your VST3 plugin folder:
   ```
   ~/Library/Audio/Plug-Ins/VST3/
   ```
   To open this folder in Finder, press `Cmd + Shift + G` and paste the path above.
4. Follow the [macOS Security](#macos-security-unsigned-plugin) steps below before opening Ableton.

### From Source (Build It Yourself)

See [Building from Source](#building-from-source). Locally built plugins skip all Gatekeeper steps.

---

## macOS Security (Unsigned Plugin)

IvanSound is not code-signed or notarized with Apple. macOS Gatekeeper will block the plugin by default. You must manually approve it before Ableton can load it.

### Step 1 — Remove the quarantine flag

Open Terminal (`Applications → Utilities → Terminal`) and run:

```bash
xattr -rd com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/IvanSound.vst3
```

This removes the quarantine attribute that macOS adds to all downloaded files.

### Step 2 — Allow in System Settings (if Ableton still won't load it)

If Ableton shows an error or the plugin doesn't appear after rescanning:

1. Open **System Settings**.
2. Go to **Privacy & Security**.
3. Scroll down. You will see:
   > "IvanSound" was blocked from use because it is not from an identified developer.
4. Click **Allow Anyway**.
5. Restart Ableton.

### Step 3 — Approve on first load

The first time Ableton loads the plugin after allowing it, macOS may show one final confirmation dialog. Click **Open**.

### If you built from source

Plugins built locally on your own machine are automatically trusted by macOS. The build system signs them with an ad-hoc signature. No quarantine removal needed.

---

## Loading in Ableton Live

1. Open Ableton Live.
2. Go to **Settings** → **Plug-ins** (or **Preferences** → **Plug-ins** on older versions).
3. Make sure **Use VST3 plug-in system folder** is **ON**.
4. Click **Rescan** to refresh the plugin list.
5. In the browser sidebar on the left, navigate to:
   **Plug-ins → VST3 → IvanSound**
6. Drag **IvanSound** onto any **audio track** or **return track**.

### Recommended: Use on a Return Track

1. In Ableton's mixer, click on a **Return Track** (A, B, C, etc.).
2. Drop IvanSound onto the return track.
3. Set the plugin's **Mix to 100%** (fully wet).
4. On any audio/instrument track, turn up the **Send** knob for that return to blend in reverb.

This lets multiple tracks share one reverb instance, saving CPU and creating a cohesive space.

---

## Controls Reference

### Mix (0% – 100%)

Blends between the dry (original) signal and the wet (reverb) signal.

- **0%** — Fully dry, no reverb.
- **50%** — Equal blend. Good starting point for insert use.
- **100%** — Fully wet. Use this on a return track.

### Decay (0.1 – 0.999)

Controls how long the reverb tail rings out.

- **0.1 – 0.3** — Short, tight ambience.
- **0.4 – 0.6** — Medium decay. Versatile for most sources.
- **0.7 – 0.9** — Long, lush tails for pads and ambient textures.
- **0.95+** — Near-infinite sustain.

### Size (0.1 – 2.0)

Scales the virtual plate dimensions by adjusting all internal delay line lengths.

- **0.1 – 0.5** — Small plate. Tighter, more metallic.
- **0.8 – 1.2** — Standard plate. Natural and balanced.
- **1.5 – 2.0** — Large plate. Spacious, hall-like quality.

### Damping (0.0 – 1.0)

Controls how quickly high frequencies are absorbed in the reverb tail.

- **0.0** — No damping. Bright, shimmery tail.
- **0.3 – 0.5** — Moderate. Highs fade faster than lows, natural sound.
- **0.7 – 1.0** — Heavy damping. Dark, warm tail.

### Pre-Delay (0 – 500 ms)

Adds a gap between the dry signal and the onset of reverb. Helps maintain clarity.

- **0 ms** — Reverb starts immediately.
- **10 – 30 ms** — Subtle separation. Keeps vocals and instruments upfront.
- **50 – 100 ms** — Noticeable gap. Sense of a larger space.
- **100 – 500 ms** — Dramatic, clearly separated reverb.

### Width (0.0 – 1.0)

Controls stereo spread of the reverb output.

- **0.0** — Mono reverb output.
- **0.5** — Moderate width.
- **1.0** — Full stereo. Maximum left/right decorrelation.

### Mode (Dropdown)

Selects the plate model. See [Plate Modes](#plate-modes).

---

## Plate Modes

### Clean

Neutral, balanced plate reverb. Works on any source material.

**Best for:** General-purpose reverb, vocals, acoustic instruments.

### Bright

Less high-frequency damping, slightly shorter delays. Retains sparkle and air.

**Best for:** Vocals needing presence, acoustic guitars, cymbals.

### Dark

More high-frequency damping, longer delays. Warm, smooth tail where highs roll off quickly.

**Best for:** Pads, synths, cinematic atmospheres, warming harsh sources.

### Lush

Enhanced internal modulation for chorus-like movement. Wide stereo, dreamy and evolving.

**Best for:** Ambient music, shoegaze, dream pop, synth pads, soundscapes.

### Dense

Tighter delay spacing, increased diffusion. Thicker reverb with rapid early reflections.

**Best for:** Drums, percussion, snare plates, adding body to thin sources.

---

## Usage Tips

### Vocals

**Mode: Clean** / **Decay: 0.4–0.6** / **Pre-Delay: 20–40ms** / **Damping: 0.3** / **Width: 0.8**

The pre-delay keeps the vocal upfront while the reverb fills in behind it.

### Drums

**Mode: Dense** / **Decay: 0.3–0.5** / **Size: 0.8** / **Pre-Delay: 0ms** / **Damping: 0.4**

Short decay and dense mode give a tight, punchy plate snare sound.

### Ambient / Atmospheric

**Mode: Lush** / **Decay: 0.9+** / **Size: 1.5** / **Pre-Delay: 50–100ms** / **Damping: 0.1–0.2** / **Width: 1.0**

Long decay with minimal damping and high modulation for an expansive, dreamy wash.

### Automating Parameters in Ableton

All parameters are exposed to Ableton for automation:

1. Click the plugin's **Configure** button in Ableton.
2. Click a knob in the plugin UI — it appears in Ableton's parameter list.
3. In Arrangement View, expand the track's automation lanes and select the parameter.
4. Draw automation curves.

Try automating **Decay** to swell the tail at the end of a phrase, or **Mix** to bring reverb in/out across sections.

---

## Troubleshooting

### Plugin does not appear in Ableton

1. Verify the file is at:
   ```
   ~/Library/Audio/Plug-Ins/VST3/IvanSound.vst3
   ```
2. Remove the quarantine flag (see [macOS Security](#macos-security-unsigned-plugin)).
3. In Ableton: **Settings → Plug-ins → Rescan**.
4. Restart Ableton.

### "Cannot be opened because the developer cannot be verified"

Follow all steps in [macOS Security](#macos-security-unsigned-plugin).

### Plugin loads but produces no sound

1. Check that audio reaches the plugin — Ableton's track meters should show signal.
2. Make sure **Mix** is above 0%.
3. Verify track routing — the plugin should be on a track that receives audio input.

### Audio glitches or crackling

1. In Ableton: **Settings → Audio → Buffer Size** — try 512 or 1024.
2. Lower the **Size** parameter.
3. Close other CPU-intensive applications.

---

## Building from Source

### Requirements

- macOS 12 or later
- CMake 3.22 or later
- Xcode Command Line Tools (`xcode-select --install`)
- Internet connection (to download JUCE on first build)

### Steps

```bash
git clone https://github.com/Salobaka/ivan-reverb.git
cd ivan-reverb
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The build will:
1. Automatically download JUCE 8.0.12 via CMake FetchContent.
2. Compile the VST3 plugin.
3. Copy it to `~/Library/Audio/Plug-Ins/VST3/`.

Locally built plugins are automatically trusted by macOS.

### Running the Linter

```bash
/opt/homebrew/opt/llvm/bin/clang-tidy Source/**/*.cpp -p build --header-filter='Source/.*\.h$'
```

---

## Uninstallation

```bash
rm -rf ~/Library/Audio/Plug-Ins/VST3/IvanSound.vst3
```

Rescan plugins in Ableton after removing.
