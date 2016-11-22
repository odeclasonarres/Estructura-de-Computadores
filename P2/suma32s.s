.section .data
	.macro linea 
		.int -1,-1,-1,-1
		#.int 1,-2,1,-2
		#.int 1,2,-3,-4
		#.int -1,-1,-1,-1
		#.int 0x7fffffff,0x7fffffff,0x7fffffff,0x7fffffff
		#.int 0x80000000,0x80000000,0x80000000,0x80000000
		.int 0x08000000,0x08000000,0x08000000,0x08000000		
		#.int 0x04000000,0x04000000,0x04000000,0x04000000		
		#.int 0xfc000000,0xfc000000,0xfc000000,0xfc000000
                #.int 0xf8000000,0xf8000000,0xf8000000,0xf8000000
		#.int 0xf0000000,0xe0000000,0xe0000000,0xd0000000	
	.endm
lista:		.irpc i,12345678
			linea
		.endr
		
#lista: 
#	.int 1,-1
	
longlista:	.int (.-lista)/4
resultado:	.quad 0x012345678ABCDEF
formato: 	.ascii "suma = %lld = %llx hex\n\0" #formato para 64 bits


.section .text
#_start:	.global _start
main: .global main

	mov    $lista, %ebx
	mov longlista, %ecx
	call suma
	mov %eax, resultado
	mov %edx, resultado+4
	
	push resultado+4
	push resultado
	push resultado+4
	push resultado
	push $formato
	call printf
	add $20, %esp #retrocedes en la pila hasta llegar a las direcciones anteriores a meterle el formato

	mov $1, %eax
	mov $0, %ebx
	int $0x80

suma:
	mov $0, %edi
	mov $0, %ebp
	mov $0, %esi
bucle:
	mov (%ebx,%esi,4), %eax
	cltd  
	add %eax, %edi
	adc %edx, %ebp
	inc %esi
	cmp  %esi,%ecx
	jne bucle

	mov %edi, %eax
	mov %ebp, %edx
	ret
