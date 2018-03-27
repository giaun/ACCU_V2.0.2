#include "app.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_accu.h"
#include "task_rs485_modbus.h"

#include "../ak/fsm.h"
#include "../ak/port.h"
#include "../ak/message.h"
#include "../ak/timer.h"

#include "../common/cmd_line.h"
#include "../common/utils.h"
#include "../common/xprintf.h"

#include "../sys/sys_irq.h"
#include "../sys/sys_io.h"
#include "../sys/sys_ctrl.h"
#include "../sys/sys_dbg.h"
#include "../sys/sys_arduino.h"


Modbus slave_accu;
bool finish_rev_data = 0;
uint32_t receive_timeout = 100;
uint32_t finish_time = 0;
uint16_t send_data[NUM_OF_CT_SENSORS*2 + LTC6803_TOTAL_IC*NUM_OF_ACCU_PER_LTC6803];
//int8_t stt = 0;

/*rs485 status*/
int8_t rs485_stt[10];// = -2;

void sys_irq_RS485() {
	slave_accu.getData_serial(io_uart_RS485_get_byte());

	if (finish_rev_data == 1){
		finish_rev_data = 0;
		timer_set(AC_TASK_RS485_ID, AC_RS485_ACCU_UPDATE, AC_RS485_UPDATE_INTERVAL, TIMER_ONE_SHOT);
	}

	finish_time = millis();
}

void task_rs485(ak_msg_t* msg) {

	switch (msg->sig) {
	case AC_RS485_INITIAL: {
//		APP_PRINT("MODBUS INIT\n");
		mem_set(rs485_stt,-2,sizeof(int8_t)*10);
		slave_accu.init(slave_accu_ID, MODBUS_RS485_DIR_PIN, BAUDRATE);
		finish_rev_data = 1; //allow 'AC_RS485_ACCU_UPDATE' when interrupt
		break;
	}
	case AC_RS485_ACCU_UPDATE: {
		if (finish_rev_data == 0){
			if (abs(millis()-finish_time) > receive_timeout){
				if(debug_accu_on) APP_PRINT("\n[RS485] Polling data....\n");

				/* set data to send_data[] buffer*/
				for (uint8_t i = 0; i < (sizeof(send_data)/sizeof(send_data[0])); i++){
					/*voltages*/
					if (i < LTC6803_TOTAL_IC * NUM_OF_ACCU_PER_LTC6803){
						send_data[i] = 0xffff&(uint16_t)(accu_data_frame.V_Bat[i/NUM_OF_ACCU_PER_LTC6803][i%NUM_OF_ACCU_PER_LTC6803]*1000);
					}
					else if (i < (LTC6803_TOTAL_IC * NUM_OF_ACCU_PER_LTC6803 + NUM_OF_CT_SENSORS * 2)){
						/*ampers*/
						if (i%2 == 0){ // signal bit (+ or -)
							// I > 0: accu being used
							if (accu_data_frame.I_Bat[(i-LTC6803_TOTAL_IC*NUM_OF_ACCU_PER_LTC6803)/2] >= 0)
								send_data[i] = 1; //bit 1 for 'discharge and normal' signal
							else
								send_data[i] = 0;
						}
						else{
//							send_data[i] = 1000;
							send_data[i] = 0xffff&(uint16_t)(accu_data_frame.I_Bat[(i-LTC6803_TOTAL_IC*NUM_OF_ACCU_PER_LTC6803-1)/2]*1000);
						}
					}
					//APP_PRINT("%d\n", (uint32_t)send_data[i]);
				}

				/* check status */
				rs485SttHistory();
				rs485_stt[0] = slave_accu.poll(send_data, sizeof(send_data));

				/* set flag for finish */
				finish_rev_data = 1;
			}
			else{
				timer_set(AC_TASK_RS485_ID, AC_RS485_ACCU_UPDATE, AC_RS485_UPDATE_INTERVAL, TIMER_ONE_SHOT);
			}
		}
	}
		break;
	default:
		break;
	}
}
void rs485SttHistory() {
	for (uint8_t i=9; i>0; i--){
		rs485_stt[i] = rs485_stt[i-1];
	}
}
