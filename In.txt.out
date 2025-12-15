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
	fmt_num db "%u", 0
	fmt_num_nl db "%u", 10, 0
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
	entry_testUint dd 2
	entry_testUintT dd 3
	L17 dd 3
	entry_testRes dd 4294967295
	entry_x dd 10
	entry_y dd 10
	entry_counter dd 110
	entry_msg_s db "10", 0
	entry_msg dd offset entry_msg_s
	entry_dateStr_s db "16.12.2025", 0
	entry_dateStr dd offset entry_dateStr_s
	L24 dd 100
	L25 dd 10
	entry_fDateNumb_s db "6", 0
	entry_fDateNumb dd offset entry_fDateNumb_s
	L27 dd 1
	entry_numb dd 6
	L29 dd 15
	L30_s db "Start Loop:", 0
	L30 dd offset L30_s
	entry_temp dd 1
	L32_s db "End", 0
	L32 dd offset L32_s
	L33 dd 0

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
mul ebx
push eax

M9: push 2

M10: pop ebx
pop eax
mov edx, 0
div ebx
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
mov edx, 0
div ebx
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

M27: push offset entry_testUint

M28: push 2

M29: pop eax
pop ebx
mov [ebx], eax
push eax

M30: push offset entry_testUintT

M31: push 3

M32: pop eax
pop ebx
mov [ebx], eax
push eax

M33: push offset entry_testRes

M34: push offset entry_testUint

M35: push offset entry_testUintT

M36: pop ebx
mov ebx, [ebx]
pop eax
mov eax, [eax]
sub eax, ebx
push eax

M37: pop eax
pop ebx
mov [ebx], eax
push eax

M38: push offset entry_testRes

M39: pop eax
mov eax, [eax]
invoke printf, offset fmt_num_nl, eax

M40: push offset entry_x

M41: push 100

M42: pop eax
pop ebx
mov [ebx], eax
push eax

M43: push offset entry_y

M44: push 10

M45: pop eax
pop ebx
mov [ebx], eax
push eax

M46: push offset entry_counter

M47: push offset entry_x

M48: push offset entry_y

M49: mov ebx, ret_ptr
mov dword ptr [ret_stack + ebx*4], offset Ret_M49
inc ret_ptr
jmp M1
Ret_M49:
push eax

M50: pop eax
pop ebx
mov [ebx], eax
push eax

M51: push offset entry_counter

M52: pop eax
mov eax, [eax]
invoke printf, offset fmt_num_nl, eax

M53: push offset entry_dateStr

M54: push offset date_buf

M55: pop eax
pop ebx
mov [ebx], eax
push eax

M56: push offset entry_dateStr

M57: pop eax
mov eax, [eax]
invoke printf, offset fmt_str_nl, eax

M58: push offset entry_x

M59: push offset entry_dateStr

M60: pop eax
mov eax, [eax]
invoke strlen, eax
push eax

M61: pop eax
pop ebx
mov [ebx], eax
push eax

M62: push offset entry_x

M63: pop eax
mov eax, [eax]
invoke printf, offset fmt_num_nl, eax

M64: push offset entry_msg

M65: push offset entry_y

M66: pop eax
mov eax, [eax]
invoke sprintf, offset common_buf, offset fmt_num, eax
push offset common_buf

M67: pop eax
pop ebx
mov [ebx], eax
push eax

M68: push offset entry_msg

M69: pop eax
mov eax, [eax]
invoke printf, offset fmt_str_nl, eax

M70: push offset entry_fDateNumb

M71: push offset entry_dateStr

M72: push 1

M73: push 1

M74: pop ecx
pop ebx
pop eax
mov eax, [eax]
invoke sub_str, eax, ebx, ecx
push eax

M75: pop eax
pop ebx
mov [ebx], eax
push eax

M76: push offset entry_numb

M77: push offset entry_fDateNumb

M78: pop eax
mov eax, [eax]
invoke touint, eax
push eax

M79: pop eax
pop ebx
mov [ebx], eax
push eax

M80: push offset entry_numb

M81: pop eax
mov eax, [eax]
invoke printf, offset fmt_num_nl, eax

M82: push offset entry_x

M83: push 15

M84: pop eax
pop ebx
mov [ebx], eax
push eax

M85: push offset entry_y

M86: push 10

M87: pop eax
pop ebx
mov [ebx], eax
push eax

M88: push offset L30_s

M89: pop eax
invoke printf, offset fmt_str_nl, eax

M90: push offset entry_x

M91: push offset entry_y

M92: pop ebx
mov ebx, [ebx]
pop eax
mov eax, [eax]
cmp eax, ebx
mov eax, 0
seta al
push eax

M93: pop eax
cmp eax, 0
je M107

M94: push offset entry_temp

M95: push offset entry_x

M96: push offset entry_y

M97: pop ebx
mov ebx, [ebx]
pop eax
mov eax, [eax]
sub eax, ebx
push eax

M98: pop eax
pop ebx
mov [ebx], eax
push eax

M99: push offset entry_temp

M100: pop eax
mov eax, [eax]
invoke printf, offset fmt_num_nl, eax

M101: push offset entry_x

M102: push offset entry_x

M103: push 1

M104: pop ebx
pop eax
mov eax, [eax]
sub eax, ebx
push eax

M105: pop eax
pop ebx
mov [ebx], eax
push eax

M106: jmp M90

M107: push offset L32_s

M108: pop eax
invoke printf, offset fmt_str_nl, eax

M109: push 0

M110: cmp ret_ptr, 0
je quit_program
dec ret_ptr
mov ebx, ret_ptr
mov eax, [ret_stack + ebx*4]
jmp eax

quit_program:
invoke ExitProcess, 0
main ENDP
end main
