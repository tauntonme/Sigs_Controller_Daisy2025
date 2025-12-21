/*
 * Utils.cpp	INTENDED TO BE PROJECT SPECIFIC
 *
 *  Created on: Feb 11, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 *
 *	For menus, and functions executed through menus.
 *	Put such clutter here,
 *
 *	Keep 'Project.cpp' for the main logical flow of your project
 */
#include	<cstdio>			//	for sprintf
#include	<cstring>			//	for strlen

//#include	"Project.hpp"
#include	"parameters.hpp"
#include	"Serial.hpp"
#include	"CmdLine.hpp"
#include	"settings.hpp"
#include	"SingleInput.hpp"
#include	"SingleOutput.hpp"

extern	Serial	pc	;//, *Com_ptrs[];

extern	bool	set_time	(struct parameters & a)	;
extern	bool	set_date	(struct parameters & a)	;
extern	bool	get_input_bit_debounced	(INPIN which_input, uint32_t how_many)	;
extern	bool	can_node_command	(int node, int command, int data)	;
extern	bool	send_can_msg	(int node11bit, uint8_t * TxData, int len)	;


//	Prototypes for functions included in 'settings_data' menu structure
bool	null_cmd	(parameters &);
bool	set_defaults_cmd	(parameters &);
bool	set_one_wrapper_cmd	(parameters &);

//enum	class	MenuType	{MENU, SETTINGS}	;	//	in 'parameters.hpp'
/*bool	ver_cmd	(parameters & par)	{
	char * p = get_version();
	pc.write	(p, strlen(p));
	pc.write	("\r\n", 2);
	return	(true);
}*/

struct cli_menu_entry_set	const  settings_data[]
{    // Can not form pointers to member functions.
	{"?",     	"Lists all user settings, alters none", null_cmd, static_cast<int32_t>(MenuType::SETTINGS)}, //	Examples of use follow
	{"defaults","Reset settings to factory defaults", set_defaults_cmd},     //	restore factory defaults to all settings
	{"mca",		"My CAN Address", 		set_one_wrapper_cmd, 	0, 0x7ff, 6, 1.0},   //
	{nullptr},	//	June 2023 new end of list delimiter. No need to pass sizeof
}	;

//	Prototypes for functions included in 'pc_command_list' menu structure
bool	menucmd	(parameters &);
bool	rtc_cmd	(parameters &);
bool	cnc_cmd	(parameters &);
bool	adc_cmd	(parameters &);
bool	st_cmd	(parameters &);
bool	sd_cmd	(parameters &);
bool	edit_settings_cmd	(parameters &);
#if 0
/**
struct  cli_menu_entry_set      const loco_command_list[] = {
List of commands accepted from external pc through non-opto isolated com port 115200, 8,n,1
*/
struct  cli_menu_entry_set	const pc_command_list[] = {
    {"?", "Lists available commands", 	menucmd, static_cast<int32_t>(MenuType::MENU)},
	{"rtc", "real time clock buggery", 	rtc_cmd},
	{"adc", "check adc dma working", 	adc_cmd},
	{"st", "real time clock Time", 		st_cmd},
	{"sd", "real time clock Date", 		sd_cmd},
	{"us", "user settings", 			edit_settings_cmd},
    {"nu", "do nothing", null_cmd},
    {nullptr},	//	June 2023 new end of list delimiter. No need to pass sizeof
}   ;
#endif


bool    AT_cmd (struct parameters & par)	;	//
bool    atb_cmd (struct parameters & par)	;	//
bool    vi_cmd (struct parameters & par)	;	//
bool    seton_cmd (struct parameters & par)	;	//
bool    clroff_cmd (struct parameters & par)	;	//
bool    i_cmd (struct parameters & par)	;	//
bool    ce_cmd (struct parameters & par)	;	//
bool    sb_cmd (struct parameters & par)	;	//
bool    pl_cmd (struct parameters & par)	;	//
bool	can_report_cmd	(parameters & par)	;
bool    x_cmd (struct parameters & par)	;	//

