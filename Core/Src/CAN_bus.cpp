/*
 * CAN_bus.cpp
 *
 *  Created on: Dec 26, 2023
 *      Author: Jon Freeman
 *
 *  Using 'StdId' - Standard ID using 11 bit ID number 0x000 to 0x7ff (TxHeader.IDE = CAN_ID_STD;)
 *
 *      Controllers use variable 'StdId' values to address peripherals by broadcast or individually
 *      Example of 'broadcast' - entire system reset.
 *
 *      Peripherals use 'n' bit addresses using the 'n' LSBs.
 *      One particular value in the (11 - 'n') MSBs confirms CAN controller message to one particular peripheral.
 *      Other (11 - 'n') MS bit codes for system reset and any other 'broadcast' or 'narrowcast' uses to be defined.
 *
 *  CAN messages from controllers -> peripherals
 *  	Controllers may send DATA and REMOTE REQUEST frames.
 *  	** NOT USING REMOTE REQUEST frames as CAN FD does not support this.
 *
 *  CAN messages from peripherals -> controllers	When and how to respond?
 *
 *  Data - Packet length 0 to 8 bytes
 *
 *	Priority
 *		The message with the lowest identifier value has the highest priority
 *		according to the arbitration of the CAN protocol
 *
 *	Device Addressing
 *		Decided - Use 7 least significant bits to address 1 of up to 127 remote devices (numbered 1 to 127)
 *		High 4 bits - arbitrary choice 0x07 (0x380 in correct bit positions) - about half way up priority
 *
 */
#include 	"main.h"

#include	<cstdio>	//	sprintf

#include	"Project.hpp"
#include	"Serial.hpp"

extern	uint32_t	millisecs;
extern	Serial	pc;
volatile 	uint32_t	can_sema  = 0;	//	More like a semaphore than a flag (counts up/dn)volatile 	bool	can_flag  = false;

#define	CAN_RESET_ALL_ID	0x007	//	Arbitrary low number for high priority - 3 LSBs not filtered
//#define	CAN_NODE_ID_HI		(0x07 << 7)	//	Or this with device number to form node StdId

/**
  * @brief  CAN Tx message header structure definition
  */
//typedef struct
//{
//  uint32_t StdId;    /*!< Specifies the standard identifier.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x7FF. */

//  uint32_t ExtId;    /*!< Specifies the extended identifier.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x1FFFFFFF. */

//  uint32_t IDE;      /*!< Specifies the type of identifier for the message that will be transmitted.
//                          This parameter can be a value of @ref CAN_identifier_type */

//  uint32_t RTR;      /*!< Specifies the type of frame for the message that will be transmitted.
//                          This parameter can be a value of @ref CAN_remote_transmission_request */

//  uint32_t DLC;      /*!< Specifies the length of the frame that will be transmitted.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 8. */

//  FunctionalState TransmitGlobalTime; /*!< Specifies whether the timestamp counter value captured on start
//                          of frame transmission, is sent in DATA6 and DATA7 replacing pData[6] and pData[7].
//                          @note: Time Triggered Communication Mode must be enabled.
//                          @note: DLC must be programmed as 8 bytes, in order these 2 bytes are sent.
//                          This parameter can be set to ENABLE or DISABLE. */

CAN_TxHeaderTypeDef   CAN_TxHeader;
CAN_RxHeaderTypeDef   CAN_RxHeader;

uint8_t	MyCAN_TxData[8] {};		//	Does initialise all to 0
uint8_t	MyCAN_RxData[8] {};

uint32_t    CAN_TxMailbox[3] {};
uint32_t	can_errors = 0;
uint32_t	cancount = 0;

//uint32_t	CAN_TxLog[128] {};	//	This will initialise all elements to zero
//uint32_t	CAN_RxLog[128] {};

extern	CAN_HandleTypeDef hcan1;


