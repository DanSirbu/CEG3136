;----------------------------------------------------------------------
; File: Keypad.asm
; Author:

; Description:
;  This contains the code for reading the
;  16-key keypad attached to Port A
;  See the schematic of the connection in the
;  design document.
;
;  The following subroutines are provided by the module
;
; char pollReadKey(): to poll keypad for a keypress
;                 Checks keypad for 2 ms for a keypress, and
;                 returns NOKEY if no keypress is found, otherwise
;                 the value returned will correspond to the
;                 ASCII code for the key, i.e. 0-9, *, # and A-D
; void initkey(): Initialises Port A for the keypad
;
; char readKey(): to read the key on the keypad
;                 The value returned will correspond to the
;                 ASCII code for the key, i.e. 0-9, *, # and A-D
;---------------------------------------------------------------------

; Include header files

**************EQUATES**********


;-----Conversion table
NUMKEYS	EQU	16	; Number of keys on the keypad
BADCODE 	EQU	$FF 	; returned of translation is unsuccessful
NOKEY		EQU 	$00   ; No key pressed during poll period
POLLCOUNT	EQU	1     ; Number of loops to create 1 ms poll time
NO_KEY_PRESSED EQU $FF

 SECTION globalConst  ; Constant data



 SECTION code_section  ; place in code section
;-----------------------------------------------------------	
; Subroutine: initKeyPad
;
; Description: 
; 	Initiliases PORT A
;-----------------------------------------------------------	
initKeyPad:
	movb #$F0, DDRA  ; set half the pins of DDRA to input
    movb #$00, PORTA ; Outputs are 0
	movb #1, PUCR ; Enable port A pull up resistors
	rts

;-----------------------------------------------------------    
; Subroutine: ch <- pollReadKey
; Parameters: none
; Local variable:
; Returns
;       ch: NOKEY when no key pressed,
;       otherwise, ASCII Code in accumulator B

; Description:
;  Loops for a period of 2ms, checking to see if
;  key is pressed. Calls readKey to read key if keypress 
;  detected (and debounced) on Port A and get ASCII code for
;  key pressed.
;-----------------------------------------------------------
; Stack Usage
	OFFSET 0  ; to setup offset into stack

pollReadKey:
   ; while no key pressed, wait()
    ; if PORTA & 0x0F == 0, wait 2ms max
    LDD #2
    jsr setDelay

    ;Wait while no key pressed, i.e. zero flag is set
    readKeyWait:
        jsr polldelay
        tsta
        bne return_poll_no_key ; 2ms elapsed, no key pressed

        LDA PORTA
        ANDA #$0F ; Keep only input bits
        SUBA #$0F ; if key pressed, zero flag is not set, bne will not execute
        beq readKeyWait
    
    LDD #10
    jsr delayms
    LDA PORTA
    SUBA #$0F ; Key should still be pressed, else it was a false alarm (zero flag is set) so we return no key
    beq return_poll_no_key

   ; call readKey
   jsr readKey
   rts
;No key pressed
return_poll_no_key:
   LDB NOKEY
   rts

;-----------------------------------------------------------	
; Subroutine: ch <- readKey
; Arguments: none
; Local variable: 
;	ch - ASCII Code in accumulator B

; Description:
;  Main subroutine that reads a code from the
;  keyboard using the subroutine readKeybrd.  The
;  code is then translated with the subroutine
;  translate to get the corresponding ASCII code.
;-----------------------------------------------------------	
; Stack Usage
	OFFSET 0  ; to setup offset into stack

readKey:
    LDX charCodeTbl; find what key is pressed
    LDB #0

charCodeTblLoop: ; while loop
    LDA B,X ; key =charCodeTbl[x]
    
    STA PORTA ; SET PORTA to key
    PSHD
    
    ; Wait 1 ms for the port value to be updated
    LDD 1
    jsr delayms

    PULD ; Get D Back

    CMPA PORTA ; key == charCode
    beq FOUND_IT

    INCB ; Increment key index
    ; if keyIndex > 13, PANIC CAUSE IT SHOULD NOT HAPPEN
    jmp charCodeTblLoop

FOUND_IT:
    LDX #charTbl
    LDA B,X ; A = char[x]
    PSHA

    ; wait for key released (i.e. wait while key is pressed)
    readKeyWaitLow:
    LDA PORTA
    ANDA #$0F ; Keep only input bits
    SUBA #$0F ; if key not pressed, Zero flag is set and the code will continue, else bne will execute
    bne readKeyWaitLow

    ; return key
    PULB ; B = character
    rts		           ;  return(ch); 

charCodeTbl
    dc.b %11101110
    dc.b %11101101
    dc.b %11101011
    dc.b %11100111
    dc.b %11011110
    dc.b %11011101
    dc.b %11011011
    dc.b %11010111
    dc.b %10111110
    dc.b %10111101
    dc.b %10111011
    dc.b %10110111
    dc.b %01111110
    dc.b %01111101
    dc.b %01111011
    dc.b %01110111

charTbl
    dc.b '1'
    dc.b '2'
    dc.b '3'
    dc.b 'A'
    dc.b '4'
    dc.b '5'
    dc.b '6'
    dc.b 'B'
    dc.b '7'
    dc.b '8'
    dc.b '9'
    dc.b 'C'
    dc.b '*'
    dc.b '0'
    dc.b '#'
    dc.b 'D'