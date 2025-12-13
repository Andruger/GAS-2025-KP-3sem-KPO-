.586
.model flat, stdcall
includelib libucrt.lib
includelib kernel32.lib
includelib ../Debug/StaticLibrary.lib
ExitProcess PROTO:DWORD 
.stack 4096


 outlich PROTO : DWORD

 outrad PROTO : DWORD

 concat PROTO : DWORD, : DWORD, : DWORD

 compare PROTO : DWORD, : DWORD, : DWORD

 rnd PROTO : DWORD, : DWORD, : DWORD

 slength PROTO : DWORD, : DWORD

 aton  PROTO : DWORD,  : DWORD

.const
		newline byte 13, 10, 0
		LTRL1 byte 'Result: ', 0
		LTRL2 sword 5
		LTRL3 sword 8
		LTRL4 byte 'Number at sixteen format: ', 0
		LTRL5 sword 58
		LTRL6 byte '----------------------', 0
		LTRL7 sword 2
		LTRL8 sword 4
		LTRL9 sword -4
		LTRL10 byte 'Result of division: ', 0
		LTRL11 byte '(', 0
		LTRL12 byte '>=', 0
		LTRL13 byte ') :', 0
		LTRL14 byte 'True', 0
		LTRL15 byte 'False', 0
		LTRL16 byte 'H', 0
		LTRL17 byte 'i', 0
		LTRL18 byte '!', 0
		LTRL19 byte 'Compare: ', 0
		LTRL20 sword 1
		LTRL21 byte 'The lines are the same', 0
		LTRL22 byte 'The lines are not the same', 0
		LTRL23 byte 'Random number: ', 0
		LTRL24 sword 10
		LTRL25 sword 7
		LTRL26 byte 'Arithmetic expression:  ', 0
		LTRL27 byte '123', 0
		LTRL28 byte 'Converting a string to a number: ', 0
		LTRL29 byte 'I love BSTU', 0
		LTRL30 byte 'Line length: ', 0
		LTRL31 sword 56
		LTRL32 sword 9
		LTRL33 byte 'Remainder after division 56 by 9: ', 0
		LTRL34 byte 'Andrey ', 0
		LTRL35 byte 'Gerasimovich', 0
		LTRL36 sword 6
		LTRL37 byte 'Error', 0
		LTRL38 byte 'Cycle  to 10: ', 0
		LTRL39 byte ' ', 0
		LTRL40 sword 0
.data
		temp sword ?
		buffer byte 256 dup(0)
		maxres sword 0
		standstr dword 0
		mainsix sword 0
		mainstr dword 0
		mainabc sword 0
		mainabcd sword 0
		mainf sword 0
		mains sword 0
		mainfinish sword 0
		standmh dword 0
		standmi dword 0
		standmiv dword 0
		standdsa dword 0
		standytr dword 0
		standasd sword 0
		standu sword 0
		standv sword 0
		standr sword 0
		standc dword 0
		standk sword 0
		standlen dword 0
		standnumb sword 0
		standremainder sword 0
		standname dword 0
		standsurname dword 0
		standpoi sword 0
		standisi sword 0
		standab sword 0
.code

;----------- max ------------
max PROC,
	maxx : DWORD, maxy : DWORD  
; --- save registers ---
push ebx
push edx
; ----------------------
mov dx, word ptr maxx
cmp dx, word ptr maxy

jle wrong1
right1:
movsx eax, word ptr maxx
push eax

pop ebx
mov word ptr maxres, bx

jmp next1

wrong1:
movsx eax, word ptr maxy
push eax

pop ebx
mov word ptr maxres, bx

next1:
; --- restore registers ---
pop edx
pop ebx
; -------------------------
mov ax, word ptr maxres
ret 8
max ENDP
;------------------------------


;----------- stand ------------
stand PROC,
	standa : DWORD, standb : DWORD  
; --- save registers ---
push ebx
push edx
; ----------------------

push standb
push standa
push offset buffer
call concat
mov standstr, eax

push offset LTRL1
call outrad


push standstr
call outrad

push offset newline
call outrad

; --- restore registers ---
pop edx
pop ebx
; -------------------------
ret 8
stand ENDP
;------------------------------


;----------- MAIN ------------
main PROC

movsx eax, word ptr LTRL3
push eax
movsx eax, word ptr LTRL2
push eax
call max
cwde
push eax
call outlich


push offset LTRL4
call outrad

movsx eax, word ptr LTRL5
push eax

pop ebx
mov word ptr mainsix, bx


movsx eax, word ptr mainsix
push eax
call outlich

push offset newline
call outrad


push offset LTRL6
call outrad

push offset newline
call outrad

movsx eax, word ptr LTRL7
push eax

pop ebx
mov word ptr mainabc, bx

movsx eax, word ptr LTRL8
push eax

pop ebx
mov word ptr mainabcd, bx

movsx eax, word ptr LTRL9
push eax

pop ebx
mov word ptr mainf, bx

movsx eax, word ptr LTRL7
push eax

pop ebx
mov word ptr mains, bx

movsx eax, word ptr mainf
push eax
movsx eax, word ptr mains
push eax
pop ebx
pop eax
cdq
idiv ebx
push eax