bool	send_can_msg	(int node11bit, uint8_t * TxData, int len)	{
	if	((len > 8) || (len < 0))
		return	(false);
	CAN_TxHeader.StdId = node11bit;	//	.IDE and .RTR constants, set in filters init
	CAN_TxHeader.DLC = len;
//	pc.write	("scm\r\n",5);	//	20 Aug 2024 get here often
	return	(HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeader, TxData, &CAN_TxMailbox[0]) == HAL_OK);
}

bool	send_can_msg	(int node4hibit, int node7lobit, uint8_t * TxData, int len)	{
	return	(send_can_msg	((((node4hibit & 0x0f) << 7) | (node7lobit & 0x7f)), TxData, len)	);
}


#if 0
//#define	TMP_REP_ENABLE

void	tmp_rep	()	{
//#ifdef	TMP_REP_ENABLE
	char	t[100];
	int len = sprintf	(t, "Rx'd CAN StdId %lx, filter %lx, %s, len %ld, data %x, %x, %x\r\n"
			, CAN_RxHeader.StdId, CAN_RxHeader.FilterMatchIndex
			, CAN_RxHeader.RTR == CAN_RTR_DATA ? "Data" : "Remote"
			, CAN_RxHeader.DLC
			, MyCAN_RxData[0], MyCAN_RxData[1], MyCAN_RxData[2])					;
	pc.write	(t, len);
//#endif
}
#endif

bool	can_echo	()	{	//	Peripherals respond using this
//	if	(!can_flag)
	if	(can_sema < 1)			//	Flag set in CAN Rx interrupt
		return	(true);
//	can_flag = false;	//	Have rec'd a message
	can_sema -= 1;
#ifdef	TMP_REP_ENABLE
	if	(CAN_RxHeader.RTR == CAN_RTR_DATA)	{
		pc.write	("RTR_DATA in can_respond\r\n", 26);	//	Execute rx'd command

	}
	else if	(CAN_RxHeader.RTR == CAN_RTR_REMOTE)	{
		pc.write	("RTR_REMOTE in can_respond\r\n", 27);	//	Report back status to controller

	}
	else	{
		pc.write	("Defective RTR in can_respond\r\n", 30);
	}
	CAN_TxHeader.StdId	= CAN_RxHeader.StdId;				//	Std ID of transmitter -
//	RxHeader.ExtId, 			//	Not using extended ID
	CAN_TxHeader.IDE	= CAN_RxHeader.IDE; 				//	specifies if we are using Standard ID or Extended ID CAN_ID_STD
	CAN_TxHeader.DLC	= CAN_RxHeader.DLC; 				//	Length of data frame in bytes 0 to 8
//	RxHeader.FilterMatchIndex, 	//
	CAN_TxHeader.RTR	= CAN_RTR_DATA;				//	Responding to Remote Transmission Request with DATA

	tmp_rep	()	;
#endif
	//	Modify peripheral state in response to received command here

//	return (HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeader, MyCAN_RxData, &CAN_TxMailbox[0]) == HAL_OK);
	//	Note                                                RxData - we are echoing received data
	return	(true);
}


bool	can_reset_all	()	{	//	Controller only issues this
	uint8_t	data[2] {}	;		//	StdId	   data, length
	return	(send_can_msg	(CAN_RESET_ALL_ID, data, 0)	);	//	Reset command is implied in StdId = CAN_RESET_ALL_ID
}

/*	can_node_command
 * Issued from controller CLI 'cnc' command. Also from 'do_can_stuff'
 *
 *
 * */
