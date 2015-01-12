#ifndef __GPS_H
#define __GPS_H	 
#include  "system_LPC17xx.h"
#include  "stdint.h" 
 									   						    
//GPS NMEA-0183协议重要参数结构体定义 
//卫星信息
typedef struct  
{										    
 	uint8_t num;		//卫星编号
	uint8_t eledeg;	//卫星仰角
	uint16_t azideg;	//卫星方位角
	uint8_t sn;		//信噪比		   
}nmea_slmsg;  
//UTC时间信息
typedef struct  
{										    
 	uint16_t year;	//年份
	uint8_t month;	//月份
	uint8_t date;	//日期
	uint8_t hour; 	//小时
	uint8_t min; 	//分钟
	uint8_t sec; 	//秒钟
}nmea_utc_time;   
//BTC时间信息
typedef struct  
{										    
 	uint16_t year;	//年份
	uint8_t month;	//月份
	uint8_t date;	//日期
	uint8_t hour; 	//小时
	uint8_t min; 	//分钟
	uint8_t sec; 	//秒钟
}nmea_btc_time;  
//NMEA 0183 协议解析后数据存放结构体
typedef struct  
{										    
 	uint8_t svnum;					//可见卫星数
	nmea_slmsg slmsg[12];		//最多12颗卫星
	nmea_utc_time utc;			//UTC时间
	nmea_btc_time btc;			//BTC时间
	uint32_t latitude;				//纬度 分扩大100000倍,实际要除以100000
	uint8_t nshemi;					//北纬/南纬,N:北纬;S:南纬				  
	uint32_t longitude;			    //经度 分扩大100000倍,实际要除以100000
	uint8_t ewhemi;					//东经/西经,E:东经;W:西经
	uint8_t gpssta;					//GPS状态:0,未定位;1,非差分定位;2,差分定位;6,正在估算.				  
 	uint8_t posslnum;				//用于定位的卫星数,0~12.
 	uint8_t possl[12];				//用于定位的卫星编号
	uint8_t fixmode;					//定位类型:1,没有定位;2,2D定位;3,3D定位
	uint16_t pdop;					//位置精度因子 0~500,对应实际值0~50.0
	uint16_t hdop;					//水平精度因子 0~500,对应实际值0~50.0
	uint16_t vdop;					//垂直精度因子 0~500,对应实际值0~50.0 

	int altitude;			 	//海拔高度,放大了10倍,实际除以10.单位:0.1m	 
	uint16_t speed;					//地面速率,放大了1000倍,实际除以1000.单位:0.001公里/小时	 
	uint16_t direction;              //地面航向（000.0~359.9 度，以真北方为参考基准） ,放大了10倍,实际除以10.单位:0.1度
}nmea_msg; 
//////////////////////////////////////////////////////////////////////////////////////////////////// 	
//UBLOX NEO-6M 时钟脉冲配置结构体
__packed typedef struct  
{										    
 	uint16_t header;					//cfg header,固定为0X62B5(小端模式)
	uint16_t id;						//CFG TP ID:0X0706 (小端模式)
	uint16_t dlength;				//数据长度
	uint32_t interval;				//时钟脉冲间隔,单位为us
	uint32_t length;				 	//脉冲宽度,单位为us
	signed char status;			//时钟脉冲配置:1,高电平有效;0,关闭;-1,低电平有效.			  
	uint8_t timeref;			   		//参考时间:0,UTC时间;1,GPS时间;2,当地时间.
	uint8_t flags;					//时间脉冲设置标志
	uint8_t reserved;				//保留			  
 	signed short antdelay;	 	//天线延时
 	signed short rfdelay;		//RF延时
	signed int userdelay; 	 	//用户延时	
	uint8_t cka;						//校验CK_A 							 	 
	uint8_t ckb;						//校验CK_B							 	 
}_ublox_cfg_tp; 
//UBLOX NEO-6M 刷新速率配置结构体
__packed typedef struct  
{										    
 	uint16_t header;					//cfg header,固定为0X62B5(小端模式)
	uint16_t id;						//CFG RATE ID:0X0806 (小端模式)
	uint16_t dlength;				//数据长度
	uint16_t measrate;				//测量时间间隔，单位为ms，最少不能小于200ms（5Hz）
	uint16_t navrate;				//导航速率（周期），固定为1
	uint16_t timeref;				//参考时间：0=UTC Time；1=GPS Time；
 	uint8_t cka;						//校验CK_A 							 	 
	uint8_t ckb;						//校验CK_B							 	 
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

 



