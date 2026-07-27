# EpochGui

EpochGui is a portable C++23 GUI layout and geometry library used by EpochEngine and standalone applications.

It owns reusable GUI state, layout calculations, hit testing, text-control behavior, docking metadata, and optional renderer-neutral geometry. It does not own editor/runtime code, platform windows, OpenGL, or another rendering backend.

## Modules

### `epoch.gui`

The core module provides:

- `Vec2` and `Rect`
- Floating-window state and layout
- Splitters and progress bars
- Loading-screen layout
- Selectable rows and segmented controls
- Popup placement and state
- Docking and dockable-window state
- Panel-host state
- Text editing, selection, navigation, and scrolling

```cpp
import epoch.gui;
```

### `epoch.gui.rounded_rect` — optional

The optional rounded-rectangle module generates renderer-neutral triangle meshes for:

- Filled rounded rectangles
- Independent radius per corner
- Optional borders with configurable width
- Pill-shaped controls
- Proportional radius normalization when corners overlap
- Configurable tessellation from 1 to 64 segments per corner

It contains no OpenGL, DirectX, Vulkan, platform-window, timing, or animation code.

Enable it with CMake:

```text
-DEPOCHGUI_ENABLE_ROUNDED_RECT=ON
```

Then import it:

```cpp
import epoch.gui.rounded_rect;

namespace rounded = epochengine::gui_lib::rounded_rect;

const rounded::RoundedRectMesh mesh = rounded::make_rounded_rect_mesh({
    .bounds = { { 40.0f, 40.0f }, { 240.0f, 96.0f } },
    .radii = { 18.0f, 18.0f, 18.0f, 18.0f },
    .border_width = 3.0f,
    .segments_per_corner = 12
});
```

`RoundedRectMesh` exposes vertex positions, fill indices, optional border indices, normalized radii and bounds, and outer/inner contour ranges. A renderer can upload those arrays through any backend.

## CMake

Build the core library:

```powershell
cmake -S . -B build
cmake --build build --target EpochGui --config Release
```

Build with rounded rectangles and tests:

```powershell
cmake -S . -B build -DEPOCHGUI_ENABLE_ROUNDED_RECT=ON -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The main target is `EpochGui`. Compatibility aliases are available as:

- `epoch_gui`
- `Autodidac::EpochGui`

## Visual Studio project

The checked-in `EpochGui.vcxproj` builds the core library by default.

Enable the optional rounded-rectangle files with:

```powershell
msbuild EpochGui.vcxproj /p:Configuration=Release /p:Platform=x64 /p:EpochGuiEnableRoundedRect=true
```

## Repository layout

```text
modules/epoch.gui.ixx                  Core public C++23 module
modules/epoch.gui.rounded_rect.ixx     Optional rounded-geometry module
include/gui/                           Compatibility headers
src/epochgui/                          Backend-neutral implementations
tests/text_control_tests.cpp           Core text-control tests
tests/rounded_rect_tests.cpp           Optional rounded-geometry tests
```

## Boundaries

EpochGui remains backend-neutral. Rendering adapters belong in the application or engine renderer layer. Demo-specific shaders, native window hosts, startup effects, asset handling, and renderer state must not be added to this library.