/*bool	can_node_command	(int node, int command, int data)	{	//	Controllers issue commands to peripherals
//	bool	rv1;
//	bool	rv2;
	if	((node < 1) || (node > 127) || (command < 0) || (command > 255) || (data < 0) || (data > 255))	{
		char t[64];
		int len = sprintf	(t, "Param error in can_node_command, node %d, command %d, data %d\r\n", node, command, data);
		pc.write	(t, len);
		return	(false);
	}
//	CAN_TxLog[node]++;
	MyCAN_TxData[0] = command;
	MyCAN_TxData[1] = data;
	CAN_TxHeader.StdId = (0x200 | node);		//
//	CAN_TxHeader.IDE = CAN_ID_STD;	//	CAN_ID_STD means that we are using the Standard ID (not extended)
//	CAN_TxHeader.RTR = CAN_RTR_REMOTE;	//	CAN_RTR_REMOTE - Sending a data frame, YES, response expected
//	CAN_TxHeader.RTR = CAN_RTR_DATA;	//	CAN_RTR_REMOTE - Sending a data frame, YES, response expected
	CAN_TxHeader.DLC = 2;			//	DLC is the Length of data bytes
//	rv1 = (HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeader, MyCAN_TxData, &CAN_TxMailbox[0]) == HAL_OK);
	return (HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeader, MyCAN_TxData, &CAN_TxMailbox[0]) == HAL_OK);

//	****	NOTE - Need to NOT use CAN_RTR_REMOTE to avoid incompatibility with CAN FD

//	CAN_TxHeader.RTR = CAN_RTR_REMOTE;	//	CAN_RTR_REMOTE - Sending a data frame, YES, response expected
//	rv2 = (HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeader, MyCAN_TxData, &CAN_TxMailbox[1]) == HAL_OK);
//	return	(rv1 && rv2);
}*/

#if 0

remote request not preferred as not compatible with HDCAN

bool	can_remote_request	(int node)	{	//	Controller issues this
	//	** NOTE ** Not compatible with CAN FD
	if	((node < 1) || (node > 127))	{
		char t[64];
		int len = sprintf	(t, "Param error in can_remote_request, node %d\r\n", node);
		pc.write	(t, len);
		return	(false);
	}
//	CAN_TxLog[node]++;
	pc.write	("rr\r\n", 4);
	CAN_TxHeader.StdId = (0x200 | node);		//
	CAN_TxHeader.IDE = CAN_ID_STD;	//	CAN_ID_STD means that we are using the Standard ID (not extended)
	CAN_TxHeader.RTR = CAN_RTR_REMOTE;	//	CAN_RTR_REMOTE - Sending a data frame, YES, response expected
	CAN_TxHeader.DLC = 2;			//	DLC is the Length of data bytes
	return (HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeader, MyCAN_TxData, &CAN_TxMailbox[0]) == HAL_OK);
}
#endif


void	ce_show	()	{
	char	t[126];
	int	len;
#ifdef	NODE_IS_CONTROLLER
	pc.write	("CONTROLLER Node Type\r\n", 22);
#endif
#ifdef	NODE_IS_PERIPHERAL
	pc.write	("PERIPHERAL Node Type\r\n", 22);
#endif
#ifndef	NODE_IS_CONTROLLER
#ifndef	NODE_IS_PERIPHERAL
	pc.write	("No Node Type\r\n", 14);
#endif
#endif
	len = std::sprintf(t, "Sid 0x%lx, Eid 0x%lx, IDE 0c%lx, DLC 0x%lx, FMI 0x%lx, RTR 0x%ld\r\n",
			/*can_errors, cancount, */
			CAN_RxHeader.StdId,				//	Std ID of transmitter -
			CAN_RxHeader.ExtId, 			//	Not using extended ID
			CAN_RxHeader.IDE, 				//	specifies if we are using Standard ID or Extended ID CAN_ID_STD
			CAN_RxHeader.DLC, 				//	Length of data frame in bytes 0 to 8
			CAN_RxHeader.FilterMatchIndex, 	//
			CAN_RxHeader.RTR				//	Remote Transmission Request
			);
	pc.write(t, len);
	len = sprintf	(t, "MyCAN_RxData [0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x], cnt %ld, errs %ld\r\n"
		,	MyCAN_RxData[0]
		,	MyCAN_RxData[1]
		,	MyCAN_RxData[2]
		,	MyCAN_RxData[3]
		,	MyCAN_RxData[4]
		,	MyCAN_RxData[5]
		,	MyCAN_RxData[6]
		,	MyCAN_RxData[7]
		,	cancount
		,	can_errors
						 );
	pc.write(t, len);
//	uint32_t q = CAN1_BASE;
}


