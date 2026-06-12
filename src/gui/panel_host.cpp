module;

#include <string_view>

module epoch.gui;

namespace epochnamespace::gui_lib
{
    namespace
    {
        [[nodiscard]] PanelHostIntent fallback_intent(const PanelHostOptions& options) noexcept
        {
            if (is_allowed_panel_host_intent(options.default_intent, options))
                return options.default_intent;
            if (options.allow_dock)
                return PanelHostIntent::docked;
            if (options.allow_float)
                return PanelHostIntent::floating;
            if (options.allow_popup)
                return PanelHostIntent::popup;
            return PanelHostIntent::external;
        }

        [[nodiscard]] DockSlot resolved_dock_slot(DockSlot requested, DockSlot fallback) noexcept
        {
            if (is_valid_dock_slot(requested))
                return requested;
            if (is_valid_dock_slot(fallback))
                return fallback;
            return DockSlot::right;
        }

        [[nodiscard]] DockableWindowOptions dockable_options(
            const PanelHostState& state,
            const PanelHostOptions& options) noexcept
        {
            DockableWindowOptions dockable = options.dockable;
            if (dockable.title.empty())
                dockable.title = options.title;
            dockable.allow_dock = options.allow_dock;
            dockable.allow_float = options.allow_float;
            dockable.allow_detach = options.allow_external;
            dockable.allow_close = options.allow_close;
            dockable.fallback_dock_slot = resolved_dock_slot(state.dock_slot, options.fallback_dock_slot);
            return dockable;
        }

        [[nodiscard]] DockableWindowMode dockable_mode_for(PanelHostIntent intent) noexcept
        {
            switch (intent)
            {
            case PanelHostIntent::floating:
                return DockableWindowMode::floating;
            case PanelHostIntent::external:
                return DockableWindowMode::detached;
            case PanelHostIntent::docked:
            case PanelHostIntent::popup:
            default:
                return DockableWindowMode::docked;
            }
        }

        [[nodiscard]] PanelHostIntent intent_from_action(
            PanelHostAction action,
            PanelHostIntent fallback) noexcept
        {
            switch (action)
            {
            case PanelHostAction::dock:
                return PanelHostIntent::docked;
            case PanelHostAction::float_panel:
                return PanelHostIntent::floating;
            case PanelHostAction::open_popup:
                return PanelHostIntent::popup;
            case PanelHostAction::request_external:
                return PanelHostIntent::external;
            case PanelHostAction::none:
            case PanelHostAction::focus:
            case PanelHostAction::close:
            default:
                return fallback;
            }
        }

        [[nodiscard]] PanelHostAction action_from_dockable(DockableWindowAction action) noexcept
        {
            switch (action)
            {
            case DockableWindowAction::focus:
                return PanelHostAction::focus;
            case DockableWindowAction::dock:
                return PanelHostAction::dock;
            case DockableWindowAction::float_window:
                return PanelHostAction::float_panel;
            case DockableWindowAction::detach:
                return PanelHostAction::request_external;
            case DockableWindowAction::close:
                return PanelHostAction::close;
            case DockableWindowAction::none:
            default:
                return PanelHostAction::none;
            }
        }

        [[nodiscard]] DockableWindowAction dockable_action_from(PanelHostAction action) noexcept
        {
            switch (action)
            {
            case PanelHostAction::focus:
                return DockableWindowAction::focus;
            case PanelHostAction::dock:
                return DockableWindowAction::dock;
            case PanelHostAction::float_panel:
                return DockableWindowAction::float_window;
            case PanelHostAction::request_external:
                return DockableWindowAction::detach;
            case PanelHostAction::close:
                return DockableWindowAction::close;
            case PanelHostAction::none:
            case PanelHostAction::open_popup:
            default:
                return DockableWindowAction::none;
            }
        }

        [[nodiscard]] DockableWindowHostState dockable_root_for(
            const PanelHostRootState& root,
            const PanelHostState& state) noexcept
        {
            DockableWindowHostState dock_root{};
            dock_root.active_window_id = root.active_panel_id == state.id ? state.dockable.id : 0U;
            dock_root.next_focus_order = root.next_focus_order;
            return dock_root;
        }

