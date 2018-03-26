/**
 ******************************************************************************
 * @author: ThanNT
 * @date:   05/09/2016
 * @Update:
 * @AnhHH: Add io function for sth11 sensor.
 ******************************************************************************
**/
#include <stdint.h>
#include <stdbool.h>

#include "io_cfg.h"
#include "platform.h"

#include "sys_dbg.h"
#include "sys_ctrl.h"

#include "utils.h"
#include "app_dbg.h"
#include "task_accu.h"

#include "eeprom.h"

#include "system.h"

#pragma GCC optimize ("O3")

/******************************************************************************
* button function
*******************************************************************************/
void io_button_mode_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(BUTTON_MODE_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = BUTTON_MODE_IO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(BUTTON_MODE_IO_PORT, &GPIO_InitStructure);
}

void io_button_up_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(BUTTON_UP_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = BUTTON_UP_IO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(BUTTON_UP_IO_PORT, &GPIO_InitStructure);
}

void io_button_down_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(BUTTON_DOWN_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = BUTTON_DOWN_IO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(BUTTON_DOWN_IO_PORT, &GPIO_InitStructure);
}

uint8_t io_button_mode_read() {
	return  GPIO_ReadInputDataBit(BUTTON_MODE_IO_PORT,BUTTON_MODE_IO_PIN);
}

uint8_t io_button_up_read() {
	return  GPIO_ReadInputDataBit(BUTTON_UP_IO_PORT,BUTTON_UP_IO_PIN);
}

uint8_t io_button_down_read() {
	return  GPIO_ReadInputDataBit(BUTTON_DOWN_IO_PORT,BUTTON_DOWN_IO_PIN);
}

/******************************************************************************
* led status function
*******************************************************************************/
void led_life_init() {
	GPIO_InitTypeDef        GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(LED_LIFE_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED_LIFE_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED_LIFE_IO_PORT, &GPIO_InitStructure);
}

void led_life_on() {
	GPIO_ResetBits(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);
}

void led_life_off() {
	GPIO_SetBits(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);
}

/******************************************************************************
* SPI control function
*******************************************************************************/
void spi_cs_init() {
	GPIO_InitTypeDef        GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(SPI_CS_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = SPI_CS_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SPI_CS_IO_PORT, &GPIO_InitStructure);
}

void spi_cs_high() {
	GPIO_SetBits(SPI_CS_IO_PORT, SPI_CS_IO_PIN);
}

void spi_cs_low() {
	GPIO_ResetBits(SPI_CS_IO_PORT, SPI_CS_IO_PIN);
}

/******************************************************************************
* nfr24l01 IO function
*******************************************************************************/
void nrf24l01_io_ctrl_init() {
	/* CE / CSN / IRQ */
	GPIO_InitTypeDef        GPIO_InitStructure;
	EXTI_InitTypeDef        EXTI_InitStruct;
	NVIC_InitTypeDef        NVIC_InitStruct;

	/* GPIOA Periph clock enable */
	RCC_AHBPeriphClockCmd(NRF_CE_IO_CLOCK, ENABLE);
	RCC_AHBPeriphClockCmd(NRF_CSN_IO_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/*CE -> PA8*/
	GPIO_InitStructure.GPIO_Pin = NRF_CE_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(NRF_CE_IO_PORT, &GPIO_InitStructure);

	/*CNS -> PB9*/
	GPIO_InitStructure.GPIO_Pin = NRF_CSN_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(NRF_CSN_IO_PORT, &GPIO_InitStructure);

	/* IRQ -> PB1 */
	GPIO_InitStructure.GPIO_Pin = NRF_IRQ_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(NRF_IRQ_IO_PORT, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);

	EXTI_InitStruct.EXTI_Line = EXTI_Line1;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void nrf24l01_spi_ctrl_init() {
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;

	/*!< SPI GPIO Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/*!< SPI Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/*!< Configure SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*!< Configure SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*!< Configure SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect PXx to SPI_SCK */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);

	/* Connect PXx to SPI_MISO */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);

	/* Connect PXx to SPI_MOSI */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	/*!< SPI Config */
	SPI_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE); /*!< SPI enable */
}