/**
struct  cli_menu_entry_set      const loco_command_list[] = {
List of commands accepted from external pc through non-opto isolated com port 115200, 8,n,1
*/
struct  cli_menu_entry_set       const pc_command_list[] {
    {"?", "Lists available commands", menucmd},
	{"AT", "AT commands for setting up Bluetooth modules", AT_cmd},
	{"atb", "Calc counter for new baud rate", atb_cmd},
	{"canrep", "Test for CAN bus errors", 	can_report_cmd},
	{"cnc", "Can Node1-127, Command0-255, Param0-255", 	cnc_cmd},
    {"vi", "Fifth, do nothing very much at all really", vi_cmd},
    {"set", "set one or  more output on", seton_cmd},
    {"clr", "clr one or more output off", clroff_cmd},
	  {"x", "test 0x hexadecimal input", x_cmd},
	  {"i", "read an input", i_cmd},
	  {"ce", "can errors", ce_cmd},
//	  {"fwistest", "user settings", fwistest},
	  {"sb", "signals buggery", sb_cmd},
	  {"rtc", "real time clock buggery", rtc_cmd},
	  {"st", "real time clock Time", st_cmd},
	  {"sd", "real time clock Date", sd_cmd},
	  {"pl", "pins lister", pl_cmd},
	  {"us", "user settings", edit_settings_cmd},
    {"nu", "do nothing", null_cmd},
    {nullptr},	//	June 2023 new end of list delimiter. No need to pass sizeof
}   ;

//CommandLineHandler	command_line_handler	(pc_command_list, & pc);	//	Nice and clean

//	************* Create Utilities *****************
extern	UART_HandleTypeDef	huart1;	//	uarts used in this project
extern	UART_HandleTypeDef	huart2;	//	uarts used in this project
extern	I2C_HandleTypeDef 	hi2c1;	//	I2C
extern	bool	set_output_bit	(OUTPIN which_output, bool hiorlo)	;

Serial				ctrl(huart1);
Serial				pc(huart2);		//, * Com_ptrs[];
i2eeprom_settings	my_settings	(settings_data, hi2c1)	;	//	Create one i2eeprom_settings object named 'j_settings'
//CommandLineHandler	command_line_handler	(pc_command_list, &pc);	//	Nice and clean
CommandLineHandler	command_line_handler	(pc_command_list);	//	Nice and clean


extern	void	CAN_status_report	()	;	//	6th March 2024
bool	can_report_cmd	(parameters & par)	{
	CAN_status_report	()	;	//	6th March 2024
	return	(true);
}


bool    AT_cmd 	(struct parameters & par)	{	//
	pc.write	(par.command_line, strlen(par.command_line));
	return	(true);
}


bool	atb_cmd 	(struct parameters & par)	{	//	baud setting
	/*
AT+BAUD AT+BAUD1 – sets the baud rate to 1200 and returns OK1200
AT+BAUD2 – sets the baud rate to 2400 and returns OK2400

Other possible baud rates are
1——— 1200	AT+BAUD8,0,0
2——— 2400
3——— 4800
4——— 9600
5——— 19200
6——— 38400
7——— 57600
8——— 115200
9——— 230400
A——— 460800
B——— 921600
C——— 1382400
*/
	const	uint32_t	baud_list[] = {1200, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
			, 230400, 460800, 921600, 1382400};
	char	t[100] {0};
	uint32_t	el = (int)par.flt[0];
	int	len;
	if	((el < 1) || (el >= (sizeof(baud_list) / sizeof(uint32_t))))	{
		len = sprintf	(t, "Wrong baud [%ld], range is 1 to %d\r\n", el, (sizeof(baud_list) / sizeof(uint32_t)) - 1);
		pc.write	(t, len);
		return	(false);
	}
//	int x = UART_DIV_SAMPLING16(HAL_RCC_GetPCLK2Freq(), newbaud);
	uint32_t x = UART_DIV_SAMPLING16(HAL_RCC_GetPCLK2Freq(), baud_list[el]);
	len = sprintf	(t, "Cntr for baud %ld is %ld\r\n", baud_list[el], x);

	USART1->BRR = x;

	pc.write	(t, len);
	len = sprintf	(t, "BRR=%ld\r\n", USART1->BRR);
	pc.write	(t, len);
	return	(true);
}


bool	cnc_cmd	(parameters & par)	{	//	Now must use 0x addressing
	//							CAN Address		Command			Parameter
//	return	(can_node_command((int)par.flt[0], (int)par.flt[1], (int)par.flt[2]));
	uint8_t	data[4];
	data[0] = (uint8_t)par.flt[1];
	data[1] = (uint8_t)par.flt[2];
	return	(send_can_msg	((int)par.flt[0], data, 2)	)	;
}


bool    x_cmd (struct parameters & par)	{	//	Test using 0x prefix - it works!
	char	t[180];
	int	len = sprintf	(t, "%s, decimal %ld",par.command_line , (int32_t)par.flt[0]);
	pc.write	(t, len);
	return	(true);
}


bool    i_cmd (struct parameters & a)	{	//	read an input
	char	t[55];
	INPIN b = (INPIN)a.flt[0];
	int	len = sprintf(t, "In%d = %c\r\n", static_cast<int>(b), get_input_bit_debounced(b,1)	? 'T' : 'F');
	pc.write(t, len);
    return	(true);
}


