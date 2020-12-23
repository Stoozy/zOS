global gdt_flush

gdt_flush:
    mov     eax, [esp+4]
    lgdt    [eax]

    mov     ax, 0x10
    mov     es, ax
    mov     ss, ax
    mov     ds, ax
    mov     fs, ax
    mov     gs, ax

    jmp     0x08:done 

done:
    ret
    

