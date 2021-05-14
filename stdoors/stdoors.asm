default rel
global _start

section .text
MAX_LEN equ 20

_start:

; Before syscall push rcx and r11

db "START_OF_THE_STDLIB"

db "INPUT_STDLIB"
input:
    push rbp

    mov rsi, rsp
    dec rsi

    mov r8, 0

    mov rdx, 1
    mov rdi, 0 ; fd 0 == stdin
    mov rax, 0

    syscall

    xor r9, r9
    cmp BYTE [rsi], 0x2D ; '-'
    jne input_mid

    mov r9, 1

input_loop_int:
    mov rdx, 1
    mov rdi, 0 ; fd 0 == stdin
    mov rax, 0
    
    syscall

input_mid:

    cmp BYTE [rsi], 0x0A ; '\n'
    je translate_num

    cmp BYTE [rsi], 0x20 ; ' '
    je translate_num

    cmp BYTE [rsi], 0x2E ; '.'
    je input_frac

    inc r8    
    dec rsi

    cmp r8, 14
    jge int_overflow

    jmp input_loop_int

input_frac:
    dec rsi

    mov r8, 0

input_frac_loop:
    mov rdx, 1
    mov rdi, 0 ; fd 0 == stdin
    mov rax, 0

    syscall

    cmp BYTE [rsi], 0x0A ; '\n'
    je translate_num

    cmp BYTE [rsi], 0x20 ; ' '
    je translate_num

    inc r8
    dec rsi

    cmp r8, 4
    jge frac_overflow

    jmp input_frac_loop

frac_overflow:

    mov BYTE [rsi], 0x0A

    jmp translate_num

int_overflow:

    mov BYTE [rsi], 0x0A

    jmp translate_num

translate_num:
    
    xor rax, rax
    mov rdx, 10
    mov rsi, rsp
    dec rsi
    mov r8, 4

translate_int_loop:
    
    cmp BYTE [rsi], 0x0A
    je input_end_while
    
    cmp BYTE [rsi], 0x20
    je input_end_while

    cmp BYTE [rsi], 0x2E
    je translate_frac

    mov bl, [rsi]
    dec rsi

    sub bl, 0x30
    movzx rbx, bl
    
    imul rax, 10
    add rax, rbx 
    
    jmp translate_int_loop

translate_frac:

    dec rsi

translate_frac_loop:

    cmp BYTE [rsi], 0x0A
    je input_end_while
    
    cmp BYTE [rsi], 0x20
    je input_end_while

    dec r8

    mov bl, [rsi]
    dec rsi

    sub bl, 0x30
    movzx rbx, bl
    
    imul rax, 10
    add rax, rbx 

    jmp translate_frac_loop

input_end_while:

    cmp r8, 0
    je input_pop

    imul rax, 10
    dec r8
    jmp input_end_while

input_pop:
    
    cmp r9, 1
    jne input_positive_number

    sub rax, 1
    not rax

input_positive_number:

    pop rbp
    ret

db "SQRT_STDLIB"
sqrt:
    push rbp
    mov rbp, rsp
    mov rax, [rbp + 16] ; input

    pxor xmm0, xmm0
    cvtsi2sd xmm0, rax ; load rax to xmm0

    movq xmm0, xmm0
    sqrtpd xmm0, xmm0

    cvttsd2si rax, xmm0

    imul rax, 100

    pop rbp
    ret


db "OUTPUT_STDLIB"
output:
    push rbp
    mov  rbp, rsp
    mov  rax, [rbp + 16] ; input number

    mov rdx, rax
    and rdx, [max_bit]
    xor r8, r8
    cmp rdx, 0

    je positive_number

    mov r8, 1 ; negative number
    not rax
    inc rax

positive_number:
    mov rbx, 10 
    lea r9, [dec_array]
    mov r10, rsp
    sub r10, 8

    mov r12, 0
    mov r13, 0

output_loop_frac:
    xor rdx, rdx
    div rbx      ; res in rax, reminder in rdx
    mov dl, byte [r9 + rdx]
    mov byte [r10], dl
    inc r12
    dec r10

    cmp r12, 4
    je output_int
    
    jmp output_loop_frac

output_int:
    mov byte [r10], 0x2E
    dec r10

output_loop_int:
    xor rdx, rdx
    div rbx      ; res in rax, reminder in rdx
    mov dl, byte [r9 + rdx]
    mov byte [r10], dl
    dec r10

    cmp rax, 0
    jne output_loop_int

    cmp r8, 1
    jne positive_number2
    mov byte [r10], 45
    jmp print

positive_number2:
    inc r10

print:
    mov rdx, rsp
    sub rdx, 8
    sub rdx, r10
    inc rdx 
    
    mov rdi, 1
    mov rsi, r10
    mov rax, 0x01
    
    syscall

    push QWORD 0x0A
    mov  rdi, 1
    mov  rsi, rsp
    mov  rdx, 1
    mov  rax, 0x01
    syscall

    add rsp, 8

    pop rbp
    ret

dec_array: db "0123456789"
max_bit:   dq 0x8000000000000000

db "END_OF_THE_STDLIB"
