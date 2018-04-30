#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files embedded using bin2o
# GRAPHICS is a list of directories containing image files to be converted with grit
#---------------------------------------------------------------------------------
TARGET		:=	$(shell basename $(CURDIR))
BUILD		:=	build

SOURCES		:=	source \
				../common	\
                source/cpu_misc \
				source/disk_fs/ \
				source/ipc/	\
				source/libnds_extends	\
				source/memory_handler_gba	\
				source/interrupts

INCLUDES	:=	include	\
				../common	\
                source/cpu_misc \
				source/disk_fs/ \
				source/ipc/	\
				source/libnds_extends	\
				source/memory_handler_gba	\
				source/interrupts

DATA		:=	data  
GRAPHICS	:=	gfx  
@NITRODATA	:=	nitrofiles

# These set the information text in the nds file
GAME_TITLE		:=	Gbaemu4ds
GAME_SUBTITLE1	:=	2015 Coto - 2013 ichfly/ichichfly
GAME_SUBTITLE2	:=	

#---------------------------------------------------------------------------------
# options for code generation  # -save-temps
#---------------------------------------------------------------------------------

export GLOBAL_SHARED_FLAGS_9 = -march=armv5te	-mthumb-interwork

#CPU code builder settings
export GLOBAL_AS_COMPILER_FLAGS =	-defsym	readvertoolchain='$(TOOLCHAIN_GENERIC_DS_VER)'	-defsym		NDS='1'
export GLOBAL_CC_COMPILER_FLAGS =	-w 	-g	-marm	-mapcs-frame	-O -fno-common -Dreadvertoolchain='$(TOOLCHAIN_GENERIC_DS_VER)'	-DSINGLE_THREADED	-falign-functions=4	-mstructure-size-boundary=8
export GLOBAL_CXX_COMPILER_FLAGS =	-g	-marm	-O	-fno-exceptions	-fno-common	-MMD	-MP	-DSINGLE_THREADED	-falign-functions=4	-mstructure-size-boundary=8

#CPU code builder settings TGDS project specific
export SPECIFIC_AS_COMPILER_FLAGS_7		= -defsym	ARM7='1'	-defsym	appver='$(EXECUTABLE_VERSION)'
export SPECIFIC_CC_COMPILER_FLAGS_7		= -DARM7	-Dappver='$(EXECUTABLE_VERSION)'
export SPECIFIC_CXX_COMPILER_FLAGS_7	= $(SPECIFIC_CC_COMPILER_FLAGS_7)

export SPECIFIC_AS_COMPILER_FLAGS_9		= -defsym	ARM9='1'	-defsym	appver='$(EXECUTABLE_VERSION)'
export SPECIFIC_CC_COMPILER_FLAGS_9		= -DARM9	-Dappver='$(EXECUTABLE_VERSION)'
export SPECIFIC_CXX_COMPILER_FLAGS_9	= $(SPECIFIC_CC_COMPILER_FLAGS_9)


CFLAGS	:= $(INCLUDE)	$(GLOBAL_SHARED_FLAGS_9)	$(GLOBAL_CC_COMPILER_FLAGS) $(SPECIFIC_CC_COMPILER_FLAGS_9)
CXXFLAGS	:= $(INCLUDE)	$(GLOBAL_SHARED_FLAGS_9)	$(GLOBAL_CXX_COMPILER_FLAGS)	$(SPECIFIC_CC_COMPILER_FLAGS_9)

ASFLAGS	:=	-s $(ARCH)
LDFLAGS       =       --specs=../tgba_arm9.specs $(ARCH) -Wl,--gc-sections,-Map,$(notdir $*.map) 


#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project (order is important)
#---------------------------------------------------------------------------------
LIBS	:=  -lnds9
 
 
#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(LIBNDS)
 
#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
					$(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PNGFILES	:=	$(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.png)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
 
#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(PNGFILES:.png=.o) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
 ifneq ($(strip $(NITRODATA)),)
	export NITRO_FILES	:=	$(CURDIR)/$(NITRODATA)
endif
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)
 
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

icons := $(wildcard *.bmp)

ifneq (,$(findstring $(TARGET).bmp,$(icons)))
	export GAME_ICON := $(CURDIR)/$(TARGET).bmp
else
	ifneq (,$(findstring icon.bmp,$(icons)))
		export GAME_ICON := $(CURDIR)/icon.bmp
	endif
endif
 
.PHONY: $(BUILD) clean
 

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -o $(BUILD) -f $(CURDIR)/Makefile
 
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds

#---------------------------------------------------------------------------------
else
 
 DEPENDS := $(OFILES:.o=.d)
 
#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).nds	: 	$(OUTPUT).elf
$(OUTPUT).elf	:	$(OFILES)
 
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

#---------------------------------------------------------------------------------
# This rule creates assembly source files using grit
# grit takes an image file and a .grit describing how the file is to be processed
# add additional rules like this for each image extension
# you use in the graphics folders
#---------------------------------------------------------------------------------
%.s %.h   : %.png %.grit
#---------------------------------------------------------------------------------
	grit $< -fts -o$*

-include $(DEPENDS)
 
#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
