/*
 * DS3234.c
 *
 *  Created on: 16.11.2021
 *      Author: Maria B¹k
 */
#include "DS3234.h"
#include "stdio.h"

uint8_t received_buffor[128];
uint8_t send_buffor[64];
uint8_t len;
uint8_t result;

int t_y, t_mon, t_date, t_day, t_h, t_m, t_s;
// DS3234_write_register(CONTROL, 0b01000000); JAKO INIT!!!

uint8_t dec2bcd(int x){
	return ( x / 10) * 16 + x % 10;
}

int bcd2dec(uint8_t x){
	return ( x >> 4 ) * 10 + ( x & 0x0F );
}

void DS3234_write_register(uint8_t reg, uint8_t data){
	reg = reg + 0x80; //write = read + 0x80

	CS_RTC_LOW;
	HAL_SPI_Transmit(&hspi2,(uint8_t*)&reg, 1, HAL_MAX_DELAY); //, HAL_MAX_DELAY
	HAL_SPI_Transmit(&hspi2,(uint8_t*)&data, 1, HAL_MAX_DELAY);
	CS_RTC_HIGH;
}

uint8_t DS3234_read_register(uint8_t reg){
	uint8_t data;

	CS_RTC_LOW;
	HAL_SPI_Transmit(&hspi2,(uint8_t*)&reg, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi2,(uint8_t*)&data, 1, HAL_MAX_DELAY);
	CS_RTC_HIGH;

	return data;
}

void DS3234_read_time(){
	czas.year   = DS3234_read_register(YEAR);
	czas.month  = DS3234_read_register(MONTH);
	czas.date   = DS3234_read_register(DATE);
	czas.day    = DS3234_read_register(DAY);

	czas.hour   = DS3234_read_register(HOURS);
	czas.minute = DS3234_read_register(MINUTES);
	czas.second = DS3234_read_register(SECONDS);
/*
	//BCD na dziesiêtny
	czas.year   = bcd2dec(czas.year);
	czas.month  = bcd2dec(czas.month);
	czas.date   = bcd2dec(czas.date);
	czas.day    = bcd2dec(czas.day);

	czas.hour   = bcd2dec(czas.hour);
	czas.minute = bcd2dec(czas.minute);
	czas.second = bcd2dec(czas.second);
*/
	uint32_t temp =  __HAL_TIM_GET_COUNTER(&htim14)/10;
	czas.milisecond = (uint16_t) temp;
	//czas.ms_dot = czas.milisecond % 10;

	czas.res = 0xFF;
}

void DS3234_write_date(int y, int mon, int date, int day){
	uint8_t uy, umon, udate, uday, uh, um, us;

	uy = dec2bcd(y);
	umon = dec2bcd(mon);
	udate = dec2bcd(date);
	uday = dec2bcd(day);

	DS3234_write_register(YEAR, uy);
	DS3234_write_register(MONTH, umon);
	DS3234_write_register(DATE, udate);
	DS3234_write_register(DAY, uday);
}

void DS3234_write_time(int h, int m, int s){
	uint8_t uh, um, us;

	if(h >= 0 && h <= 24){
		if(h == 24){
			h = 0;
		}
		uh = dec2bcd(h);

		DS3234_write_register(HOURS, uh);
	}

	if(m >= 0 && m <= 59){
		um = dec2bcd(m);

		DS3234_write_register(MINUTES, um);
	}

	if(s >= 0 && s <= 59){
		us = dec2bcd(s);

		DS3234_write_register(SECONDS, us);
	}

	czas.milisecond = 0;
	//czas.ms_dot = 0;
}

void DS3234_COM_commands(){
	/*
	if(strcmp(received_buffor, "read") == 0){ //read
		DS3234_read_time();
		//BCD na dziesiêtny
		czas.year   = bcd2dec(czas.year);
		czas.month  = bcd2dec(czas.month);
		czas.date   = bcd2dec(czas.date);
		czas.day    = bcd2dec(czas.day);

		czas.hour   = bcd2dec(czas.hour);
		czas.minute = bcd2dec(czas.minute);
		czas.second = bcd2dec(czas.second);


		len = sprintf(send_buffor, "R%d %d-%d-%d %d:%d:%d,%d\n\r", czas.day, czas.date,
				czas.month, czas.year, czas.hour, czas.minute, czas.second, czas.milisecond); //czas.ms_dot
		result = CDC_Transmit_FS(send_buffor, len);

	}else
		*/
	if(strcmp(received_buffor, "w_time") == 0){ //write
		DS3234_write_time(t_h, t_m, t_s);
	}else if(strcmp(received_buffor, "w_date") == 0){ //write
		DS3234_write_date(t_y, t_mon, t_date, t_day);
	}else if(strcmp(received_buffor, "year") == 0){
		prev_buf[0] = 'y';
	}else if(strcmp(received_buffor, "month") == 0){
		prev_buf[0] = 'n';
	}else if(strcmp(received_buffor, "date") == 0){
		prev_buf[0] = 'd';
	}else if(strcmp(received_buffor, "day") == 0){
		prev_buf[0] = 't';
	}else if(strcmp(received_buffor, "hour") == 0){
		prev_buf[0] = 'h';
	}else if(strcmp(received_buffor, "minute") == 0){
		prev_buf[0] = 'm';
	}else if(strcmp(received_buffor, "second") == 0){
		prev_buf[0] = 's';
	}else{
		if(prev_buf[0] == 'h'){							//godzina
			t_h = atoi(received_buffor);
		}else if(prev_buf[0] == 'm'){					//minuta
			t_m = atoi(received_buffor);
		}else if(prev_buf[0] == 's'){					//sekunda
			t_s = atoi(received_buffor);
		}else if(prev_buf[0] == 'y'){					//rok
			//strcpy(t_buf, received_buffor);
			t_y = atoi(received_buffor);
		}else if(prev_buf[0] == 'n'){					//miesiac
			t_mon = atoi(received_buffor);
		}else if(prev_buf[0] == 'd'){					//dzien
			t_date = atoi(received_buffor);
		}else if(prev_buf[0] == 't'){					//dzien tygodnia
			t_day = atoi(received_buffor);
		}
	}
}

