.data

#program to recieve integer input and then print it back out
.text
main:
li $v0, 5 # 5=syscall for user input
syscall
move $a0, $v0 
li $v0, 1
syscall


