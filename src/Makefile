KERNEL := stivale.elf

CC = cc
NASM= nasm

CFLAGS = -Wall -Wextra -O2 -pipe

INTERNALLDFLAGS := \
	-fno-pic  -fPIE \
	-Wl,-static,--no-dynamic-linker,-ztext \
	-static-pie    \
	-nostdlib      \
	-Tlinker.ld    \
	-z max-page-size=0x1000

INTERNALCFLAGS  :=       \
	-I.                  \
	-std=gnu11           \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic -fpie       \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone

CFILES := $(shell find ./ -type f -name '*.c')
ASM_FILES := $(shell find ./ -type f -name '*.asm')
C_OBJ    := $(CFILES:.c=.o)
NASM_OBJ    := $(ASM_FILES:.asm=.o)
.PHONY: all clean

all: $(KERNEL)

$(KERNEL): $(NASM_OBJ) $(C_OBJ)
	$(CC) $(INTERNALLDFLAGS) $(NASM_OBJ) $(C_OBJ) -o $@

stivale.h:
	wget https://github.com/stivale/stivale/raw/master/stivale.h

%.o : %.asm
	$(NASM) -f elf64 $< -o $@

%.o: %.c stivale.h
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@

clean:
	rm -rf $(KERNEL) $(C_OBJ) $(NASM_OBJ)

