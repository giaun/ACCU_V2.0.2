/**
 ******************************************************************************
 * @author: ThanNT
 * @date:   13/08/2016
 ******************************************************************************
**/

#include <stdint.h>
#include <stdlib.h>

#include "ak.h"
#include "task.h"
#include "timer.h"
#include "message.h"

#include "cmd_line.h"
#include "utils.h"
#include "xprintf.h"
#include "view_render.h"

#include "sys_ctrl.h"
#include "sys_io.h"
#include "sys_dbg.h"
#include "sys_irq.h"
#include "sys_svc.h"
#include "sys_arduino.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"
#include "app_flash.h"
#include "app_eeprom.h"
#include "app_non_clear_ram.h"

#include "task_shell.h"
#include "task_list.h"
#include "task_list_if.h"
#include "task_if.h"
#include "task_life.h"
#include "link_phy.h"
#include "link_hal.h"
#include "link_sig.h"
#include "task_accu.h"
#include "task_rs485_modbus.h"

#include "rtc.h"
#include "led.h"
#include "eeprom.h"
#include "Adafruit_ssd1306syp.h"
#include "EmonLib.h"
#include "DS1302.h"
#include "flash.h"
#include "hs1101.h"
#include "exor.h"
#include "eeprom.h"

/*****************************************************************************/
/*  local declare
 */
/*****************************************************************************/
#define STR_LIST_MAX_SIZE		50
#define STR_BUFFER_SIZE			500

static char cmd_buffer[STR_BUFFER_SIZE];
static char* str_list[STR_LIST_MAX_SIZE];
static uint8_t str_list_len;

static uint8_t str_parser(char* str);
static char* str_parser_get_attr(uint8_t);

/*****************************************************************************/
/*  command function declare
 */
/*****************************************************************************/
static int32_t shell_reset(uint8_t* argv);
static int32_t shell_ver(uint8_t* argv);
static int32_t shell_help(uint8_t* argv);
static int32_t shell_reboot(uint8_t* argv);
//static int32_t shell_ram(uint8_t* argv);
static int32_t shell_fatal(uint8_t* argv);
static int32_t shell_stt(uint8_t* argv);
//static int32_t shell_epi(uint8_t* argv);
//static int32_t shell_eps(uint8_t* argv);
//static int32_t shell_flash(uint8_t* argv);
//static int32_t shell_lcd(uint8_t* argv);
static int32_t shell_dbg(uint8_t* argv);
static int32_t shell_boot(uint8_t* argv);
static int32_t shell_fwu(uint8_t* argv);
//static int32_t shell_svc(uint8_t* argv);
//static int32_t shell_psv(uint8_t* argv);
static int32_t shell_accu(uint8_t* argv);

/*****************************************************************************/
/*  command table
 */
/*****************************************************************************/
cmd_line_t lgn_cmd_table[] = {

	/*************************************************************************/
	/* system command */
	/*************************************************************************/
	{(const int8_t*)"reset",	shell_reset,		(const int8_t*)"reset terminal"},
	{(const int8_t*)"ver",		shell_ver,			(const int8_t*)"version info"},
	{(const int8_t*)"help",		shell_help,			(const int8_t*)"help info"},
	{(const int8_t*)"reboot",	shell_reboot,		(const int8_t*)"reboot"},
//	{(const int8_t*)"ram",		shell_ram,			(const int8_t*)"ram"},
//	{(const int8_t*)"epi",		shell_epi,			(const int8_t*)"epprom init"},
	{(const int8_t*)"fatal",	shell_fatal,		(const int8_t*)"fatal info"},
	{(const int8_t*)"stt",		shell_stt,			(const int8_t*)"app status"},
//	{(const int8_t*)"eps",		shell_eps,			(const int8_t*)"epprom"},
//	{(const int8_t*)"flash",	shell_flash,		(const int8_t*)"flash"},
//	{(const int8_t*)"lcd",		shell_lcd,			(const int8_t*)"lcd"},
	{(const int8_t*)"boot",		shell_boot,			(const int8_t*)"boot share "},
	{(const int8_t*)"fwu",		shell_fwu,			(const int8_t*)"app burn firmware"},
//	{(const int8_t*)"svc",		shell_svc,			(const int8_t*)"system service call"},
//	{(const int8_t*)"psv",		shell_psv,			(const int8_t*)"psv"},
	{(const int8_t*)"accu",		shell_accu,			(const int8_t*)"accu debug info"},

	/*************************************************************************/
	/* debug command */
	/*************************************************************************/
	{(const int8_t*)"dbg",		shell_dbg,			(const int8_t*)"dbg"},

	/* End Of Table */
	{(const int8_t*)0,(pf_cmd_func)0,(const int8_t*)0}
};

uint8_t str_parser(char* str) {
	strcpy(cmd_buffer, str);
	str_list_len = 0;

	uint8_t i = 0;
	uint8_t str_list_index = 0;
	uint8_t flag_insert_str = 1;

	while (cmd_buffer[i] != 0 && cmd_buffer[i] != '\n' && cmd_buffer[i] != '\r') {
		if (cmd_buffer[i] == ' ') {
			cmd_buffer[i] = 0;
			flag_insert_str = 1;
		}
		else if (flag_insert_str) {
			str_list[str_list_index++] = &cmd_buffer[i];
			flag_insert_str = 0;
		}
		i++;
	}

	cmd_buffer[i] = 0;

	str_list_len = str_list_index;
	return str_list_len;
}

