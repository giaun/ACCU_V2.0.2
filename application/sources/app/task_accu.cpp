#include "../ak/fsm.h"
#include "../ak/port.h"
#include "../ak/timer.h"
#include "../ak/message.h"


#include "../sys/sys_ctrl.h"
#include "../sys/sys_irq.h"
#include "../sys/sys_io.h"
//include "../sys/sys_arduino.h"

#include "../driver/LTC68032/LTC68032.h"


#include "app.h"
#include "app_dbg.h"

#include "eeprom.h"

/*task include*/
#include "task_list.h"
#include "task_accu.h"

/* common include */
#include "../common/utils.h"

/************************************************************************************
 *LOCAL VARIABLES
 ************************************************************************************/
/**Common**/
bool debug_accu_on = false;
uint8_t ltc6803_stt;

/**CT sensors**/
uint16_t ADC_ConvertedValue[2];

uint8_t currentChannel; /*switch ADC channel*/
struct accu_data_t accu_data_frame;
float ctZeroCalibration[NUM_OF_CT_SENSORS];
float ctAmplifyCalibration[NUM_OF_CT_SENSORS];

/**LTC6803-4**/
uint16_t cells[LTC6803_TOTAL_IC][12];			//cell voltages
static uint8_t r_config[LTC6803_TOTAL_IC][7];	//read cfg
static uint16_t temp[LTC6803_TOTAL_IC][3];		//read temperatures
/**External Librarys**/


/************************************************************************************
 *LOCAL FUNCTIONS
 ************************************************************************************/
//void print_rx_data();
void save_cells_voltage();

//************************************************************************************/

