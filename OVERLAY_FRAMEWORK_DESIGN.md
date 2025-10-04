# Overlay and Menu Framework Design

## Overview
A flexible framework for UI overlays, menus, and configurable input handling.

## Core Components

### 1. Input System (`src/input/`)
```
input/
├── input_manager.hpp/cpp     # Central input handling
├── key_mapping.hpp/cpp       # Configurable key bindings
├── input_config.hpp/cpp      # Load/save input configuration
└── input_events.hpp          # Event system for input
```

### 2. UI Framework (`src/ui/`)
```
ui/
├── ui_manager.hpp/cpp        # Central UI state management
├── overlay.hpp/cpp           # Base overlay class
├── menu.hpp/cpp              # Menu system
├── settings_menu.hpp/cpp     # Settings menu implementation
├── widgets/
│   ├── button.hpp/cpp        # Button widget
│   ├── slider.hpp/cpp        # Slider widget
│   ├── checkbox.hpp/cpp      # Checkbox widget
│   └── text_input.hpp/cpp    # Text input widget
└── rendering/
    ├── ui_renderer.hpp/cpp   # UI rendering system
    └── fonts.hpp/cpp         # Font management
```

### 3. Configuration System (`src/config/`)
```
config/
├── config_manager.hpp/cpp    # Central config management
├── input_config.hpp/cpp      # Input-specific config
├── ui_config.hpp/cpp         # UI-specific config
└── settings.hpp/cpp          # Game settings
```

## Key Features

### Input System
- **Configurable Key Mappings**: All controls stored in `input.ini`
- **Action-Based Input**: Map keys to actions (e.g., "move_forward", "toggle_menu")
- **Context-Sensitive**: Different key maps for different UI states
- **Hot Reload**: Update key bindings without restart

### UI System
- **Layered Overlays**: Multiple UI layers (HUD, menus, dialogs)
- **Modal System**: Block input to background when menu is open
- **Widget System**: Reusable UI components
- **Theme Support**: Configurable colors, fonts, styles

### Settings Menu
- **Mouse Sensitivity**: Adjustable look sensitivity
- **Graphics Settings**: Resolution, VSync, quality presets
- **Audio Settings**: Volume controls, audio device selection
- **Key Bindings**: Visual key remapping interface

## Configuration Files

### `input.ini`
```ini
[actions]
move_forward=W
move_backward=S
move_left=A
move_right=D
look_up=MOUSE_Y_NEG
look_down=MOUSE_Y_POS
look_left=MOUSE_X_NEG
look_right=MOUSE_X_POS
toggle_menu=ESCAPE
toggle_debug=F3
toggle_wireframe=F
toggle_mouse_lock=F4
recenter_camera=R
break_block=MOUSE_LEFT
place_block=MOUSE_RIGHT

[contexts]
game=default
menu=menu_keys
```

### `ui.ini`
```ini
[theme]
primary_color=#2E3440
secondary_color=#3B4252
accent_color=#88C0D0
text_color=#ECEFF4
font_size=14
font_family=Consolas

[layout]
menu_width=400
menu_height=300
button_height=32
slider_width=200
```

## Implementation Plan

### Phase 1: Input System Refactor
1. Create `InputManager` class
2. Implement action-based input mapping
3. Add configuration file support
4. Refactor existing input handling

### Phase 2: Basic UI Framework
1. Create `UIManager` and `Overlay` base classes
2. Implement basic widget system
3. Add rendering pipeline for UI
4. Create simple menu system

### Phase 3: Settings Menu
1. Implement settings menu UI
2. Add mouse sensitivity controls
3. Add graphics settings
4. Integrate with input system

### Phase 4: Advanced Features
1. Key binding interface
2. Theme customization
3. Save/load user preferences
4. Hot reload support

## Benefits

- **Maintainable**: Clean separation of concerns
- **Extensible**: Easy to add new UI elements and controls
- **User-Friendly**: Configurable controls and settings
- **Professional**: Modern UI/UX patterns
- **Performance**: Efficient rendering and input handling
