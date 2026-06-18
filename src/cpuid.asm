.code

; uint32_t CalcCpuFeatureMask(uint32_t inputMask)
; RCX = inputMask
; 返回值 RAX
CalcCpuFeatureMask proc
    push    rsi
    push    rdi

    mov     r8d, ecx        ; 保存 inputMask
    xor     edi, edi         ; edi = 0
    xor     eax, eax
    xor     ecx, ecx
    xchg    rbx, r9

    ; CPUID 0
    ; mov     eax, 0
    cpuid
    xchg    rbx, r9
    mov     esi, eax         ; 最大标准功能号

    xor     ecx, ecx
    mov     eax, 1
    xchg    rbx, r9
    cpuid                     ; CPUID 1
    xchg    rbx, r9
    mov     r11d, ecx        ; ECX -> r11d
    mov     r10d, edx        ; EDX -> r10d
    mov     r9d, 0         ; r9d = 0
    cmp     esi, 7
    jl      skip7

    mov     eax, 7
    xor     ecx, ecx
    xchg    rbx, rdi
    cpuid                     ; CPUID 7
    xchg    rbx, rdi
    mov     r9d, ecx         ; r9d = CPUID7 ECX
skip7:

    ; 算法部分
    shr     r10d, 15h
    and     r10d, 20h

    mov     eax, r11d
    shr     eax, 3
    and     eax, 40h
    or      eax, r10d

    mov     ecx, r11d
    shr     ecx, 12h
    and     ecx, 180h
    or      eax, ecx

    mov     ecx, r11d
    shr     ecx, 0Ch
    and     ecx, 180h
    or      ecx, eax
    mov     eax, edi
    and     eax, 200h
    lea     r10d, [rcx + rax*4]
    add     r10d, 10h
    mov     eax, r11d
    not     eax
    test    eax, 1C000000h
    jne     finish

    xor     ecx, ecx
    xgetbv
    not     eax
    test    al, 6
    jne     finish

    mov     eax, edi
    and     eax, 20h
    shl     eax, 5
    mov     ecx, r11d
    and     ecx, 1000h
    shr     r11d, 10h
    and     r11d, 2000h
    or      r11d, ecx
    or      r11d, eax
    or      r10d, r11d
    or      r10d, 200h

    xor     ecx, ecx
    xgetbv
    not     eax
    test    al, 0E0h
    jne     finish

    shrd    edi, r9d, 0Fh
    and     edi, 58000h
    mov     eax, r9d
    and     eax, 40h
    shl     eax, 0Dh
    or      eax, edi
    
    mov     ecx, r9d     
    shl     ecx, 6       
    mov     edx, ecx     
    and     edx, 20000h
    or      edx, eax      
    mov     eax, r9d     
    shl     eax, 8       
    and     eax, 100000h
    or      eax, edx      
    shl     r9d, 7       
    and     r9d, 200000h
    or      r9d, eax      
    and     ecx, 4000h
    or      ecx, r9d      
    or      r10d, ecx     

finish:
    or      r10d, 1
    and     r10d, r8d        ; inputMask
    mov     eax, r10d

    pop     rdi
    pop     rsi
    ret
CalcCpuFeatureMask endp

end