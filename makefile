##-----------------------------------------------------------------------------
## rgen.art
##-----------------------------------------------------------------------------
ifeq (, $(wildcard $(ROOT)makeuser))
$(error no $(ROOT)makeuser found)
else
include $(ROOT)makeuser
endif

BIN        	?= rgen
COMPILER 	?= gcc
#ARCH		?= x86_32
ARCH		?= x86_64
#TARGET_OS	?= osx
#TARGET_OS	?= lin

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
ROOT        ?= ../

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
DIR		    ?= bin_rgen
BUILD	    ?= debug
FATAL	    ?= FALSE
MUD		    ?= false
ASAN        ?= false
VALGRIND    ?= false
CHECK       ?=

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
INCLUDES    ?=
LIBRARIES   ?=# -static -static-libgcc -static-libstdc++

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
ifeq ($(TARGET_OS), osx)
SDK_ROOT	?= /Applications/XCode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/
X11_ROOT	?= $(SDK_ROOT)System/Library/Frameworks/Tk.framework/Versions/8.5/Headers/X11/
#PCI_ROOT        ?= $(SDK_ROOT)usr/include/pci

LIBXML2_H_DIR	?= $(SDK_ROOT)usr/include/libxml2/
LIBXML2_L_DIR   ?=
LIBXML2_LIB     ?= -lxml2

LIBPNG_H_DIR    ?= /opt/local/include/
LIBPNG_L_DIR    ?= /opt/local/lib/
LIBPNG_LIB      ?= -lpng16

OPENGL_ROOT	    ?= $(SDK_ROOT)System/Library/Frameworks/OpenGL.framework/Versions/A/
OPENGL_H_DIR    ?= $(OPENGL_ROOT)Headers/
OPENGL_L_DIR    ?=
OPENGL_LIB      ?= -framework OpenGL

FREETYPE2_ROOT  ?= $(SDK_ROOT)usr/include/freetype2/

OPENAL_ROOT     ?=  $(SDK_ROOT)System/Library/Frameworks/OpenAL.framework/Versions/A/
OPENAL_H_DIR    ?=  $(OPENAL_ROOT)Headers/
OPENAL_L_DIR    ?=
OPENAL_LIB      ?=  -framework OpenAL

COCOA_LIB       ?= -framework cocoa -framework QuartzCore

else
##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
ifeq ($(TARGET_OS), lin)
SDK_ROOT	?= /usr/include/
X11_ROOT	?= $(SDK_ROOT)X11/

LIBXML2_H_DIR	?= $(SDK_ROOT)libxml2/
LIBXML2_L_DIR   ?=
LIBXML2_LIB ?= -lxml2

LIBPNG_H_DIR    ?= /usr/local/include/
LIBPNG_L_DIR    ?= /usr/local/lib/
LIBPNG_LIB      ?= -lpng16

OPENGL_H_DIR	?= $(SDK_ROOT)GL/
OPENGL_L_DIR    ?=
OPENGL_LIB      ?= -lGL

FREETYPE2_ROOT  ?= #$(SDK_ROOT)freetype2/

OPENAL_H_DIR    ?= $(SDK_ROOT)AL/
OPENAL_L_DIR    ?=
OPENAL_LIB      ?= -lopenal

else
##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
BASE_SDK ?= /mingw64/
ifeq ($(TARGET_OS), win)
ifeq ($(COMPILER), gcc)
SDK_ROOT    ?= $(BASE_SDK)
else
ifeq ($(COMPILER), llvm)
SDK_ROOT ?= $(BASE_SDK)
endif
endif

OPENGL_ROOT ?= $(BASE_SDK)
OPENGL_H_DIR	?= $(OPENGL_ROOT)include/mesa/GL/
OPENGL_L_DIR    ?= $(OPENGL_ROOT)lib/
OPENGL_LIB      ?= -lopengl32