int32_t	My_CAN_ID = 0;
bool	My_CAN_set = false;

void	can_filter_setup	()	;	//
void	can_filter_setup	(int32_t my_can_addr)	{	//
	My_CAN_ID = my_can_addr;
	My_CAN_set = true;
	can_filter_setup	();
}


//extern "C"	  void	can_filter_setup(){	//	called from can init
void	can_filter_setup	()	{	//	was called from can init, now later in setup to get MyCANaddr from eeprom
	//  can_filter_setup();
	CAN_FilterTypeDef	canfilterconfig;	//	Need to configure filters here

/*	canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
	canfilterconfig.FilterBank = 1;
	canfilterconfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	canfilterconfig.FilterIdHigh = 0x0446 << 5;
	canfilterconfig.FilterIdLow = 0x0000;
	canfilterconfig.FilterMaskIdHigh = 0x0444 << 5;	//	'1' bit to compare filter bit to incoming bit
	canfilterconfig.FilterMaskIdLow = 0x0000;			//	'0' do not test bit in this position
	canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
	canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
	//  canfilterconfig.SlaveStartFilterBank = 26;	//	Meaningless as only 1 CAN on L432KC
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);

	canfilterconfig.FilterBank = 6;
	canfilterconfig.FilterIdHigh = 0x0123 << 5;
	//	  canfilterconfig.FilterMaskIdHigh = 0x0123 << 5;
	canfilterconfig.FilterMaskIdHigh = 0x0723 << 5;
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);

	canfilterconfig.FilterBank = 10;
	canfilterconfig.FilterIdHigh = 0x0700 << 5;
	canfilterconfig.FilterMaskIdHigh = 0x0700 << 5;
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);

	canfilterconfig.FilterBank = 0;		//	Two new filters March 2024
	canfilterconfig.FilterIdHigh = CAN_RESET_ALL_ID << 5;
	//	  canfilterconfig.FilterMaskIdHigh = ~CAN_RESET_ALL_ID << 5;
	canfilterconfig.FilterMaskIdHigh = 0x7f8 << 5;
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);

	canfilterconfig.FilterBank = 2;
	//	  canfilterconfig.FilterIdHigh = My_CAN_ID << 5;	//	0x201 to 0x27f
	canfilterconfig.FilterIdHigh = 0x200 << 5;	//	0x201 to 0x27f
	canfilterconfig.FilterMaskIdHigh = 0x780 << 5;	//	Use 0x780 to not check 7 LSB bits
	//	  canfilterconfig.FilterMaskIdHigh = 0x7ff << 5;	//	Use 0x7ff to test all 11 bits
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);
*/


	canfilterconfig.FilterActivation 		= CAN_FILTER_ENABLE;
	canfilterconfig.FilterMode 				= CAN_FILTERMODE_IDMASK;
	canfilterconfig.FilterScale 			= CAN_FILTERSCALE_32BIT;
	canfilterconfig.FilterIdLow 			= 0x0000;
	canfilterconfig.FilterMaskIdLow 		= 0x0000;			//	'0' do not test bit in this position

	canfilterconfig.FilterBank 				= 0;		//	Two new filters March 2024
	canfilterconfig.FilterIdHigh = CAN_RESET_ALL_ID << 5;
	canfilterconfig.FilterMaskIdHigh = 0x7f8 << 5;
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);	//	Accepts 0x000 to 0x007 through filter 0

	canfilterconfig.FilterBank 				= 1;	//	Use for PING
