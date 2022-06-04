/*
 * DS32334.h
 *
 *  Created on: 27.10.2021
 *      Author: Maria B¹k
 */

#ifndef INC_DS3234_H_
#define INC_DS3234_H_

#include "main.h"
#include "stdio.h"

#define CS_RTC_LOW HAL_GPIO_WritePin(CS_RTC_GPIO_Port,CS_RTC_Pin, GPIO_PIN_RESET)
#define CS_RTC_HIGH HAL_GPIO_WritePin(CS_RTC_GPIO_Port,CS_RTC_Pin, GPIO_PIN_SET)

//REGISTERS
#define SECONDS					0x00
#define	MINUTES					0x01
#define HOURS					0x02
#define DAY						0x03
#define DATE					0x04
#define MONTH					0x05
#define YEAR					0x06

#define A1_SECONDS				0x07
#define A1_MINUTES				0x08
#define A1_HOURS				0x09
#define A1_DAY					0x0A

#define A2_MINUTES				0x0B
#define A2_HOURS				0x0C
#define A2_DAY					0x0D

#define CONTROL					0x0E
#define STATUS					0x0F
#define CRYSTAL_AGING_OFFSET	0x10

#define TEMP_MSB				0x11
#define TEMP_LSB				0x12
#define DISABLE_TEMP_CONV		0x13

#define SRAM_ADDRESS			0x18
#define SRAM_DATA				0x19

//VARIABLES
/*
uint8_t year, month, date, day, hour, minute, second;
uint16_t milisecond;
uint8_t ms_dot;
*/

typedef struct{
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t day; //day_of_week
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t res; //musi byc zeby dane sie nie rozwalily
	uint16_t milisecond; //"podwojna" liczba musi byc w jednej
						//czworce danych
}DS3234;
DS3234 czas;


uint8_t prev_buf[64];
int int_buff[64];
char buffor[64];
int ile;
int temp;

uint8_t dec2bcd(int x);
int bcd2dec(uint8_t x);
uint8_t DS3234_read_register(uint8_t reg);
void DS3234_write_register(uint8_t reg, uint8_t data);
void DS3234_read_time();
void DS3234_write_time(int h, int m, int s);
void DS3234_write_date(int y, int mon, int date, int day);
void DS3234_COM_commands();

#endif /* INC_DS3234_H_ */
