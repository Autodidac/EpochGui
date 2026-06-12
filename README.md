# EpochGui

EpochGui is the source-only standalone mirror surface for the reusable Epoch GUI
layout library. The current mirror metadata tracks the EpochEngine `v0.87.15`
source line. The canonical EpochEngine source still lives in the engine tree:

- `Engine/include/epoch/gui/*.hpp`
- `Engine/src/gui/*.cpp`

This directory owns the mirror build and documentation files for
`Autodidac/EpochGui`; it must not fork, duplicate, or wrap editor/runtime code.

## Current Payload

The first bounded payload is a versioned static library with backend-neutral
geometry and layout helpers:

- `epoch/gui/floating_window.hpp` plus `src/gui/floating_window.cpp`
- `epoch/gui/layout_primitives.hpp` backed by `src/gui/floating_window.cpp`
- `epoch/gui/popup_layout.hpp` plus `src/gui/popup_layout.cpp`
- `epoch/gui/dock_layout.hpp` plus `src/gui/dock_layout.cpp`
- `epoch/gui/version.hpp` for the mirror name and `0.87.15` version constants

The public namespace is `epochnamespace::gui_lib`. The code depends only on the
C++ standard library and the public `epoch/gui` headers. It does not include
`editor.cpp`, `engine.cpp`, context sources, modules, renderer backends, runtime
assets, generated output, or `Engine/src/engine.gui.cpp`.

## Supported Layouts

The CMake and MSVC files support two source layouts:

- In-tree EpochEngine checkout: this directory is `Engine/lib/EpochGui`, and the
  source root is detected two directories above it.
- Standalone mirror checkout: place `include/epoch/gui` and `src/gui` beside
  this README, and the source root is the repository root.

If neither layout applies, pass an explicit source root to CMake:

```powershell
cmake -S Engine/lib/EpochGui -B build/EpochGui -DEPOCHGUI_SOURCE_ROOT=Engine
```

## CMake

From an EpochEngine checkout:

```powershell
cmake -S Engine/lib/EpochGui -B build/EpochGui
cmake --build build/EpochGui --target EpochGui --config Debug
```

From a standalone `Autodidac/EpochGui` checkout:

```powershell
cmake -S . -B build
cmake --build build --target EpochGui --config Debug
```

The CMake target is `EpochGui`. Compatibility aliases are also provided as
`epoch_gui` and `Autodidac::EpochGui`.

The CMake project version is `0.87.15`, matching the engine source line that
separated toolbar context selection from the reusable floating GUI layout
payload.

## Visual Studio

From an EpochEngine checkout:

```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" Engine/lib/EpochGui/EpochGui.vcxproj /p:Configuration=Debug /p:Platform=x64 /m:1
```

From a standalone mirror checkout, open or build `EpochGui.vcxproj` from the
repository root after placing `include/epoch/gui` and `src/gui` beside it.

## Mirror Rules

- Keep the mirror source-only. Do not check in build output, packages, caches,
  captures, generated projects, or editor/runtime assets.
- Keep `.gitattributes`, `.gitignore`, `LICENSE`, CMake, MSVC, and this README
  with the standalone `Autodidac/EpochGui` repository.
- Keep reusable GUI implementation in `include/epoch/gui` and `src/gui`.
- Keep this directory focused on standalone build metadata and mirror docs.
- Promote new reusable controls through EpochEngine first, then update the
  mirror payload once the source and build evidence are real.
- Carry the EpochEngine license terms into the standalone mirror root.
