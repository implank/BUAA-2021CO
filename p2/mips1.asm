.macro return0
li $v0 10
syscall
.end_macro

.text

li $v0 5
syscall
move $t0 $v0
li $v0 5
syscall
move $t1 $v0

while:
	beqz $t1 end_while
	move $t2 $t1 #c=b
	
	div $t3 $t0 $t1 #a/b=t3
	mul $t3 $t3 $t1
	sub $t3 $t0 $t3 #a%b
	move $t1 $t3	#b=a%b
	
	move $t0 $t2
	j while

end_while:

li $v0 1
move $a0 $t0
syscall

return0

