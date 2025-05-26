CELL_SDK ?= C:/usr/local/cell
CELL_TARGET_PATH = $(CELL_SDK)/target
PPU_DIR = $(CELL_SDK)/host-win32/ppu/bin
PPU_PREFIX = ppu-lv2-

CC = $(PPU_DIR)/$(PPU_PREFIX)gcc
LD = $(PPU_DIR)/$(PPU_PREFIX)gcc

SCETOOL ?= ./scetool.exe

CFLAGS = -std=c99 -Wall