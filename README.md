# Zerobase-master

## Introduction

zerobase-master is the firmware project for the Windsor board. It works between mobile app and main control board of BLE devices. Mobile app can control these devices and monitor their data through the Windsor board.

The Windsor board is based on Nordic's Nrf52840 Bluetooth chip. The firmware is developed with nRF5_SDK_17.0.2 SDK, using Keil MDK5 IDE.

## Supported devices

- HBOT
- Cold Plunge
- Sauna
- Red Light Bed

## Prerequisites

- [Keil IDE](https://www.keil.com/)
- [Keil MDK](https://developer.arm.com/Tools%20and%20Software/Keil%20MDK)
- [Keil chip pack](http://www.keil.com/dd2/Pack/)
- [Nordic 52840 chip pack](https://developer.nordicsemi.com/nRF5_SDK/pieces/nRF_DeviceFamilyPack/NordicSemiconductor.nRF_DeviceFamilyPack.8.44.1.pack)
- [nrfutil.exe](https://www.nordicsemi.com/Products/Development-tools/nRF-Util/Download?lang=en#infotabs): an official command-line tool provided by Nordic Semiconductor, to generate programming file of nRF52, nRF53, nRF54, and nRF91 series chips

## Project File Structure

- app
  - config
    - arm5_no_packs: The keil project
    - armgcc: MakeFile for gcc compiler
  - src
    - projects
      - WAP-2-411 Oxford: HBOT
        - app: communication with peripherals and BLE
        - bsp: GPIO configurations
      - WAP-2-431 Sauna
        - app: communication with main board and BLE
        - bsp: GPIO configurations
      - WAP-2-411 Edinburgh: Cold Plunge
        - app: communication with main board and BLE
        - bsp: GPIO configurations
      - WAP-2-451 Bristol: Red Light Bed
        - app: communication with main board and BLE
        - bsp: GPIO configurations
    - cmds: The state machine code for executing app settings commands and the command code for actively uploading data to the app
    - config: Bluetooth SDK configuration file, controller type configuration definition file
    - efsm: Finite state machine implementation
    - tuya_cmd_management: Tuya protocol implementation, communication between Windsor and main control board
    - session: Session control implementation
    - session_sharing: Session sharing implementation
    - user_app: Main appplication, communication with mobile app, including broadcast processing, nrf52840 chip falsh read/write, Bluetooth UUID service establishment and other application related functions
    - user_ble: Bluetooth service
    - user_bsp: UART communication
    - user_drivers: Watchdog
- bootloader: DFU
  - project
    - arm5_no_packs: Keil project
    - armgcc: MakeFile for gcc compiler
  - src: Bootloader main program
- external
  - NordicSdk_17_0_2: Nordic SDK
- bin: Scripts to generate firmware file for production and OTA

## Firmware Compilation

- Open the project files in `app/config/arm5_no-packs` with Keil.
- Select target project with the "Select Target" drop-down on the toolbar
- Select "Build" on the toolbar to compile
  the project

## Bootloader Compilation

- Open the project files in bootloader/project/arm5_no-packs with Keil
- Select "Build" on the toolbar to compile
  the project

## Production and OTA file generation

- Run bat scripts in `bin` directory to generate firmware files for production and OTA.
