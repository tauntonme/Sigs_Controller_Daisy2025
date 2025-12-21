/*
 * SingleOutput.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 *
 *      See SingleOutput.hpp for documentation
 *      SingleOutput files also contain pincounters and pin use logs
 */

#include	"Serial.hpp"
#include	"SingleInput.hpp"
#include	"SingleOutput.hpp"
#include	<cstdio>	//	for sprintf
extern	Serial pc;

extern	bool	set_output_bit	(OUTPIN which_output, bool hiorlo)	;	// in Shift_Reg_SPI.cpp
extern	bool	get_outreg_1bit	(OUTPIN bitnum)	;


SingleOutput::SingleOutput	(const char * myid, OUTPIN which_output)	//	Constructor without polarity
		: SingleOutput {myid, which_output, true}	//	Delegate initialisation to SingleOutput::
{													//	'SingleOutput	(const char * myid, OUTPIN which_output, POLARITY p)'
}

SingleOutput::SingleOutput	(const char * myid, OUTPIN which_output, POLARITY p)
		: output_ionum{which_output}
		, polarity{p}
{	//	Constructor including polarity
	if	(myid)
		id = myid;
	m_pin_use_checker	()	;	//	Record fact that pin 'which_output' has been used
}


const char * SingleOutput::m_txt	()	const	{
	return	(id);
}


bool	SingleOutput::set_pin			(bool output_state)	{	//	value 'output_state ^ polarity' is output
	//	set_output_bit returns true when bit has been changed
	return	(set_output_bit	(output_ionum, (output_state ^ polarity)));
}


bool	SingleOutput::read	() const	{
	return	(get_outreg_1bit(output_ionum))	;//	return last value set in output buffer
}


OUTPIN	SingleOutput::get_pin_num	() const	{
	return	(output_ionum);
}


void	SingleOutput::set_polarity	(bool p)	{
	polarity = p;
}


/*struct	io_config_err_log	{		//	To help identify multiple use of same pin problems
	const char * t[4] = {"Spare\0", nullptr, nullptr, nullptr};	//	can form list of conflicting items
	uint32_t	pin		= 0L;
	uint32_t	count	= 0L;
}	;
*/
io_config_err_log	OP_pinlog[static_cast<int>(OUTPIN::OUT_LAST) + 1]	;

extern	io_config_err_log	IP_pinlog[];

bool	SingleOutput::m_pin_use_checker	()	{
	char	t[60];
	int len;
	int	intio = static_cast<int>(output_ionum);
	if	((output_ionum < OUTPIN::OUT_FIRST) || (output_ionum > OUTPIN::OUT_LAST))	{
		len = std::sprintf	(t, "In SingleOutput - Invalid Output num %d\r\n", intio);
		pc.write	(t, len);
		return	(false);
	}						//	Pass here only if pin number is in range
	if	(OP_pinlog[intio].count == 0)	{	//	Pin is free to use
		OP_pinlog[intio].t[0] = id;
		OP_pinlog[intio].count++;
		return	(true);
	}
	else	{	//	Pin is not free to use
		len = std::sprintf	(t, "Error! Multiple use of OUTPIN %d, [%s], [%s]\r\n", intio, OP_pinlog[intio].t[0], id);
		pc.write	(t, len);
		return	(false);
	}
}


char *	SingleOutput::report	(char * dest)	const	{
	std::sprintf	(dest, "Output is [%s],\tpin [Op%02d, {%c}]\r\n", m_txt(), static_cast<int>(get_pin_num()), read() ? 'T':'F');
	return	(dest);
}


void	pin_lister	()	{
	int		len;
	char	t[100];
#if 0
	equivalent to /* */ but not pissed off by finding /* */
#endif
	pc.write	("Listing all Inputs\r\n", 16);
	for	(int i = static_cast<int>(INPIN::IN_FIRST); i < static_cast<int>(INPIN::IN_LAST); i++)	{
		len = std::sprintf	(t, "In%2d [%s]\r\n", i, IP_pinlog[i].t[0]);
		pc.write	(t, len);
	}
	pc.write	("\r\nListing all Outputs\r\n", 19);
	for	(int i = static_cast<int>(OUTPIN::OUT_FIRST); i < static_cast<int>(OUTPIN::OUT_LAST); i++)	{
		len = std::sprintf	(t, "Out%2d [%s]\r\n", i, OP_pinlog[i].t[0]);
		pc.write	(t, len);
	}
}



