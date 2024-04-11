;; project: ece3360-lab03
;; file:    main.S
;; date:    20220309
;; author:  Oliver Emery

.include "m328Pdef.inc"

;; *** Defines
	.equ P_RPG_A	= PIND0
	.equ P_RPG_B	= PIND1
	.equ P_BTN	= PIND2
	.equ P_SER	= PIND3
	.equ P_RCLK	= PIND4
	.equ P_SRCLK	= PIND5
	.equ P_DIG0	= PIND7
	.equ P_DIG1	= PIND6

	.equ P_DHT11	= PINB0
	.equ P_LED	= PINB5

	.equ MODE_ACQUIRE	= 0x01
	.equ MODE_SET		= 0x02

	.equ STATE_IDLE		= 0x01
	.equ STATE_WAKE		= 0x02
	.equ STATE_DATA		= 0x03

	.equ ACQUIRE_WINDOW	= 4000
	.equ WAKE_WINDOW	= (4*18)


	; struct btn_s {
	.equ	btn_pressed	= 0x00	; 1 if button is pressed, else 0
	.equ	btn_mask	= 0x01	; 1 << PIN#
	.equ	btn_dwnd	= 0x02	; detect window
	.equ	btn_duration	= 0x03	; duration pressed
	.equ	btn_handler	= 0x04	; change handler subroutine
	; }
	.equ	sz_btn		= 6

.dseg	; data segment
.org 0x0100
	threshold:	.byte 2
	temperature:	.byte 2
	digit:		.byte 1

	mode:		.byte 1
	acquire_state:	.byte 1
	acquire_ctr:	.byte 2

	rpg_a:		.byte sz_btn
	rpg_b:		.byte sz_btn
	btn:		.byte sz_btn

	sensor_data:	.byte 5

.cseg	; code segment
.org 0x0000	jmp __entry

.org INT_VECTORS_SIZE
digit_bits: .db \
	0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, \
	0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111, \
	0b01000000, 0

__entry: ; entrypoint
	ldi	r16, high(RAMEND)
	out	SPH, r16
	ldi	r16, low(RAMEND)
	out	SPL, r16

	clr	r0

	call	init
	jmp	main

;; void memclr(Y: void*, r16: len)
;;
;;     clear up to r16 bytes of SRAM at YH:YL
;;
memclr:
	push	r16
	push	r17
	push	YL

	clr	r17
memclr_loop:
	st	Y+, r17
	dec	r16
	brne	memclr_loop

	pop	YL
	pop	r17
	pop	r16
	ret

init:
	; outputs
	ldi	r16, 1 << P_SER | 1 << P_RCLK | 1 << P_SRCLK | 1 << P_DIG0 | 1 << P_DIG1
	out	DDRD, r16

	; inputs
	ldi	r16, 1 << P_RPG_A | 1 << P_RPG_B | 1 << P_BTN   | 1 << P_DIG0 | 1 << P_DIG1
	out	PORTD, r16

	; led
	ldi	r16, 1 << P_LED
	out	DDRB, r16
	cbi	PORTB, P_LED

	ldi	r16, 61
	out	OCR0A, r16
	ldi	r16, 1 << WGM01
	out	TCCR0A, r16
	ldi	r16, 1 << CS01 | 1 << CS00
	out	TCCR0B, r16

	ret