FREETYPE2_ROOT ?=
LIBXML2_ROOT ?= $(SDK_ROOT)
LIBXML2_H_DIR ?= $(LIBXML2_ROOT)include/libxml2/
LIBXML2_L_DIR ?= $(LIBXML2_ROOT)lib/
LIBXML2_LIB   ?= -lxml2

LIBPNG_ROOT     ?= $(SDK_ROOT)
LIBPNG_H_DIR    ?= $(LIBPNG_ROOT)include/libpng16/
LIBPNG_L_DIR    ?= $(LIBPNG_ROOT)lib/
LIBPNG_LIB      ?= -lpng16

LIBJPEG_ROOT     ?= $(SDK_ROOT)
LIBJPEG_H_DIR    ?= $(LIBJPEG_ROOT)include/
LIBJPEG_L_DIR    ?= $(LIBJPEG_ROOT)lib/
LIBJPEG_LIB      ?= -ljpeg


OPENAL_ROOT ?= $(SDK_ROOT)
OPENAL_H_DIR	?= $(OPENAL_ROOT)include/AL/
OPENAL_L_DIR    ?= $(OPENAL_ROOT)lib/
OPENAL_LIB	    ?= -lopenal #-Wl, -Bstatic


# LIBZ_ROOT ?= $(SDK_ROOT)
# LIBZ_H_DIR ?= $(LIBZ_ROOT)include/
# LIBZ_L_DIR ?= $(LIBZ_ROOT)lib/
# LIBZ_LIB ?= -lz


endif
endif
endif

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
ARGS		?= 	ARCH="$(ARCH)" BUILD="$(BUILD)" FATAL="$(FATAL)" MUD="$(MUD)"\
                ROOT="$(ROOT)" COMPILER="$(COMPILER)" TARGET_OS="$(TARGET_OS)" DEVICE="$(DEVICE)"\
                VALGRIND="$(VALGRIND)" ASAN="$(ASAN)"

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
MAKETARGET	?=  $(ROOT)uti_make/maketarget
IMACROS		?=	imacro.h

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
ifeq "$(wildcard $(DIR))" ""
$(error invalid project directory : $(DIR))
else
endif

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
include $(DIR)/makefile

TARGET_JOB ?= 

ifeq ($(TARGET_OS), win)
TARGET_JOB ?=
#TARGET_JOB ?= -j12# --output-sync=recurse
else
TARGET_JOB
# ?= -j 4
endif

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
all:
	cd $(DIR); time $(MAKE) -j12 -f $(MAKETARGET) all TARGET="all"\
		BINARY="$(BIN)"\
		IMACROS="$(IMACROS)"\
		INCLUDES="$(INCLUDES)"\
        LIBRARIES="$(LIBRARIES)"\
		SOURCES="$(SOURCES)"\
		$(ARGS)

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
check:
	cd $(DIR); time $(MAKE) $(TARGET_JOB) -f $(MAKETARGET) check TARGET="check"\
		$(ARGS) CHECK="$(CHECK)"

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
compile:
	cd $(DIR); time $(MAKE) $(TARGET_JOB) -f $(MAKETARGET) compile TARGET="compile"\
		IMACROS="$(IMACROS)"\
        INCLUDES="$(INCLUDES)"\
		$(ARGS)

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
cleanbin:
	cd $(DIR); time $(MAKE) $(TARGET_JOB) -f $(MAKETARGET) cleanbin TARGET="cleanbin"\
		BINARY="$(BIN)"\
		$(ARGS)

clean:
	cd $(DIR); time $(MAKE) $(TARGET_JOB) -f $(MAKETARGET) clean TARGET="clean"\
		BINARY="$(BIN)"\
		$(ARGS)

cleanall:
	cd $(DIR); time $(MAKE) $(TARGET_JOB) -f $(MAKETARGET) cleanall TARGET="cleanall"\
		BINARY="$(BIN)"\
		$(ARGS)

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
.PHONY: all check compile cleanbin clean cleanall
