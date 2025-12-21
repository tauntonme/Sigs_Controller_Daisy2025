/*
 * ForeverLoop.cpp
 *
 *  Created on: Jun 6, 2023
 *      Author: Jon Freeman B Eng Hons
 */
#include 	"main.h"

#include	"Serial.hpp"
#include	"Project.hpp"
#include	"CmdLine.hpp"
#include	"microsecs.hpp"

extern	Serial	pc;		//, * Com_ptrs[];
extern	Serial	ctrl;
//extern	bool	can_flag;
extern	volatile	uint32_t	can_sema;
extern	void	ce_show	()	;
extern	void	check_commands	()	;	//	Looks after all command line input
extern	bool	send_can_msg	(int node11bit, uint8_t * TxData, int len)	;
extern	bool	send_can_msg	(int node4hibit, int node7lobit, uint8_t * TxData, int len)	;
extern	bool	ticked	()	;	//	Has a SysTick occurred since last call?
//uint32_t	millisecs 			= 0L;
//int32_t		forever_loop_timer 	= 0L;
//int32_t		slow_loop_timer 	= 0L;
//volatile	bool	timer_1ms_flag;

extern	bool	can_echo	()	;	//	Peripherals respond using this
extern	void	can_sig_test	()	;	//	call this once per second

/*void	showme	()	{
	char	txt[255];
	int	len;
	pc.write	("Lev X data\r\n", 12);
	len = sprintf	(txt, "gate1 %d %c, %d %c,\tgate2 %d %c, %d %c,\tgate3 %d %c, %d %c,\tgate4 %d %c, %d %c\r\n",
			Level_Crossing.gate1.A.get_pin_num(), Level_Crossing.gate1.A.active() ? 'T' : 'F', Level_Crossing.gate1.B.get_pin_num(), Level_Crossing.gate1.B.active() ? 'T' : 'F',
			Level_Crossing.gate2.A.get_pin_num(), Level_Crossing.gate2.A.active() ? 'T' : 'F', Level_Crossing.gate2.B.get_pin_num(), Level_Crossing.gate2.B.active() ? 'T' : 'F',
			Level_Crossing.gate3.A.get_pin_num(), Level_Crossing.gate3.A.active() ? 'T' : 'F', Level_Crossing.gate3.B.get_pin_num(), Level_Crossing.gate3.B.active() ? 'T' : 'F',
			Level_Crossing.gate4.A.get_pin_num(), Level_Crossing.gate4.A.active() ? 'T' : 'F', Level_Crossing.gate4.B.get_pin_num(), Level_Crossing.gate4.B.active() ? 'T' : 'F'
			);
	pc.write(txt, len);
	len = sprintf(txt, "alarm %d\r\n", Level_Crossing.alarm.get_pin_num());
	pc.write(txt, len);
}*/
//Sig_Dn_Main_Distant.set_occulting();

#define	FOREVER_LOOP_REPEAT_MS	20
#define	SLOW_LOOP_REPEAT_MS	500

extern	bool	spi_inputs_updated	()	;

int32_t	sigs_time;
my_microsecond_timer	mytim;

void	do_even_halfsec_stuff	()	{
	ctrl.write	("A", 1);
	can_sig_test	();
}

void	do_odd_halfsec_stuff	()	{
//char	t[100];
//	int len = sprintf	(t, "Sigs_time us = %d\r\n", sigs_time);
//	pc.write	(t, len);
}

void	do_fastest_stuff	()	{
/*	if	(can_sema > 0)	{
		can_sema--;
		char	t[44];
		int len = sprintf	(t, "Got cnc ID %lf, filt %lx\r\n", )
	}*/
	can_echo	();
	check_commands	()	;			//	Read and execute any commands rec'd from pc
}

extern	void	signals_engine	()	;


uint32_t	chs[2] {};
uint32_t	ping_id = 0x100;