char* str_parser_get_attr(uint8_t index) {
	if (index < str_list_len) {
		return str_list[index];
	}
	return NULL;
}

/*****************************************************************************/
/*  command function definaion
 */
/*****************************************************************************/

int32_t shell_reset(uint8_t* argv) {
	(void)argv;
	xprintf("\033[2J\r");
	return 0;
}

int32_t shell_ver(uint8_t* argv) {
	(void)argv;

	firmware_header_t firmware_header;
	sys_ctrl_get_firmware_info(&firmware_header);

	LOGIN_PRINT("Kerneljhgjgj version: %s\n", AK_VERSION);
	LOGIN_PRINT("App version: %s\n", app_version);
	LOGIN_PRINT("Firmware checksum: %04x\n", firmware_header.checksum);
	LOGIN_PRINT("Firmware length: %d\n", firmware_header.bin_len);

	LOGIN_PRINT("\nSystem information:\n");
	LOGIN_PRINT("\tFLASH used:\t%d bytes\n", system_info.flash_used);
	LOGIN_PRINT("\tSRAM used:\t%d bytes\n", system_info.ram_used);
	LOGIN_PRINT("\t\tdata init size:\t\t%d bytes\n", system_info.data_init_size);
	LOGIN_PRINT("\t\tdata non_init size:\t%d bytes\n", system_info.data_non_init_size);
	LOGIN_PRINT("\t\tstack avail:\t\t%d bytes\n", system_info.stack_avail);
	LOGIN_PRINT("\t\theap avail:\t\t%d bytes\n", system_info.heap_avail);
	LOGIN_PRINT("\t\tother:\t\t\t%d bytes\n", system_info.ram_other);
	LOGIN_PRINT("\n");
	LOGIN_PRINT("\tcpu clock:\t%d Hz\n", system_info.cpu_clock);
	LOGIN_PRINT("\ttime tick:\t%d ms\n", system_info.tick);
	LOGIN_PRINT("\tconsole:\t%d bps\n", system_info.console_baudrate);
	LOGIN_PRINT("\n\n");
	return 0;
}

int32_t shell_help(uint8_t* argv) {
	uint32_t idx = 0;
	switch (*(argv + 5)) {
	case 'a':{
		if ( (*(argv + 6) == 'c') && (*(argv + 7) == 'c') && (*(argv + 8) == 'u') ){
			LOGIN_PRINT("\nACCU COMMANDS INFORMATION:\n\n");
			LOGIN_PRINT("FORM:\n\taccu [ARGV]\n\n\nARGV:\n");
			LOGIN_PRINT("\ta\n\t\tcheck CT sensor\n\n");
			LOGIN_PRINT("\t\tac\n\t\t\tcalib CT sensor\n\n");

			LOGIN_PRINT("\tv\n\t\tcheck LTC6803\n\n");
			LOGIN_PRINT("\t\tv0\n\t\t\tcheck ACCU Voltage of Group 1\n\n");
			LOGIN_PRINT("\t\tv1\n\t\t\tcheck ACCU Voltage of Group 2\n\n");
			LOGIN_PRINT("\t\tv2\n\t\t\tcheck ACCU Voltage of Group 3\n\n");
			LOGIN_PRINT("\t\tv3\n\t\t\tcheck ACCU Voltage of Group 4\n\n");

			LOGIN_PRINT("\tc\n\t\tcheck RS485 connecttion\n\n");
			LOGIN_PRINT("\tr\n\t\treset all stt variables\n\n");
		}
	}
		break;
	default:
		LOGIN_PRINT("\nCOMMANDS INFORMATION:\n\n");
		while(lgn_cmd_table[idx].cmd != (const int8_t*)0) {
			LOGIN_PRINT("%s\n\t%s\n\n", lgn_cmd_table[idx].cmd, lgn_cmd_table[idx].info);
			idx++;
		}
		break;
	}
	return 0;
}

int32_t shell_reboot(uint8_t* argv) {
	(void)argv;
	sys_ctrl_delay_ms(10);
	sys_ctrl_reset();
	return 0;
}

