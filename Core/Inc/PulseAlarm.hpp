/*
 * PulseAlarm.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 *
 *      Simple alarm, output alternates On/Off,
 *      the 'On' and 'Off' times in millisecs set to 'mark' and 'space'
 *      **NOT** interrupt driven, requires regular calls to 'read_update' to make it work.
 *
 */
//#include	"main.h"
#ifndef INC_PULSEALARMCLAS_HPP_
#define INC_PULSEALARMCLAS_HPP_

#include	"SingleOutput.hpp"

/**	class	PulseAlarm	{
 * Use e.g. : -
 *
 * PulseAlarm	MyAlarm	("MyAlarmName", Output pin number);
 *
 * Polarity setting removed as not useful. Swap mark/space to suit
 *
 */
class	PulseAlarm	{
	SingleOutput	OutPin	;
	uint32_t	mark	= 250	;
	uint32_t	space	= 250	;
	uint32_t	alarm_timer	= 0L;
	bool	alarm_on	= false	;
	const char	noid[2] {'?', 0};
	const char * id = noid;
public:

	PulseAlarm	(const char * alarm_name, OUTPIN a);// : OutPin{alarm_name, a}	;
	void	mark_space	(int mark_ms, int space_ms)	;	//	set mark and space times, each in millisecs
	void	start_alarm	()	;							//	Start alarm
	void	clr	()	;									//	Stop alarm
	void	read_update	()	;							//	Call at forever loop repetition rate
	OUTPIN	get_pin_num	()	const	;
}	;	//	End of class	PulseAlarm	{


#endif



