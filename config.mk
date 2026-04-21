CELL_SDK ?= /usr/local/cell
CELL_TARGET_PATH = $(CELL_SDK)/target
HOST_DIR = $(CELL_SDK)/host-win32/bin
PPU_DIR = $(CELL_SDK)/host-win32/ppu/bin
PPU_PREFIX = ppu-lv2-

CC = $(PPU_DIR)/$(PPU_PREFIX)gcc
LD = $(PPU_DIR)/$(PPU_PREFIX)gcc
AS = $(PPU_DIR)/$(PPU_PREFIX)as

SCETOOL ?= ./scetool
MAKE_FSELF ?= $(HOST_DIR)/make_fself

CFLAGS = -std=c99 -Wall -I./

# Put your PS3's local IP here to use the install/run targets on the Makefile
PS3_IP ?= 10.0.0.2
