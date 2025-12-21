/*
 * LinesideSignal.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"LinesideSignal.hpp"
#include	<cstdio>	//	for sprintf
#include	<cstring>	//	for strcat
extern	bool	send_can_msg	(int node11bit, uint8_t * TxData, int len)	;


/*
struct	can_signal_stack_element	{	//	Array of these defined in LinesideSignal.cpp
	uint8_t	can_id {0};
	uint8_t	can_len {0};
	uint8_t	can_data[8] {0};
	CAN_LinesideSignal * which_signal {0};
}	;

*/

#include	"Serial.hpp"
extern	Serial	pc	;//, *Com_ptrs[];

class	struct_can_signal_stack	{
	int	stack_ptr {0};
	can_signal_stack_element	stack[20];
  public:
	void	push	(CAN_LinesideSignal * sigaddr, uint16_t id)	{
		stack[stack_ptr].which_signal = sigaddr;
		stack[stack_ptr].can_id = id;
		stack_ptr++;
	}	;
	int		get_size	()	{	return	(stack_ptr);	}	;
	bool	compare_ids	(int position, int search_id)	{
		if	(search_id == stack[position].can_id)
			return	(true);
		return	(false);
	}	;
	void	report	()	{
		int	ptr {0};
		char	t[88];
		int	len;
		while	(ptr < stack_ptr)	{
			len = sprintf	(t, "Stacking CAN sig this 0x%x, id 0x%x\r\n", (int)stack[ptr].which_signal, stack[ptr].can_id);
			pc.write	(t, len);
			stack[ptr].which_signal->report(t);
			ptr++;
		}
	}	;
}
	can_signal_stack	;	//	create one struct_can_signal_stack

bool	find_CANsig	(int & found_at, int search_id)	{	//	Search for 0x230 etc CAN address in can_signal_stack
	int	search_len = can_signal_stack.get_size();		//	Sets found_at to position in list if found
	int	temp {0};
	bool	rv = false;
//	*found_at = -1;	//	not found exit code
	while	(temp < search_len)	{
		if	(can_signal_stack.compare_ids(temp, search_id))	{
			found_at = temp;
			rv = true;
			temp = search_len;
		}
		temp++;
	}
	return	(rv);
}

void	cansigstackreport	()	{
	int	fnd_at {0};
	int	len;
	char	t[66];
	can_signal_stack.report();
	for	(int i = 0; i < 0x300; i++)	{
		if	(find_CANsig(fnd_at, i))
		{
			len = sprintf	(t, "Got CAN id %x at %x\r\n", i, fnd_at);
			pc.write	(t, len);
		}
	}
}

CAN_LinesideSignal::CAN_LinesideSignal	(const char * signal_name, int can_addr, int model)
	:	CAN_ADDR {can_addr}
	,	MODEL	{model}
	,	id	{signal_name}
{		//	CAN bus signal
	set_danger	()	;	//	CAN bus signal comes up in DANGER by design
	can_signal_stack.push	(this, can_addr);
}	;

char *	CAN_LinesideSignal::report(char * t)	const	{
	int s = static_cast<int>(commanded_state);
//	int	len = sprintf	(t, "CANsig this 0x%x, id 0x%x, state %d\r\n", (int)this, CAN_ADDR, static_cast<int>(commanded_state));
	int	len = sprintf	(t, "CANsig id 0x%x, state %d, %s\r\n", CAN_ADDR, s, SigStatesText[s]);
	pc.write(t, len);
	return	(t)	;
}

void	CAN_LinesideSignal::SignalVerifyRequest()	{}
void	CAN_LinesideSignal::set_clear()				{	set_to	(SignalStates::CLEAR);	}
void	CAN_LinesideSignal::set_danger()			{	set_to	(SignalStates::DANGER);	}
void	CAN_LinesideSignal::set_caution()			{	set_to	(SignalStates::CAUTION);	}
void	CAN_LinesideSignal::set_occulting()			{	set_to	(SignalStates::OCCULTING_YELLOW);	}
void	CAN_LinesideSignal::set_black()				{	set_to	(SignalStates::BLACK);	}

bool	CAN_LinesideSignal::set_to	(SignalStates s){
	uint8_t	t[4]	{1, static_cast<uint8_t>(s), 0}	;
	commanded_state = s;
	return	(send_can_msg	(CAN_ADDR, t, 2))	;	//	Issue CAN command from Controller to Signal peripheral
}

bool	CAN_LinesideSignal::set_toi	(uint8_t s){
	return	(set_to	(static_cast<SignalStates>(s)));
}


