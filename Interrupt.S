# Interrupt.S
# Written by Garo Malko and Karoline Malko


.global enable_interrupt

enable_interrupt: 
	ei	#enable interrupt globally
	jr	$ra	#return the register address of the function
	nop
	