int32_t shell_fatal(uint8_t* argv) {
	fatal_log_t login_fatal_log;
	ak_msg_t t_msg;
	exception_info_t t_exception_info;

	switch (*(argv + 6)) {
	case 't':
		FATAL("TEST", 0x02);
		break;

	case '!':
		while(1);
		break;

	case '@':
		DISABLE_INTERRUPTS();
		while(1);
		break;

	case 'r':
		memset((uint8_t*)&login_fatal_log, 0, sizeof(fatal_log_t));
//		flash_erase_sector(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR);
//		flash_write(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR, (uint8_t*)&login_fatal_log, sizeof(fatal_log_t));
		eeprom_erase(EEPROM_FATAL_LOG_ADDR,256);
		eeprom_write(EEPROM_FATAL_LOG_ADDR, (uint8_t*)&login_fatal_log, sizeof(fatal_log_t));
		LOGIN_PRINT("reset fatal log OK\n");
		break;

	case 'l': {
//		flash_read(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR, (uint8_t*)&login_fatal_log, sizeof(fatal_log_t));
		eeprom_read(EEPROM_FATAL_LOG_ADDR, (uint8_t*)&login_fatal_log, sizeof(fatal_log_t));

		LOGIN_PRINT("[times] fatal: %d\n",		login_fatal_log.fatal_times);
		LOGIN_PRINT("[times] restart: %d\n",	login_fatal_log.restart_times);

		LOGIN_PRINT("\n");
		LOGIN_PRINT("[fatal] type: %s\n",		login_fatal_log.string);
		LOGIN_PRINT("[fatal] code: 0x%02X\n",	login_fatal_log.code);

		LOGIN_PRINT("\n");
		LOGIN_PRINT("[task] id: %d\n",			login_fatal_log.current_task.id);
		LOGIN_PRINT("[task] pri: %d\n",			login_fatal_log.current_task.pri);
		LOGIN_PRINT("[task] entry: 0x%x\n",		login_fatal_log.current_task.task);

		LOGIN_PRINT("\n");
		LOGIN_PRINT("[obj] task: %d\n",			login_fatal_log.current_active_object.des_task_id);
		LOGIN_PRINT("[obj] sig: %d\n",			login_fatal_log.current_active_object.sig);
		LOGIN_PRINT("[obj] type: 0x%x\n",		get_msg_type(&login_fatal_log.current_active_object));
		LOGIN_PRINT("[obj] ref count: %d\n",	get_msg_ref_count(&login_fatal_log.current_active_object));
		LOGIN_PRINT("[obj] wait time: %d\n",	login_fatal_log.current_active_object.dbg_handler.start_exe - login_fatal_log.current_active_object.dbg_handler.start_post);

		LOGIN_PRINT("\n");
		LOGIN_PRINT("[core] ipsr: %d\n",			login_fatal_log.m3_core_reg.ipsr);
		LOGIN_PRINT("[core] primask: 0x%08X\n",		login_fatal_log.m3_core_reg.primask);
		LOGIN_PRINT("[core] faultmask: 0x%08X\n",	login_fatal_log.m3_core_reg.faultmask);
		LOGIN_PRINT("[core] basepri: 0x%08X\n",		login_fatal_log.m3_core_reg.basepri);
		LOGIN_PRINT("[core] control: 0x%08X\n",		login_fatal_log.m3_core_reg.control);

		LOGIN_PRINT("\n");
		LOGIN_PRINT("[irq] IRQ number: %d\n",			(int32_t)((int32_t)login_fatal_log.m3_core_reg.ipsr - (int32_t)SYS_IRQ_EXCEPTION_NUMBER_IRQ0_NUMBER_RESPECTIVE));
	}
		break;

	case 'm': {
		uint32_t	flash_sys_log_address = APP_FLASH_AK_DBG_MSG_SECTOR_1;
		for (uint32_t index = 0; index < (LOG_QUEUE_OBJECT_SIZE / sizeof(ak_msg_t)); index++) {
			/* reset watchdog */
			sys_ctrl_independent_watchdog_reset();
			sys_ctrl_soft_watchdog_reset();

			flash_read(flash_sys_log_address, (uint8_t*)&t_msg, sizeof(ak_msg_t));
			flash_sys_log_address += sizeof(ak_msg_t);

			uint32_t wait_time;
			if (t_msg.dbg_handler.start_exe >= t_msg.dbg_handler.start_post) {
				wait_time = t_msg.dbg_handler.start_exe - t_msg.dbg_handler.start_post;
			}
			else {
				wait_time = t_msg.dbg_handler.start_exe + (0xFFFFFFFF - t_msg.dbg_handler.start_post);
			}

			uint32_t exe_time;
			if (t_msg.dbg_handler.stop_exe >= t_msg.dbg_handler.start_exe) {
				exe_time = t_msg.dbg_handler.stop_exe - t_msg.dbg_handler.start_exe;
			}
			else {
				exe_time = t_msg.dbg_handler.stop_exe + (0xFFFFFFFF - t_msg.dbg_handler.start_exe);
			}

			LOGIN_PRINT("index: %d\ttask_id: %d\tmsg_type:0x%x\tref_count:%d\tsig: %d\t\twait_time: %d\texe_time: %d\n"\
						, index										\
						, t_msg.des_task_id								\
						, (t_msg.ref_count & AK_MSG_TYPE_MASK)		\
						, (t_msg.ref_count & AK_MSG_REF_COUNT_MASK)	\
						, t_msg.sig									\
						, (wait_time)								\
						, (exe_time));
		}
	}
		break;

	case 'e': {
		uint32_t	flash_irq_log_address = APP_FLASH_AK_DBG_IRQ_LOG_SECTOR;
		for (uint32_t index = 0; index < (LOG_QUEUE_IRQ_SIZE / sizeof(exception_info_t)); index++) {
			/* reset watchdog */
			sys_ctrl_independent_watchdog_reset();
			sys_ctrl_soft_watchdog_reset();

			flash_read(flash_irq_log_address, (uint8_t*)&t_exception_info, sizeof(exception_info_t));
			flash_irq_log_address += sizeof(exception_info_t);

			LOGIN_PRINT("index: %d\texcept_number: %d\tirq_number: %d\ttimestamp: %d\n"\
						, index										\
						, t_exception_info.except_number																				\
						, (int32_t)((int32_t)t_exception_info.except_number - (int32_t)SYS_IRQ_EXCEPTION_NUMBER_IRQ0_NUMBER_RESPECTIVE)	\
						, t_exception_info.timestamp);
		}
	}
		break;

	case 'R': {
		uint8_t len = str_parser((char*)argv);
		if (len == 3) {
			extern uint32_t _start_ram;
			extern uint32_t _estack;

			uint8_t val;
			uint32_t len_of_ram = (uint32_t)&_estack - (uint32_t)&_start_ram;

			uint32_t colum = (uint32_t)strtol(str_parser_get_attr(2), NULL, 0);

			LOGIN_PRINT("\n");
			for (uint32_t i = 0; i < len_of_ram; i++) {
				if (!(i % colum)) {
					/* reset watchdog */
					sys_ctrl_independent_watchdog_reset();
					sys_ctrl_soft_watchdog_reset();

					LOGIN_PRINT("\n0x%x\t" , (uint32_t)&_start_ram + i);
				}

				flash_read(APP_FLASH_DUMP_RAM_START_ADDR + i, &val, sizeof(uint8_t));
				LOGIN_PRINT("%d\t", val);
			}
			LOGIN_PRINT("\n");
		}
		else {
			LOGIN_PRINT("syntax error\n");
		}
	}
		break;

	default:
		break;
	}

	return 0;
}

