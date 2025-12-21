/*
 * LevelCrossing.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
//#include	"main.h"
#ifndef INC_LEVELCROSSINGCLAS_HPP_
#define INC_LEVELCROSSINGCLAS_HPP_

#include	"LinesideSignal.hpp"
#include	"MechanicalLock.hpp"
#include	"MovingGate.hpp"
#include	"PulseAlarm.hpp"
#include	"PushButton.hpp"
#include	"SingleInput.hpp"
#include	"SingleOutput.hpp"

/**
 * class	ClassLevelCrossing	{
 *
 * As defined here, a Level Crossing has 4 gates, 2 locks, 2 push buttons, 1 alarm and 1 traffic light
 * 2 actuator outputs.
 *
 * Note West Buckland level crossing object instantiated in 'SignalsMain.cpp', using pin list data
 */
class	ClassLevelCrossing	{
	//    	state {0L};	//	Reset state, we don't know the position of any parts yet.
 		uint32_t	state	= 0L	;		//	state of the level crossing (not used to 9th June 2023 J.F.)
 		char	noid[2] {'?', 0};
 		const char * id = noid;
 		const char * m_txt	()	const	{	return	(id);	}
 		MovingGate		gate1	;	//	Level crossing has 4 gates, MovingParts
 		MovingGate	 	gate2	;
 		MovingGate	 	gate3	;
 		MovingGate		gate4	;
		MechLock		lock1	;
		MechLock		lock2	;				//	2 gate locks, MovingParts
		Wired_LinesideSignal	trafficlight;			//	LinesideSignal used here as road traffic lights
 		SingleOutput	actuator_A;		//	pneumatics
 		SingleOutput	actuator_B;
 		PushButton		push_butt_1;	//	one input from switch, two outputs for indicator lights or whatever
 		PushButton		push_butt_2;
public:
 		PulseAlarm		alarm;			//

    ClassLevelCrossing (	//	Constructor
    		const char * level_crossing_name,
			INPIN gate1ina1, 	POLARITY pa1, 	INPIN gate1inb1, POLARITY pb1,	//	gate1 info,	gate is 'MovingPart' with 2 position detectors.
			INPIN gate2ina1, 	POLARITY pa2, 	INPIN gate2inb1, POLARITY pb2, //	gate2 info,	bool pa1 etc are polarity selectors for N.O. or N.C. switch types
			INPIN gate3ina1, 	POLARITY pa3, 	INPIN gate3inb1, POLARITY pb3, //	gate3 info
			INPIN gate4ina1, 	POLARITY pa4, 	INPIN gate4inb1, POLARITY pb4, //	gate4 info
			INPIN locka1, 	POLARITY lockap1, INPIN lockb1, 	POLARITY lockbp1, OUTPIN lock1opin, POLARITY lock1opol,	//	lock1 info
			INPIN locka2, 	POLARITY lockap2, INPIN lockb2, 	POLARITY lockbp2, OUTPIN lock2opin, POLARITY lock2opol,	//	lock2 info
			INPIN tli1,		INPIN	tli2,	OUTPIN	tlo1,	OUTPIN	tlo2,	//	traffic light I/O numbers
			OUTPIN	alarm_pin,						//	alarm
			OUTPIN	act1opin, POLARITY acto1pol,
			OUTPIN	act2opin, POLARITY acto2pol,
			INPIN pbut1in, POLARITY pb1pol1, OUTPIN but1opin1, POLARITY but1opol1, OUTPIN but1opin2, POLARITY but1opol2,
			INPIN pbut2in, POLARITY pb2pol1, OUTPIN but2opin1, POLARITY but2opol1, OUTPIN but2opin2, POLARITY but2opol2)
 	:	gate1		{"LX_Gate1", gate1ina1, pa1, gate1inb1, pb1}	//	two input port numbers, two boolean polarity selectors
	,	gate2		{"LX_Gate2", gate2ina1, pa2, gate2inb1, pb2}	//	Jan 2024 - added const char * helpful text
	,	gate3		{"LX_Gate3", gate3ina1, pa3, gate3inb1, pb3}
	,	gate4		{"LX_Gate4", gate4ina1, pa4, gate4inb1, pb4}
	,	lock1		{"LX_Lock1", 	locka1, lockap1,		//	Lock 1 Input A
								lockb1, lockbp1,		//	Lock 1 Input B
								lock1opin, lock1opol}	//	Lock 1 actuator output
	,	lock2		{"LX_Lock2", 	locka2, lockap2, 	//	Lock 2 Input A
								lockb2, lockbp2,		//	Lock 2 Input B
								lock2opin, lock2opol}	//	Lock 2 actuator output
	,	trafficlight{"LX_TrafficLight", tli1, tli2, tlo1, tlo2}	//	two inputs, two outputs
	,	actuator_A	{"LX_Actuator_1", 	act1opin, acto1pol}
	,	actuator_B	{"LX_Actuator_2", 	act2opin, acto2pol}
	,	push_butt_1	{"LX_Push Button 1", pbut1in, pb1pol1, but1opin1, but1opol1, but1opin2, but1opol2}
	,	push_butt_2	{"LX_Push Button 2", pbut2in, pb2pol1, but2opin1, but2opol1, but2opin2, but2opol2}
	,	alarm		{"LX_Alarm", 		alarm_pin}
{
		if	(level_crossing_name)
			id = level_crossing_name;
    	alarm.clr();
}
    //	public member functions for LevelCrossing
	bool	set				(uint32_t);	//	energise actuators
	void	read_update		();	//	Updates input info in all included MovingParts
	bool	gates_are_locked()	const;	//	Returns 'true' if gates are locked !
	bool	safe_for_road	()	const;	//	Returns 'true' if gates are locked AND all 4 gates safe for road
	bool	safe_for_rail	()	const;	//	Returns 'true' if gates are locked AND all 4 gates safe for rail
	char *	report	(char *)	const	;
	uint32_t	tolog	()	const	;
}	;


#endif