//	canfilterconfig.FilterIdHigh = 0x0446 << 5;
//	canfilterconfig.FilterMaskIdHigh = 0x0444 << 5;	//	'1' bit to compare filter bit to incoming bit
	canfilterconfig.FilterIdHigh = ((My_CAN_ID & 0x1ff) | 0x100) << 5;		//0x100 to 0x17f		0x0100 << 5;
//	canfilterconfig.FilterMaskIdHigh = 0x07ff << 5;	//	'1' bit to compare filter bit to incoming bit PERIPHERAL
	canfilterconfig.FilterMaskIdHigh = 0x0780 << 5;	//	'1' bit to compare filter bit to incoming bit CONTROLLER
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);	//	filter 1

	canfilterconfig.FilterBank 				= 2;
	canfilterconfig.FilterIdHigh = My_CAN_ID << 5;	//	0x201 to 0x27f
	//	  canfilterconfig.FilterMaskIdHigh = 0x780 << 5;	//	Use 0x780 to ignore 7 LSB bits
	canfilterconfig.FilterMaskIdHigh = 0x7ff << 5;	//	Use 0x7ff to test all 11 bits
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);

	canfilterconfig.FilterBank 				= 4;
	canfilterconfig.FilterIdHigh = (My_CAN_ID | 0x100) << 5;	//	0x301 to 0x37f
	//	  canfilterconfig.FilterMaskIdHigh = 0x780 << 5;	//	Use 0x780 to ignore 7 LSB bits
	canfilterconfig.FilterMaskIdHigh = 0x7ff << 5;	//	Use 0x7ff to test all 11 bits
//	canfilterconfig.FilterFIFOAssignment 	= CAN_RX_FIFO1;
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);	//	Accepts 0x300 to 0x37f through filter 5

	canfilterconfig.FilterBank 				= 6;		//	System wide global commands
//	canfilterconfig.FilterIdHigh = 0x0123 << 5;		//	0x120 to 0x12f
	canfilterconfig.FilterIdHigh = 0x0680 << 5;		//	0x680 to 0x6ff
	canfilterconfig.FilterMaskIdHigh = 0x0780 << 5;
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);	//	Accepts 0x680 to 0x6ff through filter 8

	canfilterconfig.FilterBank 				= 10;
	canfilterconfig.FilterIdHigh = 0x0700 << 5;
	canfilterconfig.FilterMaskIdHigh = 0x0700 << 5;
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);	//	Accepts 0x700 to 0x7ff through filter 16


	#ifdef	NODE_IS_CONTROLLER
	//	Peripherals obey commands on addresses 0x28n and respond using 0x38n.
	//	This gets around inability to use RTR in CAN FD
	canfilterconfig.FilterBank = 4;
	canfilterconfig.FilterIdHigh = 0x300 << 5;	//	0x301 to 0x37f
	canfilterconfig.FilterMaskIdHigh = 0x780 << 5;	//	Use 0x780 to not check 7 LSB bits
	//	  canfilterconfig.FilterMaskIdHigh = 0x7ff << 5;	//	Use 0x7ff to test all 11 bits
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);
	#endif
	CAN_TxHeader.IDE = CAN_ID_STD;	//	CAN_ID_STD means that we are using the Standard ID (not extended)
	CAN_TxHeader.RTR = CAN_RTR_DATA;	//	CAN_RTR_DATA - Sending a data frame
	CAN_TxHeader.TransmitGlobalTime = DISABLE;

	HAL_CAN_Start(&hcan1);

	if	(HAL_OK != HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING))
	  can_errors++;

	if	(HAL_OK != HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING))
	  can_errors++;
	if	(can_errors)
	  pc.write	("CAN errors in startup\r\n", 23);
}

#if 0

See "rm0432-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf"
Section 55 Controller area network

Last Error Codes :
	000: No error
	001: Stuff error
	010: Form error
	011: Acknowledgment error
	100: Bit recessive error
	101: Bit dominant error
	110: CRC error
	111: Set by software
#endif

extern	void	cansigstackreport	()	;

