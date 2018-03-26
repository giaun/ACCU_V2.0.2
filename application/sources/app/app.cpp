/**
 ******************************************************************************
 * @author: ThanNT
 * @date:   13/08/2016
 ******************************************************************************
**/

/* kernel include */
#include "ak.h"
#include "message.h"
#include "timer.h"
#include "fsm.h"

/* driver include */
#include "led.h"
#include "button.h"
#include "flash.h"
#include "hs1101.h"
#include "LTC68032.h"

/* app include */
#include "app.h"
#include "app_dbg.h"
#include "app_bsp.h"
#include "app_flash.h"
#include "app_non_clear_ram.h"

#include "task_list.h"
#include "task_shell.h"
#include "task_life.h"
#include "task_if.h"
#include "task_rf24_if.h"
#include "task_uart_if.h"

/* sys include */
#include "sys_boot.h"
#include "sys_irq.h"
#include "sys_io.h"
#include "sys_ctrl.h"
#include "sys_dbg.h"

/* arduino include */
#include "SPI.h"
#include "WString.h"

/* common include */
#include "utils.h"
#include "screen_manager.h"

/* net link include */
#include "link.h"
#include "link_sig.h"

const char* app_version = APP_VER;

static boot_app_share_data_t boot_app_share_data;

//static void app_power_on_reset();
static void app_start_timer();
static void app_init_state_machine();
static void app_task_init();

/*****************************************************************************/
/* app main function.
 */
/*****************************************************************************/
int main_app() {
	APP_PRINT("app version: %s\n", app_version);

	sys_soft_reboot_counter++;

	/******************************************************************************
	* init active kernel
	*******************************************************************************/
	ENTRY_CRITICAL();
	task_init();
	task_create(app_task_table);
	EXIT_CRITICAL();

	/******************************************************************************
	* init applications
	*******************************************************************************/
	/*********************
	* hardware configure *
	**********************/
	/* init watch dog timer */
	sys_ctrl_independent_watchdog_init();	/* 32s */
	sys_ctrl_soft_watchdog_init(200);		/* 20s */

	/* spi cs init */
	LTC6803_initialize();

	/* adc peripheral configure */
	/* adc configure for ct sensor */
//	io_cfg_adc1();
	io_cfg_adc1_dma1cn1();

	/* adc select channel pin configure */
	io_74hc4052_init();
	adc_74hc4052_set_pin_mode(MODE_0);

	/* RS485 configure */
	//UART init
	RS485_uart_init(115200);
	//RS485 DIR PIN init
	io_cfg_RS485_dir_pin();

	/* flash io init */
//	flash_io_ctrl_init();
//	timer_1s_init();

	/*********************
	* software configure *
	**********************/
	/* initial boot object */
	sys_boot_init();

	/* life led init */
	led_init(&led_life, led_life_init, led_life_on, led_life_off);

	/* button init */
	button_init(&btn_mode,	10,	BUTTON_MODE_ID,	io_button_mode_init,	io_button_mode_read,	btn_mode_callback);
//	button_init(&btn_up,	10,	BUTTON_UP_ID,	io_button_up_init,		io_button_up_read,		btn_up_callback);
//	button_init(&btn_down,	10,	BUTTON_DOWN_ID,	io_button_down_init,	io_button_down_read,	btn_down_callback);

	button_enable(&btn_mode);
	button_enable(&btn_up);
	button_enable(&btn_down);

	/* start timer for application */
	app_init_state_machine();
	app_start_timer();
//	timer_1s_enable();

	/******************************************************************************
	* app task initial
	*******************************************************************************/
	app_task_init();

	EXIT_CRITICAL();

	/******************************************************************************
	* run applications
	*******************************************************************************/
	return task_run();
}

/*****************************************************************************/
/* app initial function.
 */
/*****************************************************************************/

/* start software timer for application
 * used for app tasks
 */
void app_start_timer() {
	/* start timer to toggle life led */
	timer_set(AC_TASK_LIFE_ID, AC_LIFE_SYSTEM_CHECK, AC_LIFE_TASK_TIMER_LED_LIFE_INTERVAL, TIMER_PERIODIC);
//	timer_set(AC_TASK_ACCU_ID, AC_ACCU_CT_CHECK, AC_ACCU_TASK_TIMER_AMPER_CHECK, TIMER_PERIODIC);
}

/* init state machine for tasks
 * used for app tasks
 */
void app_init_state_machine() {
}

/* send first message to trigger start tasks
 * used for app tasks
 */
void app_task_init() {
	//timer_1s_enable();										//start read CT sensor by timer (NOTE: turn off DMA)
	task_post_pure_msg(AC_TASK_FW_ID, FW_CHECKING_REQ);			//checking firmware
	task_post_pure_msg(AC_TASK_ACCU_ID, AC_ACCU_INITIAL);		//start read LTC6803G-4
	task_post_pure_msg(AC_TASK_RS485_ID, AC_RS485_INITIAL);		//INIT RS485
}

/*****************************************************************************/
/* app common function
 */
/*****************************************************************************/

/* hardware timer interrupt 10ms
 * used for led, button polling
 */
void sys_irq_timer_10ms() {
	button_timer_polling(&btn_mode);
//	button_timer_polling(&btn_up);
//	button_timer_polling(&btn_down);
}

/* hardware timer interrupt 50ms
 * used for encode and decode ir
 */
void sys_irq_timer_50us() {
}

/* hardware rtc interrupt alarm
 * used for internal rtc alarm
 */
void sys_irq_timer_hs1101() {
}

/* hardware io interrupt at rev ir pin
 * used for decode ir
 */
void sys_irq_ir_io_rev() {
}

void sys_irq_usb_recv(uint8_t* data, uint32_t len) {
	(void)data;
	(void)len;
}

/* init non-clear RAM objects
 */
void app_power_on_reset() {
	sys_soft_reboot_counter = 0;
}

void* app_get_boot_share_data() {
	return (void*)&boot_app_share_data;
}