void nrf24l01_ce_low() {
	GPIO_ResetBits(NRF_CE_IO_PORT, NRF_CE_IO_PIN);
}

void nrf24l01_ce_high() {
	GPIO_SetBits(NRF_CE_IO_PORT, NRF_CE_IO_PIN);
}

void nrf24l01_csn_low() {
	GPIO_ResetBits(NRF_CSN_IO_PORT, NRF_CSN_IO_PIN);
}

void nrf24l01_csn_high() {
	GPIO_SetBits(NRF_CSN_IO_PORT, NRF_CSN_IO_PIN);
}

uint8_t nrf24l01_spi_rw(uint8_t data) {
	unsigned long rxtxData = data;
	uint32_t counter;

	/* waiting send idle then send data */
	counter = system_info.cpu_clock / 1000;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {
		if (counter-- == 0) {
			FATAL("spi", 0x01);
		}
	}

	SPI_I2S_SendData(SPI1, (uint8_t)rxtxData);

	/* waiting conplete rev data */
	counter = system_info.cpu_clock / 1000;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {
		if (counter-- == 0) {
			FATAL("spi", 0x02);
		}
	}

	rxtxData = (uint8_t)SPI_I2S_ReceiveData(SPI1);

	return (uint8_t)rxtxData;
}

/******************************************************************************
* flash IO config
*******************************************************************************/
void flash_io_ctrl_init() {
//	GPIO_InitTypeDef        GPIO_InitStructure;

//	RCC_AHBPeriphClockCmd(FLASH_CE_IO_CLOCK, ENABLE);

//	GPIO_InitStructure.GPIO_Pin = FLASH_CE_IO_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(FLASH_CE_IO_PORT, &GPIO_InitStructure);
}

void flash_cs_low() {
//	GPIO_ResetBits(FLASH_CE_IO_PORT, FLASH_CE_IO_PIN);
}

void flash_cs_high() {
//	GPIO_SetBits(FLASH_CE_IO_PORT, FLASH_CE_IO_PIN);
}

uint8_t flash_transfer(uint8_t data) {
	unsigned long rxtxData = data;
//	uint32_t counter;

//	/* waiting send idle then send data */
//	counter = system_info.cpu_clock / 1000;
//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {
//		if (counter-- == 0) {
//			FATAL("spi", 0x01);
//		}
//	}

//	SPI_I2S_SendData(SPI1, (uint8_t)rxtxData);

//	/* waiting conplete rev data */
//	counter = system_info.cpu_clock / 1000;
//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {
//		if (counter-- == 0) {
//			FATAL("spi", 0x02);
//		}
//	}

//	rxtxData = (uint8_t)SPI_I2S_ReceiveData(SPI1);

	return (uint8_t)rxtxData;
}

/******************************************************************************
* ADC reading function
*******************************************************************************/
void timer_1s_init() {
	TIM_TimeBaseInitTypeDef  timer_50us;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* timer 50us to polling receive IR signal */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	timer_50us.TIM_Period = 31248;//1592;
	timer_50us.TIM_Prescaler = 1024;//0;
	timer_50us.TIM_ClockDivision = 0;
	timer_50us.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(TIM6, &timer_50us);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM6, DISABLE);
}

void timer_1s_enable() {
	ENTRY_CRITICAL();
	TIM_Cmd(TIM6, ENABLE);
	EXIT_CRITICAL();
}

void timer_1s_disable() {
	ENTRY_CRITICAL();
	TIM_Cmd(TIM6, DISABLE);
	EXIT_CRITICAL();
}

void timer_50us_enable() {
	ENTRY_CRITICAL();
//	TIM_Cmd(TIM6, ENABLE);
	EXIT_CRITICAL();
}

void timer_50us_disable() {
	ENTRY_CRITICAL();
//	TIM_Cmd(TIM6, DISABLE);
	EXIT_CRITICAL();
}

