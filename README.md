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
degrees. The mono graphics driver supports points, lines, boxes (filled or not),
and text with multiple fonts. I am experimenting with C++ template static polymorphism here.
Not sure if I will keep it. Makes the code very verbose. If it saves enough space
in memory I will. Otherwise, I will go to regular C++ polymorphism.

The MVC part is not started yet. Still under a lot of development. Because
the displays can use I2C, PIO-based-I2C, SPI, etc.
