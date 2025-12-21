/*
 * SingleInput.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 *
 *      See SingleInput.hpp for documentation
 */

#include	"SingleInput.hpp"
#include	"Serial.hpp"

#include	<cstdio>	//	sprintf
#include	<cstring>	//	for strcat

extern	Serial pc;
extern	bool	get_input_bit_debounced	(INPIN which_in, uint32_t how_many)	;	//	from Shift_Reg_SPI.cpp

io_config_err_log	IP_pinlog[static_cast<int>(INPIN::IN_LAST) + 1 ]	;


SingleInput::SingleInput	(const char * myident, INPIN n)
	: SingleInput {myident, n, true, nullptr, nullptr}
{	//	Constructor without polarity
}


SingleInput::SingleInput	(const char * myident, INPIN n, POLARITY p)
	: SingleInput {myident, n, p, nullptr, nullptr}
{	//	Constructor
}


SingleInput::SingleInput	(const char * myident, INPIN n, POLARITY p = true, bool(*rise)() = nullptr, bool(*fall)() = nullptr)
	: input_ionum{n}
	, polarity{p}
	, rise_func{rise}
	, fall_func{fall}
{	//	Constructor
	if	(myident)
		id = myident;
	m_pin_use_checker	()	;	//	Record fact that pin 'inpin_ionum' has been used
}


bool	SingleInput::read_update	()	{	//	Copies present into previous, updates present with latest read of input pin
//	int	ones_count = 0;
//	int	threshold = (DEFAULT_DEBOUNCE_VALUES >> 1);
//	int	count = DEFAULT_DEBOUNCE_VALUES;
//	uint32_t test_bit = 0x40000000L;
	previous = present;		//	Save for possible edge detection
	present = ((get_input_bit_debounced	(input_ionum ,DEFAULT_DEBOUNCE_VALUES)) ^ polarity);
#if 0
	while	(count--)	{
		if	((in_history & test_bit) != 0)
			ones_count++;
		test_bit >>= 1;
	}
	present = (ones_count > threshold);

	 * bool	get_input_bit_debounced (text 50972 before moving functionality into SerialInput)
	 * A 'De-Bounce' implementation
	 * 			Only call is from class SingleInput::read_update().
	 *
	 * 	Inputs are scanned (and outputs updated) regularly, probably 50 times per second.
	 *
	 * Most recent 31 input[n] bit reads stored in uint32_t ins_history[n], newest in bit 30, oldest bit 0
	 * (chose 31 over 32 bits so that number positive regardless of 'int' ot 'uint')
	 * function takes odd number for samples and tests this many from most recent back in time
	 * returns 'true' for more '1's than '0's, false for more '0's than '1's
	 * On error sets bits in 'input_test_error_flags' and returns false
	 */
	//bool	get_input_bit_debounced	(uint32_t which_input, uint32_t ave_of_how_many)	{
	bool	get_input_bit_debounced	(INPIN which_in, uint32_t ave_of_how_many)	{
		uint32_t threshold;
		uint32_t ones_count = 0;
		uint32_t test_bit = 0x40000000L;
	//	if	(which_in >= (IO_DAISY_CHAIN_BYTE_LEN << 3))	{	//	'which_in' must be in range of numof inputs
		if	((ave_of_how_many & 1) == 0)	{	//	check for 'how_many' is even, should be odd
			ave_of_how_many--;					//	smallest even number possible here was 2, round down to next lower odd
			input_test_error_flags |= 4;	//	set error flag but proceed
		}								//	'how_many' now always odd number
		ones_count	= 0;
		test_bit	= 0x40000000L;	//	First bit to test is bit 30
		threshold = ave_of_how_many / 2;	//	when how_many == 1, threshold == 0
									//	when how_many == 3, threshold == 1 etc
		while	(ave_of_how_many--)	{
			if	((ins_history[which_in] & test_bit) != 0)
				ones_count++;
			test_bit >>= 1;		//	test bit30 first, next bit 29 etc
		}
		return	(ones_count > threshold);
	//	if	(ones_count > threshold)
	//		return	(true);
	//	return	(false);
	}


#endif
	if	(present != previous)	{	//	input has changed
		if	(present)	{	//	is rising edge
			if	(rise_func)
				rise_func	();
		}
		else	{	//	is falling edge
			if	(fall_func)
				fall_func	();
		}
	}
	return	(present);
}


bool	SingleInput::is_active	()	const	{	//	e.g. use if(button1.is_active()).	Returns true for active, false for inactive
	return	(present)	;	//	Returns latest debounced input read
}


bool	SingleInput::is_rising_edge		()	const	{	//	Returns true when rising edge detected	i.e.	(present && !previous)
	return	(present && !previous)	;	//	Found __--
}


bool	SingleInput::is_falling_edge		()	const	{	//	Returns true when falling edge detected	i.e.	(!present && previous)
	return	(!present && previous)	;	//	Found --__
}


INPIN	SingleInput::get_pin_num	()	const	{
	return	(input_ionum)	;
}	//


void	SingleInput::set_polarity	(bool p)	{
	polarity = p;
}


char *	SingleInput::report	(char * dest )	const	{
	sprintf	(dest, "Input is [%s],\tpin [Ip%02d, {%c}]\r\n", m_txt(), static_cast<int>(get_pin_num()), is_active() ? 'T':'F');
	return	(dest);
}

//IO_PIN_ALLOC_ERR_REC_LEN
bool	SingleInput::m_pin_use_checker	()	{
	char	t[60];
	int len;
	int	cnt;
	int	intio = static_cast<int>(input_ionum);	//	need integer representation for array index
	if	((input_ionum < INPIN::IN_FIRST) || (input_ionum > INPIN::IN_LAST))	{
		len = std::sprintf	(t, "In SingleInput - Invalid Input num %d\r\n", static_cast<int>(input_ionum));
		pc.write	(t, len);
		return	(false);
	}						//	Pass here only if pin number is in range
		//	Is in range
	cnt = IP_pinlog[intio].count;
	if	(cnt == 0)	{	//	Pin is free to use
		IP_pinlog[intio].t[cnt] = id;
		IP_pinlog[intio].count++;
		len = std::sprintf	(t, "Allocating Input %d, [%s]\r\n", intio, id);
		pc.write	(t, len);
		return	(true);
	}
	else	{	//	Pin is not free to use as proved by count > 0
		if	(cnt < IO_PIN_ALLOC_ERR_REC_LEN)	{	//	still have room on list
			IP_pinlog[intio].t[cnt] = id;
			IP_pinlog[intio].count++;
		}
		else	{	//	cnt >= IO_PIN_ALLOC_ERR_REC_LEN, List is already full
			IP_pinlog[intio].t[IO_PIN_ALLOC_ERR_REC_LEN] = "Too Many Errors";
		}
		//	now report problem
		std::sprintf	(t, "Error! Multiple use of INPIN [%d, %s] ", intio, id);
		for	(int i= 0; i < IO_PIN_ALLOC_ERR_REC_LEN; i++)	{
			std::strcat	(t, IP_pinlog[intio].t[i]);
			std::strcat	(t, ",");
		}
//		len = std::sprintf	(t, "Error! Multiple use of INPIN %d, [%s], [%s]\r\n"
//				, input_ionum, IP_pinlog[input_ionum].t[0], id);
		pc.write	(t, strlen(t));
		return	(false);
	}
}




