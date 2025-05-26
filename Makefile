include ./config.mk

C_SRCS = prx.c memory.c
PRX_TARGET = patchwork.prx

PPU_LIBS	= $(CELL_TARGET_PATH)/ppu/lib/fno-exceptions/fno-rtti/libc.a
PPU_LIBS	+= $(CELL_TARGET_PATH)/ppu/lib/hash/libsha256.a
PPU_PRX_LDFLAGS 	= -zgenprx -zgenstub -lio_stub -lfs_stub -lsysutil_stub -lsysmodule_stub -Wl,--strip-unused-data

SPRX_TARGET = $(basename $(PRX_TARGET)).sprx
SYM_TARGET = $(basename $(PRX_TARGET)).sym
OBJS_DIR = ./objs
C_OBJS = $(patsubst %, $(OBJS_DIR)/%.ppu.o, $(basename $(C_SRCS)))
C_OBJS_DEPENDS = $(patsubst %.c, $(OBJS_DIR)/$(basename %).ppu.o, $(C_SRCS))

all: $(SPRX_TARGET)


$(C_OBJS): $(OBJS_DIR)/%.ppu.o: %.c
	@mkdir -p $(dir $(@))
	$(CC) -c $(CFLAGS) -Wp,-MMD,$(@:.o=.d),-MT,$@ -o $@ $<

$(SYM_TARGET): $(C_OBJS_DEPENDS)
	@mkdir -p $(dir $(@))
	$(LD) -mprx $(C_OBJS) $(PPU_PRX_LDFLAGS) $(PPU_LIBS) -o $@

$(PRX_TARGET): $(SYM_TARGET)
	cp $(SYM_TARGET) $(PRX_TARGET)

$(SPRX_TARGET): $(PRX_TARGET)
	$(SCETOOL) -0 SELF -1 FALSE -s TRUE -2 0007 -3 1010000001000003 -4 01000002 -j FALSE -5 APP -A 0001000000000000 -6 0003005000000000 -c SPRX -9 00000000000000000000000000000000000000000000003B0000000100002000 -g patchwork.sprx -e patchwork.prx patchwork.sprx
	#$(SCETOOL) -1 TRUE --sce-type=SELF --skip-sections=FALSE --key-revision=0A --self-app-version=0001000000000000 --self-auth-id=1010000001000003 --self-vendor-id=01000002 --self-ctrl-flags=0000000000000000000000000000000000000000000000000000000000000000 --self-cap-flags=00000000000000000000000000000000000000000000003B0000000100040000 --self-type=APP --self-fw-version=0003005500000000 --compress-data true -g patchwork.sprx --encrypt patchwork.prx patchwork.sprx

clean:
	rm -rf $(OBJS_DIR) $(PRX_TARGET) $(SYM_TARGET) $(SPRX_TARGET)
