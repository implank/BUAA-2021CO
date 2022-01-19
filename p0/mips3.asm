.data
value: .space 20
.text
main:
	la $t0 value
	
	li $v0 5
	syscall
	sw $v0 0($t0)
	lw $t1 0($t0)
	
	li $t2 400
	li $t3 100
	li $t4 4
	
	div $t5 $t1 $t2
	mul $t5 $t5 $t2
	sub $t2 $t1 $t5
	
	div $t5 $t1 $t3
	mul $t5 $t5 $t3
	sub $t3 $t1 $t5
	
	div $t5 $t1 $t4
	mul $t5 $t5 $t4
	sub $t4 $t1 $t5
	
	beq $t2 0 rrr1
	beq $t4 0 rrr2
	li $a0 0
	li $v0 1
	syscall
	
	li $v0 10
	syscall
	
	
rrr1:
	li $a0 1
	li $v0 1
	syscall
	
	li $v0 10
	syscall
	
rrr2:
	bne $t3 0 rrr3
	li $a0 0
	li $v0 1
	syscall
	
	li $v0 10
	syscall

rrr3:
	li $a0 1
	li $v0 1
	syscall
	
	li $v0 10
	syscall
