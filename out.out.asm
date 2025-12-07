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
length PROTO :DWORD
tostr PROTO :DWORD
touint PROTO :DWORD
date PROTO
get_time PROTO
substr PROTO :DWORD, :DWORD, :DWORD

.stack 4096

.data
	console_handle dd 0
	newline db 13, 10, 0
	buffer db 256 dup(0)
	entry$num dd 2020
	L7 dd 0
	entry$str db "Hello World", 0
	L9 db "Hello World", 0
	entry$temp_s db "55", 0
	entry$temp_n dd 60
	entry$d db "07.12.2025", 0
	entry$t db "22:39:06", 0
	L14 dd 8
	L15 db "--- Standard Library Test ---", 0
	L16 db "Date:", 0
	L17 db "Time:", 0
	L18 db "Length of 'Hello World':", 0
	L19 dd 6
	L20 dd 5
	L21 db "Substring result:", 0
	L22 dd 2025
	L23 db "Converted number to string:", 0
	L24 db "55", 0
	L25 db "Converted string '55' + 5 = ", 0
	L26 db "--- Logic Test ---", 0
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

outnum_oct PROC uses eax ebx ecx edx, number:DWORD
	mov eax, number
	mov ecx, 0
	mov ebx, 8
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
outnum_oct ENDP

print_newline PROC
	invoke print_str, addr newline
	ret
print_newline ENDP

main PROC
L0: jmp L1

L1: push offset entry$num

L2: push 0

L3: pop eax
pop ebx
mov [ebx], eax
push eax

L4: push offset entry$str

L5: push offset L9

L6: pop eax
pop ebx
mov [ebx], eax
push eax

L7: push 8

L8: pop eax
invoke outnum_oct, eax
invoke print_newline

L9: push offset L15

L10: pop eax
invoke print_str, eax
invoke print_newline

L11: push offset entry$d

L12: call date

L13: pop eax
pop ebx
mov [ebx], eax
push eax

L14: push offset L16

L15: pop eax
invoke print_str, eax
invoke print_newline

L16: push offset entry$d

L17: pop eax
invoke print_str, eax
invoke print_newline

L18: push offset entry$t

L19: call get_time

L20: pop eax
pop ebx
mov [ebx], eax
push eax

L21: push offset L17

L22: pop eax
invoke print_str, eax
invoke print_newline

L23: push offset entry$t

L24: pop eax
invoke print_str, eax
invoke print_newline

L25: push offset entry$temp_n

L26: push offset entry$str

L27: call length

L28: pop eax
pop ebx
mov [ebx], eax
push eax

L29: push offset L18

L30: pop eax
invoke print_str, eax
invoke print_newline

L31: push offset entry$temp_n

L32: pop eax
mov eax, [eax]
invoke outnum_oct, eax
invoke print_newline

L33: push offset entry$temp_s

L34: push offset entry$str

L35: push 6

L36: push 5

L37: call substr

L38: pop eax
pop ebx
mov [ebx], eax
push eax

L39: push offset L21

L40: pop eax
invoke print_str, eax
invoke print_newline

L41: push offset entry$temp_s

L42: pop eax
invoke print_str, eax
invoke print_newline

L43: push offset entry$num

L44: push 2025

L45: pop eax
pop ebx
mov [ebx], eax
push eax

L46: push offset entry$temp_s

L47: push offset entry$num

L48: call tostr

L49: pop eax
pop ebx
mov [ebx], eax
push eax

L50: push offset L23

L51: pop eax
invoke print_str, eax
invoke print_newline

L52: push offset entry$temp_s

L53: pop eax
invoke print_str, eax
invoke print_newline

L54: push offset entry$temp_s

L55: push offset L24

L56: pop eax
pop ebx
mov [ebx], eax
push eax

L57: push offset entry$temp_n

L58: push offset entry$temp_s

L59: call touint

L60: pop eax
pop ebx
mov [ebx], eax
push eax

L61: push offset entry$temp_n

L62: push offset entry$temp_n

L63: push 5

L64: pop ebx
pop eax
mov eax, [eax]
add eax, ebx
push eax

L65: pop eax
pop ebx
mov [ebx], eax
push eax

L66: push offset L25

L67: pop eax
invoke print_str, eax
invoke print_newline

L68: push offset entry$temp_n

L69: pop eax
mov eax, [eax]
invoke outnum_oct, eax
invoke print_newline

L70: push offset L26

L71: pop eax
invoke print_str, eax
invoke print_newline

L72: push offset entry$num

L73: push 2020

L74: pop ebx
pop eax
mov eax, [eax]
cmp eax, ebx
mov eax, 0
setg al
push eax

L75: pop eax
cmp eax, 0
je L84

L76: push offset entry$num

L77: push offset entry$num

L78: push 1

L79: pop ebx
pop eax
mov eax, [eax]
sub eax, ebx
push eax

L80: pop eax
pop ebx
mov [ebx], eax
push eax

L81: push offset entry$num

L82: pop eax
mov eax, [eax]
invoke outnum_oct, eax
invoke print_newline

L83: jmp L72

L84: push 0

L85: push 0
call ExitProcess

main ENDP
end main
