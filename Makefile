include ./config.mk

SRC_DIR  = src
OBJ_DIR  = build
OBJS_DIR = objs

SRC_DIRS = \
	$(SRC_DIR) \
	$(SRC_DIR)/tools \
	$(SRC_DIR)/hooks \
	$(SRC_DIR)/toml \

PRX_TARGET = patchwork.prx

PPU_LIBS = \
	$(CELL_TARGET_PATH)/ppu/lib/fno-exceptions/fno-rtti/libc.a \
	$(CELL_TARGET_PATH)/ppu/lib/hash/libsha256.a

PPU_PRX_LDFLAGS = -zgenprx -zgenstub -lio_stub -lfs_stub \
	-lsysutil_stub -lsysmodule_stub -Wl,--strip-unused-data

SPRX_TARGET  = $(basename $(PRX_TARGET))-signed.sprx
FSPRX_TARGET = $(basename $(PRX_TARGET))-fake-signed.sprx
SYM_TARGET   = $(basename $(PRX_TARGET)).sym

C_SRCS := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.c))
ASM_SRCS := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.s))

C_OBJS   := $(C_SRCS:%.c=$(OBJS_DIR)/%.ppu.o)
ASM_OBJS := $(ASM_SRCS:%.s=$(OBJS_DIR)/%.ppu.o)

OBJS := $(C_OBJS) $(ASM_OBJS)
DEPS := $(OBJS:.ppu.o=.ppu.d)
-include $(DEPS)

$(OBJS_DIR)/%.ppu.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -Wp,-MMD,$(@:.ppu.o=.ppu.d),-MT,$@ -o $@ $<

$(OBJS_DIR)/%.ppu.o: %.s
	@mkdir -p $(dir $@)
	$(AS) -mregnames -o $@ $<

all: $(SPRX_TARGET) $(FSPRX_TARGET)

$(SYM_TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(LD) -mprx $(OBJS) $(PPU_PRX_LDFLAGS) $(PPU_LIBS) -o $@

$(PRX_TARGET): $(SYM_TARGET)
	cp $(SYM_TARGET) $(PRX_TARGET)

$(SPRX_TARGET): $(PRX_TARGET)
	$(SCETOOL) -0 SELF -1 FALSE -s TRUE -2 0007 -3 1010000001000003 -4 01000002 -j FALSE -5 APP -A 0001000000000000 -6 0003005000000000 -c SPRX -9 00000000000000000000000000000000000000000000003B0000000100002000 -g $(SPRX_TARGET) -e $(PRX_TARGET) $(SPRX_TARGET)

$(FSPRX_TARGET): $(PRX_TARGET)
	$(MAKE_FSELF) $(PRX_TARGET) $(FSPRX_TARGET)

clean:
	rm -rf $(OBJS_DIR) $(PRX_TARGET) $(SYM_TARGET) $(SPRX_TARGET) $(FSPRX_TARGET)

patch-tool:
	go build -o patch-generator/patch-generator ./patch-generator

patches: $(PRX_TARGET) patch-tool
	./patch-generator/patch-generator -input ./patchwork.prx -config ./lbpdefs -output ./patchwork-rpcs3-patch.yml


ifeq ($(OS),WINDOWS_NT)
install: $(SPRX_TARGET)
	pwsh .\scripts\install.ps1 -PS3IP $(PS3_IP)
else
install:
	curl -T ./$(SPRX_TARGET) ftp://$(PS3_IP)/dev_hdd0/plugins/patchwork.sprx
endif

ifeq ($(OS),WINDOWS_NT)
run: install
	pwsh .\scripts\run.ps1 -PS3IP $(PS3_IP)
else
run: install
	curl http://$(PS3_IP)/play.ps3
endif
	

