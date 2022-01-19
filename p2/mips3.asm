.macro return0
li $v0 10
syscall
.end_macro
.data

.text


li $v0 5
syscall 
move $t0 $v0
li $t1 65
li $t2 66
li $t3 67

addi $sp $sp -16
sw $t0 0($sp)
sw $t1 4($sp)
sw $t2 8($sp)
sw $t3 12($sp)
jal h
addi $sp $sp 16

return0


h:
	move $t9 $sp
	addi $sp $sp -20
	sw $t0 0($sp)
	sw $t1 4($sp)
	sw $t2 8($sp)
	sw $t3 12($sp)
	sw $ra 16($sp)
	
	lw $t0 0($t9)	#load aug
	lw $t1 4($t9)	
	lw $t2 8($t9)	
	lw $t3 12($t9)		
	
	beq $t0 1 if
	
	addi $sp $sp -16
	subi $t0 $t0 1
	sw $t0 0($sp)
	sw $t1 4($sp)
	sw $t3 8($sp)
	sw $t2 12($sp)
	jal h
	addi $sp $sp 16
	
	addi $sp $sp -8
	sw $t1 0($sp)
	sw $t3 4($sp)
	jal mov
	addi $sp $sp 8
	
	addi $sp $sp -16
	#subi $t0 $t0 1
	sw $t0 0($sp)
	sw $t2 4($sp)
	sw $t1 8($sp)
	sw $t3 12($sp)
	jal h
	addi $sp $sp 16
	
	j end_h
if:
	addi $sp $sp -8
	sw $t1 0($sp)
	sw $t3 4($sp)
	jal mov
	addi $sp $sp 8
	
	j end_h
	
end_h:
	lw $t0 0($sp)
	lw $t1 4($sp)
	lw $t2 8($sp)
	lw $t3 12($sp)
	lw $ra 16($sp)
	addi $sp $sp 20
	
	jr $ra
	
mov:
	move $t9 $sp
	addi $sp $sp -8
	sw $t0 0($sp)
	sw $t1 4($sp)
	lw $t0 0($t9)
	lw $t1 4($t9)
	
	li $v0 11
	move $a0 $t0
	syscall
	li $a0 45
	syscall
	li $a0 62
	syscall
	move $a0 $t1
	syscall
	li $a0 10
	syscall
	
	lw $t0 0($sp)
	lw $t1 4($sp)
	addi $sp $sp 8
	jr $ra