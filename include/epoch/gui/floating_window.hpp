#pragma once

#include <cstdint>

namespace epochnamespace::gui_lib
{
    struct Vec2
    {
        float x{};
        float y{};
    };

    struct Rect
    {
        Vec2 position{};
        Vec2 size{};
    };

    struct FloatingWindowState
    {
        Vec2 position{};
        Vec2 size{};
        Vec2 drag_offset{};
        Vec2 resize_origin_mouse{};
        Vec2 resize_origin_size{};
        bool open{ true };
        bool initialized{};
        bool dragging{};
        bool resizing{};
        bool close_pressed{};
        std::uint32_t focus_order{};
    };

    struct FloatingWindowOptions
    {
        Vec2 default_position{};
        Vec2 default_size{};
        Vec2 min_size{ 160.0f, 96.0f };
        Vec2 viewport_size{};
        float title_bar_height{ 32.0f };
        float content_padding{ 6.0f };
        bool movable{ true };
        bool resizable{ true };
        bool closable{ true };
    };

    struct FloatingWindowInput
    {
        Vec2 mouse_position{};
        bool mouse_down{};
        bool mouse_pressed{};
        bool mouse_released{};
    };

    struct FloatingWindowLayout
    {
        Rect window{};
        Rect title_bar{};
        Rect content{};
        Rect close_button{};
        Rect resize_handle{};
        bool visible{};
        bool hovered{};
        bool title_hovered{};
        bool close_hovered{};
        bool resize_hovered{};
        bool focused{};
        bool moved{};
        bool resized{};
        bool close_requested{};
    };

    bool contains(Rect rect, Vec2 point) noexcept;
    void normalize_floating_window(FloatingWindowState& state, const FloatingWindowOptions& options) noexcept;
    FloatingWindowLayout update_floating_window(
        FloatingWindowState& state,
        const FloatingWindowOptions& options,
        const FloatingWindowInput& input) noexcept;
}