void print_rx_data(uint8_t total_ic, uint8_t data_type){
	if (debug_accu_on){
		APP_DBG("data return: \n");
		if (data_type == ltc_rdcfg){
			for (uint8_t i =0; i< total_ic; i++)
			{
				for (uint8_t k = 0; k < 7; k++){
					xprintf("%02x\t",r_config[i][k]);
				}
			}
		}
		else if (data_type == ltc_rdcv){
			for (uint8_t i =0; i< total_ic; i++)
			{
				for (uint8_t k = 0; k < 12; k++){
					xprintf("%02x\t",cells[i][k]);
				}
				xprintf("\r\n");
				for (uint8_t k = 0; k < 12; k++){
					xprintf("%d\t",cells[i][k]);
				}
			}
		}
		else if (data_type == ltc_temp){
			for (uint8_t i =0; i< total_ic; i++)
			{
				for (uint8_t k = 0; k < 3; k++){
					xprintf("%d\t",temp[i][k]);
				}
			}
		}
		xprintf("\n");
	}
}
/******************************************************************
 * @func: print_cells_voltage().
 * @description: Use to update calculated voltage to
 *				 'accu_data_frame.V_Bat[]'.
*******************************************************************/
void save_cells_voltage(){
	APP_DBG("CELL VLOTAGE: \n");
	for(uint8_t i=0; i<LTC6803_TOTAL_IC; i++){
		// clear
		for (uint8_t k = 0; k < NUM_OF_ACCU_PER_LTC6803; k++){
			accu_data_frame.V_Bat[i][k] = 0;
		}

		// calculate voltage of each accu
		for (uint8_t k = 0; k < 12; k++){
			accu_data_frame.V_Bat[i][k/3] += ((float)(cells[i][k])*1.5/1000 - 0.025);
		}

		// detect accu error
		for (uint8_t k = 0; k < NUM_OF_ACCU_PER_LTC6803; k++){
			if ((accu_data_frame.V_Bat[i][k] > LTC6803_VMAX)||\
				(accu_data_frame.V_Bat[i][k] < LTC6803_VMIN)){
				accu_data_frame.V_Bat[i][k] = 0;
				for (uint8_t j = 0; j < 3; j++){
					cells[i][k*3+j] = 0;
				}
			}
		}

		APP_DBG("Group %d\n",i);
		for (uint8_t k = 0; k < NUM_OF_ACCU_PER_LTC6803; k++){
			APP_DBG("accu [%d][%d]: %d.%d%d \n",i,k,(uint32_t)accu_data_frame.V_Bat[i][k],
					(uint32_t)(accu_data_frame.V_Bat[i][k]*10)%10,
					(uint32_t)(accu_data_frame.V_Bat[i][k]*100)%10);
		}
	}
}
uint32_t dem_tmp =0;
/******************************************************************
 * @func: task_accu().
 * @description:
*******************************************************************/
void task_accu(ak_msg_t* msg) {

	float tmp_time_bat[4][LAGRANGE_LEVEL + 1];
	float tmp_volt_bat[4][LAGRANGE_LEVEL + 1];
	//int8_t ltc6803_stt=0;
	switch (msg->sig) {
	case AC_ACCU_INITIAL:{
		/* INITIALIZE */
		APP_PRINT("INITIAL ACCU\r\n");
		//setup configure register of ltc before read..
		for (uint8_t i = 0; i < LTC6803_TOTAL_IC; i++){
			w_config[i][0] = 0xF1;
			w_config[i][1] = 0xFF;
			w_config[i][2] = 0x0F;
			w_config[i][3] = 0x00;
			w_config[i][4] = 0x00;
			w_config[i][5] = 0x00;
		}

		/*reset all buffer to zero*/
		mem_set(r_config,0,7*LTC6803_TOTAL_IC);
		mem_set(cells,0,12*LTC6803_TOTAL_IC*2);
		mem_set(temp,0,3*LTC6803_TOTAL_IC*2);

		mem_set(tmp_time_bat,0,4*(LAGRANGE_LEVEL+1)*sizeof(float));
		mem_set(tmp_volt_bat,0,4*(LAGRANGE_LEVEL+1)*sizeof(float));

		mem_set(ctZeroCalibration,0,NUM_OF_CT_SENSORS*sizeof(float));
		for (uint8_t i=0; i<NUM_OF_CT_SENSORS; i++){
			ctAmplifyCalibration[i] = 2.2;
		}
		APP_PRINT("\n");
		for (uint8_t i=0; i<NUM_OF_CT_SENSORS; i++){
			APP_PRINT("CT[%d]: %d/1000", i, (int32_t)(ctAmplifyCalibration[i]*1000));
		}

		eeprom_read(EEPROM_CALIB_ZERO_ADDR, (uint8_t*)ctZeroCalibration,NUM_OF_CT_SENSORS*sizeof(float));
		eeprom_read(EEPROM_CALIB_AMPLIFY_ADDR, (uint8_t*)ctAmplifyCalibration,NUM_OF_CT_SENSORS*sizeof(float));

		for(uint8_t i=0; i<4; i++){
			APP_PRINT("EPPROM_ZERO[%d]\n",(int32_t)(ctZeroCalibration[i]*1000));
			APP_PRINT("EPPROM_AMPLIFY[%d]\n",(int32_t)(ctAmplifyCalibration[i]*1000));
		}

		/*finished initialize*/
		timer_set(AC_TASK_ACCU_ID, AC_ACCU_VOLT_CHECK, AC_ACCU_TASK_TIMER_VOLT_CHECK, TIMER_PERIODIC);
	}
		break;
	case AC_ACCU_VOLT_CHECK:{
		/*READ VOLTAGE DATA FROM LTC6803*/
		APP_DBG("\r\n*****************LTC6803-CHECK*****************\r\n");
		APP_DBG("READ VOLTAGE DATA FROM LTC6803\r\n");

		if (debug_accu_on) {APP_DBG("LTC6803_wrcfg\r\n");}
		LTC6803_wrcfg(LTC6803_TOTAL_IC, w_config);

		if (debug_accu_on) {APP_DBG("LTC6803_rdcfg\r\n");}
		ltc6803_stt = LTC6803_rdcfg(LTC6803_TOTAL_IC, r_config);

		if (ltc6803_stt != 0){
			APP_DBG("stt %d\r\n", ltc6803_stt);
			print_rx_data(LTC6803_TOTAL_IC,ltc_rdcfg);
		}

		//clear regidter for sure
		if (debug_accu_on) {APP_DBG("LTC6803_clear_stcvad\r\n");}
		LTC6803_clear_stcvad();
		sys_ctrl_delay_ms(10);

		//start converse
		if (debug_accu_on) {APP_DBG("LTC6803_stcvad\r\n");}
		LTC6803_stcvad();
		sys_ctrl_delay_ms(10);

		//read voltage conversed
		if (debug_accu_on) {APP_PRINT("LTC6803_rdcv\r\n");}
		ltc6803_stt = LTC6803_rdcv(LTC6803_TOTAL_IC, cells);
		if (ltc6803_stt != 0){
			print_rx_data(LTC6803_TOTAL_IC,ltc_rdcv);
			APP_DBG("LTC6803_rdcv PEC error!!\n");
			APP_DBG("stt %d\r\n", ltc6803_stt);
		}
		save_cells_voltage();
	}
		break;
	case AC_ACCU_CT_CHECK:
		/* TO DO CODE HERE */
		if(debug_accu_on){
			APP_PRINT("****************CT-READ******************\r\n");
			for (uint8_t i = 0; i < NUM_OF_CT_SENSORS; i++){
				APP_PRINT("I[%d]: %d mA\t",i,(int32_t)(accu_data_frame.I_Bat[i]*1000));
			}
			APP_PRINT("\n");
		}
		break;
	default:
		break;
	}
}