int32_t shell_stt(uint8_t* argv) {
	(void)argv;
	LOGIN_PRINT("SYS:\n\n");
	LOGIN_PRINT("sys_soft_reboot_counter: %d\n", sys_soft_reboot_counter);
	LOGIN_PRINT("get_nest_entry_critical_counter: %d\n", get_nest_entry_critical_counter());
	return 0;
}

int32_t shell_epi(uint8_t* argv) {
	(void)argv;
	return 0;
}

int32_t shell_eps(uint8_t* argv) {
	uint8_t val = 0;

	switch (*(argv + 4)) {
	case 'd': {					/* data DEC format */
		LOGIN_PRINT("\n");
		for (uint32_t i = 0; i < EEPROM_END_ADDR; i++) {
			if (!(i%16)) {
				/* reset watchdog */
				sys_ctrl_independent_watchdog_reset();
				sys_ctrl_soft_watchdog_reset();

				LOGIN_PRINT("\n0x%x\t" ,i);
			}
			eeprom_read(i, &val, sizeof(uint8_t));
			LOGIN_PRINT("%d\t", val);
		}
		LOGIN_PRINT("\n");
	}
		break;

	case 'h': {					/* data HEX format */
		LOGIN_PRINT("\n");
		for (uint32_t i = 0; i < EEPROM_END_ADDR; i++) {
			if (!(i%16)) {
				/* reset watchdog */
				sys_ctrl_independent_watchdog_reset();
				sys_ctrl_soft_watchdog_reset();

				LOGIN_PRINT("\n0x%x\t" ,i);
			}
			eeprom_read(i, &val, sizeof(uint8_t));
			LOGIN_PRINT("0x%x\t", val);
		}
		LOGIN_PRINT("\n");
	}
		break;

	case 'r': {
		LOGIN_PRINT("erasing...\n");
		eeprom_erase(EEPROM_START_ADDR, EEPROM_END_ADDR - EEPROM_START_ADDR);
		LOGIN_PRINT("completed\n");
	}
		break;

	default:
		LOGIN_PRINT("unkown option !\n");
		break;
	}

	return 0;
}

int32_t shell_flash(uint8_t* argv) {
	/* "flash d 0x1000 0xA000" */
	switch (*(argv + 6)) {
	case 'r':
		LOGIN_PRINT("flash erasing...\n");
		flash_erase_full();
		LOGIN_PRINT("completed\n");
		break;

	case 'd': {
		uint8_t val;
		uint8_t len = str_parser((char*)argv);
		if (len == 4) {
			char* str_start_addr = str_parser_get_attr(2);
			char* str_stop_addr = str_parser_get_attr(3);
			uint32_t start_addr = strtol(str_start_addr, NULL, 0);
			uint32_t stop_addr = strtol(str_stop_addr, NULL, 0);
			LOGIN_PRINT("start_addr: 0x%x\n", start_addr);
			LOGIN_PRINT("stop_addr: 0x%x\n", stop_addr);

			LOGIN_PRINT("\n");
			for (uint32_t i = start_addr; i < stop_addr; i++) {
				if (!(i%16)) {
					/* reset watchdog */
					sys_ctrl_independent_watchdog_reset();
					sys_ctrl_soft_watchdog_reset();

					LOGIN_PRINT("\n0x%x\t" ,i);
				}
				flash_read(i, &val, sizeof(uint8_t));
				LOGIN_PRINT("%d\t", val);
			}
			LOGIN_PRINT("\n");
		}
	}
		break;

	case 'h': {
		uint8_t val;
		uint8_t len = str_parser((char*)argv);
		if (len == 4) {
			char* str_start_addr = str_parser_get_attr(2);
			char* str_stop_addr = str_parser_get_attr(3);
			uint32_t start_addr = strtol(str_start_addr, NULL, 0);
			uint32_t stop_addr = strtol(str_stop_addr, NULL, 0);
			LOGIN_PRINT("start_addr: 0x%x\n", start_addr);
			LOGIN_PRINT("stop_addr: 0x%x\n", stop_addr);

			LOGIN_PRINT("\n");
			for (uint32_t i = start_addr; i < stop_addr; i++) {
				if (!(i%16)) {
					/* reset watchdog */
					sys_ctrl_independent_watchdog_reset();
					sys_ctrl_soft_watchdog_reset();

					LOGIN_PRINT("\n0x%x\t" ,i);
				}
				flash_read(i, &val, sizeof(uint8_t));
				LOGIN_PRINT("0x%x\t", val);
			}
			LOGIN_PRINT("\n");
		}
	}
		break;

	default:
		LOGIN_PRINT("unknow option\n");
		break;
	}

	return 0;
}

