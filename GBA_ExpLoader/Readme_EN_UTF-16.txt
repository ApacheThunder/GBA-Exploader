======  GBA ExpLoader V0.57  ======

With this program, you can run GBA games which are in SLOT1 flash card through SLOT2 expansion pack.

If your SLOT1 flash card supports soft reset, you can use your exp pack as "Rumble Pak" or "Memory Expansion Pak".

Supported SLOT2 expansion packs are "3in1 Expansion Pack",  "EZ4 lite-delux" and "EWIN EXPANSION PAK(2in1)". 
"NOR mode" and "Rumble Pak" function are enabled only in "3in1 Expansion Pack".



<Specification of 3in Expansion Pack>
1. 256Mbit(32MByte) NORfalsh	page size is 1Mbits(128KB)
2. 128Mbit(16MByte) PSRAM	page size is 1Mbits(128KB)
3. 4Mbit(512KByte)  SRAM	page size is 64Kbits(8KB)

<Specification of EZ FlashIV lite-delux>
1. 384Mbit(48MByte) NORfalsh	page size is 1Mbits(128KB)
2. 256Mbit(32MByte) PSRAM	page size is 1Mbits(128KB)
3. 8Mbit(1MByte)    SRAM	page size is 64Kbits(8KB)

<Specification of EWIN EXPANSION PAK>
1. 256Mbit(32MByte) SDRAM	page size is 16Mbits(2MB)
2. 2Mbit(256KByte)  SRAM	page size is 256Kbits(64KB)
OR
2. 1Mbit(128KByte)  SRAM	page size is 256Kbits(64KB)


<Introduction>
Please patch the DLDI on GBA_ExpLoader.NDS.
(If your flash card supports auto DLDI patch, you need not to do this.)

*Create "/GBA_SAVE" and "/GBA_SIGN" folder on the root of your SD card.
  (Modify "GBA_ExpLoader.ini", and you can set other folders.)
*Copy GBA files into your SD card.
*Put SAV files into "/GBA_SAVE".
  (If there are no SAV files, they will be created automatically.)
*Put "gbaframe.bmp (24bit or 8bit)" into the root, "/GBA_SIGN" or "/_system_" if you want to use GBA frame.


<GBA ExpLoader.ini>
Modify this file to change settings. To add a comment line, include '#' or '!' character at the beginning of a line.
Make sure you put this file into the root of SD card.
  *SaveDir
    Change the directory where SAV files are stored. (Default is /GBA_SAVE.)
  *SignDir
    Change the directory where SGN files and header.dat are stored. (Default is /GBA_SIGN.)
  *No_MultiSave
    Disable multi save function. Comment out this to enable.


===<< IMPORTANT >>===========================================================================
The SRAM save method of this program is different from that of 3in1_ExpPack_Tool.
You cannot use both tools together.
Backup your SRAM first before use this.
=========================================================================================


<Usage>
*Run "GBA_ExpLoader.NDS".
*In "3in1 Expansion Pack", <L> or <R> to change mode ("PSRAM mode", "NOR mode" or "Expansion mode (if supported)") 

If you run a GBA in PSRAM/SDRAM mode, SAV file will be saved next time.
You can select where to save or not to save.


  (PSRAM/SDRAM mode)
  *<A>: Write a GBA file into PSRAM with SRAM patch.
	 Load SRAM data from SAV file.
	 Then run GBA from PSRAM.
  *<B>: Backup SRAM data to SAV file.

  *<Y>: Restore "SRAM.BIN" to SRAM.
  *<X>: Backup all SRAM data to "SRAM.BIN".

  *<R>: Patch PSRAM for DS Browser and soft reset. (Disabled when your pack is "3in1 Expansion Pack")

    With "3in1 Expansion Pack", you can run a GBA up to 16MByte (16000KB) in PSRAM mode.

    And <SELECT> to return to your flashcard's menu if soft reset is supported.
    You can run GBA from RAM until turning off DS or pulling out your expansion pack.

    When you want to exchange SAV file, <B> to backup SAV data (or run another GBA) first and then exchange the file.
    SAV file will be written into SRAM every time you run the GBA, even if you run the same file again and again. 

    If your expansion pack has only 128KB SRAM, you cannot run GBA with FLASH 1Mb save.


  (NOR mode) Enabled only with "3in1 Expansion Pack"

  *<A>: Backup SRAM data to SAV file.
	 Write GBA file to NOR with SRAM patch.
	 Then load SRAM data from SAV file.
  *<B>: Backup SRAM data to SAV file.
  *<Y>: Load SRAM data from SAV file.
  *<X>: Run GBA (Slot2) stored in NOR.

    You can also run NOR in DS menu like a normal GBA cartridge.


  (Expansion mode) Enabled only with "3in1 Expansion Pack"
   Change Rumble level (low/medium/high) or patch PSRAM for DS Browser and then soft reset.


