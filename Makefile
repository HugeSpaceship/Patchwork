CELL_SDK ?= C:/usr/local/cell
CELL_MK_DIR ?= ./mk
include $(CELL_MK_DIR)/sdk.makedef.mk

PEXPORTPICKUP		= ppu-lv2-prx-exportpickup
CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='ecrti.o)
CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='crtbegin.o)
CRT_TAIL                += $(shell ppu-lv2-gcc -print-file-name'='crtend.o)
CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='ecrtn.o)

SCETOOL = ./scetool.exe
PRX_LDFLAGS_EXTRA	= -L ./lib -Wl,--strip-unused-data
PPU_SRCS 	= memory.c prx.c
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

sprx:
	$(SCETOOL) -0 SELF -1 FALSE -s TRUE -2 0007 -3 1010000001000003 -4 01000002 -j FALSE -5 APP -A 0001000000000000 -6 0003005000000000 -c SPRX -9 00000000000000000000000000000000000000000000003B0000000100002000 -g patchwork.sprx -e patchwork.prx patchwork.sprx
	#$(SCETOOL) -1 TRUE --sce-type=SELF --skip-sections=FALSE --key-revision=0A --self-app-version=0001000000000000 --self-auth-id=1010000001000003 --self-vendor-id=01000002 --self-ctrl-flags=0000000000000000000000000000000000000000000000000000000000000000 --self-cap-flags=00000000000000000000000000000000000000000000003B0000000100040000 --self-type=APP --self-fw-version=0003005500000000 --compress-data true -g patchwork.sprx --encrypt patchwork.prx patchwork.sprx


include $(CELL_MK_DIR)/sdk.target.mk


