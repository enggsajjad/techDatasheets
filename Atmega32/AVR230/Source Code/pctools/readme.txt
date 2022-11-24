Step-by-step instructions for AT90Mega16
========================================


The following instructions describe the steps needed to update the example
application into ATMega16. The instructions can be easily adapted for other
devices, too. Instructions are for AVR Studio 4 and IAR Embedded Workbench
EWAVR 2.28A.


1) Run 'create' tool with the following parameters:

     create -c config\mega16.cnf -e example\example.eep -f example\example.hex
       -h ..\bootldr\bootldr.h -k ..\bootldr\deskeys.inc -o example.enc

   This will use 'config\mega16.cnf' as the configuration file to create two
   files for the Boot Loader ('bootldr.h' and 'deskeys.inc') and an encrypted
   firmware file 'example.enc' which contains the update information for both
   Flash memory (fetched from 'example\example.hex') and EEPROM (from
   'example\example.eep').

   For source code of the example application, refer to 'example\example.asm'.


2) Run IAR Embedded Workbench. Open project 'bootldr\bootldr16.prj'. Choose
   'Release' as the target from 'bootldr16.prj' window and then 'Build All'
   (in Project Menu).

   The boot loader is now configured and compiled for ATMega16.


3) Make sure that the microcontroller chip on your STK500 is 'ATMEGA16'
   and jumpers are in default settings (see . Configure STK500 for In-System
   Programming (ISP) by connecting ISP6PIN to SPROG3. Connect RS232 CTRL to
   your PC, check that the power cable is connected and switch STK500 on.


4) Run AVR Studio and open file 'bootldr\Release\Exe\bootldr16.dbg'. Use
   'AVR Simulator' as the Debug Platform and 'ATMega16' as the Device.
   Then select Finish.
   
   Start 'STK500/AVRISP/JTAG ICE...' from the Tools menu.
   From 'Program' tab, set Programming mode to ISP, and check both

     [x] Erase Device Before Programming
     [x] Verify Device After Programming

   Choose 'Use Current Simulator/Emulator FLASH Memory' and press 'Program'
   button below.

   The boot loader is now programmed into the ATMega16.

   Choose 'Fuses' tab and make sure the following checkboxes are checked:
     [x] Boot Flash section size=1024 words...
     [x] Boot Reset vector Enabled...
     [x] Int. RC Osc. 8 MHz; Start-up time: 6 CK + 64 ms

 
5) On STK500, switch the power off. Wire PD7 to SW7 and then wire PD0 + PD1 to
   RXD + TXD, respectively. Also, connect PORTB to LEDS.
  
   Connect 'RS232 SPARE' into your PC using a serial cable (you can use the
   same cable that was used with AVR Studio if you first exit AVR Studio).


6) On STK500, keep SW7 pressed while switching the power on and then release
   SW7. This will switch the boot loader in ATMega16 into update mode.


7) Run 'update' tool with the following parameters (replace x in COMx with
   the number of the COM port where 'RS232 SPARE' is connected to):

     update example.enc -COMx


8) After few seconds LEDs should start to play.

   The application is now updated into ATMega16.

