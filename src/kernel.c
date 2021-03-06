
#include "typedefs.h"

//#include <stdint.h>
#include <stddef.h>
#include "stivale.h"
#include <cpu/io.h>
#include <cpu/idt.h>

#include <drivers/serial.h>
#include <drivers/gui.h>

#include <kprintf.h>

// We need to tell the stivale bootloader where we want our stack to be.
// We are going to allocate our stack as an uninitialised array in .bss.
static u8 stack[4096];

// The stivale specification says we need to define a "header structure".
// This structure needs to reside in the .stivalehdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
__attribute__((section(".stivalehdr"), used))
static struct stivale_header stivale_hdr = {
    // Let's tell the bootloader where our stack is.
    // We need to add the sizeof(stack) since in x86(_64) the stack grows
    // downwards.
    .stack = (uintptr_t)stack + sizeof(stack),
    // These flags can be used to tell the bootloader to enable or disable
    // certain features. We turn on bit 0 as we are going to ask for a
    // graphical framebuffer. Not setting this bit will make the
    // bootloader default to a CGA-compatible text mode instead.
    // We also toggle bit 3 as that will make the bootloader offset pointers
    // given to us to the higher half.
    .flags = (1 << 0) | (1 << 3),
    // We set all the framebuffer specifics to 0 as we want the bootloader
    // to pick the best it can.
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0,
    // The entry_point member is used to specify an alternative entry
    // point that the bootloader should jump to instead of the executable's
    // ELF entry point. We do not care about that so we leave it zeroed.
    .entry_point = 0
};


// The following will be our kernel's entry point.
void _start(struct stivale_struct *stivale_struct) {
    // Let's get the address of the framebuffer.
    u32 * fb_addr = (u32 *)stivale_struct->framebuffer_addr;
    // Let's try to paint a few pixels white in the top left, so we know
    // that we booted correctly.
    serial_init();
    turn_color_on();

    kprintf("%d mmap entries\n", stivale_struct->memory_map_entries);
    kprintf("mmap addr : 0x%x\n", stivale_struct->memory_map_addr);

    struct stivale_mmap_entry * mmap_entries = (struct stivale_mmap_entry*) stivale_struct->memory_map_addr;

    pmm_init();
    for(int i=0; i<stivale_struct->memory_map_entries;++i){
        if(mmap_entries[i].type == STIVALE_MMAP_USABLE){
            kprintf("Base 0x%x\n", mmap_entries[i].base);
            kprintf("Size %d MiB\n", mmap_entries[i].length/(1024*1024));
            kprintf("Type %d\n", mmap_entries[i].type);
            pmm_init_region(mmap_entries[i].base, mmap_entries[i].length);
        }
    }

    pmm_dump();

    kprintf("Framebuffer height : %d\n", stivale_struct->framebuffer_height);
    kprintf("Framebuffer width : %d\n", stivale_struct->framebuffer_width);
    kprintf("Framebuffer addr : 0x%x\n", stivale_struct->framebuffer_addr);
    kprintf("Framebuffer bpp : %d\n", stivale_struct->framebuffer_bpp);
    

    screen_init(stivale_struct);

    asm("cli");
    idt_init();
    // We're done, just hang...
    for (;;) {
        asm ("hlt");
    }
}
