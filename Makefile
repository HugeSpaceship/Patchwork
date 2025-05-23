CELL_SDK = C:/usr/local/cell
CELL_MK_DIR ?= $(CELL_SDK)/samples/mk
PRX_SAMPLES_DIR = ../..
include $(CELL_MK_DIR)/sdk.makedef.mk
#include $(PRX_SAMPLES_DIR)/mk/prx.mk

PEXPORTPICKUP		= ppu-lv2-prx-exportpickup
CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='ecrti.o)
CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='crtbegin.o)
CRT_TAIL                += $(shell ppu-lv2-gcc -print-file-name'='crtend.o)
CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='ecrtn.o)

PRX_LDFLAGS_EXTRA	= -L ./lib -Wl,--strip-unused-data
PPU_SRCS 	= memory.c config.c prx.c
PPU_PRX_TARGET 	= patchwork.prx
PPU_PRX_LDFLAGS += $(PRX_LDFLAGS_EXTRA)
PPU_INCDIRS	+= -I$(CELL_TARGET_PATH)/ppu/include/sysutil
CLEANFILES 	= $(PRX_DIR)/$(PPU_SPRX_TARGET)
#PPU_LIBS    += ./lib/liballocator_export_stub.a
PPU_LIBS	+= $(CELL_TARGET_PATH)/ppu/lib/fno-exceptions/fno-rtti/libc.a
#PPU_LIBS    += ./lib/libstdc_export_stub.a

PPU_LIBS	+= $(CELL_TARGET_PATH)/ppu/lib/hash/libsha256.a


PPU_PRX_LDLIBS 	+= -lio_stub -lfs_stub -lsysutil_stub -lsysmodule_stub -lrtc_stub

stub:

include $(CELL_MK_DIR)/sdk.target.mk