bool    seton_cmd (struct parameters & a)	{	//	set one or more outputs ON
	OUTPIN	j;
	int	k = a.numof_floats;
	while	(k > 0)	{
		j = (OUTPIN)a.flt[--k];
		set_output_bit	(j, true);
	}
    return	(true);
}

bool    clroff_cmd (struct parameters & a)	{	//	set one or more outputs OFF
	OUTPIN	j;
	int	k = a.numof_floats;
	while	(k > 0)	{
		j = (OUTPIN)a.flt[--k];
		set_output_bit	(j, false);
	}
    return	(true);
}


/**
*   void    menucmd 		(struct parameters & a)
*	void	list_settings	(const menu_entry_set * list)
*   List available terminal commands to pc terminal. No sense in touch screen using this
*/
void	list_settings	(const cli_menu_entry_set * list)	{
	int i = 0;
	int len;
	int32_t	ival;
	float	fval;
	char	t[200];
	char	ins_tab[2] {0,0};
	extern 	char * 	get_version	();//	{	return	(version_str);	}
	pc.write	(get_version(), strlen(get_version()));
	pc.write	("\r\n", 2);
	len = sprintf	(t, "Listing %s Functions and Values :\r\n", list[0].min ? "SETTINGS" : "MENU");
	pc.write	(t, len);
	while	(list[i].cmd_word)	{
		(6 > strlen(list[i].cmd_word)) ? ins_tab[0] = '\t' : ins_tab[0] = 0;
		len = sprintf	(t, "[%s]\t%s%s"
			, list[i].cmd_word
			, ins_tab
			, list[i].description	);
		pc.write	(t, len);	//	This much common to MENU and SETTINGS
		if	(list[0].min)	{	//	is SETTINGS, not MENU
			if	(my_settings.read1(list[i].cmd_word, ival, fval))	{
				(6 > strlen(list[i].cmd_word)) ? ins_tab[0] = '\t' : ins_tab[0] = 0;
				len = sprintf	(t, "\tmin%ld, max%ld, def%ld\t[is %ld]\t(float mpr %.2f)"
					, list[i].min
					, list[i].max
					, list[i].de_fault
					, ival
					, fval	);
				pc.write	(t, len);
			}
			else	pc.write	("Settings Read Error\r\n", 21);
		}	//	Endof is SETTINGS, not MENU
		pc.write	("\r\n", 2);
		i++;
	}	//	Endof 	while	(list[i].cmd_word)
	pc.write("End of List of Commands\r\n", 25);
}


bool    menucmd (struct parameters & par)     {
	list_settings	(par.command_list)	;
    return	(true);
}


bool	edit_settings_cmd (struct parameters & par)     {	//	Here from CLI having found "us "
	bool	rv =	(my_settings.edit	(par));
	list_settings	(settings_data)	;
	return	(rv)	;
}


bool    set_one_wrapper_cmd (struct parameters & par)     {	//	Called via edit, a.second_word found in edit
	return	(my_settings.set_one	(par));
}


bool    null_cmd (struct parameters & par)     {
	const char t[] = "null command - does nothing useful!\r\n";
	pc.write(t, strlen(t));
    return	(true);
}


bool    set_defaults_cmd (struct parameters & par)     {
	return	(my_settings.set_defaults());
}


extern	int32_t	run_mode;
bool    set_runmode_cmd (struct parameters & par)     {
	run_mode = (int32_t)par.flt[0];
	return	(true);
}


void	check_commands	()	{	//	Called from ForeverLoop
/**
 * bool	Serial::test_for_message	()	{
 *
 * Called from ForeverLoop at repetition rate
 * Returns true when "\r\n" terminated command line has been assembled in lin_inbuff
 */
	char * buff_start = pc.test_for_rx_message();
	if	(buff_start)
		command_line_handler.CommandExec(buff_start);
}


extern	uint32_t	can_errors;
extern	void	rtc_buggery	()	;
extern	void	adc_cnt_report	()	;

bool	adc_cmd	 (struct parameters & par)     {
#ifdef	USING_ANALOG
	adc_cnt_report();
#endif
	return	(true);
}


#ifdef	USING_RTC
extern	bool	set_time	(struct parameters & par)	;
extern	bool	set_date	(struct parameters & par)	;

bool	st_cmd	 (struct parameters & par)     {
	return	(set_time	(par));
}


bool	sd_cmd	 (struct parameters & par)     {
	return	(set_date	(par));
}

bool	rtc_cmd	 (struct parameters & par)     {
	rtc_buggery();
	return	(true);
}


#else
bool	st_cmd	 (struct parameters & par)     {
	return	(false);
}


bool	sd_cmd	 (struct parameters & par)     {
	return	(false);
}


bool	rtc_cmd	 (struct parameters & par)     {
	return	(false);
}


#endif