        void sync_from_dockable_root(
            PanelHostRootState& root,
            PanelHostState& state,
            const DockableWindowHostState& dock_root) noexcept
        {
            root.next_focus_order = dock_root.next_focus_order == 0U ? 1U : dock_root.next_focus_order;
            if (dock_root.changed_this_frame)
                root.changed_this_frame = true;
            if (dock_root.active_window_id == state.dockable.id)
                focus_panel_host(root, state);
        }

        void apply_intent(
            PanelHostRootState& root,
            PanelHostState& state,
            const PanelHostOptions& options,
            PanelHostIntent intent,
            DockSlot requested_slot,
            PanelHostResult& result) noexcept
        {
            if (!is_allowed_panel_host_intent(intent, options))
                intent = fallback_intent(options);

            const PanelHostIntent previous = state.intent;
            state.intent = intent;
            state.dock_slot = resolved_dock_slot(requested_slot, state.dock_slot);
            state.dockable.mode = dockable_mode_for(intent);
            state.dockable.dock_slot = state.dock_slot;
            state.visible = true;
            state.close_requested = false;
            state.external_requested = intent == PanelHostIntent::external;
            state.popup.open = intent == PanelHostIntent::popup;

            result.intent = intent;
            result.dock_slot = state.dock_slot;
            result.changed = result.changed || previous != intent;
            root.changed_this_frame = root.changed_this_frame || result.changed;

            switch (intent)
            {
            case PanelHostIntent::docked:
                result.dock_requested = true;
                break;
            case PanelHostIntent::floating:
                result.float_requested = true;
                break;
            case PanelHostIntent::popup:
                result.popup_requested = true;
                break;
            case PanelHostIntent::external:
                result.external_requested = true;
                break;
            default:
                break;
            }

            focus_panel_host(root, state);
        }
    }

    std::string_view PanelHostController::name() const noexcept
    {
        return "panel_host";
    }

    bool PanelHostController::is_allowed_intent(
        PanelHostIntent intent,
        const PanelHostOptions& options) const noexcept
    {
        switch (intent)
        {
        case PanelHostIntent::docked:
            return options.allow_dock;
        case PanelHostIntent::floating:
            return options.allow_float;
        case PanelHostIntent::popup:
            return options.allow_popup;
        case PanelHostIntent::external:
            return options.allow_external;
        default:
            return false;
        }
    }

    void PanelHostController::focus(
        PanelHostRootState& root,
        PanelHostState& state) const noexcept
    {
        if (!state.visible)
            return;

        if (state.id == 0U)
            state.id = root.active_panel_id != 0U ? root.active_panel_id : 1U;
        if (root.next_focus_order == 0U)
            root.next_focus_order = 1U;

        state.active = true;
        root.active_panel_id = state.id;
        state.focus_order = root.next_focus_order++;
        state.dockable.active = true;
        state.dockable.focus_order = state.focus_order;
        state.popup.focus_order = state.focus_order;
    }

