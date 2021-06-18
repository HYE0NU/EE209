### --------------------------------------------------------------------
### 20210604 정현우
### assignment #4
### mydc.s
###
### Desk Calculator (dc)
### --------------------------------------------------------------------

	.equ   ARRAYSIZE, 20
	.equ   EOF, -1
        .equ   LONG, 4          # size of a long (4 byte)
        .equ   TWOLONG, 8       # size of two longs (2*4 byte)
        .equ   PARM_A, 12       # offset of parameter 'a' in abssumf
        .equ   PARM_B, 8        # offset of parameter 'b' in abssumf
	
.section ".rodata"

scanfFormat:
	.asciz "%s"
empty_message:
        .asciz "dc: stack empty\n"
num_message:
        .asciz "%d\n"

### --------------------------------------------------------------------

        .section ".data"

### --------------------------------------------------------------------

        .section ".bss"
buffer:
        .skip  ARRAYSIZE

### --------------------------------------------------------------------

	.section ".text"

	## -------------------------------------------------------------
	## int main(void)
	## Runs desk calculator program.  Returns 0.
	## -------------------------------------------------------------

	.globl  main
	.type   main,@function

main:

	pushl   %ebp
	movl    %esp, %ebp

input:

	## dc number stack initialized. %esp = %ebp
	
	## scanf("%s", buffer)
	pushl	$buffer
	pushl	$scanfFormat
	call    scanf
	addl    $TWOLONG, %esp

	## check if user input EOF
	cmpl	$EOF, %eax
	je	quit
        
        ## check if user input is number
        ## more efficient than calling isdigit
        cmpb    $'_', buffer    # if(buffer[0]=='_' ||
        je      number
        cmpb    $'0', buffer    # !(buffer[0]<'0' || buffer[0]>'9'))
        jl      notnum
        cmpb    $'9', buffer
        jg      notnum
        jmp     number          # goto NUMBER


notnum:                         # else switch(buffer[0]){
        cmpb    $'p', buffer    # case 'p' // p command: print top entry
        je      p               
        cmpb    $'q', buffer    # case 'q' // q command: quit
        je      quit
        cmpb    $'f', buffer    # case 'f' // f command: print in LIFO
        je      f
        cmpb    $'c', buffer    # case 'c' // c command: clear stack
        je      c
        cmpb    $'d', buffer    # case 'd' // d command: duplicate top
        je      d
        cmpb    $'r', buffer    # case 'r' // r command: swap two tops
        je      r
        cmpb    $'+', buffer    # case '+' // addition
        je      plus
        cmpb    $'-', buffer    # case '-' // subtraction
        je      minus
        cmpb    $'*', buffer    # case '*' // multiplication
        je      multi
        cmpb    $'/', buffer    # case '/' // division
        je      divide
        cmpb    $'|', buffer    # case '|' // absolute value sum
        je      abssum

        jmp input               # continue loop


## number reading
## convert input to number and store it at stack
## if input is number(negative sign as '_')
number:
        ## user input is number
        cmpb    $'_', buffer    # if(buffer[0] == '_')
        je      negnum
        jmp     convert
negnum:
        movb    $'-', buffer    # buffer[0] = '-'
convert:
        pushl   $buffer         # int num = atoi(buffer)
        call    atoi             
        movl    %eax, (%esp)    # stack.push(num)
        jmp     input           ## calculations on %esp is canceled

	
## p command
## print the value on the top of the stack, without altering it
p:
        cmpl    %esp, %ebp      # if(stack.peek()==NULL)
        jbe     emptystack      # printf("dc: stack empty")
        pushl   $num_message    # else printf("%d\n", stack.top())
        call    printf
        addl    $LONG, %esp
        jmp     input


## f command
f:
        movl    %esp, %ebx      # int i = stack.size()
floop:
        cmpl    %ebx, %ebp      # while(i>0)
        jbe     input
        pushl   (%ebx)          # printf('%d\n', stack[i-1])
        pushl   $num_message
        call    printf
        addl    $TWOLONG, %esp
        addl    $LONG, %ebx     # i--
        jmp     floop


## c command
## clear the stack, rendering it empty
c:
        movl    %ebp, %esp      # stack.clear()
        jmp     input           ## stack_top = stack_base


