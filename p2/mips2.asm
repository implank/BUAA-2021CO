.macro return0
li $v0 10
syscall
.end_macro
.data
str: .space 2000
.text
li $v0 5
syscall
move $s0 $v0
li $v0 5
syscall
move $s1 $v0
li $v0 5
syscall
move $s2 $v0
la $s3 str

la $a0 str
move $a1 $s0
addi $a1 $a1 1 #?
li $v0 8
syscall

move $t0 $s1
move $t1 $s2

while:
	slt $t2 $t0 $t1
	beqz $t2 end_while
	
	
	add $t2 $t0 $s3
	add $t3 $t1 $s3
	lb $t4 0($t2)
	lb $t5 0($t3)
	sb $t5 0($t2)
	sb $t4 0($t3)
	
	addi $t0 $t0 1
	subi $t1 $t1 1
	j while
	
end_while:

la $a0 str
li $v0 4
syscall

return0