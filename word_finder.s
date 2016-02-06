# Marina Kent
# Computer Systems Assignment 1 Part 1
# 18/10/15

.data
    # the messages that the program prints
    prompt1:     .asciiz "Input:\n"
    charMessage: .asciiz "Output: \n" 
    
    # the characters that signify when something isn't a word
    SP:          .asciiz " "
    COM:         .asciiz ","
    PER:         .asciiz "."
    EXC:         .asciiz "!"
    QST:         .asciiz "?"
    UND:         .asciiz "_"
    HYP:         .asciiz "-"
    PRO:         .asciiz "("
    PRC:         .asciiz ")"
    NL:          .asciiz "\n"        # NL is also used to separate the words when printed
    
    # an array with 100 bytes. This is the input to be separated into words.
    userInput:   .space  100
    
    # num stores the size of the input - 100 chars
    num:         .word 100
    
    # these three numbers are used like a boolean to check if there are two non-word characters in a row
    number:      .word 3
    num4:        .word 4
    num3:        .word 3
  
.globl main

.text
    main:
    
    # load the "boolean" numbers into $s3 and $s4. 
    # They are initially set to be different to account for a non word character being first.
        la $t8, number
        lb $s3, 0($t8)	
        la $t8, num4
        lb $s4, 0($t8)		
    
    # prompts for a sentence - says "input"
        li $v0, 4
        la $a0, prompt1
        syscall

    # getting user's input - enter in text
        li $v0, 8
        la $a0, userInput
        li $a1, 100   
        syscall
        
    # displays the message - will say "output: "
        li $v0, 4
        la $a0, charMessage
        syscall
     
    # getting ready for print loop - initializing counters, etc.
        lw $s0, num # get size of list - how many chars to print out?
        move $s1, $zero # set counter for number of elems printed
     
    print_loop:
        bge $s1, $s0, print_loop_end # stop after last elem is printed 
        la $t0, userInput       # input stored in $t0
        add $t0, $t0, $s1       # increment through input as the loop continues
        
        la $t1, SP              # $t1 holds the address of a space
        lb $t2, 0($t0)          # $t2 now holds the current char, input held in $t0
        lb $t3, 0($t1)          # $t3 holds the byte that contains a space
          
        # if else statements								
        bne  $t2, $t3, COMMA   # branch if current char is not a space - will check if it is the next symbol
      	jal newLine            # if it is a space, print a new line. This is a subroutine made to save space.
      	
      	# the rest of the if else statements are the same as the first, but check for various types of input.
      	
      	# checks for a comma
      	COMMA:  
	la $t1, COM		
        lb $t3, 0($t1)			     	
      	bne  $t2, $t3, PERIOD     	
        jal newLine
        
        # checks for a period
        PERIOD:  
	la $t1, PER
        lb $t3, 0($t1)		     	
      	bne  $t2, $t3, EXCMARK     	
        jal newLine        
        
        # checks for an exclamation point
        EXCMARK:
        la $t1, EXC	
        lb $t3, 0($t1)	     	
      	bne  $t2, $t3, QMARK   	
        jal newLine
        
        # checks for a question mark
        QMARK:
        la $t1, QST
        lb $t3, 0($t1)    	     	
      	bne  $t2, $t3, UNDSCR   	
        jal newLine
        
        # checks for an underscore
        UNDSCR:      
        la $t1, UND
        lb $t3, 0($t1)
      	bne  $t2, $t3, HYPH	
        jal newLine
        
        # checks for a hyphen
        HYPH:
        la $t1, HYP
        lb $t3, 0($t1)	     	
      	bne  $t2, $t3, OPENP
        jal newLine
        
        # checks for an open parenthesis
        OPENP:
        la $t1, PRO
        lb $t3, 0($t1)	 	     	
      	bne  $t2, $t3, CLOSEP    	
        jal newLine
        
        # checks for a closed parenthesis
        CLOSEP:
        la $t1, PRC
        lb $t3, 0($t1) 	     	
      	bne  $t2, $t3, NEWL  	
        jal newLine
        
        # checks for a new line (this prevents the extra space at the end of the program if it ends in an allowed word)
        NEWL:
        la $t1, NL
        lb $t3, 0($t1)    	     	
      	bne  $t2, $t3, ELSE   # since it has checked all non-word symbols, can then print the character.
        jal newLine
        
        # if input char is part of a word, will print the character out
        ELSE:        
          # print the character
          lb $a0, ($t0)		
          li $v0, 11
          syscall
  
          # resets the "boolean" numbers so they are equal again.
          la $t1, num3
          lb $s4, 0($t1)
        
        # if the character is printed, or a new line is printed, will increment the loop counters, and move on to next input value
        L1:  		
          addi $s1, $s1, 1 # increment the loop counter
          j print_loop # repeat the loop
    
    # end of loop
    print_loop_end: 
        
    # End of program
        li $v0, 10
        syscall
     
     # subroutine: prints a new line, changes "boolean" values to show that a new line was printed  
     newLine:
        
        # if the two "boolean" values are different (two non-word chars found subsequently), move to next input char, don't print an extra new line
     	bne $s3, $s4, L1
        
        # prints out a new line
        la $a0, NL
        li $v0, 4
        syscall
	
	# updates "boolean" values to be different (now are set to 3 and 4)
        la $t1, num4
        lb $s4, 0($t1)			
        
        # once finished, start loop again. This prevents the character from being still printed.
        j L1 
        
  