    void PanelHostController::normalize(
        PanelHostRootState& root,
        PanelHostState& state,
        const PanelHostOptions& options) const noexcept
    {
        root.changed_this_frame = false;
        if (root.next_focus_order == 0U)
            root.next_focus_order = 1U;
        if (state.id == 0U)
            state.id = 1U;

        if (!state.initialized)
        {
            state.initialized = true;
            state.intent = fallback_intent(options);
            state.visible = true;
            state.dock_slot = resolved_dock_slot(state.dock_slot, options.fallback_dock_slot);
        }

        if (!is_allowed_intent(state.intent, options))
            state.intent = fallback_intent(options);

        state.dock_slot = resolved_dock_slot(state.dock_slot, options.fallback_dock_slot);
        state.external_requested = state.visible && state.intent == PanelHostIntent::external;
        state.close_requested = !state.visible && state.close_requested;

        state.dockable.id = state.id;
        state.dockable.visible = state.visible && state.intent != PanelHostIntent::popup;
        state.dockable.mode = dockable_mode_for(state.intent);
        state.dockable.dock_slot = state.dock_slot;
        state.dockable.detach_requested = state.external_requested;
        state.dockable.close_requested = state.close_requested;

        DockableWindowHostState dock_root = dockable_root_for(root, state);
        normalize_dockable_window(dock_root, state.dockable, dockable_options(state, options));
        sync_from_dockable_root(root, state, dock_root);

        const bool was_popup_open = state.popup.open;
        PopupInput popup_input{};
        popup_input.open_requested = state.visible && state.intent == PanelHostIntent::popup;
        popup_input.close_requested = !popup_input.open_requested;
        normalize_popup(state.popup, options.popup, popup_input);
        state.popup.open = state.visible && state.intent == PanelHostIntent::popup && (state.popup.open || was_popup_open);

        state.active = state.visible && root.active_panel_id == state.id;
        state.dockable.active = state.active;
    }

    PanelHostLayout PanelHostController::make_layout(
        const PanelHostState& state,
        const PanelHostOptions& options,
        const PanelHostInput& input) const noexcept
    {
        PanelHostLayout layout{};
        layout.intent = state.intent;
        layout.visible = state.visible;
        layout.active = state.active;
        layout.docked = state.visible && state.intent == PanelHostIntent::docked;
        layout.floating = state.visible && state.intent == PanelHostIntent::floating;
        layout.external_requested = state.visible && state.intent == PanelHostIntent::external;

        if (!state.visible)
            return layout;

        if (state.intent == PanelHostIntent::popup)
        {
            PopupState popup = state.popup;
            PopupInput popup_input{};
            popup_input.mouse_position = input.mouse_position;
            popup_input.mouse_pressed = input.mouse_pressed;
            popup_input.mouse_released = input.mouse_released;
            popup_input.owner_pressed = input.owner_pressed;
            popup_input.escape_pressed = input.escape_pressed;
            layout.popup = update_popup(popup, options.popup, popup_input);
            layout.frame = layout.popup.popup;
            layout.content = layout.popup.popup;
            layout.hovered = layout.popup.hovered;
            layout.popup_open = layout.popup.visible;
            return layout;
        }

        layout.dockable_chrome = make_dockable_window_chrome(
            state.dockable,
            dockable_options(state, options),
            DockableWindowInput{
                .mouse_position = input.mouse_position,
                .mouse_down = input.mouse_down,
                .mouse_pressed = input.mouse_pressed,
                .mouse_released = input.mouse_released
            });
        layout.frame = layout.dockable_chrome.frame;
        layout.content = layout.dockable_chrome.content;
        layout.hovered = layout.dockable_chrome.hovered;
        return layout;
    }

