#include "rtc.h"
#include "../IO/io.h"

#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

#define CMOS_SECONDS 0x00
#define CMOS_MINUTES 0x02
#define CMOS_HOURS 0x04
#define CMOS_DAY 0x07
#define CMOS_MONTH 0x08
#define CMOS_YEAR 0x09
#define CMOS_STATUS_B 0x0B

static uint8_t rtc_enabled = 0;

static inline uint8_t bcd_to_bin(uint8_t bcd)
{
    return ((bcd / 16) * 10) + (bcd & 0x0F);
}

uint8_t rtc_read(uint8_t reg)
{
    if (!rtc_enabled)
        return 0;

    outb(CMOS_ADDR, reg);
    return inb(CMOS_DATA);
}

void rtc_init()
{
    rtc_enabled = 1;
}

void rtc_stop()
{
    rtc_enabled = 0;

    outb(CMOS_ADDR, 0x80);

    outb(CMOS_ADDR, 0x0C);
    inb(CMOS_DATA);
}

void rtc_get_datetime(uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *day, uint8_t *month, uint16_t *year)
{
    if (!rtc_enabled)
    {
        *hour = *minute = *second = *day = *month = 0;
        *year = 0;
        return;
    }

    uint8_t sec = rtc_read(CMOS_SECONDS);
    uint8_t min = rtc_read(CMOS_MINUTES);
    uint8_t hr = rtc_read(CMOS_HOURS);
    uint8_t dy = rtc_read(CMOS_DAY);
    uint8_t mon = rtc_read(CMOS_MONTH);
    uint8_t yr = rtc_read(CMOS_YEAR);

    uint8_t regB = rtc_read(CMOS_STATUS_B);

    if (!(regB & 0x04))
    {
        sec = bcd_to_bin(sec);
        min = bcd_to_bin(min);
        hr = bcd_to_bin(hr);
        dy = bcd_to_bin(dy);
        mon = bcd_to_bin(mon);
        yr = bcd_to_bin(yr);
    }

    if (!(regB & 0x02) && (hr & 0x80))
    {
        hr = ((hr & 0x7F) + 12) % 24;
    }

    *second = sec;
    *minute = min;
    *hour = hr;
    *day = dy;
    *month = mon;
    *year = 2000 + yr;
}