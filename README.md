# File Follower Friend

Using an embedded device to display file directory information from an external device.

## Team

- Brodie Abrew
- Lucas Berry

## Requirements

- [ x ] Code for Project
- [ x ] Product Design Document
- [ x ] Specification Report
- [ x ] Slideshow Presentation

## Design Goals

- Quickly send requests to an external device and receive data back
  - UART communication between STM board and host device
  - Application running on host device to handle requests
- Display file directory information on the LCD display
  - Text rendering
  - Basic shape rendering
  - (_Possibly_) Image rendering
- Navigate the menu using a joystick
- Open new directories and go back using the buttons
- (_Possibly_) Allow for the screen to be turned off/reset

## Build/Run Instructions

**Work in Progress**

- Install PySerial with the following command: `python -m pip install pyserial`
- Install Pillow with the following command: `python -m pip install pillow`
