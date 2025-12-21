/*
 * SystemSetup.cpp
 *
 *  Created on: Jan 14, 2024
 *      Author: Jon Freeman B Eng Hons
 */

#include	<cstdio>
#include	<cstring>

#include	"Serial.hpp"
#include	"settings.hpp"

extern	Serial	pc;//		(huart2);
extern	Serial	ctrl;//	(huart1);	//	No further reference to huart1, huart2

#define	COM_PORT	pc	//	Name of com port setup in serial

constexpr	char const	version_str[] = "West Buck Sigs Controller v1," __DATE__;
//constexpr forces initalisation at compile time
const 	char * 	get_version	()	{	return	(version_str);	}
//extern	bool	get_settings	();

extern	i2eeprom_settings	my_settings;

extern	void	setup_alarms	()	;
extern	void  can_filter_setup();
//extern	uint32_t	find_daisy_length	()	;
bool	eeprom_valid_flag = false;
//void	myportsreport	()	;

extern	"C"	void	Signal_sys_Setup	()	{	//	Called from main immediately before start of forever loop
	pc.start_rx();
	ctrl.start_rx();
	COM_PORT.write("\r\n\n\n\n", 5);
	COM_PORT.write(version_str, strlen(version_str));
	COM_PORT.write("\r\n\n", 3);
//	eeprom_valid_flag = get_settings	();
	eeprom_valid_flag = my_settings.load	();
	if	(eeprom_valid_flag)	{	//	Read 24LC64 eeprom for any config data
		COM_PORT.write("Loaded eeprom settings ok\r\n", 27);
	}
	else	{
		COM_PORT.write("eeprom settings load FAILED\r\n", 29);
	}
//	find_daisy_length	();	//	does not work
	setup_alarms	()	;
//	myportsreport();
  can_filter_setup();	//	Also starts CAN
}


