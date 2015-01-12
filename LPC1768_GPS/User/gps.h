#ifndef __GPS_H
#define __GPS_H	 
#include  "system_LPC17xx.h"
#include  "stdint.h" 
 									   						    
//GPS NMEA-0183Э����Ҫ�����ṹ�嶨�� 
//������Ϣ
typedef struct  
{										    
 	uint8_t num;		//���Ǳ��
	uint8_t eledeg;	//��������
	uint16_t azideg;	//���Ƿ�λ��
	uint8_t sn;		//�����		   
}nmea_slmsg;  
//UTCʱ����Ϣ
typedef struct  
{										    
 	uint16_t year;	//���
	uint8_t month;	//�·�
	uint8_t date;	//����
	uint8_t hour; 	//Сʱ
	uint8_t min; 	//����
	uint8_t sec; 	//����
}nmea_utc_time;   
//BTCʱ����Ϣ
typedef struct  
{										    
 	uint16_t year;	//���
	uint8_t month;	//�·�
	uint8_t date;	//����
	uint8_t hour; 	//Сʱ
	uint8_t min; 	//����
	uint8_t sec; 	//����
}nmea_btc_time;  
//NMEA 0183 Э����������ݴ�Žṹ��
typedef struct  
{										    
 	uint8_t svnum;					//�ɼ�������
	nmea_slmsg slmsg[12];		//���12������
	nmea_utc_time utc;			//UTCʱ��
	nmea_btc_time btc;			//BTCʱ��
	uint32_t latitude;				//γ�� ������100000��,ʵ��Ҫ����100000
	uint8_t nshemi;					//��γ/��γ,N:��γ;S:��γ				  
	uint32_t longitude;			    //���� ������100000��,ʵ��Ҫ����100000
	uint8_t ewhemi;					//����/����,E:����;W:����
	uint8_t gpssta;					//GPS״̬:0,δ��λ;1,�ǲ�ֶ�λ;2,��ֶ�λ;6,���ڹ���.				  
 	uint8_t posslnum;				//���ڶ�λ��������,0~12.
 	uint8_t possl[12];				//���ڶ�λ�����Ǳ��
	uint8_t fixmode;					//��λ����:1,û�ж�λ;2,2D��λ;3,3D��λ
	uint16_t pdop;					//λ�þ������� 0~500,��Ӧʵ��ֵ0~50.0
	uint16_t hdop;					//ˮƽ�������� 0~500,��Ӧʵ��ֵ0~50.0
	uint16_t vdop;					//��ֱ�������� 0~500,��Ӧʵ��ֵ0~50.0 

	int altitude;			 	//���θ߶�,�Ŵ���10��,ʵ�ʳ���10.��λ:0.1m	 
	uint16_t speed;					//��������,�Ŵ���1000��,ʵ�ʳ���1000.��λ:0.001����/Сʱ	 
	uint16_t direction;              //���溽��000.0~359.9 �ȣ����汱��Ϊ�ο���׼�� ,�Ŵ���10��,ʵ�ʳ���10.��λ:0.1��
}nmea_msg; 
//////////////////////////////////////////////////////////////////////////////////////////////////// 	
//UBLOX NEO-6M ʱ���������ýṹ��
__packed typedef struct  
{										    
 	uint16_t header;					//cfg header,�̶�Ϊ0X62B5(С��ģʽ)
	uint16_t id;						//CFG TP ID:0X0706 (С��ģʽ)
	uint16_t dlength;				//���ݳ���
	uint32_t interval;				//ʱ��������,��λΪus
	uint32_t length;				 	//������,��λΪus
	signed char status;			//ʱ����������:1,�ߵ�ƽ��Ч;0,�ر�;-1,�͵�ƽ��Ч.			  
	uint8_t timeref;			   		//�ο�ʱ��:0,UTCʱ��;1,GPSʱ��;2,����ʱ��.
	uint8_t flags;					//ʱ���������ñ�־
	uint8_t reserved;				//����			  
 	signed short antdelay;	 	//������ʱ
 	signed short rfdelay;		//RF��ʱ
	signed int userdelay; 	 	//�û���ʱ	
	uint8_t cka;						//У��CK_A 							 	 
	uint8_t ckb;						//У��CK_B							 	 
}_ublox_cfg_tp; 
//UBLOX NEO-6M ˢ���������ýṹ��
__packed typedef struct  
{										    
 	uint16_t header;					//cfg header,�̶�Ϊ0X62B5(С��ģʽ)
	uint16_t id;						//CFG RATE ID:0X0806 (С��ģʽ)
	uint16_t dlength;				//���ݳ���
	uint16_t measrate;				//����ʱ��������λΪms�����ٲ���С��200ms��5Hz��
	uint16_t navrate;				//�������ʣ����ڣ����̶�Ϊ1
	uint16_t timeref;				//�ο�ʱ�䣺0=UTC Time��1=GPS Time��
 	uint8_t cka;						//У��CK_A 							 	 
	uint8_t ckb;						//У��CK_B							 	 
}_ublox_cfg_rate; 
				 
int NMEA_Str2num(uint8_t *buf,uint8_t*dx);
void GPS_Analysis(nmea_msg *gpsx,uint8_t *buf);
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,uint8_t *buf);
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,uint8_t *buf);
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,uint8_t *buf);
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,uint8_t *buf);
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,uint8_t *buf);
void NMEA_GPVTG_Analysis(nmea_msg *gpsx,uint8_t *buf);
void Ublox_Cfg_Tp(uint32_t interval,uint32_t length,signed char status);
void Ublox_Cfg_Rate(uint16_t measrate,uint8_t reftime);
#endif  

 