main:
	ldi	YH, 0x01
	ldi	ZH, high(digit_bits << 1)

	ldi	r16, MODE_ACQUIRE
	sts	mode, r16

	ldi	r16, STATE_IDLE
	sts	acquire_state, r16

	ldi	r16, high(ACQUIRE_WINDOW)
	sts	acquire_ctr, r16
	ldi	r16, low(ACQUIRE_WINDOW)
	sts	acquire_ctr+1, r16
	

	ldi	YL, low(threshold)
	st	Y+, r0
	st	Y, r0

	ldi	YL, low(temperature)
	st	Y+, r0
	st	Y, r0

	sts	digit, r0

	; Initialize button structures
	ldi	r16, sz_btn

	ldi	YL, low(rpg_a)
	rcall	memclr
	ldi	r17, 1 << P_RPG_A
	ldi	r18, high(rpg_a_changed)
	ldi	r19, low(rpg_a_changed)
	std	Y+btn_mask, r17
	std	Y+btn_handler, r18
	std	Y+btn_handler+1, r19

	ldi	YL, low(rpg_b)
	rcall	memclr
	ldi	r17, 1 << P_RPG_B
	ldi	r18, high(rpg_b_changed)
	ldi	r19, low(rpg_b_changed)
	std	Y+btn_mask, r17
	std	Y+btn_handler, r18
	std	Y+btn_handler+1, r19

	ldi	YL, low(btn)
	rcall	memclr
	ldi	r17, 1 << P_BTN
	ldi	r18, high(btn_changed)
	ldi	r19, low(btn_changed)
	std	Y+btn_mask, r17
	std	Y+btn_handler, r18
	std	Y+btn_handler+1, r19

main_loop:
	call	every_interval
main_wait_ocf:
	sbis	TIFR0, OCF0A
	rjmp	main_wait_ocf
	sbi	TIFR0, OCF0A

	clr	r16
	out	TCCR0B, r16
	ldi	r16, 10
main_adjust_cycles:	; perfect 250us intervals
	dec	r16
	brne	main_adjust_cycles

	ldi	r16, 1 << CS01 | 1 << CS00
	out	TCCR0B, r16

	; reset prescaler
	in	r16, GTCCR
	sbr	r16, PSRSYNC
	out	GTCCR, r16
	
	rjmp	main_loop


every_interval:
	lds	r16, mode

	cpi	r16, MODE_SET
	brne	every_interval_mode_elsif_acquire
	ldi	YL, low(threshold)
	ldi	r18, 1
	rjmp	every_interval_write_digits

every_interval_mode_elsif_acquire:
	cpi	r16, MODE_ACQUIRE
	brne	every_interval_debounce

	ldi	YL, low(temperature)
	ldi	r18, 0

every_interval_write_digits:
	rcall	write_digits

every_interval_debounce:	
	ldi	YL, low(rpg_a)
	rcall	debounce
	ldi	YL, low(rpg_b)
	rcall	debounce
	ldi	YL, low(btn)
	rcall	debounce

	lds	r16, acquire_state
	cpi	r16, STATE_DATA
	breq	every_interval_ret

	ldi	YL, low(acquire_ctr)
	ld	r24, Y+
	ld	r25, Y
	sbiw	r25:r24, 1
	st	Y, r25
	st	-Y, r24
	brne	every_interval_ret


	lds	r16, acquire_state
	cpi	r16, STATE_IDLE
	brne	every_interval_state_elsif_wake

	ldi	r16, WAKE_WINDOW
	st	Y, r16

	ldi	r16, STATE_WAKE
	sts	acquire_state, r16

	; pull data pin lo
	sbi	DDRB, P_DHT11

	rjmp	every_interval_ret
every_interval_state_elsif_wake:
	cpi	r16, STATE_WAKE
	brne	every_interval_ret

	ldi	r24, low(ACQUIRE_WINDOW)
	ldi	r25, high(ACQUIRE_WINDOW)
	st	Y+, r24
	st	Y, r25

	ldi	r16, STATE_DATA
	sts	acquire_state, r16

	; release data pin
	cbi	DDRB, P_DHT11

	rcall	do_acquire
every_interval_ret:
	ret


wait_for_data_lo:
	sbic	PINB, P_DHT11
	rjmp	wait_for_data_lo
	ret

wait_for_data_hi:
	sbis	PINB, P_DHT11
	rjmp	wait_for_data_hi
	ret