int32_t shell_lcd(uint8_t* argv) {
	switch (*(argv + 4)) {
	case 'i':
		view_render.initialize();
		break;

	case 'o':
		view_render.display_on();
		break;

	case 'f':
		view_render.display_off();
		break;

	case 'b':
		view_render.fillScreen(BLACK);
		view_render.update();
		break;

	case 'w':
		view_render.fillScreen(WHITE);
		view_render.update();
		break;

	case 't':
		view_render.setTextSize(1);
		view_render.setTextColor(WHITE);
		view_render.setCursor(0, 3);
		view_render.print("   __    _  _ ");
		view_render.setCursor(0, 10);
		view_render.print("  /__\\  ( )/ )");
		view_render.setCursor(0, 20);
		view_render.print(" /(__)\\ (   (");
		view_render.setCursor(0, 30);
		view_render.print("(__)(__)(_)\\_)");
		view_render.setCursor(0, 45);
		view_render.print("Active Kernel");
		view_render.update();
		break;

	default:
		LOGIN_PRINT("unknow option\n");
		break;
	}

	return 0;
}

#include "hal_nrf.h"

int32_t shell_dbg(uint8_t* argv) {
	(void)(argv);
	return 0;
}

int32_t shell_ram(uint8_t* argv) {
	extern uint32_t _start_ram;
	extern uint32_t _estack;

	char* str_start_addr = NULL;
	char* str_stop_addr = NULL;
	uint8_t* start_addr = NULL;
	uint8_t* stop_addr = NULL;

	uint8_t len = str_parser((char*)argv);

	LOGIN_PRINT("RAM start: 0x%x\n", ((uint32_t)&_start_ram));
	LOGIN_PRINT("RAM   end: 0x%x\n", ((uint32_t)&_estack));

	/* "ram x 0x1000 0xA000" */
	switch (*(argv + 4)) {
	case 'r': {
		if (len == 4) {
			str_start_addr	= str_parser_get_attr(2);
			str_stop_addr	= str_parser_get_attr(3);
			start_addr	= (uint8_t*)((uint32_t)strtol(str_start_addr, NULL, 0));
			stop_addr 	= (uint8_t*)((uint32_t)strtol(str_stop_addr, NULL, 0));

			LOGIN_PRINT("start_addr: 0x%x\n", start_addr);
			LOGIN_PRINT("stop_addr: 0x%x\n", stop_addr);

			if ((uint32_t)start_addr >= ((uint32_t)&_start_ram) && (uint32_t)stop_addr <= ((uint32_t)&_estack)) {

				/* start dump ram */
				LOGIN_PRINT("\n");

				for (uint8_t* addr_index = start_addr; addr_index < stop_addr; addr_index++) {
					if (!((uint32_t)addr_index % 4)) {
						/* reset watchdog */
						sys_ctrl_independent_watchdog_reset();
						sys_ctrl_soft_watchdog_reset();

						LOGIN_PRINT("\n0x%x\t", addr_index);
					}

					*addr_index = 0;
					LOGIN_PRINT("%d\t", *addr_index);
				}

				LOGIN_PRINT("\n");
				/* end dump ram */
			}
		}
		else {
			LOGIN_PRINT("syntax error\n");
		}
	}
		break;

	case 'd': { /* ram d 0x20000000 0x20004000 */
		if (len == 4) {
			str_start_addr	= str_parser_get_attr(2);
			str_stop_addr	= str_parser_get_attr(3);
			start_addr	= (uint8_t*)((uint32_t)strtol(str_start_addr, NULL, 0));
			stop_addr 	= (uint8_t*)((uint32_t)strtol(str_stop_addr, NULL, 0));

			LOGIN_PRINT("start_addr: 0x%x\n", start_addr);
			LOGIN_PRINT("stop_addr: 0x%x\n", stop_addr);

			if ((uint32_t)start_addr >= ((uint32_t)&_start_ram) && (uint32_t)stop_addr <= ((uint32_t)&_estack)) {

				/* start dump ram */
				LOGIN_PRINT("\n");

				for (uint8_t* addr_index = start_addr; addr_index < stop_addr; addr_index++) {
					if (!((uint32_t)addr_index % 4)) {
						/* reset watchdog */
						sys_ctrl_independent_watchdog_reset();
						sys_ctrl_soft_watchdog_reset();

						LOGIN_PRINT("\n0x%x\t", addr_index);
					}

					LOGIN_PRINT("%d\t", *addr_index);
				}

				LOGIN_PRINT("\n");
				/* end dump ram */
			}
		}
		else {
			LOGIN_PRINT("syntax error\n");
		}
	}
		break;

	case 'h': {
		if (len == 4) {
			str_start_addr	= str_parser_get_attr(2);
			str_stop_addr	= str_parser_get_attr(3);
			start_addr	= (uint8_t*)((uint32_t)strtol(str_start_addr, NULL, 0));
			stop_addr 	= (uint8_t*)((uint32_t)strtol(str_stop_addr, NULL, 0));

			LOGIN_PRINT("start_addr: 0x%x\n", start_addr);
			LOGIN_PRINT("stop_addr: 0x%x\n", stop_addr);

			if ((uint32_t)start_addr >= ((uint32_t)&_start_ram) && (uint32_t)stop_addr <= ((uint32_t)&_estack)) {

				/* start dump ram */
				LOGIN_PRINT("\n");

				for (uint8_t* addr_index = start_addr; addr_index < stop_addr; addr_index++) {
					if (!((uint32_t)addr_index % 4)) {
						/* reset watchdog */
						sys_ctrl_independent_watchdog_reset();
						sys_ctrl_soft_watchdog_reset();

						LOGIN_PRINT("\n0x%x\t", addr_index);
					}

					LOGIN_PRINT("0x%x\t", *addr_index);
				}

				LOGIN_PRINT("\n");
				/* end dump ram */
			}
		}
		else {
			LOGIN_PRINT("syntax error\n");
		}
	}
		break;

	case 's': {
		sys_dbg_stack_space_dump();
	}
		break;

	default:
		LOGIN_PRINT("unknown option\n");
		break;
	}

	return 0;
}

