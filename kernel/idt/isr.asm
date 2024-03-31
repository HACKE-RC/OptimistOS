section .text
global load_idt

load_idt:
    mov rax, rdi
    lidt [rax]
    sti
    ret

%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popaq 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

extern excp_handler

_int_stub:
	pushaq
	cld
	call excp_handler
	popaq
	add rsp, 16
	iretq

%macro _isr_noerr 1
isr_%+%1:
	cli
	push 0
	push %1
	jmp _int_stub
%endmacro

%macro _isr_err 1
isr_%+%1:
	cli
	push %1
	jmp _int_stub
%endmacro

_isr_noerr 0
_isr_noerr 1
_isr_noerr 2
_isr_noerr 3
_isr_noerr 4
_isr_noerr 5
_isr_noerr 6
_isr_noerr 7
_isr_err 8
_isr_noerr 9
_isr_err 10
_isr_err 11
_isr_err 12
_isr_err 13
_isr_err 14
_isr_noerr 15
_isr_noerr 16
_isr_err 17
_isr_noerr 18
_isr_noerr 19
_isr_noerr 20
_isr_noerr 21
_isr_noerr 22
_isr_noerr 23
_isr_noerr 24
_isr_noerr 25
_isr_noerr 26
_isr_noerr 27
_isr_noerr 28
_isr_noerr 29
_isr_err 30
_isr_noerr 31

_isr_noerr 32
_isr_noerr 33
_isr_noerr 34
_isr_noerr 35
_isr_noerr 36
_isr_noerr 37
_isr_noerr 38
_isr_noerr 39
_isr_noerr 40
_isr_noerr 41
_isr_noerr 42
_isr_noerr 43
_isr_noerr 44
_isr_noerr 45
_isr_noerr 46
_isr_noerr 47
_isr_noerr 48
_isr_noerr 49
_isr_noerr 50
_isr_noerr 51
_isr_noerr 52
_isr_noerr 53
_isr_noerr 54
_isr_noerr 55
_isr_noerr 56
_isr_noerr 57
_isr_noerr 58
_isr_noerr 59
_isr_noerr 60
_isr_noerr 61
_isr_noerr 62
_isr_noerr 63
_isr_noerr 64
_isr_noerr 65
_isr_noerr 66
_isr_noerr 67
_isr_noerr 68
_isr_noerr 69
_isr_noerr 70
_isr_noerr 71
_isr_noerr 72
_isr_noerr 73
_isr_noerr 74
_isr_noerr 75
_isr_noerr 76
_isr_noerr 77
_isr_noerr 78
_isr_noerr 79
_isr_noerr 80
_isr_noerr 81
_isr_noerr 82
_isr_noerr 83
_isr_noerr 84
_isr_noerr 85
_isr_noerr 86
_isr_noerr 87
_isr_noerr 88
_isr_noerr 89
_isr_noerr 90
_isr_noerr 91
_isr_noerr 92
_isr_noerr 93
_isr_noerr 94
_isr_noerr 95
_isr_noerr 96
_isr_noerr 97
_isr_noerr 98
_isr_noerr 99
_isr_noerr 100
_isr_noerr 101
_isr_noerr 102
_isr_noerr 103
_isr_noerr 104
_isr_noerr 105
_isr_noerr 106
_isr_noerr 107
_isr_noerr 108
_isr_noerr 109
_isr_noerr 110
_isr_noerr 111
_isr_noerr 112
_isr_noerr 113
_isr_noerr 114
_isr_noerr 115
_isr_noerr 116
_isr_noerr 117
_isr_noerr 118
_isr_noerr 119
_isr_noerr 120
_isr_noerr 121
_isr_noerr 122
_isr_noerr 123
_isr_noerr 124
_isr_noerr 125
_isr_noerr 126
_isr_noerr 127
_isr_noerr 128
_isr_noerr 129
_isr_noerr 130
_isr_noerr 131
_isr_noerr 132
_isr_noerr 133
_isr_noerr 134
_isr_noerr 135
_isr_noerr 136
_isr_noerr 137
_isr_noerr 138
_isr_noerr 139
_isr_noerr 140
_isr_noerr 141
_isr_noerr 142
_isr_noerr 143
_isr_noerr 144
_isr_noerr 145
_isr_noerr 146
_isr_noerr 147
_isr_noerr 148
_isr_noerr 149
_isr_noerr 150
_isr_noerr 151
_isr_noerr 152
_isr_noerr 153
_isr_noerr 154
_isr_noerr 155
_isr_noerr 156
_isr_noerr 157
_isr_noerr 158
_isr_noerr 159
_isr_noerr 160
_isr_noerr 161
_isr_noerr 162
_isr_noerr 163
_isr_noerr 164
_isr_noerr 165
_isr_noerr 166
_isr_noerr 167
_isr_noerr 168
_isr_noerr 169
_isr_noerr 170
_isr_noerr 171
_isr_noerr 172
_isr_noerr 173
_isr_noerr 174
_isr_noerr 175
_isr_noerr 176
_isr_noerr 177
_isr_noerr 178
_isr_noerr 179
_isr_noerr 180
_isr_noerr 181
_isr_noerr 182
_isr_noerr 183
_isr_noerr 184
_isr_noerr 185
_isr_noerr 186
_isr_noerr 187
_isr_noerr 188
_isr_noerr 189
_isr_noerr 190
_isr_noerr 191
_isr_noerr 192
_isr_noerr 193
_isr_noerr 194
_isr_noerr 195
_isr_noerr 196
_isr_noerr 197
_isr_noerr 198
_isr_noerr 199
_isr_noerr 200
_isr_noerr 201
_isr_noerr 202
_isr_noerr 203
_isr_noerr 204
_isr_noerr 205
_isr_noerr 206
_isr_noerr 207
_isr_noerr 208
_isr_noerr 209
_isr_noerr 210
_isr_noerr 211
_isr_noerr 212
_isr_noerr 213
_isr_noerr 214
_isr_noerr 215
_isr_noerr 216
_isr_noerr 217
_isr_noerr 218
_isr_noerr 219
_isr_noerr 220
_isr_noerr 221
_isr_noerr 222
_isr_noerr 223
_isr_noerr 224
_isr_noerr 225
_isr_noerr 226
_isr_noerr 227
_isr_noerr 228
_isr_noerr 229
_isr_noerr 230
_isr_noerr 231
_isr_noerr 232
_isr_noerr 233
_isr_noerr 234
_isr_noerr 235
_isr_noerr 236
_isr_noerr 237
_isr_noerr 238
_isr_noerr 239
_isr_noerr 240
_isr_noerr 241
_isr_noerr 242
_isr_noerr 243
_isr_noerr 244
_isr_noerr 245
_isr_noerr 246
_isr_noerr 247
_isr_noerr 248
_isr_noerr 249
_isr_noerr 250
_isr_noerr 251
_isr_noerr 252
_isr_noerr 253
_isr_noerr 254
_isr_noerr 255

section .data

global isr_tbl
isr_tbl:
%assign i 0
%rep 256
	dq isr_%+i
%assign i i+1
%endrep
