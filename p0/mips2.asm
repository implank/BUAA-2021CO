.data
value: .space 20
.text
main:
	la $t0 value
	
	li $v0 5
	syscall
	move $t1 $v0
	
	li $t2 2
	beq $t1 1 chk

fori:
	beq $t2 $t1 don
	
	div $t3 $t1 $t2
	mul $t3 $t3 $t2
	sub $t3 $t1 $t3
	
	beq $t3 0 chk
	
	add $t2 $t2 1
	j fori
	
chk:
	li $a0 0
	li $v0 1
	syscall
	
	li $v0 10
	syscall

don:
	li $a0 1
	li $v0 1
	syscall
	
	li $v0 10
	syscall