do_acquire:
	ldi	r20, 1 << CS01	; /8 scaling aka 0.5us
	out	TCCR0B, r20

	ldi	r20, 50
	rcall	wait_for_nus
	rcall	wait_for_data_hi
	rcall	wait_for_data_lo
	rcall	wait_for_data_hi

	ldi	YL, low(sensor_data)

	ldi	r16, 5
do_acquire_bytes:
	ldi	r17, 8
do_acquire_bits:
	ldi	r20, 40
	rcall	wait_for_nus

	sbic	PINB, P_DHT11
	rjmp	do_acquire_bits_hi

	ldi	r20, 30
	rcall	wait_for_nus
	clc
	rjmp	do_acquire_bits_shift
do_acquire_bits_hi:
	ldi	r20, 60
	rcall	wait_for_nus
	sec
do_acquire_bits_shift:
	rol	r18
	rcall	wait_for_data_hi

	dec	r17
	brne	do_acquire_bits

	st	Y+, r18

	dec	r16
	brne	do_acquire_bytes

	ldi	YL, low(threshold)
	ld	r17, Y+
	ld	r18, Y
	ldi	r19, 10
	mul	r18, r19
	mov	r18, r0
	clr	r0
	add	r17, r18

	ldi	YL, low(sensor_data)
	ldd	r16, Y+2

	cp	r16, r17
	brlo	do_acquire_heat
	cbi	PORTB, P_LED
	rjmp	do_acquire_div
do_acquire_heat:
	sbi	PORTB, P_LED
do_acquire_div:
	ldi	r17, 10
	rcall	div8u

	ldi	YL, low(temperature)
	st	Y+, r15
	st	Y, r16

	ldi	r16, STATE_IDLE
	sts	acquire_state, r16

do_acquire_ret:
	; reset timer for normal mode
	ldi	r16, 61
	out	OCR0A, r16
	ldi	r16, 1 << CS01 | 1 << CS00
	out	TCCR0B, r16

	ret


wait_for_nus:
	lsl	r20
	out	OCR0A, r20
	out	TCNT0, r0
wait_for_nus_wait:
	sbis	TIFR0, OCF0A
	rjmp	wait_for_nus_wait
	sbi	TIFR0, OCF0A
	ret

rpg_a_changed:
	push	YL
	tst	r16
	brne	rpg_a_changed_ret
	ldi	YL, low(rpg_b)
	ldd	r16, Y+btn_pressed
	tst	r16
	brne	rpg_a_changed_ret
	ldi	YL, low(mode)
	ld	r16, Y
	cpi	r16, MODE_SET
	brne	rpg_a_changed_ret
	rcall	inc_threshold
rpg_a_changed_ret:
	pop	YL
	ret


rpg_b_changed:
	push	YL
	tst	r16
	brne	rpg_b_changed_ret
	ldi	YL, low(rpg_a)
	ldd	r16, Y+btn_pressed
	tst	r16
	brne	rpg_b_changed_ret
	ldi	YL, low(mode)
	ld	r16, Y
	cpi	r16, MODE_SET
	brne	rpg_b_changed_ret
	rcall	dec_threshold
rpg_b_changed_ret:
	pop	YL
	ret


btn_changed:
	push	YL
	tst	r16
	breq	btn_changed_ret
	ldi	YL, low(mode)
	ld	r16, Y
	cpi	r16, MODE_SET
	brne	btn_changed_mode_elsif_acquire
	ldi	r16, MODE_ACQUIRE
	rjmp	btn_changed_mode_store
btn_changed_mode_elsif_acquire:
	cpi	r16, MODE_ACQUIRE
	brne	btn_changed_ret
	ldi	r16, MODE_SET
btn_changed_mode_store:
	sts	mode, r16
btn_changed_ret:
	pop	YL
	ret


inc_threshold:
	lds	r16, threshold
	inc	r16
	cpi	r16, 10
	brne	inc_threshold_nowrap
	clr	r16
	lds	r17, threshold+1
	inc	r17
	cpi	r17, 10
	brne	inc_threshold_nw2
	clr	r17