<Composition of SRAM of 3in1 Expansion Pack and EZ4>
  SRAM capacity is 512KB (4Mbit) and you can use it by 8KB(64Kbits) paging.
  Save area for NOR is stored from offset 64KB like this:
 64KB		UNUSED
 128KB		SAVE area for NOR
 8KB		SAVE management area
 128KB		SAVE are for PSRAM
 Rest of space	UNUSED


<Composition of SRAM of EWIN EXPANSION PAK>
  SRAM capacity is 256KB (1Mbit) and you can use it by 64KB(256Kbits) paging like these:
 128KB		SAVE for SDRAM
 64KB		SAVE management area
 64KB		UNUSED
  or if your pack has 128KB SRAM, composition is like these:
 64KB		SAVE for SDRAM
 64KB		SAVE management area


<About soft reset>
SLOT1 flash cards which support soft reset are:
  1. Fully supported
	R4/M3Simply
	DSLink
  2. System file required
	SCDS(ONE)
	N-Card		Please copy udisk.nds to the root of SD card.
	Acekard R.P.G	Please copy akmenu4.nds to the root of SD card.
	X9SD
	DSTT(TTDS)
  3. Firmware file required
	If your flash card doesn't be listed above, backup your flash card and name it SoftReset.****, and copy it to the root of SD card.
 *SC DS ONE:	SCF_DSGB00.nds ->	SoftReset.SCDS
 *G6 DS Real:	PASSCARD3_AMFE00.nds ->	SoftReset.g6ds
 *M3 DS Real:	PASSCARD3_AMFE00.nds ->	SoftReset.M3DS
    You cannot use this method on R4, Evo, AK and AK+.

    *EZ5 supports soft reset, but Rumble or Memory Expansion function will be disabled in EZ5 Kernel menu.
     This means you cannot use these functions on EZ5.


<Repairing of header>
  GBA ExpLoader will repair corrupted header of GBA. (BIN file will not be repaired automatically.)
  Due to some reasons, please setup before use this function.
  First, cut out the head 256Byte from commercial GBA ROM.
  Second, rename it header.dat and copy to "/GBA_SIGN" folder.
  If you cannot understand what I mean, you can use small commercial GBA ROM as header.dat.


<Notice>
*If save type is not SRAM, GBA ExpLoader Will patch SRAM automatically.
  Supported save types:
	EEPROM_V111, EEPROM_V120, EEPROM_V121, EEPROM_V122, EEPROM_V124, EEPROM_V125, EEPROM_V126
	FLASH_V120, FLASH_V121, FLASH_V123, FLASH_V124, FLASH_V125, FLASH_V126
	FLASH512_V130, FLASH512_V131, FLASH512_V133
	FLASH1M_V102, FLASH1M_V103
	Special save type of Famicom Mini(Classic NES)EEPROM_V122, EEPROM_V124 and undefined type

*The size of SAV file will be adjusted to the real SAVE size.
  But GBA ExpLoader cannot detect EEPROM 512B or 8KB, so all EEPROM size will be set to 8KB.