/******************************************************************************
* ir IO function
*******************************************************************************/
void ir_rev_io_init() {
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* ir read -> PA15*/
	RCC_AHBPeriphClockCmd(IR_RX_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = IR_RX_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IR_RX_IO_PORT, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);

	EXTI_InitStruct.EXTI_Line = EXTI_Line15;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void ir_rev_io_irq_disable() {
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line15;
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
}

void ir_rev_io_irq_enable() {
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line15;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
}

void ir_carrier_freq_init() {
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_AHBPeriphClockCmd( IR_TX_IO_CLOCK, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 841;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 281;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC3Init(TIM2, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM2, ENABLE);

	TIM_Cmd(TIM2, DISABLE);
}

void ir_carrier_freq_on() {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* enable PWM output GPIO */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin   = IR_TX_IO_PIN;
	GPIO_Init(IR_TX_IO_PORT, &GPIO_InitStructure);

	GPIO_PinAFConfig(IR_TX_IO_PORT, GPIO_PinSource10, GPIO_AF_TIM2);

	/* enable timer */
	ENTRY_CRITICAL();
	TIM_Cmd(TIM2, ENABLE);
	EXIT_CRITICAL();
}

void ir_carrier_freq_off() {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* PWM output GPIO LOW level */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin   = IR_TX_IO_PIN;
	GPIO_Init(IR_TX_IO_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(IR_TX_IO_PORT, IR_TX_IO_PIN);

	/* disable timer */
	ENTRY_CRITICAL();
	TIM_Cmd(TIM2, DISABLE);
	EXIT_CRITICAL();
}

/******************************************************************************
* direction IR IO function
*******************************************************************************/
void ir_dir_io_config(){
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(IR_DIR1_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = IR_DIR1_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(IR_DIR1_IO_PORT, &GPIO_InitStructure);

	RCC_AHBPeriphClockCmd(IR_DIR2_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = IR_DIR2_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(IR_DIR2_IO_PORT, &GPIO_InitStructure);
}

void ir_select_direction(uint8_t ir_number){
	switch (ir_number) {
	case 0:
		GPIO_SetBits(IR_DIR1_IO_PORT,IR_DIR1_IO_PIN);
		GPIO_SetBits(IR_DIR2_IO_PORT,IR_DIR2_IO_PIN);
		break;

	case 1:
		GPIO_ResetBits(IR_DIR1_IO_PORT,IR_DIR1_IO_PIN);
		GPIO_SetBits(IR_DIR2_IO_PORT,IR_DIR2_IO_PIN);
		break;

	case 2:
		GPIO_ResetBits(IR_DIR1_IO_PORT,IR_DIR1_IO_PIN);
		GPIO_ResetBits(IR_DIR2_IO_PORT,IR_DIR2_IO_PIN);
		break;

	case 3:
		GPIO_SetBits(IR_DIR1_IO_PORT,IR_DIR1_IO_PIN);
		GPIO_ResetBits(IR_DIR2_IO_PORT,IR_DIR2_IO_PIN);
		break;

	default:
		break;
	}
}

/******************************************************************************
* sht11 IO function
*******************************************************************************/
void sht1x_clk_input_mode(){
	GPIO_InitTypeDef    GPIO_InitStructure;
	/* GPIOA Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void sht1x_clk_output_mode(){
	GPIO_InitTypeDef    GPIO_InitStructure;

	/* GPIOA Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void sht1x_clk_digital_write_low() {
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}

void sht1x_clk_digital_write_high(){
	GPIO_SetBits(GPIOA, GPIO_Pin_11);
}

void sht1x_data_input_mode(){
	GPIO_InitTypeDef    GPIO_InitStructure;
	/* GPIOA Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void sht1x_data_output_mode(){
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void sht1x_data_digital_write_low(){
	GPIO_ResetBits(GPIOA, GPIO_Pin_12);
}

void sht1x_data_digital_write_high(){
	GPIO_SetBits(GPIOA, GPIO_Pin_12);
}

int sht1x_data_digital_read(){
	return (int)(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12));
}

/******************************************************************************
* ds1302 IO function
*******************************************************************************/
/* rst pin config*/
void ds1302_ce_input_mode() {
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(DS1302_CE_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = DS1302_CE_IO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(DS1302_CE_IO_PORT, &GPIO_InitStructure);
}

void ds1302_ce_output_mode() {
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(DS1302_CE_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = DS1302_CE_IO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(DS1302_CE_IO_PORT, &GPIO_InitStructure);
}

void ds1302_ce_digital_write_low() {
	GPIO_ResetBits(DS1302_CE_IO_PORT, DS1302_CE_IO_PIN);
}

void ds1302_ce_digital_write_high(){
	GPIO_SetBits(DS1302_CE_IO_PORT, DS1302_CE_IO_PIN);
}

/* scl pin config*/
void ds1302_clk_input_mode() {
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(DS1302_CLK_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = DS1302_CLK_IO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(DS1302_CLK_IO_PORT, &GPIO_InitStructure);
}

void ds1302_clk_output_mode() {
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(DS1302_CLK_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = DS1302_CLK_IO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(DS1302_CLK_IO_PORT, &GPIO_InitStructure);
}

void ds1302_clk_digital_write_low() {
	GPIO_ResetBits(DS1302_CLK_IO_PORT, DS1302_CLK_IO_PIN);
}

void ds1302_clk_digital_write_high(){
	GPIO_SetBits(DS1302_CLK_IO_PORT, DS1302_CLK_IO_PIN);
}

/* sda pin config*/
void ds1302_data_input_mode() {
	GPIO_InitTypeDef    GPIO_InitStructure;
	/* GPIOA Periph clock enable */
	RCC_AHBPeriphClockCmd(DS1302_DATA_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = DS1302_DATA_IO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(DS1302_DATA_IO_PORT, &GPIO_InitStructure);
}

void ds1302_data_output_mode() {
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(DS1302_DATA_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = DS1302_DATA_IO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(DS1302_DATA_IO_PORT, &GPIO_InitStructure);
}

void ds1302_data_digital_write_low() {
	GPIO_ResetBits(DS1302_DATA_IO_PORT, DS1302_DATA_IO_PIN);
}

void ds1302_data_digital_write_high() {
	GPIO_SetBits(DS1302_DATA_IO_PORT, DS1302_DATA_IO_PIN);
}

uint8_t ds1302_data_digital_read(){
	return GPIO_ReadInputDataBit(DS1302_DATA_IO_PORT, DS1302_DATA_IO_PIN);
}

/******************************************************************************
* hs1101 IO function
* config DAC, COMP, PWM for read hs1101 function
*******************************************************************************/
void io_cfg_dac_hs1101() {
	DAC_InitTypeDef DAC_InitStructure;

	RCC_LSICmd(ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	/* DAC channel1 Configuration */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	/* Enable DAC Channel1 */
	DAC_Cmd(DAC_Channel_1, DISABLE);

	/* Set DAC Channel1 DHR register
	 * VREF ~ 3.3 V
	 * DAC_OUT = (3.3 * 2588) / 4095 ~> 2.86 V = 3.3*(1-exp(-t/R*C)) with t=R*C
	 * Set DAC Value = 2588
	*/
	DAC_SetChannel1Data(DAC_Align_12b_R, 2588);
}

void io_cfg_comp_hs1101() {
	COMP_InitTypeDef COMP_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd( HS1101_IN_CLOCK, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin   = HS1101_IN_PIN;
	GPIO_Init(HS1101_IN_PORT, &GPIO_InitStructure);

	COMP_InitStructure.COMP_InvertingInput = COMP_InvertingInput_DAC1;
	COMP_InitStructure.COMP_OutputSelect   = COMP_OutputSelect_TIM4IC4;
	COMP_InitStructure.COMP_Speed		   = COMP_Speed_Fast;
	COMP_Init(&COMP_InitStructure);

	/* Close the I/O analog switch number n */
	SYSCFG_RIIOSwitchConfig(RI_IOSwitch_GR6_2, ENABLE);
	COMP_WindowCmd(DISABLE);
	COMP_Cmd(DISABLE);
}

void io_cfg_timer3_hs1101() {
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	uint16_t period = (uint16_t)((SystemCoreClock) / 1100);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHBPeriphClockCmd( HS1101_OUT_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin   = HS1101_OUT_PIN;
	GPIO_Init(HS1101_OUT_PORT, &GPIO_InitStructure);

	GPIO_PinAFConfig(HS1101_OUT_PORT, GPIO_PinSource0, GPIO_AF_TIM3);

	TIM_TimeBaseStructure.TIM_Period        = period;
	TIM_TimeBaseStructure.TIM_Prescaler     = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_Pulse       = period/2;
	TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_Low;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);

	TIM4->SR = 0x00;

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Enable);
	TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3,DISABLE);
}

void io_cfg_timer4_hs1101() {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	uint16_t period = (uint16_t)((SystemCoreClock) / 1100);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = period;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_ICInitStructure.TIM_Channel		= TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter    = 0;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);

	TIM4->SR = 0x00;

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Trigger);
	TIM_SelectInputTrigger(TIM4, TIM_TS_ITR3);

	TIM_ITConfig(TIM4,TIM_IT_CC4,ENABLE);
	TIM_Cmd(TIM4, DISABLE);
}

uint32_t io_timer4_get_capture() {
	return TIM_GetCapture4(TIM4);
}

void io_reset_timer4_capture() {
	TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Reset);
}

void io_start_timer4_capture() {
	TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Trigger);
}

void io_hs1101_read_enable() {
	DAC_Cmd(DAC_Channel_1, ENABLE);

	COMP_Cmd(ENABLE);

	ENTRY_CRITICAL();

	TIM_Cmd(TIM3, ENABLE);

	TIM_Cmd(TIM4, ENABLE);
	NVIC_EnableIRQ(TIM4_IRQn);

	EXIT_CRITICAL();
}

void io_hs1101_read_disable() {
	DAC_Cmd(DAC_Channel_1, DISABLE);

	COMP_Cmd(DISABLE);

	ENTRY_CRITICAL();

	TIM_Cmd(TIM3, DISABLE);

	TIM_Cmd(TIM4, DISABLE);
	NVIC_DisableIRQ(TIM4_IRQn);

	EXIT_CRITICAL();

	GPIO_ResetBits(HS1101_OUT_PORT,HS1101_OUT_PIN);
}

/******************************************************************************
* adc function
* + CT sensor
* + themistor sensor
* Note: MUST be enable internal clock for adc module.
*******************************************************************************/
void io_cfg_adc1_dma1cn1(){
	GPIO_InitTypeDef		GPIO_InitStructure;
	ADC_InitTypeDef			ADC_InitStructure;
	DMA_InitTypeDef			DMA_InitStructure;

	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 2;								//Read 2 ADC channel
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* Enable DMA1 Channel Transfer complete interrupt */
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

	//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitTypeDef NVIC_InitStructure;
	//Enable DMA1 channel IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* ADC GPIO cfg */
	RCC_AHBPeriphClockCmd(CT_ADC_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin  = CT_ADC_PIN | CT_ADC_VREF_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(CT_ADC_PORT, &GPIO_InitStructure);

	/* ADC configuration */
	RCC_HSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
	/* Enable ADC1 & GPIO clock */
	RCC_APB2PeriphClockCmd(CT_ADC_CLOCK , ENABLE);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //DISABLE
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 2;
	ADC_Init(ADC1, &ADC_InitStructure);
	//	ADC_Cmd(ADC1, ENABLE);

	/* Select the channels to be read from */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_384Cycles); /* read Vout of CT sensor */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 2, ADC_SampleTime_384Cycles); /* read Vref = 1.65V*/
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 3, ADC_SampleTime_384Cycles);

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);
	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConv(ADC1);

	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
}

void io_cfg_adc1(void) {
	ADC_InitTypeDef ADC_InitStructure;
	RCC_HSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

	/* Enable ADC1 clock */
	RCC_APB2PeriphClockCmd(CT_ADC_CLOCK , ENABLE);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode =DISABLE; //DISABLE
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Cmd(ADC1, ENABLE);
}

void adc_ct_io_cfg() {
	GPIO_InitTypeDef	GPIO_InitStructure;

	/* Enable ADC clock */
	RCC_AHBPeriphClockCmd(CT_ADC_IO_CLOCK, ENABLE);

	/* ADC GPIO cfg */
	GPIO_InitStructure.GPIO_Pin = CT_ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(CT_ADC_PORT, &GPIO_InitStructure);
}

void adc_thermistor_io_cfg() {
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(THER_ADC_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = THER_ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(THER_ADC_PORT, &GPIO_InitStructure);
}

uint16_t adc_ct_io_read(uint8_t chanel) {
	ADC_RegularChannelConfig(ADC1, chanel, 1, ADC_SampleTime_96Cycles);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);

	ADC_SoftwareStartConv(ADC1);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	return ADC_GetConversionValue(ADC1);
}

uint16_t adc_thermistor_io_read(uint8_t chanel) {
	ADC_RegularChannelConfig(ADC1, chanel, 1, ADC_SampleTime_4Cycles);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);

	ADC_SoftwareStartConv(ADC1);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	return ADC_GetConversionValue(ADC1);
}

/* used to select adc chanel*/
void io_74hc4052_init(){
	GPIO_InitTypeDef        GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(ADC_SELECT_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = ADC_SELECT_IO_PIN_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(ADC_SELECT_IO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ADC_SELECT_IO_PIN_2;
	GPIO_Init(ADC_SELECT_IO_PORT, &GPIO_InitStructure);
}

void adc_74hc4052_set_pin_mode (uint8_t pin_mode) {

//	APP_DBG("MODE CHANGE\n");
	switch (pin_mode) {
	case MODE_0:
//		APP_DBG("MODE: %d\n",pin_mode);
		GPIO_ResetBits(ADC_SELECT_IO_PORT, ADC_SELECT_IO_PIN_1);
		GPIO_ResetBits(ADC_SELECT_IO_PORT, ADC_SELECT_IO_PIN_2);
		break;
	case MODE_1:
//		APP_DBG("MODE: %d\n",pin_mode);
		GPIO_SetBits(ADC_SELECT_IO_PORT, ADC_SELECT_IO_PIN_1);
		GPIO_ResetBits(ADC_SELECT_IO_PORT, ADC_SELECT_IO_PIN_2);
		break;
	case MODE_2:
//		APP_DBG("MODE: %d\n",pin_mode);
		GPIO_ResetBits(ADC_SELECT_IO_PORT, ADC_SELECT_IO_PIN_1);
		GPIO_SetBits(ADC_SELECT_IO_PORT, ADC_SELECT_IO_PIN_2);
		break;
	case MODE_3:
//		APP_DBG("MODE: %d\n",pin_mode);
		GPIO_SetBits(ADC_SELECT_IO_PORT, ADC_SELECT_IO_PIN_1);
		GPIO_SetBits(ADC_SELECT_IO_PORT, ADC_SELECT_IO_PIN_2);
		break;
	default:
		break;
	}
}

/******************************************************************************
* ssd1306 oled IO function
*******************************************************************************/
void ssd1306_clk_input_mode() {
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(SSD1306_CLK_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SSD1306_CLK_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SSD1306_CLK_IO_PORT, &GPIO_InitStructure);
}

void ssd1306_clk_output_mode() {
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(SSD1306_CLK_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = SSD1306_CLK_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SSD1306_CLK_IO_PORT, &GPIO_InitStructure);
}

void ssd1306_clk_digital_write_low() {
	GPIO_ResetBits(SSD1306_CLK_IO_PORT,SSD1306_CLK_IO_PIN);
}

void ssd1306_clk_digital_write_high() {
	GPIO_SetBits(SSD1306_CLK_IO_PORT,SSD1306_CLK_IO_PIN);
}

void ssd1306_data_input_mode() {
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(SSD1306_DATA_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SSD1306_DATA_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SSD1306_DATA_IO_PORT, &GPIO_InitStructure);
}

void ssd1306_data_output_mode() {
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(SSD1306_DATA_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SSD1306_DATA_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SSD1306_DATA_IO_PORT, &GPIO_InitStructure);
}

void ssd1306_data_digital_write_low() {
	GPIO_ResetBits(SSD1306_DATA_IO_PORT, SSD1306_DATA_IO_PIN);
}

void ssd1306_data_digital_write_high() {
	GPIO_SetBits(SSD1306_DATA_IO_PORT, SSD1306_DATA_IO_PIN);
}

int ssd1306_data_digital_read() {
	return (int)(GPIO_ReadInputDataBit(SSD1306_DATA_IO_PORT, SSD1306_DATA_IO_PIN));
}

/******************************************************************************
* eeprom function
* when using function DATA_EEPROM_ProgramByte,
* must be DATA_EEPROM_unlock- DATA_EEPROM_ProgramByte- DATA_EEPROM_lock
*******************************************************************************/
uint8_t io_eeprom_read(uint32_t address, uint8_t* pbuf, uint32_t len) {
	uint32_t eeprom_address = 0;

	eeprom_address = address + EEPROM_BASE_ADDRESS;

	if ((uint8_t*)pbuf == (uint8_t*)0 || len == 0 ||(address + len)> EEPROM_MAX_SIZE) {
		return EEPROM_DRIVER_NG;
	}

	DATA_EEPROM_Unlock();
	memcpy(pbuf, (const uint8_t*)eeprom_address, len);
	DATA_EEPROM_Lock();

	return EEPROM_DRIVER_OK;
}

uint8_t io_eeprom_write(uint32_t address, uint8_t* pbuf, uint32_t len) {
	uint32_t eeprom_address = 0;
	uint32_t index = 0;
	uint32_t flash_status;

	eeprom_address = address + EEPROM_BASE_ADDRESS;

	if ((uint8_t*)pbuf == (uint8_t*)0 || len == 0 ||(address + len)> EEPROM_MAX_SIZE) {
		return EEPROM_DRIVER_NG;
	}

	DATA_EEPROM_Unlock();

	while (index < len) {
		flash_status = DATA_EEPROM_ProgramByte((eeprom_address + index), (uint32_t)(*(pbuf + index)));

		if(flash_status == FLASH_COMPLETE) {
			index++;
		}
		else {
			FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
							| FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);
		}
	}

	DATA_EEPROM_Lock();

	return EEPROM_DRIVER_OK;
}

uint8_t io_eeprom_erase(uint32_t address, uint32_t len) {
	uint32_t eeprom_address = 0;
	uint32_t index = 0;
	uint32_t flash_status;

	eeprom_address = address + EEPROM_BASE_ADDRESS;

	if (len == 0 ||(address + len)> EEPROM_MAX_SIZE) {
		return EEPROM_DRIVER_NG;
	}

	DATA_EEPROM_Unlock();

	while(index < len) {
		sys_ctrl_soft_watchdog_reset();
		sys_ctrl_independent_watchdog_reset();

		flash_status = DATA_EEPROM_ProgramByte(eeprom_address + index, 0x00);

		if(flash_status == FLASH_COMPLETE) {
			index++;
		}
		else {
			FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
							| FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);
		}
	}

	DATA_EEPROM_Lock();

	return EEPROM_DRIVER_OK;
}

void internal_flash_unlock() {
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
					FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
}

void internal_flash_lock() {
	FLASH_Lock();
}

void internal_flash_erase_pages_cal(uint32_t address, uint32_t len) {
	uint32_t page_number;
	uint32_t index;

	page_number = len / 256;

	if ((page_number * 256) < len) {
		page_number++;
	}

	for (index = 0; index < page_number; index++) {
		FLASH_ErasePage(address + (index * 256));
	}
}

uint8_t internal_flash_write_cal(uint32_t address, uint8_t* data, uint32_t len) {
	uint32_t temp;
	uint32_t index = 0;
	FLASH_Status flash_status = FLASH_BUSY;

	while (index < len) {
		temp = 0;

		memcpy(&temp, &data[index], (len - index) >= sizeof(uint32_t) ? sizeof(uint32_t) : (len - index));

		flash_status = FLASH_FastProgramWord(address + index, temp);

		if(flash_status == FLASH_COMPLETE) {
			index += sizeof(uint32_t);
		}
		else {
			FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
							FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
		}
	}

	return 1;
}

/******************************************************************************
* RS485 UART Init function
*******************************************************************************/

void RS485_uart_init(uint32_t baudrate) {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd( USART_RS485_TX_GPIO_CLK | USART_RS485_RX_GPIO_CLK, ENABLE);

	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(USART_RS485_CLK, ENABLE);

	/* Connect PXx to USART_RS485_Tx */
	GPIO_PinAFConfig(USART_RS485_TX_GPIO_PORT, USART_RS485_TX_SOURCE, USART_RS485_TX_AF);

	/* Connect PXx to USART_RS485_Rx */
	GPIO_PinAFConfig(USART_RS485_RX_GPIO_PORT, USART_RS485_RX_SOURCE, USART_RS485_RX_AF);

	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin = USART_RS485_TX_PIN;
	GPIO_Init(USART_RS485_TX_GPIO_PORT, &GPIO_InitStructure);


	// GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = USART_RS485_RX_PIN;
	GPIO_Init(USART_RS485_RX_GPIO_PORT, &GPIO_InitStructure);

	/* USART_RS485 configuration */
	USART_InitStructure.USART_BaudRate = baudrate ;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART_RS485, &USART_InitStructure);

	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USART_RS485 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART_RS485_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearITPendingBit(USART_RS485,USART_IT_RXNE);
	USART_ITConfig(USART_RS485, USART_IT_RXNE, ENABLE);

	/* Enable USART */
	USART_Cmd(USART_RS485, ENABLE);
}

/******************************************************************************
* RS485 dir pin mode select
*******************************************************************************/
void io_cfg_RS485_dir_pin() {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RS485_DIR_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin		= RS485_DIR_IO_PIN;
	GPIO_Init(RS485_DIR_IO_PORT, &GPIO_InitStructure);
}

void io_uart_RS485_mode_set(uint8_t mode) {
	if(mode == 0) { // output
		GPIO_SetBits(RS485_DIR_IO_PORT, RS485_DIR_IO_PIN); // set dir pin -> enable TX function
	} else if(mode == 1) { // input
		GPIO_ResetBits(RS485_DIR_IO_PORT, RS485_DIR_IO_PIN); // reset dir pin -> enable RX function
	}
}

void io_RS485_dir_pin_set() {
	GPIO_SetBits(RS485_DIR_IO_PORT, RS485_DIR_IO_PIN); // reset dir pin -> enable RX function
}

void io_RS485_dir_pin_reset() {
	GPIO_ResetBits(RS485_DIR_IO_PORT, RS485_DIR_IO_PIN); // reset dir pin -> enable RX function
}

void io_uart_rs485_transfer(uint8_t byte) {
	while (USART_GetFlagStatus(USART_RS485, USART_FLAG_TXE) == RESET);
	USART_SendData(USART_RS485, byte);
	while (USART_GetFlagStatus(USART_RS485, USART_FLAG_TC) == RESET);
}

uint8_t io_uart_RS485_get_byte() {
	volatile uint8_t c = 0;
	if(USART_GetITStatus(USART_RS485, USART_IT_RXNE) == SET) {
		c = (uint8_t)USART_ReceiveData(USART_RS485);
		USART_ClearITPendingBit(USART_RS485, USART_IT_RXNE);
//		APP_DBG("%02X:",c);
	}
	return c;
}
