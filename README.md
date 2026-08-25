# XB Converter (X360 & OG Xbox Disc Utility)

<p align="center">
  <img src="resources/app_icon.png" alt="XB Converter Logo" width="180"/>
</p>

<p align="center">
  <b>The ultimate disc conversion and image utility for Original Xbox, Xbox 360, Emulators (Xemu, Xenia), and Xbox DevMode (UWP).</b>
</p>

<p align="center">
  <a href="https://github.com/ibmk15uchiha-dev/XGDTool"><img src="https://img.shields.io/badge/GitHub-ibmk15uchiha--dev%2FXGDTool-brightgreen" alt="GitHub Repository"/></a>
  <a href="https://github.com/ibmk15uchiha-dev/XGDTool/releases"><img src="https://img.shields.io/badge/Version-v1.0.0-blue" alt="Version"/></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-GPL--3.0-green" alt="License"/></a>
  <a href="#"><img src="https://img.shields.io/badge/Author-Hbolt-orange" alt="Author"/></a>
</p>

---

## 🌟 Overview

**XB Converter** (formerly XGDTool) is a high-performance Xbox disc utility designed to seamlessly convert, scrub, compress, extract, and reauthor game images between all major formats without requiring intermediate temporary files.

It is available as:
1. **Desktop GUI Application** (`XB converter.exe`) – Sleek dark theme with real-time conversion logging and multi-format support.
2. **Xbox DevMode UWP Application** – Runs natively on **Xbox Series X|S** and **Xbox One** DevMode consoles with full Xbox Gamepad support.
3. **Command Line Interface (CLI)** – High-speed batch processing and automated pipeline integration.

---

## 🚀 Supported Formats

| Format | Extension | Description | Supported Platforms |
|---|---|---|---|
| **ISO / XISO** | `.iso` | Standard Xbox disc image | OG Xbox, X360, Xemu, Xenia |
| **Games on Demand (GoD)** | Directory / Live Header | Container for Xbox 360 & OG Xbox digital games | Xbox 360, OG Xbox |
| **CCI** | `.cci` | Compressed image with LZ4 / LZ4HC compression | OG Xbox, Emulators |
| **CSO** | `.cso` | Compressed ISO image with Deflate/Zlib compression | OG Xbox, Emulators |
| **ZAR** | `.zar` | High-efficiency Zstandard compressed archive | PC, Emulators |
| **Extracted Folder** | Direct Files | Extracted XEX / XBE filesystem (`default.xex`, `default.xbe`) | All |

---

## ✨ Features

- **Direct In-Memory Streaming**: Direct conversion between formats (e.g. GoD to ZAR, ISO to CCI, or Extracted folder to GoD) without unnecessary temporary disk writes.
- **Partial Scrubbing**: Removes blank sectors and unused padding, trimming the file to its minimum physical size.
- **Full Reauthoring (Full Scrub)**: Reconstructs the internal AVL directory tree from scratch for maximum space savings.
- **Multithreaded Compression**: High-speed parallel compression for CCI and CSO formats.
- **Batch Processing**: Convert entire directories containing mixed game formats into a targeted format in a single run.
- **Auto-Split Support**: Automatically splits large images into multi-part files for FATX filesystem compatibility (`.1.iso`, `.2.iso`).
- **Target Profiles**: 1-click presets for **OG Xbox**, **Xbox 360**, **Xemu**, and **Xenia**.
- **Offline & Online Title Lookup**: Built-in offline database with optional online lookup for automatic title renaming.
- **Attach XBE Generator**: Generates companion attach XBE files for loading images on Original Xbox.

---

## 🎮 Xbox DevMode (UWP) Controller Bindings

When running the UWP version on your Xbox console in DevMode:

| Controller Button | Action |
|---|---|
| **`(Y)` Button** | **Start Conversion** |
| **`(X)` Button** | **Refresh / Reset Form** |
| **Right Thumbstick `(RS)`** | **Smooth Scroll Interface** |

---

## 💻 CLI Usage

```powershell
"XB converter.exe" <output_format> [settings_flags] <input_path> [output_directory]
```

### Output Format Arguments (Mutually Exclusive)
- `--extract` : Extracts all files to a directory
- `--xiso` : Creates an XISO image
- `--god` : Creates a Games on Demand container
- `--cci` : Creates a compressed CCI archive
- `--cso` : Creates a compressed CSO archive
- `--zar` : Creates a compressed ZAR archive
- `--ogxbox` : Automatically configures format and settings for OG Xbox
- `--xbox360` : Automatically configures format and settings for Xbox 360
- `--xemu` : Automatically configures format and settings for Xemu
- `--xenia` : Automatically configures format and settings for Xenia

### Settings & Flags
- `--partial-scrub` : Removes padding sectors and trims image
- `--full-scrub` : Completely reauthors the filesystem tree for smallest size
- `--split` : Splits resulting image into parts for FATX
- `--rename` : Renames the output to the matched database title
- `--attach-xbe` : Generates an attach XBE launcher alongside the output
- `--offline` : Disables online network lookup
- `--debug` : Enables verbose debug output
- `--quiet` : Suppresses output except for errors

---

## 🛠️ Building from Source

### Prerequisites
- **Visual Studio 2022 or 2026** (with *Desktop development with C++* and *Universal Windows Platform development* workloads)
- **CMake 3.15+**
- **Git**

### 1. Build Desktop GUI (`XB converter.exe`)
```powershell
git clone --recursive https://github.com/ibmk15uchiha-dev/XGDTool.git
cd XGDTool
cmake -S . -B build -G "Visual Studio 18 2026" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
The output executable will be generated at `build/Release/XB converter.exe`.

### 2. Build Xbox DevMode UWP Package (`.msix` / `.appx`)
```powershell
cmake -S . -B build-uwp -G "Visual Studio 18 2026" -A x64 -T host=x64 "-DCMAKE_SYSTEM_NAME=WindowsStore" "-DCMAKE_SYSTEM_VERSION=10.0"
MSBuild.exe build-uwp/XGDTool.vcxproj /p:Configuration=Release /p:Platform=x64
```
The deployable `.msix` package will be generated in `build-uwp/AppPackages/`.

---

## 📦 Xbox DevMode Deployment

1. Open your Xbox DevMode console and open the **Xbox Device Portal** in your PC browser (`https://<console-ip>:11443`).
2. Click **Add** under the **My games & apps** section.
3. Upload `XB_converter_UWP.msix` along with `Microsoft.VCLibs.x64.14.00.appx` when prompted for dependencies.
4. Launch **XB converter UWP** directly from your Xbox dashboard.

---

## 📄 License & Credits

- **Author**: [Hbolt](https://github.com/ibmk15uchiha-dev/XGDTool)
- **Repository**: [https://github.com/ibmk15uchiha-dev/XGDTool](https://github.com/ibmk15uchiha-dev/XGDTool)
- **Original Base**: XGDTool by WiredOpposite
- **License**: GNU General Public License v3.0 (GPL-3.0) — see [LICENSE](LICENSE) for details.
