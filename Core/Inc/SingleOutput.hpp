/*
 * SingleOutput.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 *
 *      SingleOutput files also contain pincounters and pin use logs
 */
#include	"main.h"
#ifndef INC_SINGOUTPCLAS_HPP_
#define INC_SINGOUTPCLAS_HPP_

typedef	bool		POLARITY;

enum	class	OUTPIN
	{OpA00, OpA01, OpA02, OpA03, OpA04, OpA05, OpA06, OpA07,
	OpA08, OpA09, OpA10, OpA11, OpA12, OpA13, OpA14, OpA15,
	OpB00, OpB01, OpB02, OpB03, OpB04, OpB05, OpB06, OpB07,
	OpB08, OpB09, OpB10, OpB11, OpB12, OpB13, OpB14, OpB15,
	OpC00, OpC01, OpC02, OpC03, OpC04, OpC05, OpC06, OpC07,
	OpC08, OpC09, OpC10, OpC11, OpC12, OpC13, OpC14, OpC15,
	OpD00, OpD01, OpD02, OpD03, OpD04, OpD05, OpD06, OpD07,
	OpD08, OpD09, OpD10, OpD11, OpD12, OpD13, OpD14, OpD15,
	 OUT_FIRST = OpA00, OUT_LAST = OpD15,
	 x00 = 0,  x01, x02, x03, x04, x05, x06, x07,
	 x10, x11, x12, x13, x14, x15, x16, x17,
	 x18, x19, x1A, x1B, x1C, x1D, x1E, x1F,
	 x20, x21, x22, x23, x24, x25, x26, x27,
	 x28, x29, x2A, x2B, x2C, x2D, x2E, x2F,
	 x30, x31, x32, x33, x34, x35, x36, x37,
	 x38, x39, x3A, x3B, x3C, x3D, x3E, x3F,
	}	;


/**	class	SingleOutput	{
 *	Use : -
 *	SingleOutput	Fred	("Name of Output", Board IO Output pin number, Output polarity true or false);
 *
 *	Output polarity, true or false, selects output sense.
 *	Polarity set either as parameter at instantiation, or
 *	explicitly set using 'set_polarity(bool p)'
 *
 *	SingleOutput is the 'atomic' and only approved method for setting Outputs.
 *	Higher level objects using 'n' outputs contain 'n' SingleOutput objects
 */
class	SingleOutput	{	//	Normal use is to drive output pin. Also usable as 1 bit register with invalid output_ionum
	OUTPIN		output_ionum {};
	POLARITY	polarity {true};	//	Outputs normally 'active on', allows for choice of 'active off'.
	const char	noid[2] {'?', 0};
	const char * id = noid;
	const char * m_txt	()	const	;
	bool	m_pin_use_checker	()	;

  public:

	/**		SingleOutput	(const char * my_id, OUTPIN);
	 * e.g.
	 * SingleInput	Fred	("My text id", OutB07);
	 * 		sets id to "My text id", binds input OutB07 to input item Fred
	 * 		polarity defaults to true
	 */
	SingleOutput	(const char * myid, OUTPIN which_output)	;
	SingleOutput	(const char * myid, OUTPIN which_output, POLARITY p)	;
	bool	set_pin			(bool output_state)	;	//	value 'output_state ^ polarity' is output
	bool	read		() const	;		//	returns last value set in output buffer
	OUTPIN	get_pin_num	() const	;		//
	void	set_polarity	(bool p)	;	//
	char *	report	(char *)	const	;	//
	//	void	read_update	()	;			//	There is nothing to update here
}	;	//	Endof class	SingleOutput	{


#endif




