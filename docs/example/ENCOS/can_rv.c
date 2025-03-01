#include "can_rv.h"
#include "math_ops.h"

#define KP_MIN 0.0f
#define KP_MAX 500.0f
#define KD_MIN 0.0f
#define KD_MAX 5.0f
#define POS_MIN -12.5f
#define POS_MAX 12.5f
#define SPD_MIN -18.0f
#define SPD_MAX 18.0f
#define T_MIN -30.0f
#define T_MAX 30.0f
#define I_MIN -30.0f
#define I_MAX 30.0f
union RV_TypeConvert
{
	float to_float;
	int to_int;
	unsigned int to_uint;
	uint8_t buf[4];
}rv_type_convert;

union RV_TypeConvert2
{
	int16_t to_int16;
	uint16_t to_uint16;
	uint8_t buf[2];
}rv_type_convert2;

//MOTOR SETTING
/*
cmd:
0x00:NON
0x01:set the communication mode to automatic feedback.
0x02:set the communication mode to response.
0x03:set the current position to zero.
*/
void MotorSetting(uint16_t motor_id,uint8_t cmd)
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.StdId = 0x7FF;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 4;
	
	if(cmd==0) return;
	
  TxMessage.Data[0]=motor_id>>8;
	TxMessage.Data[1]=motor_id&0xff;
	TxMessage.Data[2]=0x00;
	TxMessage.Data[3]=cmd;
	
  CAN_Transmit(CAN1, &TxMessage);
}
// Reset Motor ID 
void MotorIDReset(void)
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.StdId = 0x7FF;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 6;
	
  TxMessage.Data[0]=0x7F;
	TxMessage.Data[1]=0x7F;
	TxMessage.Data[2]=0x00;
	TxMessage.Data[3]=0x05;
	TxMessage.Data[4]=0x7F;
	TxMessage.Data[5]=0x7F;
	
  CAN_Transmit(CAN1, &TxMessage);
}
// set motor new ID
void MotorIDSetting(uint16_t motor_id,uint16_t motor_id_new)
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.StdId = 0x7FF;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 6;
	
  TxMessage.Data[0]=motor_id>>8;
	TxMessage.Data[1]=motor_id&0xff;
	TxMessage.Data[2]=0x00;
	TxMessage.Data[3]=0x04;
	TxMessage.Data[4]=motor_id_new>>8;
	TxMessage.Data[5]=motor_id_new&0xff;
	
  CAN_Transmit(CAN1, &TxMessage);
}
// read motor communication mode
void MotorCommModeReading(uint16_t motor_id)
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.StdId = 0x7FF;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 4;
	
  TxMessage.Data[0]=motor_id>>8;
	TxMessage.Data[1]=motor_id&0xff;
	TxMessage.Data[2]=0x00;
	TxMessage.Data[3]=0x81;
	
  CAN_Transmit(CAN1, &TxMessage);
}
// read motor ID
void MotorIDReading(void)
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.StdId = 0x7FF;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 4;
	
  TxMessage.Data[0]=0xFF;
	TxMessage.Data[1]=0xFF;
	TxMessage.Data[2]=0x00;
	TxMessage.Data[3]=0x82;
	
  CAN_Transmit(CAN1, &TxMessage);
}