/*
 *
	void	SignalVerifyRequest	();	//	Issue CAN bus request message
	void	set_clear	();
	void	set_danger	();	//	default at power-up
	void	set_caution	();
	void	set_occulting	();	//	Do not EVER call this flashing! (Mustn't upset the gricers)
	void	set_black	();
	bool	can_sig_good	()	{	return	can_sig_good_flag;	}
	char *	report	(char *)	const	;
 */




Wired_LinesideSignal::Wired_LinesideSignal	(const char * signal_name, INPIN ClearInputNum, INPIN DangerInputNum,
				OUTPIN ClearOutputNum, OUTPIN DangerOutputNum)
	:	In_A		{"Signal_Inp_C", ClearInputNum, true}
	,	In_B		{"Signal_Inp_D", DangerInputNum, true}
	,	Out_A		{"Signal_Out_C", ClearOutputNum, true}
	,	Out_B		{"Signal_Out_D", DangerOutputNum, true}
{	//	This is the "constructor"	//
	set_danger	()	;	//	also sets commanded_state
	if	(signal_name)
		id = signal_name;
}	;


uint32_t	Wired_LinesideSignal::get_two_inbits	()	const	{		//	The state read back from input lines
	uint32_t	rv = 0;
	if	(In_A.is_active())	rv |= 1;
	if	(In_B.is_active())	rv |= 2;
	return	(rv)	;							//	Returns 0, 1, 2 or 3
}

uint32_t	Wired_LinesideSignal::get_two_outbits	()	const	{		//	The state read back from output buffers
	uint32_t	rv = 0;
	if	(Out_A.read())	rv |= 1;
	if	(Out_B.read())	rv |= 2;
	return	(rv)	;							//	Returns 0, 1, 2 or 3
}

void	Wired_LinesideSignal::set_danger	()	{
	Out_A.set_pin	(false);
	Out_B.set_pin	(true);
	commanded_state = SignalStates::DANGER;
}

void	Wired_LinesideSignal::set_caution	()	{
	Out_A.set_pin	(true);
	Out_B.set_pin	(true);
	commanded_state = SignalStates::CAUTION;
}

void	Wired_LinesideSignal::set_occulting	()	{	//	OCCULTING_YELLOW
	//	Requires yellow flash with on time > off time
	commanded_state = SignalStates::OCCULTING_YELLOW;
}

void	Wired_LinesideSignal::set_clear	()	{
	Out_A.set_pin	(true);
	Out_B.set_pin	(false);
	commanded_state = SignalStates::CLEAR;
}

void	Wired_LinesideSignal::set_black	()	{
	Out_A.set_pin	(false);
	Out_B.set_pin	(false);
	commanded_state = SignalStates::BLACK;
}

void	Wired_LinesideSignal::set_to	(SignalStates state_to_set)	{	//	Affects two outputs only
	commanded_state = state_to_set;	//	record most recent setting
	switch	(state_to_set)	{
		case	SignalStates::DANGER:
			set_danger	()	;
			break;
		case	SignalStates::CAUTION:
			set_caution	()	;
			break;
		case	SignalStates::OCCULTING_YELLOW:
			set_occulting	()	;
			break;
		case	SignalStates::CLEAR:
			set_clear	()	;
			break;
		case	SignalStates::BLACK:
			set_black	()	;
			break;
		default:
//			pc.write("Bad", 3);
			break;
	}
}


void	Wired_LinesideSignal::read_update	()	{	//	Affects two outputs only
	const	int	OCCULT_ON_MS	=	300;
	const	int	OCCULT_OFF_MS	=	100;

	In_A.read_update();	//	There are few if any reasons to read inputs back, except
	In_B.read_update();	//	to determine signal head type, head presence or absence.
	if	((commanded_state == SignalStates::OCCULTING_YELLOW) && (flash_timer < HAL_GetTick()))	{
		if	(flash_state)
			flash_timer = HAL_GetTick() + OCCULT_ON_MS;
		else
			flash_timer = HAL_GetTick() + OCCULT_OFF_MS;
		flash_state = !flash_state;
		Out_A.set_pin	(flash_state);
		Out_B.set_pin	(flash_state);
	}
}


char *	Wired_LinesideSignal::report	(char * dest)	const	{
	char	t[60];
	sprintf	(dest, "Signal is [%s], ", txt());
	strcat	(dest, In_A.report	(t));
	strcat	(dest, In_B.report	(t));
	strcat	(dest, Out_A.report	(t));
	strcat	(dest, Out_B.report	(t));
	strcat	(dest, "\r\n");
	strcat	(dest,	 (In_A.is_active() && !In_B.is_active())? "Clear" :
			(!In_A.is_active() && In_B.is_active())? "Danger" : "Wrong");
//		pc.write(dest, strlen(dest));
	return	(dest);
}	;





