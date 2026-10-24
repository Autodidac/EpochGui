# EpochGui

MPL-2.0 permits commercial use, including proprietary larger works subject to its covered-file obligations. See [Commercial use and giving back](#commercial-use-and-giving-back) for licensing, origin links and voluntary credit requests.

EpochGui is a portable C++23 GUI layout, input-adapter, raster-data, and geometry library used by EpochEngine and standalone applications.

Current source version is `0.90.0`. Historical release `v0.89.32` remains
unchanged. This source update incorporates the reviewed portable GUI extensions
and MPL-2.0 licensing; release artifacts and interactive acceptance are separate.

It owns reusable GUI state, layout calculations, hit testing, text-control behavior, docking metadata, an embedded fallback bitmap font, bounded PPM decoding, and optional renderer-neutral helpers. It does not own editor/runtime code, platform windows, OpenGL, or another rendering backend.

## Modules

### `epoch.gui`

The core module provides:

- `Vec2` and `Rect`
- Floating-window state and layout
- Splitters and progress bars
- Loading-screen layout
- Selectable rows, segmented controls, and responsive tab-strip planning that
  preserves the active route behind a bounded overflow selector, plus
  deterministic wrapped keyboard navigation that skips disabled routes
- Popup placement and state
- Docking, dock guides, context insertion grids, and dockable-window state
- Panel-host state
- Reusable node-graph, system, tile, and virtualized asset-grid workspaces
- A reusable high-density hierarchy-tree controller with stable IDs,
  ancestor-retaining filtering, range/toggle selection, keyboard traversal,
  virtualized row planning, scroll-to-visible, locked/disabled rows, and
  deterministic context-action routing
- Text editing, selection, navigation, and scrolling

```cpp
import epoch.gui;
```

### `gui/hierarchy_tree.hpp`

The renderer-neutral hierarchy controller is a public header/source API for
dense trees such as outliners, GUI/component hierarchies, script browsers, and
evidence explorers. It provides stable numeric identities, transactional tree
replacement, deterministic ordering, expand/collapse state, ancestor-retaining
filtering, single/toggle/range selection, keyboard traversal, virtualized row
planning, scroll-to-visible calculations, locked and disabled row policy, and
deterministic context-action routing.

```cpp
#include <gui/hierarchy_tree.hpp>

namespace tree = epochengine::gui_lib::hierarchy_tree;

tree::Controller hierarchy{};
const tree::ReplaceResult admitted = hierarchy.replace_nodes(nodes);
const tree::LayoutPlan visible = hierarchy.plan_rows(viewport);
```

EpochGui owns the portable state and calculations. A consuming application
still supplies its domain nodes, draws the planned rows, feeds input commands,
and executes accepted context routes. This standalone release does not claim
that a particular EpochEngine editor surface has adopted the control yet.

### `epoch.gui.tile_workspace`

The tile-workspace module provides reusable authoring control state without
owning project documents, render backends, native windows, or files:

- tool and palette selection
- virtualized palette rows
- ordered layer selection and visibility state
- bounded grid layout and cell hit testing
- cursor-anchored zoom and clamped pan
- renderer-neutral layout snapshots and metrics

```cpp
import epoch.gui.tile_workspace;
```

Applications bind this portable controller to their own temporal map document,
persistence, rendering, and input adapters.

### `epoch.gui.font`

The font module provides the built-in renderer-neutral fallback font:

- Hard-coded 5x7 glyph rows stored as compact bit fields
- Uppercase letters, digits, common punctuation, and lowercase folding
- Constant glyph, character-advance, and line-advance metrics
- Pixel testing without allocation or file I/O
- Text measurement using the same metrics

```cpp
import epoch.gui.font;

namespace font = epochengine::gui_lib::font;

constexpr font::BitmapGlyph glyph = font::default_glyph('A');
if (font::pixel_on(glyph, 2, 0))
{
    // Emit one pixel quad through the application's renderer.
}
```

The embedded font is the always-available default. Applications can still layer a richer font system over EpochGui.

### `epoch.gui.image`

The image module provides bounded, renderer-neutral raster loading and layout:

- ASCII P3 and binary P6 PPM decoding
- 8-bit and 16-bit PPM sample support
- Explicit width, height, pixel-count, and file-size limits
- RGBA8 output with no OpenGL, Vulkan, DirectX, or platform dependency
- Contain and stretch layout inside an EpochGui `Rect`
- Per-pixel rectangle calculation for software or simple batched renderers

```cpp
import epoch.gui.image;

namespace image = epochengine::gui_lib::image;

const image::ImageResult loaded = image::load_ppm_file("assets/example.ppm");
if (loaded)
{
    const image::RasterImageLayout layout = image::make_raster_image_layout(
        loaded.image,
        { { 20.0f, 20.0f }, { 640.0f, 360.0f } },
        image::ImageFit::contain);
}
```

EpochGui decodes and lays out raster data. The consuming renderer remains responsible for uploading a texture or emitting geometry.

### `epoch.gui.rounded_rect` — optional, disabled by default

The optional rounded-rectangle module generates renderer-neutral triangle meshes for:

- Filled rounded rectangles
- Independent radius per corner
- Optional borders with configurable width
- Pill-shaped controls
- Proportional radius normalization when corners overlap
- Configurable tessellation from 1 to 64 segments per corner

It contains no OpenGL, DirectX, Vulkan, platform-window, timing, or animation code.

Enable it with:

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

### `epoch.gui.input`

Most applications should continue using their existing engine, platform, or window-system input layer. `epoch.gui.input` provides backend-neutral per-frame normalization and modal arbitration for small tools, standalone demos, tests, and integrations. It is part of the core library so every host can enforce the same modal-input boundary.

Import it directly:

```cpp
import epoch.gui.input;

namespace input = epochengine::gui_lib::input;

input::InputTracker tracker;
tracker.set_pointer_position({ mouse_x, mouse_y });
tracker.set_pointer_button(input::PointerButton::left, button_down);

const auto floating_input = input::floating_window_input(tracker.frame());
```

The fallback module provides:

- Per-frame pointer, wheel, key, and modifier transitions
- Conversion into existing EpochGui `FloatingWindowInput` and `PopupInput`
- Right-click context-menu requests
- Borderless replacement-title-bar layout and hit testing
- Resize-edge/corner, caption, minimize, maximize, and close regions
- Window-command detection for custom chrome
- Modal keyboard capture plus bounded or fully blocked pointer arbitration

It contains no Win32, X11, Cocoa, SDL, GLFW, rendering, or operating-system calls. Native hosts remain responsible for feeding events and performing requested native window actions.

## CMake

Build the core library:

```powershell
cmake -S . -B build
cmake --build build --target EpochGui --config Release
```

Build the optional rounded-geometry feature and every test:

```powershell
cmake -S . -B build \
  -DEPOCHGUI_ENABLE_ROUNDED_RECT=ON \
  -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The main target is `EpochGui`. Compatibility aliases are available as:

- `epoch_gui`
- `Autodidac::EpochGui`

## Visual Studio project

The checked-in `EpochGui.vcxproj` builds the core library, embedded font, and PPM image support by default.

Enable optional features explicitly:

```powershell
msbuild EpochGui.vcxproj \
  /p:Configuration=Release \
  /p:Platform=x64 \
  /p:EpochGuiEnableRoundedRect=true
```

## Repository layout

```text
modules/epoch.gui.ixx                  Core public C++23 module
modules/epoch.gui.font.ixx             Embedded fallback bitmap font
modules/epoch.gui.image.ixx            Raster image API and layout
modules/epoch.gui.rounded_rect.ixx     Optional rounded-geometry module
modules/epoch.gui.input.ixx            Core input and modal-arbitration module
include/gui/                           Compatibility headers
src/epochgui/                          Backend-neutral implementations
tests/font_tests.cpp                   Embedded-font tests
tests/image_tests.cpp                  PPM decoder and image-layout tests
tests/dock_layout_tests.cpp            Dock-context grid and insertion tests
tests/node_graph_workspace_tests.cpp   Node-graph workspace tests
tests/system_workspace_tests.cpp       Systems workspace tests
tests/asset_grid_tests.cpp             Virtualized asset-grid tests
tests/text_control_tests.cpp           Core text-control tests
tests/rounded_rect_tests.cpp           Optional rounded-geometry tests
tests/input_tests.cpp                  Input and modal-arbitration tests
```

## Boundaries

EpochGui remains backend-neutral. It owns reusable font bits, decoded raster data, and raster layout, but not font shaping, GPU texture creation, shaders, draw submission, native event translation, operating-system windows, native frame presentation, or host commands. Existing engine or platform input should be preferred when available.

## Portable text and scrollbar extension

The existing text controller now retains bounded undo/redo history. Visual-line
wrapping and caret hit mapping accept the consumer's synchronous glyph metrics,
preserving its font/UTF-8 policy. External line ranges are clamped; nonfinite
pointer coordinates cannot become invalid indices. Scrollbar geometry and drag
mapping share one compiled owner for drawing and input.

`EpochGui.TextLayout`, `EpochGui.ScrollbarLayout` and `EpochGui.TextControl`
exercise these additions against the real module library. No engine Context,
renderer queue, editor composition or AI tools are required.

The same integration includes responsive side-dock/chrome geometry, modal owner
input scopes, chat-composer/status-strip geometry, UTF-8 prefix clipping, circle
and rounded-corner raster helpers. They depend only on standard C++/EpochGui;
chat geometry contains no model transport, source access or agent policy.

EpochGui remains GUI-only: it has no Platform, Media, Audio, Engine or AI target
dependency. Demo applications compose those libraries. Source admission uses
this repository or an explicit source-root argument; it never searches an
Engine parent checkout as a fallback.

## Commercial use and giving back

Quick guide to [MPL-2.0](LICENSE); the full license governs. "Covered files"
means this library's MPL source, your changes to it, and new files containing
its code. Separate files in a larger application can use another license.

### Allowed

- [x] Use the library commercially, including in proprietary applications.
- [x] Sell applications or redistribute the library itself, changed or unchanged.
- [x] Make private/internal changes without publishing them or sending a PR.
- [x] License your separate application files under your own terms while meeting
  MPL obligations for the covered files.

### Required when distributing outside your organization

- [ ] Keep covered source, including your modifications, available under MPL-2.0.
- [ ] When shipping binaries, tell recipients how to obtain that covered source
  reasonably and promptly, charging no more than its distribution cost.
- [ ] Tell recipients the source is MPL-2.0 and include the license or explain
  how to obtain it.
- [ ] Preserve required copyright, patent, license and warranty/liability notices.
- [ ] Keep recipients' MPL source rights intact under any additional terms.

### Not allowed

- [ ] Do not make distributed covered source proprietary-only or withhold the
  covered source required for your binaries.
- [ ] Do not remove substantive required notices or present someone else's work
  as your own.
- [ ] Do not assume MPL grants rights to project names/logos or imply official
  endorsement. Use distinct product branding; see [TRADEMARKS.md](TRADEMARKS.md).

### Recommended, entirely optional

- [ ] Make the library's origin and [source repository](https://github.com/Autodidac/EpochGui) easy for
  other developers to find in dependency metadata, credits, notices or technical
  documentation. No particular README or About-screen placement is requested.
- [ ] If you credit comparable libraries or tools, include this library alongside
  them instead of obscuring its role. A factual link to
  [EpochEngine](https://github.com/Autodidac/EpochEngine) also helps support the work.
- [ ] Explain your own additions accurately. A wrapper, new name or small feature
  does not make the underlying library your original invention.
- [ ] Share useful fixes, features, tests and documentation through a
  [pull request](https://github.com/Autodidac/EpochGui/pulls); accepted contributions receive author credit.
- [ ] Follow [CONTRIBUTING.md](CONTRIBUTING.md) when choosing to contribute.

**MPL does not require an upstream pull request. But it would be a lot cooler if
you shared!** Contributions help these libraries support themselves and continued
work on EpochEngine. These recommendations add no copyright-license conditions.

See [Mozilla's MPL FAQ](https://www.mozilla.org/en-US/MPL/2.0/FAQ/) for examples.

## License

First-party source uses **MPL-2.0**; LICENSE contains the unmodified full text.
Private modification is allowed; distribution obligations apply to covered
files under that license. Commercial use is allowed. Required notices and
third-party licenses remain in force. Historical grants are not revoked.
See NOTICE for origin, TRADEMARKS.md for separate branding rules, and
CONTRIBUTING.md / CONTRIBUTOR_LICENSE.md for explicit contribution acceptance.