////////////////*******************************////////////////////////////
////////////////*******************************////////////////////////////
// This function use in auto feedback communication mode.
/*
motor_quantity:1~8
*/
void set_motors_current(OD_Motor_Msg rv_MotorMsg[8],uint8_t motor_quantity)
{
	uint16_t i;
	uint8_t mbox;
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.StdId = 0x1FF;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 8;
	
	if(motor_quantity<1)
		return;
	for(i=0;i<4;i++)
	{
		TxMessage.Data[2*i]  = rv_MotorMsg[i].current_desired_int>>8;
		TxMessage.Data[2*i+1]= rv_MotorMsg[i].current_desired_int&0xff;
	}
	i=0;
  mbox=CAN_Transmit(CAN1, &TxMessage);
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))
	{
		i++;	//�ȴ����ͽ���
		if(i>=0XFFF)
			break;
	}
	if(motor_quantity<5)
		return;
	TxMessage.StdId = 0x2FF;
	for(i=0;i<4;i++)
	{
		TxMessage.Data[2*i]  = rv_MotorMsg[i+4].current_desired_int>>8;
		TxMessage.Data[2*i+1]= rv_MotorMsg[i+4].current_desired_int&0xff;
	}
	i=0;
  mbox=CAN_Transmit(CAN1, &TxMessage);
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))
	{
		i++;	//�ȴ����ͽ���
		if(i>=0XFFF)
			break;
	}
}

// This function use in ask communication mode.
/*
motor_id:1~0x7FE
kp:0~500
kd:0~50
pos:-12.5rad~12.5rad
spd:-18rad/s~18rad/s
tor:-30Nm~30Nm
*/
void send_motor_ctrl_cmd(uint16_t motor_id,float kp,float kd,float pos,float spd,float tor)
{
	int kp_int;
	int kd_int;
	int pos_int;            
	int spd_int;
	int tor_int;
	
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.StdId = motor_id;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 8;
		
	if(kp>KP_MAX) kp=KP_MAX;
		else if(kp<KP_MIN) kp=KP_MIN;
	if(kd>KD_MAX ) kd=KD_MAX;
		else if(kd<KD_MIN) kd=KD_MIN;	
	if(pos>POS_MAX)	pos=POS_MAX;
		else if(pos<POS_MIN) pos=POS_MIN;
	if(spd>SPD_MAX)	spd=SPD_MAX;
		else if(spd<SPD_MIN) spd=SPD_MIN;
	if(tor>T_MAX)	tor=T_MAX;
		else if(tor<T_MIN) tor=T_MIN;

  kp_int = float_to_uint(kp, KP_MIN, KP_MAX, 12);
  kd_int = float_to_uint(kd, KD_MIN, KD_MAX, 9);
	pos_int = float_to_uint(pos, POS_MIN, POS_MAX, 16);            
  spd_int = float_to_uint(spd, SPD_MIN, SPD_MAX, 12);
  tor_int = float_to_uint(tor, T_MIN, T_MAX, 12);	
	
  TxMessage.Data[0]=0x00|(kp_int>>7);//kp5
	TxMessage.Data[1]=((kp_int&0x7F)<<1)|((kd_int&0x100)>>8);//kp7+kd1
	TxMessage.Data[2]=kd_int&0xFF;
	TxMessage.Data[3]=pos_int>>8;
	TxMessage.Data[4]=pos_int&0xFF;
	TxMessage.Data[5]=spd_int>>4;
	TxMessage.Data[6]=(spd_int&0x0F)<<4|(tor_int>>8);
	TxMessage.Data[7]=tor_int&0xff;
	
  CAN_Transmit(CAN1, &TxMessage);
	
//	//for test,data unpack;
//	kp_int=kd_int=pos_int=spd_int=cur_int=0;
//	
//	kp_int=(TxMessage.Data[0]&0x1F)<<7|(TxMessage.Data[1]&0xFE)>>1;
//	kd_int=(TxMessage.Data[1]&0x01)<<8|TxMessage.Data[2];
//	pos_int=TxMessage.Data[3]<<8|TxMessage.Data[4];
//	spd_int=TxMessage.Data[5]<<4|(TxMessage.Data[6]&0xF0)>>4;
//	cur_int=(TxMessage.Data[6]&0x0F)<<8|TxMessage.Data[7];
//	
//	kp 	= uint_to_float(kp_int, KP_MIN, KP_MAX, 12);
//  kd	= uint_to_float(kd_int, KD_MIN, KD_MAX, 9);
//	pos = uint_to_float(pos_int, POS_MIN, POS_MAX, 16);            
//  spd = uint_to_float(spd_int, SPD_MIN, SPD_MAX, 12);
//  cur = uint_to_float(cur_int, I_MIN, I_MAX, 12);	
}

