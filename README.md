NanoVNA - Very tiny handheld Vector Network Analyzer
==========================================================
[release]: https://github.com/DiSlord/NanoVNA-D/releases

<div align="center">
<img src="/doc/nanovna.jpg" width="480px">
</div>

# About

**NanoVNA-H** and **NanoVNA-H4** are very tiny handheld Vector Network Analyzers (VNA).
They are standalone portable devices withLCD and battery.
This project aims to provide improved firmware for this useful instrument for enthusiast.

This repository contains the source code of the improved NanoVNA-H and NanoVNA-H4 firmware.

The documentation describes the build and flash process on a MacOS or a Linux (Debian or Ubuntu) system, other Linux (or even BSD) systems may behave similar.

## Prepare ARM Cross Tools

**UPDATE**: Recent gcc version works to build NanoVNA, no need to use old version.

### MacOSX

Install cross tools and firmware updating tool.

    brew tap px4/px4
    brew install gcc-arm-none-eabi-80
    brew install dfu-util

### Linux (ubuntu)

Download arm cross tools from [here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).

    wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/8-2018q4/gcc-arm-none-eabi-8-2018-q4-major-linux.tar.bz2
    sudo tar xfj gcc-arm-none-eabi-8-2018-q4-major-linux.tar.bz2 -C /usr/local
    PATH=/usr/local/gcc-arm-none-eabi-8-2018-q4-major/bin:$PATH
    sudo apt install -y dfu-util

### Debian

    sudo apt install gcc-arm-none-eabi
    sudo apt install -y dfu-util

## Fetch Source Code

Fetch the firmware source and the submodule, do this once to initialize your local clone from GitHub:

    git clone https://github.com/DiSlord/NanoVNA-D.git
    cd NanoVNA-D
    git submodule update --init --recursive

## Update Source Code

To get updates from the GitHub repository, go to your `NanoVNA-D` directory and type:

    git pull

## Build the NanoVNA-H Firmware

Go to your `NanoVNA-D` directory and type:

    export TARGET=F072
    make clean
    make

## Build the NanoVNA-H4 Firmware

Go to your `NanoVNA-D` directory and type:

    export TARGET=F303
    make clean
    make

## Flash Firmware

When the build of your firmware is finished, you can flash it onto your NanoVNA device.
First, let the device enter DFU mode by one of following methods.

* Open the device and jumper `BOOT0` pin to `Vdd` pin when powering the device.
* Select menu Config->DFU (needs recent firmware).
* Press the jog switch on your -H4 when powering the device.

Then, flash the firmware using `dfu-util` via USB.

#### For NanoVNA-H:

Go to your `NanoVNA-D` directory and type:

    dfu-util -d 0483:df11 -a 0 -s 0x08000000:leave -D build/H.bin

#### For NanoVNA-H4:

Go to your `NanoVNA-D` directory and type:

    dfu-util -d 0483:df11 -a 0 -s 0x08000000:leave -D build/H4.bin

#### Or simply type directly after building the firmware (for both variants).

Go to your `NanoVNA-D` directory and type:

    make flash

#### Ignore the apparent error message during flashing

The low-level tool `dfu-util` displays a lot of information that is very useful especially for developers, but can confuse the user.
In particular, please ignore the message about corrupt firmware, this is the normal behaviour of the unit before clearing the status.
It is important to note that after clearing the status, there is no longer an error condition present.

```
...
Determining device status...
DFU state(10) = dfuERROR, status(10) = Device's firmware is corrupt. It cannot return to run-time (non-DFU) operations
Clearing status
Determining device status...
DFU state(2) = dfuIDLE, status(0) = No error condition is present
...
```

## Companion Tools

There are several numbers of great companion PC tools from third-party.

* [NanoVNA-App software](https://github.com/OneOfEleven/NanoVNA-H/blob/master/Release/NanoVNA-App.rar) by OneOfEleven
* [NanoVNASharp Windows software](https://drive.google.com/drive/folders/1IZEtx2YdqchaTO8Aa9QbhQ8g_Pr5iNhr) by hugen79
* [NanoVNA WebSerial/WebUSB](https://github.com/cho45/NanoVNA-WebUSB-Client) by cho45
* [Android NanoVNA app](https://play.google.com/store/apps/details?id=net.lowreal.nanovnawebapp) by cho45
* [NanoVNASaver](https://github.com/NanoVNA-Saver/nanovna-saver) by mihtjel and the members of NanoVNA-Saver
* [TAPR VNAR4](https://groups.io/g/nanovna-users/files/NanoVNA%20PC%20Software/TAPR%20VNA) supports NanoVNA by erikkaashoek
* [The NanoVNA toolbox](https://github.com/Ho-Ro/nanovna-tools) by Ho-Ro
* see [python](/python/README.md) directory to use NanoVNA with Python and Jupyter Notebook.

## Documentation

* [NanoVNA User Guide(ja)](https://cho45.github.io/NanoVNA-manual/) by cho45. [(en:google translate)](https://translate.google.com/translate?sl=ja&tl=en&u=https%3A%2F%2Fcho45.github.io%2FNanoVNA-manual%2F)
* [NanoVNA user group](https://groups.io/g/nanovna-users/topics) on groups.io.

## Reference

* [Schematics](/doc/nanovna-sch.pdf)
* [PCB Photo](/doc/nanovna-pcb-photo.jpg)
* [Block Diagram](/doc/nanovna-blockdiagram.png)
* Kit available from https://ttrf.tk/kit/nanovna

## Note

Hardware design material is disclosed to prevent bad quality clone. Please let me know if you would have your own unit.

## Credit
* [@DiSlord](https://github.com/DiSlord/)

## Based on code from:
* [@edy555](https://github.com/edy555)

### Contributors
* [@OneOfEleven](https://github.com/OneOfEleven)
* [@hugen79](https://github.com/hugen79)
* [@cho45](https://github.com/cho45)

