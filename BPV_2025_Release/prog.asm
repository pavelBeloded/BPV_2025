.586
.model flat, stdcall
option casemap :none

; --- Libs ---
includelib kernel32.lib
includelib libucrt.lib

; --- WinAPI ---
ExitProcess PROTO :DWORD
GetStdHandle PROTO :DWORD
WriteConsoleA PROTO :DWORD, :DWORD, :DWORD, :DWORD, :DWORD

; --- User Lib ---
str_len PROTO :DWORD
tostr PROTO :DWORD
touint PROTO :DWORD
date PROTO
get_time PROTO
sub_str PROTO :DWORD, :DWORD, :DWORD

.stack 4096

.data
	console_handle dd 0
	newline db 13, 10, 0
	buffer db 256 dup(0)
	entry$num dd 2020
	L7 dd 0
	entry$str_s db "Hello World", 0
	entry$str dd entry$str_s
	L9_s db "Hello World", 0
	L9 dd L9_s
	entry$temp_s_s db "55", 0
	entry$temp_s dd entry$temp_s_s
	entry$temp_n dd 60
	entry$d_s db "09.12.2025", 0
	entry$d dd entry$d_s
	entry$t_s db "15:29:36", 0
	entry$t dd entry$t_s
	L14_s db "--- Standard Library Test ---", 0
	L14 dd L14_s
	L15 dd 55
	L16_s db "Date:", 0
	L16 dd L16_s
	L17_s db "hello karina", 0
	L17 dd L17_s
	L18_s db "Time:", 0
	L18 dd L18_s
	L19_s db "Length of 'Hello World':", 0
	L19 dd L19_s
	L20 dd 6
	L21 dd 5
	L22_s db "Substring result:", 0
	L22 dd L22_s
	L23 dd 2025
	L24_s db "Converted number to string:", 0
	L24 dd L24_s
	L25_s db "55", 0
	L25 dd L25_s
	L26_s db "Converted string '55' + 5 = ", 0
	L26 dd L26_s
	L27_s db "--- Logic Test ---", 0
	L27 dd L27_s
	L28 dd 2020
	L29 dd 1

.code

print_str PROC uses eax ebx ecx edx, pstr:DWORD
	push -11
	call GetStdHandle
	mov console_handle, eax
	mov edx, pstr
	xor ecx, ecx
calc_len:
	cmp byte ptr [edx+ecx], 0
	je print_now
	inc ecx
	jmp calc_len
print_now:
	push 0
	push 0
	push ecx
	push pstr
	push console_handle
	call WriteConsoleA
	ret
print_str ENDP

outnum PROC uses eax ebx ecx edx, number:DWORD
	mov eax, number
	mov ecx, 0
	mov ebx, 10
div_loop:
	xor edx, edx
	div ebx
	add dl, '0'
	push edx
	inc ecx
	test eax, eax
	jnz div_loop
print_loop:
	pop eax
	mov buffer[0], al
	push ecx
	invoke print_str, addr buffer
	pop ecx
	loop print_loop
	ret
outnum ENDP

print_newline PROC
	invoke print_str, addr newline
	ret
print_newline ENDP

main PROC
M0: jmp M1

M1: push offset entry$num

M2: push 0

M3: pop eax
pop ebx
mov [ebx], eax
push eax

M4: push offset entry$str

M5: push L9

M6: pop eax
pop ebx
mov [ebx], eax
push eax

M7: push L14

M8: pop eax
invoke print_str, eax
invoke print_newline

M9: push 55

M10: pop eax
invoke outnum, eax
invoke print_newline

M11: push offset entry$d

M12: call date
push eax

M13: pop eax
pop ebx
mov [ebx], eax
push eax

M14: push L16

M15: pop eax
invoke print_str, eax
invoke print_newline

M16: push offset entry$d

M17: pop eax
mov eax, [eax]
invoke print_str, eax
invoke print_newline

M18: push L17

M19: pop eax
invoke print_str, eax
invoke print_newline

M20: push offset entry$t

M21: call get_time
push eax

M22: pop eax
pop ebx
mov [ebx], eax
push eax

M23: push L18

M24: pop eax
invoke print_str, eax
invoke print_newline

M25: push offset entry$t

M26: pop eax
mov eax, [eax]
invoke print_str, eax
invoke print_newline

M27: push offset entry$temp_n

M28: push offset entry$str

M29: pop eax
mov eax, [eax]
push eax
call str_len
push eax

M30: pop eax
pop ebx
mov [ebx], eax
push eax

M31: push L19

M32: pop eax
invoke print_str, eax
invoke print_newline

M33: push offset entry$temp_n

M34: pop eax
mov eax, [eax]
invoke outnum, eax
invoke print_newline

M35: push offset entry$temp_s

M36: push offset entry$str

M37: push 6

M38: push 5

M39: pop ecx
pop ebx
pop eax
mov eax, [eax]
push ecx
push ebx
push eax
call sub_str
push eax

M40: pop eax
pop ebx
mov [ebx], eax
push eax

M41: push L22

M42: pop eax
invoke print_str, eax
invoke print_newline

M43: push offset entry$temp_s

M44: pop eax
mov eax, [eax]
invoke print_str, eax
invoke print_newline

M45: push offset entry$num

M46: push 2025

M47: pop eax
pop ebx
mov [ebx], eax
push eax

M48: push offset entry$temp_s

M49: push offset entry$num

M50: pop eax
mov eax, [eax]
push eax
call tostr
push eax

M51: pop eax
pop ebx
mov [ebx], eax
push eax

M52: push L24

M53: pop eax
invoke print_str, eax
invoke print_newline

M54: push offset entry$temp_s

M55: pop eax
mov eax, [eax]
invoke print_str, eax
invoke print_newline

M56: push offset entry$temp_s

M57: push L25

M58: pop eax
pop ebx
mov [ebx], eax
push eax

M59: push offset entry$temp_n

M60: push offset entry$temp_s

M61: pop eax
mov eax, [eax]
push eax
call touint
push eax

M62: pop eax
pop ebx
mov [ebx], eax
push eax

M63: push offset entry$temp_n

M64: push offset entry$temp_n

M65: push 5

M66: pop ebx
pop eax
mov eax, [eax]
add eax, ebx
push eax

M67: pop eax
pop ebx
mov [ebx], eax
push eax

M68: push L26

M69: pop eax
invoke print_str, eax
invoke print_newline

M70: push offset entry$temp_n

M71: pop eax
mov eax, [eax]
invoke outnum, eax
invoke print_newline

M72: push L27

M73: pop eax
invoke print_str, eax
invoke print_newline

M74: push offset entry$num

M75: push 2020

M76: pop ebx
pop eax
mov eax, [eax]
cmp eax, ebx
mov eax, 0
setg al
push eax

M77: pop eax
cmp eax, 0
je M86

M78: push offset entry$num

M79: push offset entry$num

M80: push 1

M81: pop ebx
pop eax
mov eax, [eax]
sub eax, ebx
push eax

M82: pop eax
pop ebx
mov [ebx], eax
push eax

M83: push offset entry$num

M84: pop eax
mov eax, [eax]
invoke outnum, eax
invoke print_newline

M85: jmp M74

M86: push 0

M87: push 0
call ExitProcess

main ENDP
end main