// This function use in ask communication mode.
/*
motor_id:1~0x7FE
pos:float
spd:0~18000
cur:0~3000
ack_status:0~3
*/
void set_motor_position(uint16_t motor_id,float pos,uint16_t spd,uint16_t cur,uint8_t ack_status)
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;//CAN_ID_EXT
	TxMessage.StdId = motor_id;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 8;
	
//	if(spd>18000) spd=18000;
//	if(cur>3000) cur=3000;
	if(ack_status>3) 
		return;
	
	rv_type_convert.to_float=pos;
  TxMessage.Data[0]=0x20|(rv_type_convert.buf[3]>>3);
	TxMessage.Data[1]=(rv_type_convert.buf[3]<<5)|(rv_type_convert.buf[2]>>3);
	TxMessage.Data[2]=(rv_type_convert.buf[2]<<5)|(rv_type_convert.buf[1]>>3);
	TxMessage.Data[3]=(rv_type_convert.buf[1]<<5)|(rv_type_convert.buf[0]>>3);
	TxMessage.Data[4]=(rv_type_convert.buf[0]<<5)|(spd>>10);
	TxMessage.Data[5]=(spd&0x3FC)>>2;
	TxMessage.Data[6]=(spd&0x03)<<6|(cur>>6);
	TxMessage.Data[7]=(cur&0x3F)<<2|ack_status;
	
  CAN_Transmit(CAN1, &TxMessage);
}

// This function use in ask communication mode.
/*
motor_id:1~0x7FE
spd:-18000~18000
cur:0~3000
ack_status:0~3
*/
void set_motor_speed(uint16_t motor_id,float spd,uint16_t cur,uint8_t ack_status)
{
		CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;//CAN_ID_EXT
	TxMessage.StdId = motor_id;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 7;
	
//	if(spd>18000) spd=18000;
//	else if(spd<-18000) spd=-18000;
//	if(cur>3000) cur=3000;
//	if(ack_status>3) 
//		return;
	
	rv_type_convert.to_float=spd;
  TxMessage.Data[0]=0x40|ack_status;
	TxMessage.Data[1]=rv_type_convert.buf[3];
	TxMessage.Data[2]=rv_type_convert.buf[2];
	TxMessage.Data[3]=rv_type_convert.buf[1];
	TxMessage.Data[4]=rv_type_convert.buf[0];
	TxMessage.Data[5]=cur>>8;
	TxMessage.Data[6]=cur&0xff;
	
  CAN_Transmit(CAN1, &TxMessage);
}

// This function use in ask communication mode.
/*
motor_id:1~0x7FE
cur:-3000~3000
ctrl_status:
					0:current control
					1:torque control
					2:variable damping brake control(also call full brake)
					3:dynamic brake control
					4:regenerative brake control
					5:NON
					6:NON
					7:NON
ack_status:0~3
*/
void set_motor_cur_tor(uint16_t motor_id,int16_t cur_tor,uint8_t ctrl_status,uint8_t ack_status)
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;//CAN_ID_EXT
	TxMessage.StdId = motor_id;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 3;
	
	if(ack_status>3) 
		return;
	if(ctrl_status>7)
		return;
	if(ctrl_status) //enter torque control mode or brake mode
	{
		if(cur_tor>3000) cur_tor=3000;
		else if(cur_tor<-3000) cur_tor=-3000;
	}
	else
	{
		if(cur_tor>2000) cur_tor=2000;
		else if(cur_tor<-2000) cur_tor=-2000;
	}

	
  TxMessage.Data[0]=0x60|ctrl_status<<2|ack_status;
	TxMessage.Data[1]=cur_tor>>8;
	TxMessage.Data[2]=cur_tor&0xff;
	
  CAN_Transmit(CAN1, &TxMessage);
}

