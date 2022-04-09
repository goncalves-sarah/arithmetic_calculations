.macro mostra_resultado
	loop_operacao:
		cbi PORTB, 1
		cbi PORTB, 0
		mov R16, @0
		mov R21, R16
		ldi R17, 100
		cpi R30, 1
		brne loop_espera
		sbi PORTB, 0
	loop_espera:
		sbis PINB, 3
		rjmp fim
		sbic PINB, 2
		rjmp loop_espera
		cpi R17, 1
		breq loop_espera
	loop_bits:
		ldi R18, 0
		mov R19,R16
	loop_div:
		sub R16, R17
		brlo fim_div
		inc R18
		rjmp loop_div
	fim_div:
		out PORTD, R18
		pause_i_ms 220
		mov R19, R18
		ldi R20, 0
	loop_resto:
		add R20, R17
		dec R19
		cpi R19, 0
		brne loop_resto
	fim_resto:
		sub R21, R20
		mov R16, R21
		cpi R17, 10
		breq ultimo_loop
		ldi R17, 10
		cpi R18, 0
		breq loop_bits
		rjmp loop_espera
	ultimo_loop:
		ldi R17, 1
		sbic PINB, 2
		rjmp ultimo_loop
		out PORTD, R21
		pause_i_ms 220
		sbi PORTB, 1
		rjmp loop_espera
	fim:
		ldi R16, 0x00
		out PORTD, R16
   cbi PORTB, 0
.endm


rjmp start
.include "minha_lib.inc"

start:
  ldi R16, 0xFF
  out DDRD, R16
  ldi R16, 0x03
  out DDRB, R16
  ldi R16, 0x00
  out PORTD, R16
  ldi R16, 0x0C
  out PORTB, R16

  ldi R16, 15
  ldi R17, 4
  sub R16, R17
  ldi R30, 1
  mostra_resultado R16
	 pause_i_ms 220

  ldi R16, 5
  ldi R17, 9
  add R16, R17
  ldi R30, 0
  mostra_resultado R16
	 pause_i_ms 220

  ldi R16, 10
  ldi R17, 15
  mul R16, R17
  mov R16, R0
  mov R17, R1
  ldi R30, 1
  mostra_resultado R16
	 pause_i_ms 220

  ldi R16, 10
	 ldi R17, 3
	 ldi R18, 1
	 mov R19, R16
loop_div4:
	 sub R16, R17
	 cp  R16, R17
	 brlo fim_div4
	 inc R18
	 rjmp loop_div4
fim_div4:
  ldi R30, 0
  mostra_resultado R18
	 pause_i_ms 220

  ldi R16, 22
  ldi R17, 3
  push R16
  push R17
	 ldi R18, 1
	 mov R19, R16
loop_div5:
	 sub R16, R17
	 cp  R16, R17
	 brlo fim_div5
	 inc R18
	 rjmp loop_div5
fim_div5:
  pop R17
  pop R16
  mul R17, R18
  sub R16, R0
  ldi R30, 0
  mostra_resultado R16
	 pause_i_ms 220

  ldi R16, 3
	 ldi R17, 3
	 mov R20, R16
loop_potencia6:
	 cpi R17, 1
	 breq fim_potencia6
	 dec R17
	 mul R16, R20
	 mov R20, R0
	 rjmp loop_potencia6
fim_potencia6:
	 mov R16, R20
  ldi R30, 0
  mostra_resultado R16
	 pause_i_ms 220

  ldi R16, 9
  ldi R17, 20
  add R16, R17
  ldi R30, 1
  mostra_resultado R16
	 pause_i_ms 220

	loop_final:
		sbi PORTB, 1
		rjmp loop_final