inc_threshold_nw2:
	sts	threshold+1, r17
inc_threshold_nowrap:
	sts	threshold, r16
	rcall	write_digit
	ret

dec_threshold:
	lds	r16, threshold
	dec	r16
	brpl	dec_threshold_nowrap
	ldi	r16, 9
	lds	r17, threshold+1
	dec	r17
	brpl	dec_threshold_nw2
	ldi	r17, 9
dec_threshold_nw2:
	sts	threshold+1, r17
dec_threshold_nowrap:
	sts	threshold, r16
	rcall	write_digit
	ret


;; void debounce(YL: *button)
;;
;;     Sample and process raw button input data to reliably detect and handle
;;     button events. Big idea: register a change in button state if and only
;;     if it holds the changed state steady for a specified window of time.
;;
debounce:
	push	r2
	push	r1
	push	r16
	push	ZH
	push	ZL
	clr	r16
	in	r2, PIND
	ldd	r1, Y+btn_mask
	and	r2, r1
	brne	debounce_notpressed
	inc	r16
debounce_notpressed:		; byte pressed = (PINB & btn->mask) ? 0 : 1;
	ldd	r2, Y+btn_pressed
	cp	r16, r2
	breq	debounce_coda		; if (btn->pressed != pressed) {
	ldd	r2, Y+btn_dwnd
	dec	r2
	std	Y+btn_dwnd, r2
	brne	debounce_ret		; if (--btn->dwnd) return;
	std	Y+btn_pressed, r16	;     btn->pressed = pressed;
	ldd	ZH, Y+btn_handler
	ldd	ZL, Y+btn_handler+1
	icall				;     btn->handler();
debounce_coda:				; }
	ldi	r16, 8
	std	Y+btn_dwnd, r16		; btn->dwnd = WND_MSC;
debounce_ret:
	pop	ZL
	pop	ZH
	pop	r16
	pop	r1
	pop	r2
	ret

write_digits:
	lds	r17, digit
	add	YL, r17
	ld	r16, Y
	tst	r17
	brne	write_digits_d1
	sbi	PORTD, P_DIG1
	rcall	write_digit
	cbi	PORTD, P_DIG0	
	ldi	r17, 1
	rjmp	write_digits_dun
write_digits_d1:
	sbi	PORTD, P_DIG0
	rcall	write_digit
	cbi	PORTD, P_DIG1	
	ldi	r17, 0
write_digits_dun:
	sts	digit, r17
	ret


;; void write_digit(r16: charn, r18: decimal)
write_digit:
	ldi	ZH, high(digit_bits << 1)
	ldi	ZL, low(digit_bits << 1)
	add	ZL, r16
	lpm	r19, Z
	tst	r18
	breq	write_digit_no_dp
	ori	r19, 1 << 7
write_digit_no_dp:
	rcall	put_sr_byte
	ret


;; void put_sr_byte(r19: byte)
;;
;;     Put a byte into the shift register.
;;
put_sr_byte:
	ldi	r20, 8
put_sr_byte_while:
	rol	r19
	brcs	put_sr_byte_while_hibit
	cbi	PORTD, P_SER
	rjmp	put_sr_byte_wend
put_sr_byte_while_hibit:
	sbi	PORTD, P_SER
put_sr_byte_wend:
	; trigger SRCLK, shifting SER into the shift register. note that there
	; is no need for a delay: even if SBI/CBI only took 1 clock cycle, the
	; SN74HC595N supports up to 20 MHz while the UNO runs at only 16 MHz
	sbi	PORTD, P_SRCLK
	cbi	PORTD, P_SRCLK
	dec	r20
	brne	put_sr_byte_while
	; trigger RCLK to transfer shift register data to the storage register
	sbi	PORTD, P_RCLK
	cbi	PORTD, P_RCLK
	ret


.include "div8u.inc" ; subroutine from atmel AVR200 library

.exit
