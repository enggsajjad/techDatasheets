HOW TO BUILD THE BOOTLOADER

1. Use the PC tools to create the files 'aeskeys.inc' and 'bootldr.h'.
2. Open the IAR EW workspace and select the project for you AVR device.
   - If no project file exist for your device, consult the appnote document
     and use information for the device's datasheet for set up your own
     project file.
3. Choose 'Rebuild All'.
4. Program you device with resulting HEX file.

