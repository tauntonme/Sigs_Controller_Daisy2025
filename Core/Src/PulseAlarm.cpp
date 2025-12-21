/*
 * PulseAlarm.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"PulseAlarm.hpp"

PulseAlarm::PulseAlarm	(const char * alarm_name, OUTPIN a)
	: OutPin{alarm_name, a}
{
	OutPin.set_pin	(false);
	if	(alarm_name)	id = alarm_name;
}

void	PulseAlarm::read_update	()	{
	uint32_t	current_time = HAL_GetTick();
	if	(alarm_on && (current_time > alarm_timer))	{	//	part-period timed out ?
		if	(OutPin.read())	{							//	toggle output, swap mark/space
			alarm_timer = current_time + space;			//	restart timer
			OutPin.set_pin	(false);
		}
		else	{
			alarm_timer = current_time + mark;
			OutPin.set_pin	(true);
		}
	}
}

void	PulseAlarm::mark_space	(int mark_ms, int space_ms)	{	//	set mark and space times, each in millisecs
	mark = mark_ms;
	space = space_ms;
}

void	PulseAlarm::start_alarm	()	{							//	Start alarm
	alarm_on = true;
	alarm_timer = HAL_GetTick() + mark;
	OutPin.set_pin	(true);
}

void	PulseAlarm::clr	()	{									//	Stop alarm
	alarm_on = false;
	OutPin.set_pin	(false);
}

OUTPIN	PulseAlarm::get_pin_num	()	const	{
	return	(OutPin.get_pin_num())	;
}


