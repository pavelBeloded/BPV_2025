.586
.model flat, stdcall
option casemap :none

; --- Libs ---
includelib msvcrt.lib
includelib legacy_stdio_definitions.lib

; --- Externs ---
ExitProcess PROTO :DWORD
printf PROTO C :DWORD, :VARARG
strlen PROTO C :DWORD
sprintf PROTO C :DWORD, :DWORD, :VARARG
touint PROTO :DWORD
sub_str PROTO :DWORD, :DWORD, :DWORD

.stack 4096

.data
	fmt_num db "%d", 0
	fmt_num_nl db "%d", 10, 0
	fmt_str_nl db "%s", 10, 0
	msg_start db "[ASM] Program Started", 10, 0
	common_buf db 256 dup(0)
	date_buf db "12.12.2025", 0
	ret_stack dd 1024 dup(?)
	ret_ptr dd 0
	calculate_a dd 100
	calculate_b dd 10
	calculate_res dd 110
	L10 dd 2
	greeting_name_s db 0
	greeting_name dd offset greeting_name_s
	greeting_hello_s db 0
	greeting_hello dd offset greeting_hello_s
	L14_s db "Hello, ", 0
	L14 dd offset L14_s
	entry_x dd 10
	entry_y dd 10
	entry_counter dd 110
	entry_msg_s db "10", 0
	entry_msg dd offset entry_msg_s
	entry_dateStr_s db "12.12.2025", 0
	entry_dateStr dd offset entry_dateStr_s
	L20 dd 100
	L21 dd 10
	entry_fDateNumb_s db "2", 0
	entry_fDateNumb dd offset entry_fDateNumb_s
	L23 dd 1
	entry_numb dd 2
	L25 dd 15
	L26_s db "Start Loop:", 0
	L26 dd offset L26_s
	entry_temp dd 1
	L28_s db "End", 0
	L28 dd offset L28_s
	L29 dd 0

.code
main PROC
	invoke printf, offset msg_start
M0: jmp M27

M1: pop eax
mov calculate_b, eax

M2: pop eax
mov calculate_a, eax

M3: push offset calculate_res

M4: push offset calculate_a

M5: push offset calculate_b

M6: pop ebx
mov ebx, [ebx]
pop eax
mov eax, [eax]
add eax, ebx
push eax

M7: push 2

M8: pop ebx
pop eax
imul eax, ebx
push eax

M9: push 2

M10: pop ebx
pop eax
cdq
idiv ebx
push eax

M11: pop eax
pop ebx
mov [ebx], eax
push eax

M12: push offset calculate_res

M13: push offset calculate_res

M14: push offset calculate_a

M15: push 2

M16: pop ebx
pop eax
mov eax, [eax]
cdq
idiv ebx
mov eax, edx
push eax

M17: pop ebx
pop eax
mov eax, [eax]
add eax, ebx
push eax

M18: pop eax
pop ebx
mov [ebx], eax
push eax

M19: push offset calculate_res

M20: cmp ret_ptr, 0
je quit_program
dec ret_ptr
mov ebx, ret_ptr
mov eax, [ret_stack + ebx*4]
jmp eax

M21: pop eax
mov greeting_name, eax

M22: push offset greeting_hello

M23: push offset L14_s

M24: pop eax
pop ebx
mov [ebx], eax
push eax

M25: push offset greeting_hello

M26: cmp ret_ptr, 0
je quit_program
dec ret_ptr
mov ebx, ret_ptr
mov eax, [ret_stack + ebx*4]
jmp eax

M27: push offset entry_x

M28: push 100

M29: pop eax
pop ebx
mov [ebx], eax
push eax

M30: push offset entry_y

M31: push 10

M32: pop eax
pop ebx
mov [ebx], eax
push eax

M33: push offset entry_counter

M34: push offset entry_x

M35: push offset entry_y

M36: mov ebx, ret_ptr
mov dword ptr [ret_stack + ebx*4], offset Ret_M36
inc ret_ptr
jmp M1
Ret_M36:
push eax

M37: pop eax
pop ebx
mov [ebx], eax
push eax

M38: push offset entry_counter

M39: pop eax
mov eax, [eax]
invoke printf, offset fmt_num_nl, eax

M40: push offset entry_dateStr

M41: push offset date_buf

M42: pop eax
pop ebx
mov [ebx], eax
push eax

M43: push offset entry_dateStr

M44: pop eax
mov eax, [eax]
invoke printf, offset fmt_str_nl, eax

M45: push offset entry_x

M46: push offset entry_dateStr

M47: pop eax
mov eax, [eax]
invoke strlen, eax
push eax

M48: pop eax
pop ebx
mov [ebx], eax
push eax

M49: push offset entry_x

M50: pop eax
mov eax, [eax]
invoke printf, offset fmt_num_nl, eax

M51: push offset entry_msg

M52: push offset entry_y

M53: pop eax
mov eax, [eax]
invoke sprintf, offset common_buf, offset fmt_num, eax
push offset common_buf

M54: pop eax
pop ebx
mov [ebx], eax
push eax

M55: push offset entry_msg

M56: pop eax
mov eax, [eax]
invoke printf, offset fmt_str_nl, eax

M57: push offset entry_fDateNumb

M58: push offset entry_dateStr

M59: push 1

M60: push 1

M61: pop ecx
pop ebx
pop eax
mov eax, [eax]
invoke sub_str, eax, ebx, ecx
push eax

M62: pop eax
pop ebx
mov [ebx], eax
push eax

M63: push offset entry_numb

M64: push offset entry_fDateNumb

M65: pop eax
mov eax, [eax]
invoke touint, eax
push eax

M66: pop eax
pop ebx
mov [ebx], eax
push eax

M67: push offset entry_numb

M68: pop eax
mov eax, [eax]
invoke printf, offset fmt_num_nl, eax

M69: push offset entry_x

M70: push 15

M71: pop eax
pop ebx
mov [ebx], eax
push eax

M72: push offset entry_y

M73: push 10

M74: pop eax
pop ebx
mov [ebx], eax
push eax

M75: push offset L26_s

M76: pop eax
invoke printf, offset fmt_str_nl, eax

M77: push offset entry_x

M78: push offset entry_y

M79: pop ebx
mov ebx, [ebx]
pop eax
mov eax, [eax]
cmp eax, ebx
mov eax, 0
setg al
push eax

M80: pop eax
cmp eax, 0
je M94

M81: push offset entry_temp

M82: push offset entry_x

M83: push offset entry_y

M84: pop ebx
mov ebx, [ebx]
pop eax
mov eax, [eax]
sub eax, ebx
push eax

M85: pop eax
pop ebx
mov [ebx], eax
push eax

M86: push offset entry_temp

M87: pop eax
mov eax, [eax]
invoke printf, offset fmt_num_nl, eax

M88: push offset entry_x

M89: push offset entry_x

M90: push 1

M91: pop ebx
pop eax
mov eax, [eax]
sub eax, ebx
push eax

M92: pop eax
pop ebx
mov [ebx], eax
push eax

M93: jmp M77

M94: push offset L28_s

M95: pop eax
invoke printf, offset fmt_str_nl, eax

M96: push 0

M97: cmp ret_ptr, 0
je quit_program
dec ret_ptr
mov ebx, ret_ptr
mov eax, [ret_stack + ebx*4]
jmp eax

quit_program:
invoke ExitProcess, 0
main ENDP
end main
