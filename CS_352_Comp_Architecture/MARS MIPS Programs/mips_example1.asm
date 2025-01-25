
.text
main:
	li $v0, 4 #11=system code for printing a character, $v0=register that gets the system code for printing as value
	la $a0, greeting #'a'=our example character, $a0=register that accepts the character for printing
	syscall #Call to the System to execute the instructions and print the character at the a0

	li $v0, 10
	syscall
	
.data
greeting: .asciiz "hello"
