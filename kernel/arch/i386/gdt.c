#include <kernel/gdt.h>
#include <stdio.h>


typedef struct gdtdesc gdt_desc;
typedef struct gdtr gdt_ptr;
typedef struct tss TSS; 

void init_gdt_desc(int w, uint32_t base, uint32_t lim, uint8_t access, uint8_t gran);

gdt_desc gdt[3];
gdt_ptr gp;
TSS t;

extern uint32_t gdt_flush(gdt_ptr *);
//static uint64_t GDT_DEFAULT[3] = {0x0000000000000000,  // null segment
//                                  0x00DF9A000000FFFF,  // code segment
//                                  0x00DF92000000FFFF}; // data segment
//
void init_gdt(){
    gp.limit =  ((sizeof(gdt_desc)*5)-1);
    gp.base = &gdt[0];

	init_gdt_desc(0, 0, 0, 0, 0); // null Segment
	init_gdt_desc(1, 0,  0xFFFFFFFF, 0x9A, 0xCF); // kernel code segment 
    init_gdt_desc(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // kernel data segment

    init_gdt_desc(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user code segment
	init_gdt_desc(4, 0, 0xFFFFFFFF, 0xFE, 0xCF); // user data segment


    gdt_flush(&gp);

    

    printf("\n[kernel] GDT Loaded \n\n");
}

void init_gdt_desc(int w, uint32_t base, uint32_t lim,
				   uint8_t access, uint8_t gran)
{
    /* Setup the descriptor base address */
    gdt[w].basea = (base & 0xFFFF);
    gdt[w].baseb = (base >> 16) & 0xFF;
    gdt[w].basec = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt[w].lima = (lim & 0xFFFF);
    gdt[w].limflags = ((lim >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    gdt[w].limflags |= (gran & 0xF0);
    gdt[w].ab = access;

}





