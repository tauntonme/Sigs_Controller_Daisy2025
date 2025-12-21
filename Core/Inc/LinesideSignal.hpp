/*
 * LinesideSignal.hpp
 *
 * LinesideSignal has two types of interface, hard wired, or CAN
 * This requires two classes, Wired_LinesideSignal and CAN_LinesideSignal
 *
 * There may be a way to combine these using inheritance or polymorphism but that's for another day
 *	Not important. Need to know what hardware we have when creating objects.
 *	Thereafter, 'Signl1' and 'Signl2' driven by functions of same name
 *
 *  Created on: Jan 20, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#ifndef INC_LINESIDESIGNALCLAS_HPP_
#define INC_LINESIDESIGNALCLAS_HPP_

#include	"SingleInput.hpp"
#include	"SingleOutput.hpp"
//#include	<string>
enum	class SignalStates	{
	BLACK,	//	Uses 4 bits of space in tolog ?? Need to look further at this - see below
	DANGER,					//	binary 0 0 1
	CAUTION,				//	binary 0 1 0
	CLEAR,					//	binary 0 1 1
	BLUE,					//	binary 1 0 0
	WHITE,					//	binary 1 0 1
	OCCULTING_YELLOW = 6	//	binary 1 1 0	in tolog, lose msb. Thus CAUTION and OCCULTING report same.
}	;//	States to which LED signals may be set. Semaphores to show DANGER when not set to CLEAR
	//	Note Occulting is flash where on_time > off_time. See https://www.railforums.co.uk/threads/flashing-signals.186537/
	char	*	const	SigStatesText[]  {{(char*)"BLACK"}
										, {(char*)"DANGER"}
										, {(char*)"CAUTION"}
										, {(char*)"CLEAR"}
										, {(char*)"BLUE"}
										, {(char*)"WHITE"}
										, {(char*)"OCCULTING_YELLOW"}
//										, {(char*)"7"}
//										, {(char*)"8"}
//										, {(char*)"9"}	//	need to guard against reading beyond end
	};

/*
class	LinesideSignal	{	//	Looking for a way to drive two very different signal types, wired and can, from one class
	SignalStates	commanded_state { SignalStates::DANGER };
	int			CAN_ADDR	{0};
	int			MODEL		{0};
	uint32_t	flash_timer	{ 0L };
	bool		flash_state { false };	//	Used in on/off toggle when 'occulting' (flashing)
	bool		can_sig_good_flag {false};
	const char	no_id[2] { '?', 0 };
	const char * id { no_id };
	const char * txt	()	const	{	return	(id);	}
public:
	LinesideSignal	(const char * , int can_addr, int model)	;		//	CAN bus signal
	LinesideSignal	(const char * , INPIN, INPIN, OUTPIN, OUTPIN )	;	//	Hard wired signal

}	;
*/

/**	class	CAN_LinesideSignal	{
 *
 * Lineside Railway Signals, coloured light.
 *	May have one or more light units as identified by 'model'
 *
 *	CAN_LinesideSignal functions set signal aspect.
 *	SignalVerifyRequest is the only function requiring a CAN bus response.
 *	Should a correct response be received, 'can_sig_good' flag to be set true
 */
class	CAN_LinesideSignal	{
	SignalStates	commanded_state { SignalStates::DANGER };
	int			CAN_ADDR	{0};
	int			MODEL		{0};
	uint32_t	flash_timer	{ 0L };
	bool		flash_state { false };	//	Used in on/off toggle when 'occulting' (flashing)
	bool		can_sig_good_flag {false};
	bool		signal_is_wrong	{ true };	//	New Aug 2024
	const char	noid[2] { '?', 0 };
	const char * id { noid };
	const char * txt	()	const	{	return	(id);	}	;
  public:
//	LinkedListNode	node;	//	Initialises pointers to nullptr
	void	SignalVerifyRequest	();	//	Issue CAN bus request message
	void	set_clear	();
	void	set_danger	();	//	default at power-up
	void	set_caution	();
	void	set_occulting	();	//	Do not EVER call this flashing! (Mustn't upset the gricers)
	void	set_black	();
	bool	set_to		(SignalStates);	//	set state to one of DANGER, CAUTION, CLEAR, BLACK, OCCULTING_Y
	bool	set_toi		(uint8_t);	//	set state to one of DANGER, CAUTION, CLEAR, BLACK, OCCULTING_Y
	bool	can_sig_good	()	{	return	can_sig_good_flag;	}
//	char *	report	(char *)	const	;
	char *	report	(char *)	const	;

