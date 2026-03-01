# IvanSound Plate Reverb — User Manual

## Table of Contents

1. [Installation](#installation)
2. [macOS Security (Unsigned Plugin)](#macos-security-unsigned-plugin)
3. [Loading in a DAW](#loading-in-a-daw)
4. [Standalone App](#standalone-app)
5. [Controls Reference](#controls-reference)
6. [Plate Modes](#plate-modes)
7. [Usage Tips](#usage-tips)
8. [Troubleshooting](#troubleshooting)
9. [Building from Source](#building-from-source)
10. [Uninstallation](#uninstallation)

---

## Installation

### From Release (Pre-built)

1. Download `IvanSound-v0.0.1-macOS.zip` from the [Releases page](https://github.com/Salobaka/ivan-reverb/releases).
2. Unzip the archive. You will find:
   - `IvanSound.vst3` — the VST3 plugin
   - `IvanSound.app` — the standalone application
3. Copy `IvanSound.vst3` to your VST3 plugin folder:
   ```
   ~/Library/Audio/Plug-Ins/VST3/
   ```
   To open this folder in Finder, press `Cmd + Shift + G` and paste the path above.
4. Optionally, move `IvanSound.app` to your `/Applications` folder.

### From Source (Build It Yourself)

See [Building from Source](#building-from-source) at the end of this manual.

---

## macOS Security (Unsigned Plugin)

IvanSound is not code-signed or notarized with Apple. macOS Gatekeeper will block the plugin by default. You must manually approve it before your DAW or the standalone app can load it.

### Step 1 — Remove the quarantine flag

Open Terminal and run:

```bash
xattr -rd com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/IvanSound.vst3
```

If you also downloaded the standalone app:

```bash
xattr -rd com.apple.quarantine /Applications/IvanSound.app
```

This removes the quarantine attribute that macOS adds to downloaded files.

### Step 2 — Allow in System Settings (if still blocked)

If your DAW still fails to load the plugin or you see a dialog saying the file "cannot be opened because the developer cannot be verified":

1. Open **System Settings** (or System Preferences on older macOS).
2. Go to **Privacy & Security**.
3. Scroll down. You should see a message:
   > "IvanSound" was blocked from use because it is not from an identified developer.
4. Click **Allow Anyway**.
5. Restart your DAW.

### Step 3 — Approve on first launch

The first time you open the plugin or standalone app after allowing it, macOS may show one final dialog. Click **Open** to confirm.

### If you built from source

Plugins built locally on your own machine are automatically trusted by macOS. You do not need to perform any of the steps above. The build system signs the plugin with an ad-hoc signature, which macOS accepts for locally-built binaries.

---

## Loading in a DAW

### Ableton Live

1. Open **Preferences** → **Plug-ins**.
2. Make sure **Use VST3 plug-in system folder** is enabled.
3. Click **Rescan** to refresh the plugin list.
4. In the browser sidebar, navigate to **Plug-ins** → **VST3** → **IvanSound**.
5. Drag **IvanSound** onto any **audio track** or **return track**.

### Logic Pro

1. On an audio track, click an empty **Audio FX** slot.
2. Navigate to **Audio Units** → **IvanSound** → **IvanSound**.
3. If Logic shows a validation warning, click **OK** and then re-scan (Logic validates AU plugins on first use).

### REAPER

1. Open **Preferences** → **Plug-ins** → **VST**.
2. Ensure the VST3 path includes: `~/Library/Audio/Plug-Ins/VST3`
3. Click **Re-scan**.
4. On any track, click **FX** → search for **IvanSound**.

### FL Studio (macOS)

1. Open **Options** → **Manage plugins**.
2. Click **Rescan** (with "Verify plugins" enabled).
3. Find **IvanSound** in the plugin list and enable the star to add it to favorites.
4. Insert on a mixer track via **Mixer** → select slot → **IvanSound**.

### Bitwig Studio

1. Bitwig automatically scans the system VST3 folder.
2. Open the **device browser** and search for **IvanSound**.
3. Drag it onto any track.

### Studio One

1. Open **Studio One** → **Options** → **Locations** → **VST Plug-ins**.
2. Ensure the system VST3 folder is listed.
3. Click **Rescan**.
4. In the browser, find **IvanSound** under **Effects** → **VST3**.

---

## Standalone App

The standalone application lets you test IvanSound without a DAW.

1. Open `IvanSound.app`.
2. Go to the menu bar: **Options** → **Audio/MIDI Settings**.
3. Select your **audio input** device (e.g., built-in microphone, audio interface).
4. Select your **audio output** device.
5. Set the **sample rate** (44100 or 48000 recommended).
6. Set the **buffer size** (256 or 512 for low latency).
7. Close the settings window. Audio from your input device will now pass through the reverb and play through your output device.

---

## Controls Reference

### Mix (0% – 100%)

Blends between the dry (original) signal and the wet (reverb) signal.

- **0%** — Fully dry, no reverb audible.
- **50%** — Equal blend of dry and wet. Good starting point for insert use.
- **100%** — Fully wet, only reverb output. Use this when the plugin is on a send/return track.

### Decay (0.1 – 0.999)

Controls how long the reverb tail rings out.

- **0.1 – 0.3** — Short, tight ambience. Good for adding space without washing out the source.
- **0.4 – 0.6** — Medium decay. Versatile for vocals, guitars, and drums.
- **0.7 – 0.9** — Long, lush tails. Great for pads, ambient textures, and cinematic effects.
- **0.95+** — Very long, near-infinite sustain. The reverb will sustain almost indefinitely.

### Size (0.1 – 2.0)

Scales the virtual plate dimensions by adjusting all internal delay line lengths.

- **0.1 – 0.5** — Small plate. Tighter, more metallic character.
- **0.8 – 1.2** — Standard plate size. Natural and balanced.
- **1.5 – 2.0** — Large plate. Spacious, hall-like quality.

### Damping (0.0 – 1.0)

Controls how quickly high frequencies are absorbed in the reverb tail.

- **0.0** — No damping. Bright, shimmery tail with full high-frequency content.
- **0.3 – 0.5** — Moderate damping. Natural-sounding decay where highs fade faster than lows.
- **0.7 – 1.0** — Heavy damping. Dark, warm tail. High frequencies die out quickly.

### Pre-Delay (0 – 500 ms)

Adds a gap between the dry signal and the onset of the reverb. This separates the source from the reverb in time, which helps maintain clarity.

- **0 ms** — No gap. Reverb starts immediately.
- **10 – 30 ms** — Subtle separation. Keeps vocals and instruments clear in a mix.
- **50 – 100 ms** — Noticeable gap. Creates a sense of a large room where reflections arrive later.
- **100 – 500 ms** — Dramatic effect. The reverb is clearly separated from the source.

### Width (0.0 – 1.0)

Controls the stereo spread of the reverb output.

- **0.0** — Mono. Both channels output the same signal. Useful for mono-compatible mixes.
- **0.5** — Moderate stereo width.
- **1.0** — Full stereo. Maximum left/right decorrelation for a wide, immersive sound.

### Mode (Dropdown)

Selects the plate model. Each mode changes the internal structure of the reverb to produce a different tonal character. See [Plate Modes](#plate-modes) for details.

---

## Plate Modes

### Clean

The default, neutral plate reverb. Balanced frequency response with moderate density and natural-sounding decay. Works well on any source material.

**Best for:** General-purpose reverb, vocals, acoustic instruments.

### Bright

Reduced high-frequency damping and slightly shorter internal delays. The tail retains more sparkle and air. Slightly tighter feel than Clean.

**Best for:** Vocals that need presence, acoustic guitars, cymbals, adding shimmer.

### Dark

Increased high-frequency damping with longer internal delays. Produces a warm, smooth tail where the highs roll off quickly, leaving a rich low-mid body.

**Best for:** Pads, synths, cinematic atmospheres, warming up harsh sources.

### Lush

Enhanced internal modulation for a chorus-like movement in the tail. Wider stereo image with a dreamy, evolving character. The tail shimmers and breathes.

**Best for:** Ambient music, shoegaze, dream pop, synth pads, soundscapes.

### Dense

Tighter delay spacing with increased diffusion. Produces a thicker, more packed reverb with rapid early reflections. The tail feels heavier and more solid.

**Best for:** Drums, percussion, snare plates, adding body to thin sources.

---

## Usage Tips

### On a Send/Return Track

For most mixing scenarios, place IvanSound on a **send/return track** (also called a bus or auxiliary track) and set **Mix to 100%**. Send varying amounts of signal from each track to the reverb bus. This lets multiple instruments share the same reverb, creating a cohesive space while saving CPU.

### As an Insert Effect

When used directly on a track as an insert, start with **Mix at 20–40%** and adjust to taste. This works well for vocals, lead instruments, or any source that benefits from built-in space.

### Vocals

Start with: **Mode: Clean**, **Decay: 0.4–0.6**, **Pre-Delay: 20–40ms**, **Damping: 0.3**, **Width: 0.8**. The pre-delay keeps the vocal upfront while the reverb fills in behind it.

### Drums

For a classic plate snare sound: **Mode: Dense**, **Decay: 0.3–0.5**, **Size: 0.8**, **Pre-Delay: 0ms**, **Damping: 0.4**. Short decay and dense mode give a tight, punchy plate character.

### Ambient / Atmospheric

For lush, evolving textures: **Mode: Lush**, **Decay: 0.9+**, **Size: 1.5**, **Pre-Delay: 50–100ms**, **Damping: 0.1–0.2**, **Width: 1.0**. Long decay with minimal damping and high modulation creates an expansive, dreamy wash.

### Automating Parameters

All parameters are exposed to your DAW for automation. Try automating **Decay** to swell the reverb tail at the end of a phrase, or automate **Mix** to bring the reverb in and out during different sections of a song.

---

## Troubleshooting

### Plugin does not appear in my DAW

1. Verify the file is in the correct location:
   ```
   ~/Library/Audio/Plug-Ins/VST3/IvanSound.vst3
   ```
2. Remove the quarantine flag (see [macOS Security](#macos-security-unsigned-plugin)).
3. Rescan plugins in your DAW's preferences.
4. Restart the DAW.

### "Cannot be opened because the developer cannot be verified"

This is macOS Gatekeeper blocking the unsigned plugin. Follow all steps in the [macOS Security](#macos-security-unsigned-plugin) section.

### Plugin loads but produces no sound

1. Check that audio is actually reaching the plugin. The input meters in your DAW should show signal.
2. Make sure **Mix** is above 0%.
3. Verify the track routing — the plugin should be on a track that receives audio.

### Audio glitches or crackling

1. Increase your audio buffer size in your DAW (try 512 or 1024 samples).
2. Lower the **Size** parameter — larger sizes require more memory and processing.
3. Close other CPU-intensive applications.

### Plugin crashes the DAW

1. Remove the quarantine flag if you haven't already.
2. Delete the plugin and re-download or rebuild from source.
3. Verify your DAW supports VST3 plugins.
4. Check that you are running macOS 12 or later.

### AU version does not validate in Logic Pro

1. Open Terminal and run:
   ```bash
   auval -a | grep IvanSound
   ```
2. If validation fails, remove quarantine:
   ```bash
   xattr -rd com.apple.quarantine ~/Library/Audio/Plug-Ins/Components/IvanSound.component
   ```
3. Run validation again:
   ```bash
   auval -v aufx Ivs1 Ivan
   ```

---

## Building from Source

### Requirements

- macOS 12 or later
- CMake 3.22 or later
- Xcode Command Line Tools (or full Xcode)
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
2. Compile the VST3, AU, and Standalone targets.
3. Copy the VST3 to `~/Library/Audio/Plug-Ins/VST3/`.
4. Copy the AU to `~/Library/Audio/Plug-Ins/Components/`.

The standalone app will be at:
```
build/IvanSound_artefacts/Release/Standalone/IvanSound.app
```

Plugins built locally are automatically trusted by macOS — no quarantine removal needed.

### Running the Linter

```bash
/opt/homebrew/opt/llvm/bin/clang-tidy Source/**/*.cpp -p build --header-filter='Source/.*\.h$'
```

Or enable it during the build:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_CLANG_TIDY=ON
cmake --build build --config Release
```

---

## Uninstallation

Remove these files to fully uninstall:

```bash
rm -rf ~/Library/Audio/Plug-Ins/VST3/IvanSound.vst3
rm -rf ~/Library/Audio/Plug-Ins/Components/IvanSound.component
rm -rf /Applications/IvanSound.app
```

Rescan plugins in your DAW after removing the files.
