#ifndef __TASK_ACCU__
#define __TASK_ACCU__

//#include "../driver/led/led.h"
#include "../platform/stm32l/io_cfg.h"

/*******************************************************************************************
 * Defines
********************************************************************************************/
enum{
	ltc_rdcfg,
	ltc_rdcv,
	ltc_temp,
};

#define LTC6803_VMAX					((float)16.0)
#define LTC6803_VMIN					((float)2.0)
#define LTC6803_BRC_ADDR				(0x00)
#define LTC6803_1ST_ADDR				(0x80)
#define LTC6803_TOTAL_IC				((uint8_t)4)
#define NUM_OF_ACCU_PER_LTC6803			((uint8_t)4)
#define LAGRANGE_LEVEL					(10)

#define VREF_ADDR						(((uint16_t*)0x1FF80078))
#define VREF_CT_SENSOR					(3300)

#define NUM_OF_CT_SENSORS				((uint8_t)4)
#define NUM_OF_SAMPLES_CT_SENSOR		(10000)
#define ADC_BITS_CT_SENSOR				(12)
#define ADC_COUNTS_CT_SENSOR			(1<<ADC_BITS_CT_SENSOR)
#define ADC_CT_RATIO_mV_PER_A			((float)12.5)
//#define ADC_CT_AMPLIFY				((float)2.1)

//detect accu operation
#define COLLECT_DATA_DURATION			(2*60000)		//(x * 60 * 1000ms) min
#define FullOfChagred_Voltage			((float)12.45)	//full chagred
#define DISCHARGED						(0x01)
#define CHAGRING						(0x02)
#define FULLCHAGRED						(0x04)
#define WORKING							(0x08)

/*******************************************************************************************
 * Extern Variables
********************************************************************************************/
struct accu_data_t {
	float I_Bat[NUM_OF_CT_SENSORS];										//unit: Ampere
	float V_Bat[NUM_OF_CT_SENSORS][NUM_OF_ACCU_PER_LTC6803];			//unit: Volt
} ;

/* debug */
extern bool debug_accu_on;
extern uint8_t ltc6803_stt;
extern float ctZeroCalibration[NUM_OF_CT_SENSORS];
extern float ctAmplifyCalibration[NUM_OF_CT_SENSORS];
//extern int8_t rs485_stt;
/* DMA read ADC */
extern uint16_t ADC_ConvertedValue[2];
/* Share data for task RS485 */
extern struct accu_data_t accu_data_frame;
/*  */
extern uint16_t cells[LTC6803_TOTAL_IC][12];

#endif //__TASK_ACCU__