    PanelHostResult PanelHostController::update(
        PanelHostRootState& root,
        PanelHostState& state,
        const PanelHostOptions& options,
        const PanelHostInput& input) const noexcept
    {
        normalize(root, state, options);

        PanelHostResult result{};
        result.intent = state.intent;
        result.dock_slot = state.dock_slot;

        PanelHostAction action = input.requested_action;
        if (action == PanelHostAction::focus && state.visible)
        {
            focus(root, state);
            result.focused = true;
        }
        else if (action == PanelHostAction::close && options.allow_close)
        {
            state.visible = false;
            state.popup.open = false;
            state.dockable.visible = false;
            state.dockable.floating.open = false;
            state.close_requested = true;
            state.external_requested = false;
            result.action = action;
            result.close_requested = true;
            result.changed = true;
            root.changed_this_frame = true;
            if (root.active_panel_id == state.id)
                root.active_panel_id = 0U;
        }
        else if (action == PanelHostAction::dock
            || action == PanelHostAction::float_panel
            || action == PanelHostAction::open_popup
            || action == PanelHostAction::request_external)
        {
            result.action = action;
            apply_intent(root, state, options, intent_from_action(action, state.intent), input.requested_dock_slot, result);
        }

        if (state.visible && state.intent == PanelHostIntent::popup)
        {
            PopupInput popup_input{};
            popup_input.mouse_position = input.mouse_position;
            popup_input.mouse_pressed = input.mouse_pressed;
            popup_input.mouse_released = input.mouse_released;
            popup_input.owner_pressed = input.owner_pressed;
            popup_input.escape_pressed = input.escape_pressed;
            popup_input.open_requested = action == PanelHostAction::open_popup;
            popup_input.close_requested = action == PanelHostAction::close;

            const bool was_open = state.popup.open;
            const PopupLayout popup = update_popup(state.popup, options.popup, popup_input);
            if (popup.opened || popup.closed || was_open != state.popup.open)
            {
                result.changed = true;
                root.changed_this_frame = true;
            }
            if (popup.closed)
            {
                state.visible = false;
                state.close_requested = true;
                result.close_requested = true;
            }
            if ((popup.hovered || popup.owner_hovered) && input.mouse_pressed)
            {
                focus(root, state);
                result.focused = true;
            }
        }
        else if (state.visible)
        {
            DockableWindowHostState dock_root = dockable_root_for(root, state);
            DockableWindowInput dock_input{};
            dock_input.mouse_position = input.mouse_position;
            dock_input.mouse_down = input.mouse_down;
            dock_input.mouse_pressed = input.mouse_pressed;
            dock_input.mouse_released = input.mouse_released;
            dock_input.requested_action = dockable_action_from(action);
            dock_input.requested_dock_slot = input.requested_dock_slot;

            DockableWindowResult dock_result = update_dockable_window(
                dock_root,
                state.dockable,
                dockable_options(state, options),
                dock_input);
            sync_from_dockable_root(root, state, dock_root);

            if (dock_result.action != DockableWindowAction::none)
                result.action = action_from_dockable(dock_result.action);
            if (dock_result.changed)
            {
                result.changed = true;
                root.changed_this_frame = true;
            }
            result.focused = result.focused || dock_result.focused;
            result.dock_requested = result.dock_requested || dock_result.dock_requested;
            result.float_requested = result.float_requested || dock_result.float_requested;
            result.external_requested = result.external_requested || dock_result.detach_requested;
            result.close_requested = result.close_requested || dock_result.close_requested;

            if (dock_result.dock_requested)
                state.intent = PanelHostIntent::docked;
            else if (dock_result.float_requested)
                state.intent = PanelHostIntent::floating;
            else if (dock_result.detach_requested)
                state.intent = PanelHostIntent::external;

            state.visible = state.dockable.visible;
            state.dock_slot = state.dockable.dock_slot;
            state.external_requested = state.visible && state.intent == PanelHostIntent::external;
            state.close_requested = !state.visible || state.dockable.close_requested;
        }

        result.intent = state.intent;
        result.dock_slot = state.dock_slot;
        result.layout = make_layout(state, options, input);
        return result;
    }

    const PanelHostController& panel_host_controller() noexcept
    {
        static const PanelHostController controller{};
        return controller;
    }

    bool is_allowed_panel_host_intent(
        PanelHostIntent intent,
        const PanelHostOptions& options) noexcept
    {
        return panel_host_controller().is_allowed_intent(intent, options);
    }

    void focus_panel_host(
        PanelHostRootState& root,
        PanelHostState& state) noexcept
    {
        panel_host_controller().focus(root, state);
    }

    void normalize_panel_host(
        PanelHostRootState& root,
        PanelHostState& state,
        const PanelHostOptions& options) noexcept
    {
        panel_host_controller().normalize(root, state, options);
    }

    PanelHostLayout make_panel_host_layout(
        const PanelHostState& state,
        const PanelHostOptions& options,
        const PanelHostInput& input) noexcept
    {
        return panel_host_controller().make_layout(state, options, input);
    }

    PanelHostResult update_panel_host(
        PanelHostRootState& root,
        PanelHostState& state,
        const PanelHostOptions& options,
        const PanelHostInput& input) noexcept
    {
        return panel_host_controller().update(root, state, options, input);
    }
}