*If GBA ExpLoader cannot detect save type, it will be set to SRAM 64K.
*It's OK even if SAVE data has dummy area at the tail of the file. E.g. real file size is 64K but file size is 128K.
*SAV file will be saved to "/GBA_SAVE". File name will be the same of the ROM file name.
*When you run a ROM first time, analyzed data(SGN file) will be saved to  "/GBA_SIGN".
*If the file extension is BIN, GBA ExpLoader won't patch. And SAV file size will be set to 64KB.
*In file select window, top "SAV" will be saved as ".SAV" and "<1>" to "<5>" will be saved ".SV1" to ".SV5"
*You'd better create "/GBA_SAVE" and "/GBA_SIGN" yourself.
  (Some Slot1 flash cards cannot create these folders correctly. N-Card series will take a long time to do this.)

<History>
*V0.0 2008/2/6	 Prototype version.
*V0.1 2008/2/8	 Support 3in1/2in1 games.
*V0.2 2008/2/10	 Fixed the bug about SAV file extension.
		  Set SAVE size of BIN file and UNKNOWN games to 64KB.
		  Changed the color of the menu in NOR mode.
		  Stopped DMA transfer in PSRAM mode. Speed will be slow but more stable.
*V0.3 2008/2/21 Fixed the bug that soft rest is disabled if your flash card is R4/M3S.
		 Fixed the bug of EEPROM_V111 patch.
		 Fixed the bug of FLASH1M_V103 patch in NOR mode.
		 Support "EZ4 lite-delux" and "EWIN EXPANSION PAK (2in1)"｣.
		 SAVE data in PSRAM/SDRAM mode will be saved to SAV file when GBA_ExpLoader is run.
*V0.4 2008/3/1  Changed some message.
		 Fixed the bug that EWIN EXP PAK won't be detected after you select Memory RAM or LINK and reset.
		 Fixed the detection of EWIN EXP PAK with 128KB SRAM.
		 Add  SAV file; 6 files: SAV and <1> to <5>(sv1 tosv5).
*V0.5 beta 2008/3/9
		 Support EEPROM_V124 Famicom Mini(Classic NES). (EEPROM_V122 is not yet.)
		 Support EWIN EXPANSION PAK (2in1) with 128KB SRAM temporarily.
		 Added auto header fix function.
		 Added setting about SAVE/SIGN directories and MultiSave option.
*V0.5 beta(1) 2008/3/10
		 Has module error. replaced.

*V0.5 2008/3/12 V0.5 final release
		 Support EEPROM_V122 Famicom Mini(Classic NES) and undefined type.
		 Fixed the bug of the reset after setting Rumble Pak.
		 Added directory create date.
*V0.51 2008/3/15
		 Fixed libfat to change the alias name. Now Windows compatible(Digit is set to double figures.)
		 Fixed so-called "0xE5 problem". All Japanese words and short name supported.
		 Fixed extension pack recognition.
		 Fixed the bug of the process of SRAM.BIN in [EXP128K] mode.
*V0.53 2008/3/22
		Fixed the recognition of DSTT GBA&EXPLORER 2IN1 which have 128K SRAM.
		Now, M3(SLOT2), G6(SLOT2) and GBA Expansion Pack are supported temporarily. (FLASH1M save is not supported yet.)
		Fixed the bug of the message when cannot soft reset.
		Support DLDI of SLOT1 flash cards.
*V0.54 2008/3/23
		Has save bug. With M3, G6 or GBA Expansion Pack, save data will be damaged. replaced.
*V0.55 2008/3/30
		 Fixed the bug of NOR writing.
		 Support FLASH1M in DSTT GBA&EXPLORER 2IN1.
*V0.56 2008/4/13
		 Fixed the bug of timestamp of save file.
		 Fixed the display of Japanese character.
		 Fixed the bug of the display when filename is short name(shorter than 8 words).
*V0.57 2008/5/12
		 Fixed the bug of timestamp of save file again.
		 Changed Famicom Mini(Classic NES) save data format to VBA save format. Attention! Compatibility is lost.


by Rudolph (皇帝)

----
Translated by takeru
Thanks to tekito for the base of the translation!
Please tell me if you find  any mistakes.
http://7mc.org
info@7mc.org
