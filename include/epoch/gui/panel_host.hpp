#pragma once

#include "epoch/gui/dockable_window.hpp"
#include "epoch/gui/popup_layout.hpp"

#include <cstdint>
#include <string_view>

namespace epochnamespace::gui_lib
{
    enum class PanelHostIntent : std::uint8_t
    {
        docked,
        floating,
        popup,
        external
    };

    enum class PanelHostAction : std::uint8_t
    {
        none,
        focus,
        dock,
        float_panel,
        open_popup,
        request_external,
        close
    };

    struct PanelHostState
    {
        std::uint32_t id{};
        PanelHostIntent intent{ PanelHostIntent::docked };
        DockSlot dock_slot{ DockSlot::right };
        DockableWindowState dockable{};
        PopupState popup{};
        bool visible{ true };
        bool initialized{};
        bool active{};
        bool external_requested{};
        bool close_requested{};
        std::uint32_t focus_order{};
    };

    struct PanelHostRootState
    {
        std::uint32_t active_panel_id{};
        std::uint32_t next_focus_order{ 1 };
        bool changed_this_frame{};
    };

    struct PanelHostOptions
    {
        std::string_view title{};
        PanelHostIntent default_intent{ PanelHostIntent::docked };
        DockSlot fallback_dock_slot{ DockSlot::right };
        DockableWindowOptions dockable{};
        PopupOptions popup{};
        bool allow_dock{ true };
        bool allow_float{ true };
        bool allow_popup{ true };
        bool allow_external{ true };
        bool allow_close{ true };
    };

    struct PanelHostInput
    {
        Vec2 mouse_position{};
        bool mouse_down{};
        bool mouse_pressed{};
        bool mouse_released{};
        bool owner_pressed{};
        bool escape_pressed{};
        PanelHostAction requested_action{ PanelHostAction::none };
        DockSlot requested_dock_slot{ DockSlot::none };
    };

    struct PanelHostLayout
    {
        PanelHostIntent intent{ PanelHostIntent::docked };
        Rect frame{};
        Rect content{};
        DockableWindowChrome dockable_chrome{};
        PopupLayout popup{};
        bool visible{};
        bool hovered{};
        bool active{};
        bool docked{};
        bool floating{};
        bool popup_open{};
        bool external_requested{};
    };

    struct PanelHostResult
    {
        PanelHostLayout layout{};
        PanelHostIntent intent{ PanelHostIntent::docked };
        PanelHostAction action{ PanelHostAction::none };
        DockSlot dock_slot{ DockSlot::none };
        bool changed{};
        bool focused{};
        bool dock_requested{};
        bool float_requested{};
        bool popup_requested{};
        bool external_requested{};
        bool close_requested{};
    };

    class PanelHostController final : public LayoutController
    {
    public:
        [[nodiscard]] std::string_view name() const noexcept override;
        [[nodiscard]] bool is_allowed_intent(
            PanelHostIntent intent,
            const PanelHostOptions& options) const noexcept;
        void focus(
            PanelHostRootState& root,
            PanelHostState& state) const noexcept;
        void normalize(
            PanelHostRootState& root,
            PanelHostState& state,
            const PanelHostOptions& options) const noexcept;
        [[nodiscard]] PanelHostLayout make_layout(
            const PanelHostState& state,
            const PanelHostOptions& options,
            const PanelHostInput& input) const noexcept;
        [[nodiscard]] PanelHostResult update(
            PanelHostRootState& root,
            PanelHostState& state,
            const PanelHostOptions& options,
            const PanelHostInput& input) const noexcept;
    };

    [[nodiscard]] const PanelHostController& panel_host_controller() noexcept;
    [[nodiscard]] bool is_allowed_panel_host_intent(
        PanelHostIntent intent,
        const PanelHostOptions& options) noexcept;
    void focus_panel_host(
        PanelHostRootState& root,
        PanelHostState& state) noexcept;
    void normalize_panel_host(
        PanelHostRootState& root,
        PanelHostState& state,
        const PanelHostOptions& options) noexcept;
    [[nodiscard]] PanelHostLayout make_panel_host_layout(
        const PanelHostState& state,
        const PanelHostOptions& options,
        const PanelHostInput& input) noexcept;
    [[nodiscard]] PanelHostResult update_panel_host(
        PanelHostRootState& root,
        PanelHostState& state,
        const PanelHostOptions& options,
        const PanelHostInput& input) noexcept;
}
