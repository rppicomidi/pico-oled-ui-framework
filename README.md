# pico-oled-ui-framework
A small  SSD1306 based OLED display UI framework (C++) for Raspberry Pi Pico

This framework is designed to support application with user interfaces
that use small (128 x 64 or smaller) OLEDs for the display(s). I have a number
of MIDI boxes in mind that would benefit from a display.

# Goals
- Display driver easily ported to different physical interfaces
- Display driver and graphics driver have clear MIT license (to the best of my ability)
- Ability to work with OLED in portrait or landscape mode through 90 degree rotation
- Support for rotary encoders, USB keyboards, buttons, etc. for UI control
- Model View Controller (MVC) Pattern support
- Support clickable, scrollable menus with menu items such as
  - text
  - checkbox and text (for enabling features)
- Support a matrix routing screen for, for example, routing MIDI IN to one or more MIDI OUT
- Support a virtual keyboard for text entry
- Support non-volatile storage of the Model parameters (for example, store settings to local flash)

# Current status
The SSD1306 driver manages the display and supports screen rotation of 0, 90, 180 and 270
degrees. I have tested this with a 128 x 64 oled and a 128 x 32 oled. The mono graphics driver 
supports points, lines, boxes (filled or not), and text with multiple fonts. I discarded
C++ template static polymorphism and reverted to regular C++ Vtable polymorphism. It
was much less painful to code.

Physcial interfaces supported now are RP2040 I2C and RP2040 PIO-based I2C.

The MVC part is not started yet. Turns out creating graphics screens is fun.
Still under a lot of development.
