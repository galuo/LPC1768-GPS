#ifndef __GPRS_H
#define __GPRS_H
#include  "system_LPC17xx.h"
#include  "stdint.h"

extern void init_gprs_mode(void);
uint8_t GPRS_send_str(uint8_t const *pucStr, uint32_t ulNum);
typedef enum {
    GPRS_AT_ATE0_0,
    GPRS_AT_CGATT_1 ,
	GPRS_AT_CGDCONT_2,
    GPRS_AT_CGRGE_3 ,
	GPRS_AT_ETCPIP_4,
	GPRS_AT_ETCPIP_Q_5,
	GPRS_AT_DNSR_6,
	GPRS_AT_IPOPEN_7,
    GPRS_OPEN_FINISH,       /// GPRS 打开成功了

}GTM900C_STATUS ;



#endif