## d command
## duplicate the value on the top; push another copy of it
d:
        cmpl    %esp, %ebp      # if(stack.size() > 0)
        jbe     emptystack      
        pushl   (%esp)          # stack.push(stack.top())
        jmp     input


## r command
## reverse the order of(swap) the top two values on the stack
r:
        movl    %esp, %ebx      # if(stack.size() > 1)
        addl    $LONG, %ebx
        cmpl    %ebx, %ebp
        jbe     emptystack

        movl    (%esp), %eax    ##swap
        movl    LONG(%esp), %ebx     
        movl    %eax, LONG(%esp)
        movl    %ebx, (%esp)
        jmp     input


## + operation
## pop two values off the stack, add them, push result
plus:
        movl    %esp, %ebx      # if(stack.size() > 1)
        addl    $LONG, %ebx
        cmpl    %ebx, %ebp
        jbe     emptystack

        popl    %ebx            # int b = stack.pop()
        popl    %eax            # int a = stack.pop()
        addl    %ebx, %eax      # a = a + b
        pushl   %eax            # stack.push(a)
        jmp     input


## - operation
## pop two values, subtract first from second, push result 
minus:
        movl    %esp, %ebx      # if(stack.size() > 1)
        addl    $LONG, %ebx
        cmpl    %ebx, %ebp
        jbe     emptystack

        popl    %ebx            # int b = stack.pop()
        popl    %eax            # int a = stack.pop()
        subl    %ebx, %eax      # a = a - b
        pushl   %eax            # stack.push(a)
        jmp     input


## * operation
## pop two values, multiply them, push result
multi:
        movl    %esp, %ebx      # if(stack.size() > 1)
        addl    $LONG, %ebx
        cmpl    %ebx, %ebp
        jbe     emptystack

        popl    %ebx            # int b = stack.pop()
        popl    %eax            # int a = stack.pop()
        imull   %ebx            # int a = a * b
        pushl   %eax            # stack.push(a)
        jmp     input


## / operation
## pop two values, divide second by first, push result
divide:
        movl    %esp, %ebx      # if(stack.size() > 1)
        addl    $LONG, %ebx
        cmpl    %ebx, %ebp
        jbe     emptystack

        popl    %ebx            # int b = stack.pop()
        popl    %eax            # int a = stack.pop()
        cdq
        idivl   %ebx            # a = a / b
        pushl   %eax            # stack.push(a)
        jmp     input


## | operation function
## pop two values, sum two absolute values, push result
abssum:
        movl    %esp, %ebx      # if(stack.size() > 1)
        addl    $LONG, %ebx
        cmpl    %ebx, %ebp
        jbe     emptystack

        #int a = abssumf(stack.pop(), stack.pop())
        ## in fact, two arguments have reverse order
        call    abssumf         
        addl    $TWOLONG, %esp       
        pushl   %eax            # stack.push(a) 
        jmp     input

## print error and return to loop if user access empty stack
emptystack:
        pushl   $empty_message  # printf("dc: stack empty\n")
        call    printf
        addl    $LONG, %esp
        jmp     input           ## go back to loop


## quit; terminate program
quit:	
	## return 0
	movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret


	## -------------------------------------------------------------
	## int abssumf(int a, int b)
	## Sum absolute values from min(a,b) to max(a,b).  
        ## Returns sum by %eax.
	## -------------------------------------------------------------
abssumf:
        pushl   %ebp            ## prolog
        movl    %esp, %ebp

        movl    $0, %eax                # int s = 0
        movl    PARM_A(%ebp), %ecx      # int i = a
        movl    PARM_B(%ebp), %ebx      # int n = b 
        cmpl    %ebx, %ecx              # if(i>n)
        jg      swapab                  
        jmp     abssumloop            
swapab:
        movl    %ecx, %edx              # swap(i, n)
        movl    %ebx, %ecx
        movl    %edx, %ebx

abssumloop:                     # for(;i <= n;)
        cmpl    %ebx, %ecx
        jg      endloop

        cmpl    $0, %ecx        # if(i>=0)
        jl      abs
        addl    %ecx, %eax      # s += i
        addl    $1, %ecx        # i++
        jmp     abssumloop
abs:
        subl    %ecx, %eax      # else s -= i
        addl    $1, %ecx        # i++
        jmp abssumloop
endloop:

        movl    %ebp, %esp      ## epilog
        popl    %ebp
        ret                     # return s; stored in %eax