// This function use in ask communication mode.
/*
motor_id:1~0x7FE
acc:0~2000
ack_status:0~3
*/
void set_motor_acceleration(uint16_t motor_id,uint16_t acc,uint8_t ack_status)
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;//CAN_ID_EXT
	TxMessage.StdId = motor_id;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 4;
	
	if(ack_status>2) 
		return;
	if(acc>2000) acc=2000;
	
  TxMessage.Data[0]=0xC0|ack_status;
	TxMessage.Data[1]=0x01;
	TxMessage.Data[2]=acc>>8;
	TxMessage.Data[3]=acc&0xff;
	
  CAN_Transmit(CAN1, &TxMessage);
}

// This function use in ask communication mode.
/*
motor_id:1~0x7FE
linkage:0~10000
speedKI:0~10000
ack_status:0/1
*/
void set_motor_linkage_speedKI(uint16_t motor_id,uint16_t linkage,uint16_t speedKI,uint8_t ack_status)
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;//CAN_ID_EXT
	TxMessage.StdId = motor_id;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 6;
	
	if(ack_status>2) 
		return;
	if(linkage>10000) linkage=10000;
	if(speedKI>10000) speedKI=10000;

  TxMessage.Data[0]=0xC0|ack_status;
	TxMessage.Data[1]=0x02;
	TxMessage.Data[2]=linkage>>8;
	TxMessage.Data[3]=linkage&0xff;
	TxMessage.Data[4]=speedKI>>8;
	TxMessage.Data[5]=speedKI&0xff;
	
  CAN_Transmit(CAN1, &TxMessage);
}

// This function use in ask communication mode.
/*
motor_id:1~0x7FE
fdbKP:0~10000
fbdKD:0~10000
ack_status:0/1
*/
void set_motor_feedbackKP_KD(uint16_t motor_id,uint16_t fdbKP,uint16_t fdbKD,uint8_t ack_status)
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;//CAN_ID_EXT
	TxMessage.StdId = motor_id;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 6;
	
	if(ack_status>2) 
		return;
	if(fdbKP>10000) fdbKP=10000;
	if(fdbKD>10000) fdbKD=10000;

  TxMessage.Data[0]=0xC0|ack_status;
	TxMessage.Data[1]=0x03;
	TxMessage.Data[2]=fdbKP>>8;
	TxMessage.Data[3]=fdbKP&0xff;
	TxMessage.Data[4]=fdbKD>>8;
	TxMessage.Data[5]=fdbKD&0xff;
	
  CAN_Transmit(CAN1, &TxMessage);
}
// This function use in ask communication mode.
/*
motor_id:1~0x7FE
param_cmd:1~9
*/
void get_motor_parameter(uint16_t motor_id,uint8_t param_cmd)
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;//CAN_ID_EXT
	TxMessage.StdId = motor_id;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 2;
	
  TxMessage.Data[0]=0xE0;
	TxMessage.Data[1]=param_cmd;
	
  CAN_Transmit(CAN1, &TxMessage);
}