void	CAN_status_report	()	{	//	6th March 2024
	const	char	* lectab[] ={
			{"No Error"},
			{"Stuff error"},
			{"Form error"},
			{"Acknowledgment error"},
			{"Bit recessive error"},
			{"Bit dominant error"},
			{"CRC error"},
			{"Set by software"},
	}	;
	char	t[100];
	int	len;
	uint32_t	stat = CAN1->ESR;
	CAN->ESR	= 0x70;		//	Set last_err_code bits.
	uint32_t	tx_errs 		= (stat >> 16) & 0x0ff;
	uint32_t	rx_errs 		= (stat >> 24) & 0x0ff;
	uint32_t	last_err_code	= (stat >> 4) & 0x07;
	bool		boff			= (stat & 4);	//	Bus off
	bool		epvf			= (stat & 2);	//	Error passive flag
	bool		ewgf			= (stat & 1);	//	Error waning flag
	if	((tx_errs > 0) || (rx_errs > 0) || (last_err_code > 0) || boff || epvf || ewgf)	{
		pc.write("\r\nCAN Errors\r\n", 14);
		len = sprintf	(t, "tx_errors %ld\r\n", tx_errs);
		pc.write	(t, len);
		len = sprintf	(t, "rx_errors %ld\r\n", rx_errs);
		pc.write	(t, len);
		len = sprintf	(t, "%s\r\n", lectab[last_err_code]);
		pc.write	(t, len);
		len = sprintf	(t, "Bus off\t%s\r\n", boff ? "true" : "false");
		pc.write	(t, len);
		len = sprintf	(t, "Error passive\t%s\r\n", epvf ? "true" : "false");
		pc.write	(t, len);
		len = sprintf	(t, "Error warning\t%s\r\n", ewgf ? "true" : "false");
		pc.write	(t, len);

	}
	else	{
		len = sprintf	(t, "No known CAN errors\r\n");
		pc.write	(t, len);
	}
	len = sprintf	(t, "cancount %ld, can_errors %ld\r\n", cancount, can_errors);
	pc.write	(t, len);
	cansigstackreport	()	;
}


#ifdef	NODE_IS_CONTROLLER	//	Apr 1 2024

bool	can_stuff_to_do = false	;
//int		can_device_num = 0;

bool	do_can_stuff	(int can_device_num)	{	//	Called from one_ms_stuff() while can_stuff_to_do true
	//	write sniff command to CAN address 'device++'
	//can_echo	()	;	//	can_echo reports to pc any response from CAN bus DONE IN one_ms_stuff()
//	bool	can_node_command	(int node, int command, int data)	{	//	Controllers issue commands to peripherals
//	return	(can_node_command	(can_device_num, 19, 0));
	uint8_t	data[2] {};
//	bool	send_can_msg	(int node11bit, uint8_t * TxData, int len)	{
	return	(send_can_msg	(can_device_num, data, 0));
}

void	search_canbus	()	{	//	To start CAN sniff ops at 1 (or more) ms intervals
	can_stuff_to_do = true;		//	Picked up in one_ms_stuff()
//	can_device_num = 0;
}

#endif

//uint32_t	cancount = 0;

void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)	//	Not this one
{
	cancount++;
	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN_RxHeader, MyCAN_RxData);
//	stdid[0] = RxHeader.StdId;
}

void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan)	//	Not this one
{
	cancount++;
	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO1, &CAN_RxHeader, MyCAN_RxData);
//	stdid[1] = RxHeader.StdId;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)	//	Yes this one
{
	cancount++;
//	can_flag = true;
	can_sema += 1;
	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN_RxHeader, MyCAN_RxData);
//	stdid[2] = RxHeader.StdId;
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)	//	Yes this one
{
	cancount++;
//	can_flag = true;
	can_sema += 1;
	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO1, &CAN_RxHeader, MyCAN_RxData);
//	stdid[2] = RxHeader.StdId;
}