int32_t shell_boot(uint8_t* argv) {
	switch (*(argv + 5)) {
	case 'i': {
		sys_boot_t sys_boot;
		sys_boot_get(&sys_boot);
		LOGIN_PRINT("\n[b_fwc] psk: 0x%08X checksum: 0x%08X bin_len: %d\n", \
					sys_boot.current_fw_boot_header.psk, \
					sys_boot.current_fw_boot_header.checksum, \
					sys_boot.current_fw_boot_header.bin_len);
		LOGIN_PRINT("[b_fwu] psk: 0x%08X checksum: 0x%08X bin_len: %d\n", \
					sys_boot.update_fw_boot_header.psk, \
					sys_boot.update_fw_boot_header.checksum, \
					sys_boot.update_fw_boot_header.bin_len);
		LOGIN_PRINT("[b_cmd] cmd: %d container:%d io_driver: %d des_addr: 0x%08X src_addr: 0x%08X\n", \
					sys_boot.fw_boot_cmd.cmd, \
					sys_boot.fw_boot_cmd.container, \
					sys_boot.fw_boot_cmd.io_driver, \
					sys_boot.fw_boot_cmd.des_addr, \
					sys_boot.fw_boot_cmd.src_addr);

		LOGIN_PRINT("\n[a_fwc] psk: 0x%08X checksum: 0x%08X bin_len: %d\n", \
					sys_boot.current_fw_app_header.psk, \
					sys_boot.current_fw_app_header.checksum, \
					sys_boot.current_fw_app_header.bin_len);
		LOGIN_PRINT("[a_fwu] psk: 0x%08X checksum: 0x%08X bin_len: %d\n", \
					sys_boot.update_fw_app_header.psk, \
					sys_boot.update_fw_app_header.checksum, \
					sys_boot.update_fw_app_header.bin_len);
		LOGIN_PRINT("[a_cmd] cmd: %d container:%d io_driver: %d des_addr: 0x%08X src_addr: 0x%08X\n", \
					sys_boot.fw_app_cmd.cmd, \
					sys_boot.fw_app_cmd.container, \
					sys_boot.fw_app_cmd.io_driver, \
					sys_boot.fw_app_cmd.des_addr, \
					sys_boot.fw_app_cmd.src_addr);
	}
		break;

	case 'r': {
		sys_boot_t sys_boot;
		memset(&sys_boot, 0, sizeof(sys_boot_t));
		sys_boot_set(&sys_boot);
	}
		break;

	case 't': {
		sys_boot_t sb;
		sys_boot_get(&sb);

		/* cmd update request */
		sb.fw_app_cmd.cmd			= SYS_BOOT_CMD_UPDATE_REQ;
		sb.fw_app_cmd.container		= SYS_BOOT_CONTAINER_EXTERNAL_FLASH;
		sb.fw_app_cmd.io_driver		= SYS_BOOT_IO_DRIVER_NONE;
		sb.fw_app_cmd.des_addr		= APP_START_ADDR;
		sb.fw_app_cmd.src_addr		= APP_FLASH_FIRMWARE_START_ADDR;

		sys_boot_set(&sb);
	}
		break;

	default:
		break;
	}

	return 0;
}

