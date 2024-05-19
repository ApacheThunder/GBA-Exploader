# GBA-Exploader
This project is a modernized recompiled version of GBA Exploader 0.57. It has been improved upon by adding back 3 in 1 Plus support which 0.57 source was missing.
(source to 0.58 and later seems to have been lost. Rudolph could not find it).

This for the most part has all the features of the normal version of 0.57 build of GBA Exploader but with the following exceptions:

	* 3 in 1 Plus support.
	* Can write 64MB GBA roms to 3 in 1 Plus cards. Note though this does not mean you can run 64MB GBA Video roms. They are not supported by 3 in 1 Plus.
	* Rumble Menu soft reset code improved and more reliable.
	* SuperCard Lite (and most other similar age SuperCards also supported) support. Note that most games may not have working saves even after prepatching. Save support is not fully implemented for this card.


The NDS files used for Rumble settings soft reset has changed. Please note the following info on how to correctly set this up:


First GBA Exploader will try and find a NDS file with a filename matching the ioType of the DLDI currently being used. For example original R4 DLDI uses R4TF so in that case it will look for a file called R4TF.nds on root of MicroSD card.
If matching NDS file is not found it will then look for the following based ioType of the current DLDI being used:

	* SCDS -> MSFORSC.NDS (Moon SHell for Super Card)
	* NRIO -> udisk.nds (uDisk for N-Card and Clones)
	* RPGN -> akmenu4.nds (AKMENU for AK.R.P.G NAND)
	* RPGS -> akmenu4.nds (AKMENU AK.R.P.G SD)
	* X9SD -> loader.nds (Kernel for X9 SD)
	* TTIO -> TTMENU.DAT" (Kernel for DSTT)
	* DEMO -> R4.nds (Kernel for DSTTi timebombed clones like R4i SDHC Gold Pro etc)
	
If none of the above found it will then try boot.nds. If that isn't present then Rumble menu will be unavailable.

Please refer to the original readme files for the features the original builds of GBA Exploader have that carry over to this one.


Credits:

	* Rudolph for creating GBA Exploader.
	* cory1492 for providing updated source code to gbaldr which was used as reference for restoring missing 3 in 1 Plus support.
	* stl25 for playtesting GBA Exploader to ensure 3 in 1 Plus stuff was working.
	* WinterMute/devKitPro for the bootloader being used to replace the old NDS loader for Rumble Menu soft-reset.


# GBA-Exploader - Old
GBA Exploader 57 source code

I got this source code from Rudolph (皇帝) by email(kotei.blog@gmail.com)

this is what he said about it:

```
「GBA ExpLoader V0.58b」が最終版ですが、リソースを何故か紛失していました。

よって、申し訳ないですが「GBA ExpLoader V0.57」のリソース関連を添付いたします。
ソース等は、ご自由に使用されて構いません。

時間が経過していますので、内容については完全に失念しておりますので、
ビルド方法などご質問いただいてもお答えできません。

一部のファイルの拡張子は削除しています。
・Fami_mini_cnv.vbs
・make_bin.bat
・setenv_devkitPro.bat

by Rudolph (皇帝)

```

It requires a real old version of devkitpro to compile, I was not able to do that, so I just post the original code he gave me, hope that someone can make use of it.
