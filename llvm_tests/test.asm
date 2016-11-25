[bits 64]

struc test
    .x: resd 1
    .y: resd 1
endstruc

section .data
test_struct:
    istruc test
        at test.x, dd 0
        at test.y, dd 0
    iend

format: db "x: %d, y: %d", 10, 0

section .text

extern printf

global main
main:
    mov word [test_struct + test.x], 10
    mov word [test_struct + test.y], 15

    mov rdi, format
    mov rsi, qword [test_struct + test.x]
    mov rdx, qword [test_struct + test.y]
    xor rax, rax
    call printf

    ret
