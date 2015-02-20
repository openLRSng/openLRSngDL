openLRSng Data Limk
===================

This is modified version of openLRSng which provides only bidirectional transparent serial link.

SUPPORTED HW:
============
  - TX_BOARD_TYPE 0 (Arduino Mini/nano 328 16MHz)
    - Flytron openLRS M1 (100mW) TX

  - RX_BOARD_TYPE 2 (TX module as RX) (328P 16MHz)
    - Flytron openLRS M2 (100mW)/M3(1W) TX
    - OrangeRX UHF TX unit


  - RX_BOARD_TYPE 3 (Arduino Mini/nano 328 16MHz)
    - Flytron openLRS RX
    - OrangeRX UHF RX (NOTE both LEDs are RED!!)
    - HawkEye OpenLRS RX

  - TX_BOARD_TYPE 4 (Arduino Mini/nano 328 16MHz)
    - openLRSngTX ('kha' or DIY based on ngTX schematics)
    - HawkEye OpenLRSng TX

  - RX_BOARD_TYPE 5 (Arduino Mini/nano 328 16MHz)
    - DTFUHF/HawkEye 4ch/6ch RX

  - RX_BOARD_TYPE 7
    - Brotronics PowerTowerRX

  - RX_BOARD_TYPE 8
    - openLRSng microRX

    Notably the 32u4 based unit is not currently supported for simplicity.

SOFTWARE CONFIGURATION:
=======================
  Only hardware related selections are done in the openLRSngDL.ino.

  Run time configuration is done by connecting to the TX module (which is put into binding mode) with serial terminal. For best restults use real terminal program like Putty, TeraTerm, minicom(Linux) but it is possible to use Arduino Serial Monitor too.
  Sending '<CR>' (enter) will enter the menu which will guide further. It should be noted that doing edits will automatically change 'RF magic' to force rebinding, if you want to use a specific magic set that using the command (and further automatic changes are ceased for the edit session).

  Datarates are: 0==4800bps 1==9600bps 2==19200bps 3==57600bps 4==125kbps

UPLOADING:
==========
Use a 3v3 FTDI (or other USB to TTL serial adapter) or USBASP (3v3!!)

GENERATED FIRMWARE FILES:
=========================
Makefile generates TX/RX firmware files for each frequency 433/868/915 as well as all hardware types DL-[#].hex.

The "*-bl.hex" files contain bootloader and can be used when flashing via ICSP.
  o 328p model (all but type 6) use optiboot bootloader and thus needs fuses set to: Low: FF High: DE Ext: 05.
  o type 6 uses 'caterina' and fuses should be set to: Low: FF High: D8 Ext: CB.

USERS GUIDE
===========

tbd.