void	do_forever_loop_rate_stuff	()	{
	send_can_msg	(ping_id++, (uint8_t*)chs, 0);	//	launch empty message to all addresses, note responses
	ping_id &= 0x17f;
	chs[0]++;
	chs[1]--;

//	if	(can_sema > 0)	{
//		can_flag = false;
//		can_sema--;
//		ce_show	();
//	}

	mytim.restart();

//	int32_t	old_sigs_time =	__HAL_TIM_GET_COUNTER(&htim6);
//	int	pld_sigs_time =	(&htim6)->Instance->CNT;	//	same as '__HAL_TIM_GET_COUNTER(&htim6)' above


		signals_engine	();


//		sigs_time =	__HAL_TIM_GET_COUNTER(&htim6) - old_sigs_time;	//	time consumed by signals_engine in us
	sigs_time =	mytim.read();	//	time consumed by signals_engine in us
	if	(sigs_time < 0)
		sigs_time += 1000;
}

#ifdef	NODE_IS_CONTROLLER	//	Apr 1 2024
extern	bool	can_stuff_to_do;
extern	bool	do_can_stuff	(int)	;
int		can_device_num = 0;
#endif

void	one_ms_stuff	()	{
#ifdef	NODE_IS_CONTROLLER	//	Apr 1 2024
	if	(can_stuff_to_do)	{
		do_can_stuff	(can_device_num++);
		if	(can_device_num > 126)
			can_stuff_to_do = false;
	}
#endif
	can_echo	()	;				//	Reports to pc. on message rec'd over CAN bus
	pc.tx_any_buffered();
	ctrl.tx_any_buffered();
	if	(spi_inputs_updated())	{	//	Returns true after update, false when updates not yet available
		do_forever_loop_rate_stuff	()	;	//	As soon as latest inputs have been updated
	}
}

void	do_slow_loop_rate_stuff	()	{
	static	bool	oddeven;
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);	//	as good a place as any to toggle green Nucleo led
	oddeven = !oddeven;
	oddeven ? do_even_halfsec_stuff	() : do_odd_halfsec_stuff	()	;
}


extern	void	start_spi_TxRx	();	//	Call this to initiate multi-byte SPI transfer

extern "C" void	ForeverLoop	()	{	// Jumps to here from 'main.c'
	pc.start_rx();
	ctrl.start_rx();
	while	(true)	{				//	Always forever loop top
		do_fastest_stuff	();		//	While waiting for timer to sync slower stuff
		if	(ticked())	{
//		if	(timer_1ms_flag)	{
//			timer_1ms_flag = false;
			one_ms_stuff	();		//	Do all the once per millisec stuff
			if	((HAL_GetTick() % FOREVER_LOOP_REPEAT_MS) == 0)	{
//			if	(forever_loop_timer >= FOREVER_LOOP_REPEAT_MS)	{
//				forever_loop_timer -= FOREVER_LOOP_REPEAT_MS;
//no				do_forever_loop_rate_stuff	();
				//	start spi In/Out update sequence all under DMA control.
				start_spi_TxRx	();		//	SPI comms all happens under DMA control with no code interventions
				//	When spi shift reg read/write ends, DMA raises 'complete' interrupt
				//	It is this interrupt that initiates 'do_forever_loop_rate_stuff	()'
				//	By so doing, io data is used at its freshest possible.
				if	((HAL_GetTick() % SLOW_LOOP_REPEAT_MS) == 0)	{
//				if	(slow_loop_timer > SLOW_LOOP_REPEAT_MS)	{
//					slow_loop_timer -= SLOW_LOOP_REPEAT_MS;
					do_slow_loop_rate_stuff	();					//	once per half second
				}	//	endof if	(slow_loop_timer > SLOW_LOOP_REPEAT_MS)
			}		//	endof if	(forever_loop_timer >= FOREVER_LOOP_REPEAT_MS)
		}			//	endof if	(timer_1ms_flag)
	}				//	endof while	(true)
}					//	endof void	ForeverLoop(). Never gets to here, function never returns.

/*
extern	TIM_HandleTypeDef htim6;	//	TIM6 counts 16 bit micro secs, interrupts once per milli sec
//	Callback: timer has reset
void	HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{	//	check for timer 6
	if	(htim == &htim6)	{
		timer_1ms_flag = true;
		millisecs++;		//	Global count of milli seconds since power-up
		forever_loop_timer++;
		slow_loop_timer++;
	}
}
*/

