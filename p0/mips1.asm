.data
value: .space 20
.text
main:
	la $t0 value
	
	li $v0 5
	syscall
	sw $v0 0($t0)
	lw $t1 0($t0)
	
	li $v0 5
	syscall
	sw $v0 4($t0)
	lw $t2 4($t0)
	
	li $v0 5
	syscall
	sw $v0 8($t0)
	lw $t3 8($t0)
	
	beq $t3 0 add0p
	beq $t3 1 sub0p
	beq $t3 2 mul0p
	beq $t3 3 div0p
	
add0p:
	add $t4 $t1 $t2
	j print
sub0p:
	sub $t4 $t1 $t2
	j print
mul0p:
	mul $t4 $t1 $t2
	j print
div0p:
	div $t4 $t1 $t2
	j print
print:
	sw $t4 12($t0)
	lw $a0 12($t0)
	li $v0 1
	syscall
	
	li $v0 10
	syscall