pop ebx
mov word ptr mainfinish, bx


push offset LTRL10
call outrad


movsx eax, word ptr mainfinish
push eax
call outlich

push offset newline
call outrad


push offset LTRL6
call outrad

push offset newline
call outrad


push offset LTRL11
call outrad


movsx eax, word ptr mainabc
push eax
call outlich


push offset LTRL12
call outrad


movsx eax, word ptr mainabcd
push eax
call outlich


push offset LTRL13
call outrad

mov dx, word ptr mainabc
cmp dx, word ptr mainabcd

jl wrong1
right1:
mov standstr, offset LTRL14
jmp next1

wrong1:
mov standstr, offset LTRL15
next1:

push standstr
call outrad

push offset newline
call outrad


push offset LTRL6
call outrad

push offset newline
call outrad

mov standmh, offset LTRL16
mov standmi, offset LTRL17
mov standmiv, offset LTRL18

push standmh
call outrad


push standmi
call outrad


push standmiv
call outrad

push offset newline
call outrad


push offset LTRL6
call outrad

push offset newline
call outrad

mov standdsa, offset LTRL15
mov standytr, offset LTRL15

push offset LTRL19
call outrad


push standytr
push standdsa
push offset buffer
call compare
cwde
push eax

pop ebx
mov word ptr standasd, bx

mov dx, word ptr standasd
cmp dx, word ptr LTRL20

jnz wrong2
right2:

push offset LTRL21
call outrad

jmp next2

wrong2:

push offset LTRL22
call outrad

next2:
push offset newline
call outrad


push offset LTRL6
call outrad

push offset newline
call outrad


push offset LTRL23
call outrad


movsx eax, word ptr LTRL24
push eax
movsx eax, word ptr LTRL20
push eax
push offset buffer
call rnd
cwde
push eax
call outlich

push offset newline
call outrad


push offset LTRL6
call outrad

push offset newline
call outrad

movsx eax, word ptr LTRL2
push eax

pop ebx
mov word ptr standu, bx

movsx eax, word ptr LTRL25
push eax

pop ebx
mov word ptr standv, bx


push offset LTRL26
call outrad

movsx eax, word ptr standu
push eax
movsx eax, word ptr standv
push eax
movsx eax, word ptr LTRL7
push eax
pop ebx
pop eax
imul eax, ebx
push eax
pop ebx
pop eax
add eax, ebx
push eax
movsx eax, word ptr LTRL8
push eax
movsx eax, word ptr LTRL8
push eax
pop ebx
pop eax
add eax, ebx
push eax
movsx eax, word ptr LTRL7
push eax
pop ebx
pop eax
cdq
idiv ebx
push eax
pop ebx
pop eax
sub eax, ebx
push eax

movsx eax, word ptr LTRL3
push eax
movsx eax, word ptr LTRL2
push eax
call max
cwde
push eax
pop ebx
pop eax
add eax, ebx
push eax

pop ebx
mov word ptr standr, bx


movsx eax, word ptr standr
push eax
call outlich

push offset newline
call outrad


push offset LTRL6
call outrad

push offset newline
call outrad

mov standc, offset LTRL27

push offset LTRL28
call outrad


push standc
push offset buffer
call aton
cwde
push eax
call outlich

push offset newline
call outrad


push offset LTRL6
call outrad

push offset newline
call outrad

mov standlen, offset LTRL29

push offset LTRL30
call outrad


push standlen
push offset buffer
call slength
cwde
push eax
call outlich

push offset newline
call outrad


push offset LTRL6
call outrad

push offset newline
call outrad

movsx eax, word ptr LTRL31
push eax

pop ebx
mov word ptr standnumb, bx

movsx eax, word ptr standnumb
push eax
movsx eax, word ptr LTRL32
push eax
pop ebx
pop eax
cdq
idiv ebx
push edx

pop ebx
mov word ptr standremainder, bx


push offset LTRL33
call outrad


movsx eax, word ptr standremainder
push eax
call outlich

push offset newline
call outrad


push offset LTRL6
call outrad

push offset newline
call outrad

mov standname, offset LTRL34
mov standsurname, offset LTRL35
movsx eax, word ptr LTRL36
push eax

pop ebx
mov word ptr standpoi, bx

movsx eax, word ptr LTRL8
push eax

pop ebx
mov word ptr standisi, bx

mov dx, word ptr standpoi
cmp dx, word ptr standisi

jg wrong3
right3:

push standsurname
push standname
call stand

jmp next3

wrong3:

push offset LTRL37
call outrad

next3:

push offset LTRL6
call outrad

push offset newline
call outrad

movsx eax, word ptr LTRL7
push eax

pop ebx
mov word ptr standab, bx


push offset LTRL38
call outrad

do4:

movsx eax, word ptr standab
push eax
call outlich


push offset LTRL39
call outrad

movsx eax, word ptr standab
push eax
movsx eax, word ptr LTRL7
push eax
pop ebx
pop eax
add eax, ebx
push eax

pop ebx
mov word ptr standab, bx

jmp donext4
donext4:

movsx eax, word ptr standab
push eax
call outlich

push offset newline
call outrad

push 0
call ExitProcess
main ENDP
end main
