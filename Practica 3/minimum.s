#PURPOSE:	This program finds the minimum number of a
#		set of data items.
#
# %ebx - Smallest data item found
# %eax - Current data item
# %edi - Current position in the list
#
#The following memory locations are used:
#
#data_itemas - contains the item data. A 0 is used
#to terminate the data
#
.section .data
data_items:		#These are the data items
.long 3,67,34,222,45,75,54,34,44,33,22,11,66,255

.section .text

.globl _start
_start:
movl $0,%edi			#move 0 into the index register
movl data_items(,%edi,4),%eax	#load the first byte of data
movl %eax,%ebx			#since this is the first item, %eax is the smallest

start_loop:
cmpl $255,%eax			#check to see if we've hit the end
je loop_exit
incl %edi			#load next value
movl data_items(,%edi,4),%eax	
cmpl %ebx,%eax			#compare values
jge start_loop			#jump to llop beginning if the new one isn't bigger
movl %eax,%ebx			#move the value as the largest
jmp start_loop			#jump to loop beggining

loop_exit:

# %ebx is the status code for the exit system call
# and it alredy has the maximum number
movl $1,%eax			#1 is the exit() syscall
int $0x80