int32_t shell_fwu(uint8_t* argv) {
	(void)argv;
	sys_boot_t sb;
	sys_boot_get(&sb);

	/* cmd update request */
	sb.fw_app_cmd.cmd			= SYS_BOOT_CMD_UPDATE_REQ;
	sb.fw_app_cmd.container		= SYS_BOOT_CONTAINER_DIRECTLY;
	sb.fw_app_cmd.io_driver		= SYS_BOOT_IO_DRIVER_UART;
	sb.fw_app_cmd.des_addr		= APP_START_ADDR;
	sb.fw_app_cmd.src_addr		= 0;
	sys_boot_set(&sb);

	sys_ctrl_reset();
	return 0;
}

int32_t shell_svc(uint8_t* argv) {
	switch (*(argv + 4)) {
	case 'r': {
		sys_svc_restart_app();
	}
		break;

	default:
		break;
	}

	return 0;
}

int32_t shell_psv(uint8_t* argv) {
	(void)argv;
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	return 0;
}

uint32_t myPow(uint8_t x, uint8_t y){
	uint32_t tmp = 1;
	if (y == 0)
	{
		return 1;
	}
	for (uint8_t i = 1; i <= y; i++){
		tmp = tmp*x;
	}
	return tmp;
}

int32_t shell_accu(uint8_t* argv){
	switch (*(argv + 5)) {
	/*check ct sensor (a -> amper)*/
	case 'a': {

		if (*(argv + 6) == 'c'){

			bool changed = 0;

			if ((*(argv + 7) == ' ') && (*(argv + 9) == ' ')){
				uint8_t i = 10;
				uint8_t amperNum = 0;
				uint32_t amper = 0;
				uint8_t accu_number = 0;
				float gain = 1;


				accu_number = *(argv + 8)-48;

				while((*(argv + i) != '\r') && (*(argv + i) != '\n'))
				{
					if(((*(argv + i)) < '0') || ((*(argv + i)) > '9')){
						LOGIN_PRINT("ERROR CMD!, i: %d\n",i);
						return -1;
					}
					else{
						i++;
					}
				}
				amperNum = i-- - 10;
				for (uint8_t k = 0; k < amperNum ; k++){

					amper += ((*(argv + i)-48) * myPow(10,k));
					i--;
				}

				if(amper > 0){
					if(accu_number >= NUM_OF_CT_SENSORS){
						for (uint8_t acnum = 0; acnum < NUM_OF_CT_SENSORS; acnum++){
							gain = (abs(accu_data_frame.I_Bat[acnum] * 1000) / amper);
							ctAmplifyCalibration[acnum] = (ctAmplifyCalibration[acnum] * gain);
							ctZeroCalibration[acnum] =(ctZeroCalibration[acnum] / gain);
							LOGIN_PRINT("new amper calibration for accu %d: %d/1000\n",acnum, (uint32_t)(ctAmplifyCalibration[acnum]*1000));
						}
						changed = 1;
					}
					else{
						//LOGIN_PRINT("amper to Calibration: %d, accu: %d\n", amper, accu_number);
						gain = (abs(accu_data_frame.I_Bat[accu_number] * 1000) / amper);
						ctAmplifyCalibration[accu_number] = (ctAmplifyCalibration[accu_number] * gain);
						ctZeroCalibration[accu_number] =(ctZeroCalibration[accu_number] / gain);
						LOGIN_PRINT("new amper calibration for accu %d: %d/1000\n",accu_number, (uint32_t)(ctAmplifyCalibration[accu_number]*1000));
					}
					changed = 1;
				}

				if (changed)
					eeprom_write(EEPROM_CALIB_AMPLIFY_ADDR, (uint8_t*)ctAmplifyCalibration, NUM_OF_CT_SENSORS*sizeof(float));
			}

			/* enter calib CT to zero */
			LOGIN_PRINT("CT SENSORS CALIBRATING...\n");
			for(int i=0; i< NUM_OF_CT_SENSORS; i++){
				if ((ctZeroCalibration[i] == 0)&&(accu_data_frame.I_Bat[i] > -30.0)&&(accu_data_frame.I_Bat[i] < 30.0)) {
					(ctZeroCalibration[i] = accu_data_frame.I_Bat[i]);
					changed = 1;
				}
			}
			if (changed)
				eeprom_write(EEPROM_CALIB_ZERO_ADDR, (uint8_t*)ctZeroCalibration, NUM_OF_CT_SENSORS*sizeof(float));
			LOGIN_PRINT("CALIBRATION DONE!\n");
		}
		else{
			LOGIN_PRINT("CHECK CT CONNECTION...\n");
			for (uint8_t i = 0; i < NUM_OF_CT_SENSORS; i++){
				LOGIN_PRINT("I[%d]: %dmA\t",i,\
					    (int32_t)(accu_data_frame.I_Bat[i]*1000));
				LOGIN_PRINT("%s\n", ( accu_data_frame.I_Bat[i] < -30.0 ? "FAIL" : "OK"));
			}
		}
	}
		break;
	/* check LTC6803 connection */
	case 'v': {
		LOGIN_PRINT("CHECK LTC6803 CONNECTION...\n");
		uint8_t group_accu = 0;

		switch (*(argv + 6)) {
		case '0':
			group_accu = 0;
			for(uint8_t current_accu = 0; current_accu < NUM_OF_ACCU_PER_LTC6803; current_accu++){
				LOGIN_PRINT("ACCU [%d][%d]: %d.%d%d\n",group_accu,current_accu,\
						(uint32_t)accu_data_frame.V_Bat[group_accu][current_accu],\
						(uint32_t)(accu_data_frame.V_Bat[group_accu][current_accu]*10)%10,\
						(uint32_t)(accu_data_frame.V_Bat[group_accu][current_accu]*100)%10);
			}
			break;
		case '1':
			group_accu = 1;
			for(uint8_t current_accu = 0; current_accu < NUM_OF_ACCU_PER_LTC6803; current_accu++){
				LOGIN_PRINT("ACCU [%d][%d]: %d.%d%d\n",group_accu,current_accu,\
						(uint32_t)accu_data_frame.V_Bat[group_accu][current_accu],\
						(uint32_t)(accu_data_frame.V_Bat[group_accu][current_accu]*10)%10,\
						(uint32_t)(accu_data_frame.V_Bat[group_accu][current_accu]*100)%10);
			}
			break;
		case '2':
			group_accu = 2;
			for(uint8_t current_accu = 0; current_accu < NUM_OF_ACCU_PER_LTC6803; current_accu++){
				LOGIN_PRINT("ACCU [%d][%d]: %d.%d%d\n",group_accu,current_accu,\
						(uint32_t)accu_data_frame.V_Bat[group_accu][current_accu],\
						(uint32_t)(accu_data_frame.V_Bat[group_accu][current_accu]*10)%10,\
						(uint32_t)(accu_data_frame.V_Bat[group_accu][current_accu]*100)%10);
			}

			break;
		case '3':
			group_accu = 3;
			for(uint8_t current_accu = 0; current_accu < NUM_OF_ACCU_PER_LTC6803; current_accu++){
				LOGIN_PRINT("ACCU [%d][%d]: %d.%d%d\n",group_accu,current_accu,\
						(uint32_t)accu_data_frame.V_Bat[group_accu][current_accu],\
						(uint32_t)(accu_data_frame.V_Bat[group_accu][current_accu]*10)%10,\
						(uint32_t)(accu_data_frame.V_Bat[group_accu][current_accu]*100)%10);
			}

			break;
		default:
			for (uint8_t current_ic = 0; current_ic < LTC6803_TOTAL_IC; current_ic++){
				LOGIN_PRINT("IC %d: %s\t",current_ic,\
							( (ltc6803_stt & (0x01 << current_ic) ) ? "FAIL":"OK"));
			}
			break;
		}


		LOGIN_PRINT("\n");
	}
		break;
	/* check RS485 connection */
	case 'c':{
		LOGIN_PRINT("CHECK RS485 CONNECTION...\n");
		LOGIN_PRINT("POLLED STT: %d",rs485_stt[0]);
		if (rs485_stt[0] == 0){
			LOGIN_PRINT("\tID ERROR!\n");
		}
		else if ((rs485_stt[0] < 8)&&(rs485_stt[0] > 0)){
			LOGIN_PRINT("\tCOMMUNICATION ERROR!\n");
		}
		else if (rs485_stt[0] == -1){
			LOGIN_PRINT("\tNO_REPLY, DATA ERROR!\n");
		}
		else if (rs485_stt[0] == -2){
			delay(1000);
			LOGIN_PRINT("\tCONNECTION ERROR!\n");
		}
		else {
			LOGIN_PRINT("\tOK!\n");
		}
		LOGIN_PRINT("HISTORY!\n");
		for(uint8_t i=0; i<10; i++){
			LOGIN_PRINT("%d\n",rs485_stt[i]);
		}

	}
		break;
	/* reset all stt variables */
	case 'r':{
		LOGIN_PRINT("RESET ALL STT VARIABLE..\n");
		for(uint8_t i=0; i<10; i++){
			rs485_stt[i] = -2;
		}
//		rs485_stt = -2;
		ltc6803_stt = 0;
		mem_set(ctZeroCalibration,0,NUM_OF_CT_SENSORS*sizeof(float));
		for (uint8_t i=0; i<NUM_OF_CT_SENSORS; i++){
			ctAmplifyCalibration[i] = 2.2;
		}
		LOGIN_PRINT("DONE!\n");
	}
		break;
	case 'd':{
		if(*(argv + 7) == 'o'){
			if(*(argv + 8) == 'n'){
				LOGIN_PRINT("Debug Accu On!\n");
				debug_accu_on = true;
			}
			else if(*(argv + 8) == 'f'){
				LOGIN_PRINT("Debug Accu Off!\n");
				debug_accu_on = false;
			}
		}
	}
		break;
	default:
		break;
	}

	return 0;
}
