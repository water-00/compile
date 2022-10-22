	.arch armv7-a
	.fpu vfpv3-d16
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"main.c"
	.text
	.align	1
	.global	fibonacci
	.syntax unified
	.thumb
	.thumb_func
	.type	fibonacci, %function
fibonacci:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #24
	add	r7, sp, #0
	str	r0, [r7, #4]
	movs	r3, #0
	str	r3, [r7, #8]
	movs	r3, #1
	str	r3, [r7, #12]
	movs	r3, #1
	str	r3, [r7, #16]
	ldr	r0, [r7, #12]
	bl	putint(PLT)
	movs	r0, #10
	bl	putchar(PLT)
	b	.L2
.L3:
	ldr	r2, [r7, #8]
	ldr	r3, [r7, #12]
	add	r3, r3, r2
	str	r3, [r7, #20]
	ldr	r0, [r7, #20]
	bl	putint(PLT)
	movs	r0, #10
	bl	putchar(PLT)
	ldr	r3, [r7, #12]
	str	r3, [r7, #8]
	ldr	r3, [r7, #20]
	str	r3, [r7, #12]
	ldr	r3, [r7, #16]
	adds	r3, r3, #1
	str	r3, [r7, #16]
.L2:
	ldr	r2, [r7, #16]
	ldr	r3, [r7, #4]
	cmp	r2, r3
	blt	.L3
	nop
	nop
	adds	r7, r7, #24
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
	.size	fibonacci, .-fibonacci
	.align	1
	.global	main
	.syntax unified
	.thumb
	.thumb_func
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #8
	add	r7, sp, #0
	bl	getint(PLT)
	str	r0, [r7, #4]
	ldr	r3, [r7, #4]
	cmp	r3, #0
	ble	.L5
	ldr	r0, [r7, #4]
	bl	fibonacci(PLT)
.L5:
	movs	r3, #0
	mov	r0, r3
	adds	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.2.0-17ubuntu1) 11.2.0"
	.section	.note.GNU-stack,"",%progbits