MotorCommFbd motor_comm_fbd;
OD_Motor_Msg rv_motor_msg[8]; 
uint16_t motor_id_check=0;
void RV_can_data_repack(CanRxMsg *RxMessage,uint8_t comm_mode)
{
	uint8_t motor_id_t=0;
	uint8_t ack_status=0;
	int pos_int=0;            
	int spd_int=0;
	int cur_int=0;
	if(RxMessage->StdId==0x7FF)
	{
		if(RxMessage->Data[2]!=0x01)//determine whether it is a motor feedback instruction
			return;			//it is not a motor feedback instruction 
		if((RxMessage->Data[0]==0xff)&&(RxMessage->Data[1]==0xFF))
		{
			motor_comm_fbd.motor_id=RxMessage->Data[3]<<8|RxMessage->Data[4];
			motor_comm_fbd.motor_fbd=0x01;
		}
		else if((RxMessage->Data[0]==0x80)&&(RxMessage->Data[1]==0x80))//inquire failed
		{
			motor_comm_fbd.motor_id=0;
			motor_comm_fbd.motor_fbd=0x80;
		}
		else if((RxMessage->Data[0]==0x7F)&&(RxMessage->Data[1]==0x7F))//reset ID succeed
		{
			motor_comm_fbd.motor_id=1;
			motor_comm_fbd.motor_fbd=0x05;
		}
		else
		{
			motor_comm_fbd.motor_id=RxMessage->Data[0]<<8|RxMessage->Data[1];
			motor_comm_fbd.motor_fbd=RxMessage->Data[3];
		}
	}
	else if(comm_mode==0x00)//Response mode
	{
		ack_status=RxMessage->Data[0]>>5;
		motor_id_t=RxMessage->StdId-1;motor_id_check=RxMessage->StdId;
		rv_motor_msg[motor_id_t].motor_id=motor_id_t;
		rv_motor_msg[motor_id_t].error=RxMessage->Data[0]&0x1F;
		if(ack_status==1)//response frame 1
		{
			pos_int=RxMessage->Data[1]<<8|RxMessage->Data[2];
			spd_int=RxMessage->Data[3]<<4|(RxMessage->Data[4]&0xF0)>>4;
			cur_int=(RxMessage->Data[4]&0x0F)<<8|RxMessage->Data[5];
			
			rv_motor_msg[motor_id_t].angle_actual_rad=uint_to_float(pos_int,POS_MIN,POS_MAX,16);
			rv_motor_msg[motor_id_t].speed_actual_rad=uint_to_float(spd_int,SPD_MIN,SPD_MAX,12);
			rv_motor_msg[motor_id_t].current_actual_float=uint_to_float(cur_int,I_MIN,I_MAX,12);
			rv_motor_msg[motor_id_t].temperature=(RxMessage->Data[6]-50)/2;
		}
		else if(ack_status==2)//response frame 2
		{
			rv_type_convert.buf[0]=RxMessage->Data[4];
			rv_type_convert.buf[1]=RxMessage->Data[3];
			rv_type_convert.buf[2]=RxMessage->Data[2];
			rv_type_convert.buf[3]=RxMessage->Data[1];
			rv_motor_msg[motor_id_t].angle_actual_float=rv_type_convert.to_float;
			rv_motor_msg[motor_id_t].current_actual_int=RxMessage->Data[5]<<8|RxMessage->Data[6];
			rv_motor_msg[motor_id_t].temperature=(RxMessage->Data[7]-50)/2;
			rv_motor_msg[motor_id_t].current_actual_float=rv_motor_msg[motor_id_t].current_actual_int/100.0f;
		}
		else if(ack_status==3)//response frame 3
		{
			rv_type_convert.buf[0]=RxMessage->Data[4];
			rv_type_convert.buf[1]=RxMessage->Data[3];
			rv_type_convert.buf[2]=RxMessage->Data[2];
			rv_type_convert.buf[3]=RxMessage->Data[1];
			rv_motor_msg[motor_id_t].speed_actual_float=rv_type_convert.to_float;
			rv_motor_msg[motor_id_t].current_actual_int=RxMessage->Data[5]<<8|RxMessage->Data[6];
			rv_motor_msg[motor_id_t].temperature=(RxMessage->Data[7]-50)/2;
			rv_motor_msg[motor_id_t].current_actual_float=rv_motor_msg[motor_id_t].current_actual_int/100.0f;
		}
		else if(ack_status==4)//response frame 4
		{
			if(RxMessage->DLC!=3)	return;
			motor_comm_fbd.INS_code=RxMessage->Data[1];
			motor_comm_fbd.motor_fbd=RxMessage->Data[2];
		}
		else if(ack_status==5)//response frame 5
		{
			motor_comm_fbd.INS_code=RxMessage->Data[1];
			if(motor_comm_fbd.INS_code==1&RxMessage->DLC==6)//get position
			{
				rv_type_convert.buf[0]=RxMessage->Data[5];
				rv_type_convert.buf[1]=RxMessage->Data[4];
				rv_type_convert.buf[2]=RxMessage->Data[3];
				rv_type_convert.buf[3]=RxMessage->Data[2];
				rv_motor_msg[motor_id_t].angle_actual_float=rv_type_convert.to_float;
			}
			else if(motor_comm_fbd.INS_code==2&RxMessage->DLC==6)//get speed
			{
				rv_type_convert.buf[0]=RxMessage->Data[5];
				rv_type_convert.buf[1]=RxMessage->Data[4];
				rv_type_convert.buf[2]=RxMessage->Data[3];
				rv_type_convert.buf[3]=RxMessage->Data[2];
				rv_motor_msg[motor_id_t].speed_actual_float=rv_type_convert.to_float;
			}
			else if(motor_comm_fbd.INS_code==3&RxMessage->DLC==6)//get current
			{
				rv_type_convert.buf[0]=RxMessage->Data[5];
				rv_type_convert.buf[1]=RxMessage->Data[4];
				rv_type_convert.buf[2]=RxMessage->Data[3];
				rv_type_convert.buf[3]=RxMessage->Data[2];
				rv_motor_msg[motor_id_t].current_actual_float=rv_type_convert.to_float;
			}
			else if(motor_comm_fbd.INS_code==4&RxMessage->DLC==6)//get power
			{
				rv_type_convert.buf[0]=RxMessage->Data[5];
				rv_type_convert.buf[1]=RxMessage->Data[4];
				rv_type_convert.buf[2]=RxMessage->Data[3];
				rv_type_convert.buf[3]=RxMessage->Data[2];
				rv_motor_msg[motor_id_t].power=rv_type_convert.to_float;
			}
			else if(motor_comm_fbd.INS_code==5&RxMessage->DLC==4)//get acceleration
			{
				rv_motor_msg[motor_id_t].acceleration=RxMessage->Data[2]<<8|RxMessage->Data[3];
			}
			else if(motor_comm_fbd.INS_code==6&RxMessage->DLC==4)//get linkage_KP
			{
				rv_motor_msg[motor_id_t].linkage_KP=RxMessage->Data[2]<<8|RxMessage->Data[3];
			}
			else if(motor_comm_fbd.INS_code==7&RxMessage->DLC==4)//get speed_KI
			{
				rv_motor_msg[motor_id_t].speed_KI=RxMessage->Data[2]<<8|RxMessage->Data[3];
			}
			else if(motor_comm_fbd.INS_code==8&RxMessage->DLC==4)//get feedback_KP
			{
				rv_motor_msg[motor_id_t].feedback_KP=RxMessage->Data[2]<<8|RxMessage->Data[3];
			}
			else if(motor_comm_fbd.INS_code==9&RxMessage->DLC==4)//get feedback_KD
			{
				rv_motor_msg[motor_id_t].feedback_KD=RxMessage->Data[2]<<8|RxMessage->Data[3];
			}
		}
	}
	else if(comm_mode==0x01)//automatic feedback mode
	{
		motor_id_t=RxMessage->StdId-0x205;
		rv_motor_msg[motor_id_t].angle_actual_int=(uint16_t)(RxMessage->Data[0]<<8|RxMessage->Data[1]);
		rv_motor_msg[motor_id_t].speed_actual_int=(int16_t)(RxMessage->Data[2]<<8|RxMessage->Data[3]);
		rv_motor_msg[motor_id_t].current_actual_int=(RxMessage->Data[4]<<8|RxMessage->Data[5]);
		rv_motor_msg[motor_id_t].temperature=RxMessage->Data[6];
		rv_motor_msg[motor_id_t].error=RxMessage->Data[7];
	}
	
}


