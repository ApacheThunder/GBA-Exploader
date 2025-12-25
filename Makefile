#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TARGET := GBA_ExpLoader
export TOPDIR := $(CURDIR)

export VERSION_MAJOR	:= 0
export VERSION_MINOR	:= 67
export VERSTRING	:=	$(VERSION_MAJOR).$(VERSION_MINOR)

# GMAE_ICON is the image used to create the game icon, leave blank to use default rule
GAME_ICON :=

# specify a directory which contains the nitro filesystem
# this is relative to the Makefile
# NITRO_FILES := nitroFiles


# These set the information text in the nds file
#GAME_TITLE     := My Wonderful Homebrew
#GAME_SUBTITLE1 := built with devkitARM
#GAME_SUBTITLE2 := http://devitpro.org

include $(DEVKITARM)/ds_rules

.PHONY: data ndsbootloader clean

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: ndsbootloader checkarm7 checkarm9 $(TARGET).nds

#---------------------------------------------------------------------------------
checkarm7:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
checkarm9:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
$(TARGET).nds : arm7/$(TARGET).elf arm9/$(TARGET).elf
	@ndstool	-c $@ -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf \
				-b $(CURDIR)/logo32.bmp "GBA ExpLoader;Version $(VERSTRING);By Rudolph" \
				-h 0x200

data:
	@mkdir -p arm9/data
	
ndsbootloader: data
	$(MAKE) -C ndsbootloader LOADBIN=$(CURDIR)/arm9/data/load.bin

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
arm9/$(TARGET).elf: ndsbootloader
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	$(MAKE) -C ndsbootloader clean
	rm -rf arm9/data
	rm -f $(TARGET).nds

