.macro return0
li $v0 10
syscall
.end_macro
.data 
arr: .space 4000
space: .asciiz ","
left_bracket: .asciiz "["
right_bracket: .asciiz "]"
.text
init:
	li $v0 5
	syscall  # in n
	move $s0 $v0 # s0=n
	la $s1 arr
	li $t0 0 # i=1

for1:
	slt $t2 $t0 $s0		
	beqz $t2 for1_end	#i<=n
	li $v0 5			#in a[i]
	syscall
	sll $t2 $t0 2		#store array (>>2)
	add $t2 $t2 $s1
	sw $v0 0($t2)
	addi $t0 $t0 1		#i++
	j for1
for1_end:
	li $a0 0
	move $a1 $s0
	add $a1 $a1 -1
	jal quicksort

print_init:
	li $t0 0
	la $a0 left_bracket
	li $v0 4
	syscall
print:
	slt $t2 $t0 $s0
	beqz $t2 print_end #i<=n
	sll $t2 $t0 2		
	add $t2 $t2 $s1
	lw $a0 0($t2)
	li $v0 1
	syscall				#print a[i]
	
	addi $t1 $t0 1
	slt $t2 $t1 $s0
	beqz $t2 kaka
	la $a0 space
	li $v0 4
	syscall				#print " "
kaka:
	addi $t0 $t0 1		#i++
	j print
print_end:
	la $a0 right_bracket
	li $v0 4
	syscall

return0

quicksort:
	sw $t0 0($sp)		#instock
	addi $sp $sp -4
	sw $t1 0($sp)
	addi $sp $sp -4
	sw $t2 0($sp)
	addi $sp $sp -4
	sw $ra, 0($sp)
	addi $sp, $sp, -4
	
	move $t0 $a0
	move $t1 $a1
	slt $t2 $t0 $t1
	beqz $t2 quicksort_end
	move $a0 $t0		#
	move $a1 $t1
	jal getstandard
	move $t2 $v0		#
	addi $t2 $t2 -1		#
	move $a0 $t0
	move $a1 $t2
	jal quicksort		#
	addi $t2 $t2 2		#
	move $a0 $t2
	move $a1 $t1
	jal quicksort		#	
quicksort_end:
	addi $sp, $sp, 4
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t2 0($sp)
	addi $sp $sp 4
	lw $t1 0($sp)
	addi $sp $sp 4
	lw $t0 0($sp)
	jr $ra
	
getstandard:
	sw $t0 0($sp)		#instock
	addi $sp $sp -4
	sw $t1 0($sp)
	addi $sp $sp -4
	sw $t2 0($sp)
	addi $sp $sp -4
	sw $t3 0($sp)
	addi $sp $sp -4
	
	move $t0 $a0
	move $t1 $a1
	sll $t2 $t0 2
	add $t2 $t2 $s1
	lw $t3 0($t2)
while1:
	slt $t5 $t0 $t1
	beqz $t5 while1_end
	
while2:
	slt $t5 $t0 $t1		#i<j
	sll $t2 $t1 2
	add $t2 $t2 $s1		#arr+j
	lw $t4 0($t2)		#t4=arr[j]
	sle $t6 $t3 $t4 	#key(t3)<=arr[j](t4)
	and $t5 $t5 $t6
	beqz $t5 while2_end
	addi $t1 $t1 -1
	j while2
while2_end:
	
	slt $t5 $t0 $t1		#i<j
	beqz $t5 while3
	sll $t2 $t1 2
	add $t2 $t2 $s1
	lw $t4 0($t2)		#t4=a[j]
	sll $t2 $t0 2
	add $t2 $t2 $s1
	sw $t4 0($t2)		#a[i]=a[j]
	
while3:
	slt $t5 $t0 $t1		#i<j
	sll $t2 $t0 2
	add $t2 $t2 $s1		#arr+i
	lw $t4 0($t2)		#t4=arr[i]
	sle $t6 $t4 $t3 	#arr[i](t4)<=key(t3)
	and $t5 $t5 $t6
	beqz $t5 while3_end
	addi $t0 $t0 1
	j while3
while3_end:
	
	slt $t5 $t0 $t1		#i<j
	beqz $t5 while1
	sll $t2 $t0 2
	add $t2 $t2 $s1
	lw $t4 0($t2)		#t4=a[i]
	sll $t2 $t1 2
	add $t2 $t2 $s1
	sw $t4 0($t2)		#a[j]=a[i](t4)
	
	j while1
while1_end:
	sll $t2 $t0 2
	add $t2 $t2 $s1
	sw $t3 0($t2)
	move $v0 $t1
	
	addi $sp $sp 4
	lw $t3 0($sp)		
	addi $sp $sp 4
	lw $t2 0($sp)
	addi $sp $sp 4
	lw $t1 0($sp)
	addi $sp $sp 4
	lw $t0 0($sp)
	jr $ra