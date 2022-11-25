# Superseded
The code from this project is abandonded. I have actually put together a framework
for doing this, but it is in several different repositories. I didn't write all of them.
- [pico-ssd1306-mono-graphics-lib](https://github.com/rppicomidi/pico-ssd1306-mono-graphics-lib) :
The low level SSD1306 OLED engine
- [pico-mono-ui-lib](https://github.com/rppicomidi/pico-mono-ui-lib) : the UI library with View,
Menu classes and JSON serializable settings
- [parson](https://github.com/kgabis/parson) : handles JSON serialization and deserialization
- [pico-littlefs](https://github.com/rppicomidi/littlefs-lib) (fork) : handles local setting storage in
program flash.
- [EmbeddedCli](https://github.com/funbiscuit/embedded-cli) : helps with testing and debugging
- [elm-chan FatFs](http://elm-chan.org/fsw/ff/00index_e.html) : provides a FAT filesystem for
use with projects that have a USB host port; allows you to backup and restore settings from
the program flash to an external USB flash drive.
- [tinyusb](https://github.com/hathach/tinyusb) : provides USB Host code and MSC code for
external flash drives. It is more up-to-date than the tinyusb code provided with the
[pico-sdk](https://github.com/raspberrypi/pico-sdk).

See the project [pico-usb-midi-processor](https://github.com/rppicomidi/pico-usb-midi-processor)
for a sample project that uses this framwork. In particular, you will need files in the `lib`
directory of that project for character fonts. 

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
