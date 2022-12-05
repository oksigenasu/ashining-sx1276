# Ashining AS32 SX1278 Software for the Raspberry Pi 

Library for Ashining AS32 clone from Ebyte. derived from blog post below
# List of changes:
1. rename all E32 instances to AS32 
2. add encryption command from Ashining (0xC6)
3. change read version command to support Ashining ascii return
4. fix transparent and fixed mode swapped on config  

See this [Blog Post](https://lloydrochester.com/post/hardware/as32-sx1276-lora/) for details.

This repository contains the source code, as well as, the source code to distribute the tool which requres GNU Autotools to build. If you just want to run the tool I recommend just getting the tarball below where you can build from source.

This code has also been run on a Pine64 and Orange Pi Zero.

# Getting Started

We're going to assume you have 2 AS32 Modules attached to two Raspberry PIs. Thus, one can transmit and the other receive and vice-versa. Details for each step in the [Blog Post](https://lloydrochester.com/post/hardware/as32-sx1276-lora/).

1. Wire up your AS32 module. We require 3 pins. Two for the Mode pins and 1 for the Aux Pin. See section below to change wiring if needed.
2. Using `raspi-config` configure your Serial Port, Unix groups and UART File Permissions.
3. Install the `as32` command line tool. See below.
4. Read the version and status from the `./as32 --status`. If this doesn't work the next one won't.
5. Do an end-to-end test to transmit from one and receive on the other. See below.

## Install the `as32` command line tool and get status

```
wget http://lloydrochester.com/code/as32-1.10.0.tar.gz
tar zxf as32-1.10.0.tar.gz
cd as32-1.10.0
./configure
make
sudo make install
as32 --help
as32 --status
```

## Wiring

The wiring defaults to:

```
RPi Pin 23 -> AS32 M0 (Input)
RPi Pin 24 -> AS32 M1 (Input)
RPi Pin 18 -> AS32 AUX (Output)
```

We can change the wiring two ways:
1. Specify pins on the command line. For example `as32 --m0 27 --m1 22 --aux 17`.
2. For a more permanent solution we can change the build to have different defaults. This can be done by doing `CFLAGS="-DGPIO_M0_PIN=27 -DGPIO_M1_PIN=22 -DGPIO_AUX_PIN=17" ./configure` on the installation. Now the defaults will be permanently changed

If we used option #2 to build in the defaults we can view them by doing a `as32 -h` and the default pin will be printed out.

## End-to-End test - Transmit from one AS32 and receive on the other

Again we assume you have two AS32 modules attached to two different Raspberry Pi modules.

Run `as32` on both at the same time, no options are needed. In one terminal type something and hit enter. This will transmit what was typed. On the other terminal you should see what you typed. By doing this what you typed when through the UART to one AS32, was transmitted, received by the other AS32, read out the other UART and was output onto the terminal. Now do this in the other direction.

# Advanced Features

The tool offers more than just taking input from a keyboard. It's meant to run as a daemon and run in the background. If, however, you don't run it as a daemon you can send files and/or save to a file.

When running as a daemon communication to and from the `as32` is via Unix Domain Socket. This allows other tools written an any language to communicate wirelessly by just sending and receiving from a socket. See the blog post for an example in Python.

## Building the distribution

If you don't want the tarball you could build using the GNU Autotools.

```
# clone this repo
./autogen.sh # this creates the configure script and Makefiles
./configure
make
```

## Changing as32 settings

We can use the `-w HEX` option to change settings. For example we could save the settings by doing a `as32 -w C000001A1744`. See the datasheet for each of these options. For the form XXYYYY1AZZ44. If XX=C0 parameters are saved to as32's EEPROM, if XX=C2 settings will be lost on power cycle. The address is represented by YYYY and the channel is represented by ZZ.

## Changing as32 encryption data

We can use the `-e HEX` option to change encryption data settings. For example we could save the settings by doing a `as32 -e 0102030405060708090A0B0C0D0E0F10`. Only modules with same encryption can communicate with each other. Also there is no way to know current encryption setting. Always make sure the modules use the same encryption.