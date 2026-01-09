#pragma once
#include <stdint.h>

void rtc_init();
void rtc_stop();
uint8_t rtc_read(uint8_t reg);

void rtc_get_datetime(uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *day, uint8_t *month, uint16_t *year);