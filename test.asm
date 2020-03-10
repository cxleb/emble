Emble Compiler (C++ version)
extern _printf
global _main
section .data
cnst0 db "Hello, World!", 0
section .text
_main:
push ebp
mov ebp, esp
sub ebp, 0
_main_b0:
push cnst0
call _printf
mov eax, 0
push eax
pop eax
jmp _main_ret
_main_ret:
mov esp, ebp
pop ebp
ret
