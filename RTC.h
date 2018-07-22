//
// Created by pasquale on 20/07/2018.
//

#ifndef OROLOGIO_RTC_H
#define OROLOGIO_RTC_H


#include <bcm2835.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "time.h"

#define   RTC_I2C_ADDR 0x68

class RTC {
private:
    uint8_t bcd2bin (uint8_t val);
    uint8_t bin2bcd (uint8_t val);
    uint8_t bcdToDec(uint8_t val);
    uint8_t decToBcd(uint8_t val);
    void writeControlByte(uint8_t control, bool which);
    uint8_t readControlByte(bool which);

    void setYear(uint8_t year);
    void setMonth(uint8_t month);
    void setDay(uint8_t day);
    void setDayOfWeew(uint8_t dayOfWeek);
    void setHour(uint8_t hour);
    void setMinute(uint8_t minute);
    void setSecond(uint8_t second);

public:
    void now(struct tm *time);
    void parseTime(tm *theTime, char *timeStr);
    void setTime(tm *theTime);
};


#endif //OROLOGIO_RTC_H
