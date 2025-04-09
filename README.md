# File Follower Friend
Using an embedded device to display file directory information from an external device.

## Team
* Brodie Abrew
* Lucas Berry

## Requirements
- [ ] Code for Project
- [ ] Product Design Document
- [ ] Specification Report
- [ ] Slideshow Presentation 

## Design Goals
* Quickly send requests to an external device and receive data back
    * UART communication between STM board and host device
    * Application running on host device to handle requests
* Display file directory information on the LCD display
    * Text rendering
    * Basic shape rendering
    * (_Possibly_) Image rendering
* Navigate the menu using a joystick
* Open new directories and go back using the buttons
* (_Possibly_) Allow for the screen to be turned off/reset

## Ideas
Python (maybe) program which grabs current directory (cap directory size) and sends it over usb to the board. Possibly using the PySerial library.