	CAN_LinesideSignal	(const char * , int can_addr, int model)	;				//	CAN bus signal
};

struct	can_signal_stack_element	{	//	Array of these defined in LinesideSignal.cpp
	int			can_id {0};
	uint16_t	can_len {0};
	uint8_t		can_data[8] {0};
	CAN_LinesideSignal * which_signal {0};
}	;


/**	class	Wired_LinesideSignal	{
 *
 * Lineside Railway Signals, coloured light or semaphore, connect to two output lines from the controller,
 * 		and connect two 'status' output lines back to controller inputs.
 * 	That is, the physical hardware signal has 4 I/O (Input Outputs) :
 * 		Two status Outputs connected to controller Inputs informing the controller of signal status
 * 		Two command Inputs driven by controller Outputs to set signal to 'Danger', 'Clear', 'Caution', 'Black'.
 * 		Semaphores are to show 'Clear' only for the 'Clear' state, 'Danger' otherwise.
 *
 *
 *
 *
 */
class	Wired_LinesideSignal	{	//	Lineside railway signal, 3 aspect colour light or 2 aspect semaphore
	SignalStates	commanded_state { SignalStates::DANGER };
	uint32_t	flash_timer	{ 0L };
	bool		flash_state { false };	//	Used in on/off toggle when 'occulting' (flashing)
	bool		signal_is_wrong	{ true };	//	New Aug 2024
	const char	noid[2] { '?', 0 };
	const char * id { noid };
	SingleInput		In_A;	//	Two inputs to conform to the Moving Part Rule
	SingleInput		In_B;
	SingleOutput	Out_A;	//	Two outputs to define states 'Danger', 'Caution', 'Clear' and 'Black'
	SingleOutput	Out_B;
	const char * txt	()	const	{	return	(id);	}	;
  public:
//	LinkedListNode	node;	//	Initialises pointers to nullptr
	void	set_clear	();
	void	set_danger	();	//	default at power-up
	void	set_caution	();
	void	set_occulting	();	//	Do not EVER call this flashing! (Mustn't upset the gricers)
	void	set_black	();
	char *	report	(char *)	const	;
	uint32_t	tolog	()	const
	{
		return	(get_two_inbits() + (get_two_outbits() << 2) + (static_cast<uint32_t>(commanded_state) << 4));
	}	//	return 8 status bits

/*
 * LinesideSignal::LinesideSignal	(const char * signal_name,
 * 				INPIN ClearInputNum, INPIN DangerInputNum,
 * 				OUTPIN ClearOutputNum, OUTPIN DangerOutputNum)	:
 *
 */
	Wired_LinesideSignal	(const char * , INPIN, INPIN, OUTPIN, OUTPIN )	;	//	Hard wired signal

	void	set_to		(SignalStates);	//	set state to one of DANGER, CAUTION, CLEAR, BLACK, OCCULTING_Y
	void	read_update	();	//	Read state read back from input lines

	uint32_t	get_two_inbits	()	const	;	//	The state read back from the two input lines
	uint32_t	get_two_outbits	()	const	;	//	The state read back from the two output lines

	//	bool	is_semaphore();	//	Returns 'true' for semaphore detected, false otherwise (colour light assumed)
}	;	//	End of class	LinesideSignal	{


#endif



