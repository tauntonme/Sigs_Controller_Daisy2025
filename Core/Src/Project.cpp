/*
 * Project.cpp
 *
 *  Created on: Feb 11, 2024
 *      Author: Jon Freeman B Eng Hons
 */


#include	<cctype>	//	isdigit
#include	<cstdio>
#include	<cstdlib>	//	strtof
#include	<cstring>	//	strlen	strncmp

#include	"Project.hpp"
#include	"parameters.hpp"
#include	"Serial.hpp"
#include	"CmdLine.hpp"
#include	"SingleInput.hpp"
#include	"SingleOutput.hpp"

//	Utils
//#define	COM_PORT	pc

//extern const char * get_version	();
extern	uint32_t	can_errors;
extern	Serial	pc	;//, *Com_ptrs[];
//extern	Serial	ctrl;//, *Com_ptrs[];

//extern	bool    fwistest (struct parameters & a)     ;
extern	void	sigs_buggery	()	;	//	In SignalsMain.cpp
extern	void	pin_lister	()	;	//	In RailwayHardware.cpp
extern	void	rtc_buggery	()	;
extern	bool	set_time	(struct parameters & a)	;
extern	bool	set_date	(struct parameters & a)	;

/**
 * bool	set_output_bit	(uint32_t which_out, bool hiorlo)	{
 * Function in "main.c"
 * Number of output BYTES defined as 'IO_DAISY_CHAIN_BYTE_LEN' defined in "main.c", (probably 8)
 * Number of outputs = BYTES * 8 (numbered 0 to 63 when IO_DAISY_CHAIN_BYTE_LEN == 8)
 *
 * Function returns 'false' if 'which_output' contains invalid output number, returns 'true' otherwise
 * Using hiorlo 'true' turns output on, 'false' off.
 */
//extern	"C"	bool	set_output_bit	(uint32_t which_output, bool hiorlo)	;
extern	bool	set_output_bit	(OUTPIN which_output, bool hiorlo)	;
//extern	"C"	bool	get_input_bit_debounced	(uint32_t which_input, uint32_t how_many)	;
extern	bool	get_input_bit_debounced	(INPIN which_input, uint32_t how_many)	;

extern	bool	edit_settings_cmd	(struct parameters & a);

bool	pl_cmd	 (struct parameters & a)     {
	pin_lister();
	return	(true);
}


bool	sb_cmd	 (struct parameters & a)     {
	sigs_buggery();
	return	(true);
}

/*bool    null_cmd (struct parameters & a)     {
	const char t[] = "null command - does nothing useful!\r\n";
	pc.write(t, strlen(t));
    return	(true);
}*/

bool    vi_cmd (struct parameters & a)	{	//
	pc.write	("Nothing for vi command to do!\r\n", 31);
    return	(true);
}

extern	void	ce_show	()	;

//bool	find_word_in_list	(const struct cli_menu_entry_set * list, char * word, int & position)	{
//extern	bool	test_fwil (struct parameters & a)     ;
extern	bool	edit_settings (struct parameters & a)     ;	//	Here from CLI

bool	ce_cmd (struct parameters & a)     {
	ce_show();
    return	(true);
}
#if 0
/**
*   void    menucmd (struct parameters & a)
*
*   List available terminal commands to pc terminal. No sense in touch screen using this
*/
bool    menucmd (struct parameters & a)     {
    char    txt[240];
//    if  (a.respond) {
        int len = sprintf     (txt, "\r\n\n%s\r\nListing Commands:-\r\n", get_version());
        pc.write    (txt, len);
        int i = 0;
        while	(a.command_list[i].cmd_word)	{
            int len = sprintf     (txt, "[%s]\t\t%s\r\n", a.command_list[i].cmd_word, a.command_list[i].description);
            pc.write    (txt, len);
            i++;
        }	//	Endof while	()
        pc.write("End of List of Commands\r\n", 25);
//    }	//	Endof if (a.respond)
        return	(true);
}


/**
struct  cli_menu_entry_set      const loco_command_list[] = {
List of commands accepted from external pc through non-opto isolated com port 115200, 8,n,1
*/
struct  cli_menu_entry_set       const pc_command_list[] {
    {"?", "Lists available commands", menucmd},
    {"vi", "Fifth, do nothing very much at all really", vi_cmd},
    {"set", "set one or  more output on", seton_cmd},
    {"clr", "clr one or more output off", clroff_cmd},
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


extern	CommandLineHandler	command_line_handler;//	(pc_command_list, & pc);	//	Nice and clean

void	check_commands	()	{	//	Called from ForeverLoop
/**
 * bool	Serial::test_for_message	()	{
 *
 * Called from ForeverLoop at repetition rate
 * Returns true when "\r\n" terminated command line has been assembled in lin_inbuff
 */
	char * buff_start = pc.test_for_message();
	if	(buff_start)
		command_line_handler.CommandExec(buff_start);
}

#endif


