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
	entry$d_s db "10.12.2025", 0
	entry$d dd entry$d_s
	entry$t_s db "19:13:02", 0
	entry$t dd entry$t_s
	L14_s db "--- Standard Library Test ---", 0
	L14 dd L14_s
	L15 dd 55
	L16_s db "Date:", 0
	L16 dd L16_s
	L17_s db "Time:", 0
	L17 dd L17_s
	L18_s db "Length of 'Hello World':", 0
	L18 dd L18_s
	L19 dd 6
	L20 dd 5
	L21_s db "Substring result:", 0
	L21 dd L21_s
	L22 dd 2025
	L23_s db "Converted number to string:", 0
	L23 dd L23_s
	L24_s db "55", 0
	L24 dd L24_s
	L25_s db "Converted string '55' + 5 = ", 0
	L25 dd L25_s
	L26_s db "--- Logic Test ---", 0
	L26 dd L26_s
	L27 dd 2020
	L28 dd 1

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

M18: push offset entry$t

M19: call get_time
push eax

M20: pop eax
pop ebx
mov [ebx], eax
push eax

M21: push L17

M22: pop eax
invoke print_str, eax
invoke print_newline

M23: push offset entry$t

M24: pop eax
mov eax, [eax]
invoke print_str, eax
invoke print_newline

M25: push offset entry$temp_n

M26: push offset entry$str

M27: pop eax
mov eax, [eax]
push eax
call str_len
push eax

M28: pop eax
pop ebx
mov [ebx], eax
push eax

M29: push L18

M30: pop eax
invoke print_str, eax
invoke print_newline

M31: push offset entry$temp_n

M32: pop eax
mov eax, [eax]
invoke outnum, eax
invoke print_newline

M33: push offset entry$temp_s

M34: push offset entry$str

M35: push 6

M36: push 5

M37: pop ecx
pop ebx
pop eax
mov eax, [eax]
push ecx
push ebx
push eax
call sub_str
push eax

M38: pop eax
pop ebx
mov [ebx], eax
push eax

M39: push L21

M40: pop eax
invoke print_str, eax
invoke print_newline

M41: push offset entry$temp_s

M42: pop eax
mov eax, [eax]
invoke print_str, eax
invoke print_newline

M43: push offset entry$num

M44: push 2025

M45: pop eax
pop ebx
mov [ebx], eax
push eax

M46: push offset entry$temp_s

M47: push offset entry$num

M48: pop eax
mov eax, [eax]
push eax
call tostr
push eax

M49: pop eax
pop ebx
mov [ebx], eax
push eax

M50: push L23

M51: pop eax
invoke print_str, eax
invoke print_newline

M52: push offset entry$temp_s

M53: pop eax
mov eax, [eax]
invoke print_str, eax
invoke print_newline

M54: push offset entry$temp_s

M55: push L24

M56: pop eax
pop ebx
mov [ebx], eax
push eax

M57: push offset entry$temp_n

M58: push offset entry$temp_s

M59: pop eax
mov eax, [eax]
push eax
call touint
push eax

M60: pop eax
pop ebx
mov [ebx], eax
push eax

M61: push offset entry$temp_n

M62: push offset entry$temp_n

M63: push 5

M64: pop ebx
pop eax
mov eax, [eax]
add eax, ebx
push eax

M65: pop eax
pop ebx
mov [ebx], eax
push eax

M66: push L25

M67: pop eax
invoke print_str, eax
invoke print_newline

M68: push offset entry$temp_n

M69: pop eax
mov eax, [eax]
invoke outnum, eax
invoke print_newline

M70: push L26

M71: pop eax
invoke print_str, eax
invoke print_newline

M72: push offset entry$num

M73: push 2020

M74: pop ebx
pop eax
mov eax, [eax]
cmp eax, ebx
mov eax, 0
setg al
push eax

M75: pop eax
cmp eax, 0
je M84

M76: push offset entry$num

M77: push offset entry$num

M78: push 1

M79: pop ebx
pop eax
mov eax, [eax]
sub eax, ebx
push eax

M80: pop eax
pop ebx
mov [ebx], eax
push eax

M81: push offset entry$num

M82: pop eax
mov eax, [eax]
invoke outnum, eax
invoke print_newline

M83: jmp M72

M84: push 0

M85: push 0
call ExitProcess

main ENDP
end main