void sys_irq_timer_1s() {
	dem_tmp++;
	if(dem_tmp >= 3){
		dem_tmp = 0;
	xprintf("\r\n\n");
	APP_DBG("****************CT-CHECK - timer 1s******************\r\n");
//	for (uint8_t i = 0; i < NUM_OF_CT_SENSORS; i++){
	static uint8_t  i =0;
		adc_74hc4052_set_pin_mode(i);
		sys_ctrl_delay_ms(1);
		accu_data_frame.I_Bat[i] = (float)(((adc_ct_io_read(ADC_Channel_9) - adc_ct_io_read(ADC_Channel_8))*3300)/
					(ADC_COUNTS_CT_SENSOR * ADC_CT_RATIO_mV_PER_A * ctAmplifyCalibration[i]));
		APP_DBG("I[%d]: %d.%d%d\t",i,(uint32_t)accu_data_frame.I_Bat[i],(uint32_t)(accu_data_frame.I_Bat[i]*10)%10,(uint32_t)(accu_data_frame.I_Bat[i]*100)%10);
//	}
		if(i++ == 3)
		{i=0;}
	}
}

/************************************************************
 * @func: sys_irq_dma1cn1_ctsensor().
 * @description: Process ADC raw values when DMA interrupt.
 * 				ADC raw values	->	LPF(fc = 1Hz)
 * 				-> Calibration with 'vdd'
 * 				-> save in 'accu_data_frame.I_Bat[]'.
 * @note:	+ 'ADC_ConvertedValue[]' are raw values array of DMA ADC
*************************************************************/
void sys_irq_dma1cn1_ctsensor(){
	static uint16_t Number_of_Samples = 0;
	static uint8_t ct_sensor = 0;

	uint16_t vdd = 3300;

	static int16_t sampleI;
	static double sumI;

	/*ADC value*/
	sampleI = ADC_ConvertedValue[0] - ADC_ConvertedValue[1]; /* Vref = 1.65V = ADC_ConvertedValue[1]*/
	/*Sum*/
	sumI += sampleI;

	Number_of_Samples++;
	if (Number_of_Samples == NUM_OF_SAMPLES_CT_SENSOR){
//		APP_DBG("sampleI = %d - %d\n", ADC_ConvertedValue[0], ADC_ConvertedValue[1]);
		/*Calculation I_Bat*/
		accu_data_frame.I_Bat[ct_sensor] = ((sumI / Number_of_Samples) * vdd) /
				(ADC_COUNTS_CT_SENSOR * ADC_CT_RATIO_mV_PER_A * ctAmplifyCalibration[ct_sensor]) - ctZeroCalibration[ct_sensor];

		/*Switch ADC channel*/
		ct_sensor++;
		if (ct_sensor > 3){
			ct_sensor = 0;
		}
		adc_74hc4052_set_pin_mode(ct_sensor);
		sys_ctrl_delay_ms(1);
//		APP_PRINT("Mode: %d\n", ct_sensor);
//		APP_PRINT("I[%d]: %d\n", ct_sensor,(int32_t)(accu_data_frame.I_Bat[ct_sensor]*1000));

		/*Reset variables*/
		sumI = 0;
		Number_of_Samples = 0;
	}
}


