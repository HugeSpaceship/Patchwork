include ./config.mk

C_SRCS = prx.c memory.c
PRX_TARGET = patchwork.prx

PPU_LIBS	= $(CELL_TARGET_PATH)/ppu/lib/fno-exceptions/fno-rtti/libc.a
PPU_LIBS	+= $(CELL_TARGET_PATH)/ppu/lib/hash/libsha256.a
PPU_PRX_LDFLAGS 	= -zgenprx -zgenstub -lio_stub -lfs_stub -lsysutil_stub -lsysmodule_stub -Wl,--strip-unused-data

SPRX_TARGET = $(basename $(PRX_TARGET)).sprx
RPCS3_SPRX_TARGET = $(basename $(PRX_TARGET))-rpcs3.sprx
SYM_TARGET = $(basename $(PRX_TARGET)).sym
OBJS_DIR = ./objs
C_OBJS = $(patsubst %, $(OBJS_DIR)/%.ppu.o, $(basename $(C_SRCS)))
C_OBJS_INFO = $(patsubst %, $(OBJS_DIR)/%.ppu.d, $(basename $(C_SRCS)))
C_OBJS_DEPENDS = $(patsubst %.c, $(OBJS_DIR)/$(basename %).ppu.o, $(C_SRCS))

all: $(SPRX_TARGET) $(RPCS3_SPRX_TARGET)

$(C_OBJS_INFO): $(C_OBJS)

$(C_OBJS): $(OBJS_DIR)/%.ppu.o: %.c
	@mkdir -p $(dir $(@))
	$(CC) -c $(CFLAGS) -Wp,-MMD,$(@:.o=.d),-MT,$@ -o $@ $<

$(SYM_TARGET): $(HOOK_OBJS_DEPENDS) $(C_OBJS_DEPENDS)
	@mkdir -p $(dir $(@))
	$(AS) "-mregnames" -o ./objs/hooks.o hooks.s
	$(LD) -mprx $(C_OBJS) ./objs/hooks.o $(PPU_PRX_LDFLAGS) $(PPU_LIBS) -o $@

$(PRX_TARGET): $(SYM_TARGET)
	cp $(SYM_TARGET) $(PRX_TARGET)

$(SPRX_TARGET): $(PRX_TARGET)
	$(SCETOOL) -0 SELF -1 FALSE -s TRUE -2 0007 -3 1010000001000003 -4 01000002 -j FALSE -5 APP -A 0001000000000000 -6 0003005000000000 -c SPRX -9 00000000000000000000000000000000000000000000003B0000000100002000 -g $(SPRX_TARGET) -e $(PRX_TARGET) $(SPRX_TARGET)

$(RPCS3_SPRX_TARGET): $(PRX_TARGET)
	$(MAKE_FSELF) $(PRX_TARGET) $(RPCS3_SPRX_TARGET)

clean:
	rm -rf $(OBJS_DIR) $(PRX_TARGET) $(SYM_TARGET) $(SPRX_TARGET) $(RPCS3_SPRX_TARGET)

install: $(SPRX_TARGET)
	pwsh .\scripts\install.ps1 -PS3IP $(PS3_IP)

run: install
	pwsh .\scripts\run.ps1 -PS3IP $(PS3_IP)
