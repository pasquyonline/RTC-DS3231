//
// Created by pasquale on 20/07/2018.
//

#include "RTC.h"

void RTC::now(struct tm *time) {
    if(time==NULL) return;

    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(RTC_I2C_ADDR);

    char buf[7];
    buf[0] = 0;
    bcm2835_i2c_write(buf, 1);
    usleep(10000);

    bcm2835_i2c_read(buf, 7);

    time->tm_sec = bcd2bin(buf[0] & 0x7F);
    time->tm_min = bcd2bin(buf[1]);
    time->tm_hour = bcd2bin(buf[2]);
    time->tm_wday = bcd2bin(buf[3]);
    time->tm_mday = bcd2bin(buf[4]);
    time->tm_mon = bcd2bin(buf[5]);
    time->tm_year = bcd2bin(buf[6]) + 2000;

    /*for(uint8_t i=0; i<7; i++) {
        printf("%d ", bcd2bin(buf[i]));
    }
    printf("\n");*/

    bcm2835_i2c_end();
}

uint8_t RTC::bcd2bin (uint8_t val) {
    return val - 6 * (val >> 4);
}
uint8_t RTC::bin2bcd (uint8_t val) {
    return val + 6 * (val / 10);
}

void RTC::parseTime(tm *theTime, char *timeStr) {
    int tokenN=0;
    char *pch;
    pch = strtok(timeStr,"TZ-:");
    while (pch != NULL) {
        printf ("%s\n",pch);
        switch(tokenN) {
            case 0:
                sscanf(pch, "%d", &theTime->tm_year);
                theTime->tm_year -= 2000;
                break;
            case 1:
                sscanf(pch, "%d", &theTime->tm_mon);
                break;
            case 2:
                sscanf(pch, "%d", &theTime->tm_mday);
                break;
            case 3:
                sscanf(pch, "%d", &theTime->tm_hour);
                break;
            case 4:
                sscanf(pch, "%d", &theTime->tm_min);
                break;
            case 5:
                sscanf(pch, "%d", &theTime->tm_sec);
                break;
            case 6:
                sscanf(pch, "%d", &theTime->tm_wday); //1->domenica, 2->lunedi', ..., 7->sabato
                break;

        }
        tokenN++;
        pch = strtok (NULL, "TZ-:");
    }
}

void RTC::setTime(tm *theTime) {
    setYear(theTime->tm_year);
    setMonth(theTime->tm_mon);
    setDay(theTime->tm_mday);
    setDayOfWeew(theTime->tm_wday);
    setHour(theTime->tm_hour);
    setMinute(theTime->tm_min);
    setSecond(theTime->tm_sec);
}

/**
 * Sets the seconds.
 * This function also resets the Oscillator Stop Flag, which is set whenever power is interrupted.
 * @param second
 */
void RTC::setSecond(uint8_t second) {
    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(RTC_I2C_ADDR);
    char buf2[2] = {0x00, decToBcd(second)};
    bcm2835_i2c_write(buf2, 2);
    bcm2835_i2c_end();

    // Clear OSF flag
    uint8_t temp_buffer = readControlByte(1);
    writeControlByte((temp_buffer & 0b01111111), 1);
}

/**
 *  Sets the minutes
 * @param minute
 */
void RTC::setMinute(uint8_t minute) {
    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(RTC_I2C_ADDR);
    char buf2[2] = {0x01, decToBcd(minute)};
    bcm2835_i2c_write(buf2, 2);
    bcm2835_i2c_end();
}

/**
 * Sets the hour, without changing 12/24h mode.
 * The hour must be in 24h format.
 * @param hour
 */
void RTC::setHour(uint8_t hour) {
    bool h12;

    // Start by figuring out what the 12/24 mode is
    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(RTC_I2C_ADDR);
    char buf[1] = {0x02};
    bcm2835_i2c_write(buf, 1);
    usleep(10000);
    bcm2835_i2c_read(buf, 1);
    h12 = (buf[0] & 0b01000000);

    // if h12 is true, it's 12h mode; false is 24h.
    if (h12) {
        // 12 hour
        if (hour > 12) {
            hour = decToBcd(hour-12) | 0b01100000;
        } else {
            hour = decToBcd(hour) & 0b11011111;
        }
    } else {
        // 24 hour
        hour = decToBcd(hour) & 0b10111111;
    }

    /*bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(RTC_I2C_ADDR);*/

    char buf2[2] = {0x02, hour};
    bcm2835_i2c_write(buf2, 2);

    bcm2835_i2c_end();
}

/**
 * Sets the Day of Week
 * @param DoW
 */
void RTC::setDayOfWeew(uint8_t dayOfWeek) {
    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(RTC_I2C_ADDR);
    char buf[2] = {0x03, decToBcd(dayOfWeek)};
    bcm2835_i2c_write(buf, 2);
    bcm2835_i2c_end();
}

/**
 * Sets the day
 * @param day
 */
void RTC::setDay(uint8_t day) {
    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(RTC_I2C_ADDR);
    char buf[2] = {0x04, decToBcd(day)};
    bcm2835_i2c_write(buf, 2);
    bcm2835_i2c_end();
}

/**
 * Sets the month
 * @param month
 */
void RTC::setMonth(uint8_t month) {
    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(RTC_I2C_ADDR);
    char buf[2] = {0x05, decToBcd(month)};
    bcm2835_i2c_write(buf, 2);
    bcm2835_i2c_end();
}
/**
 * Sets the year
 * @param year
 */
void RTC::setYear(uint8_t year) {
    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(RTC_I2C_ADDR);
    char buf[2] = {0x06, decToBcd(year)};
    bcm2835_i2c_write(buf, 2);
    bcm2835_i2c_end();
}

/**
 *
 * @param which
 * @return
 */
uint8_t RTC::readControlByte(bool which) {
    // Read selected control byte
    // first byte (0) is 0x0e, second (1) is 0x0f
    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(RTC_I2C_ADDR);
    char buf[1];
    buf[0] = which ? 0x0f : 0x0e;
    bcm2835_i2c_write(buf, 1);
    usleep(10000);
    bcm2835_i2c_read(buf, 1);
    bcm2835_i2c_end();
    return buf[0];
}

/**
 *
 * @param control
 * @param which
 */
void RTC::writeControlByte(uint8_t control, bool which) {
    // Write the selected control byte.
    // which=false -> 0x0e, true->0x0f.
    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(RTC_I2C_ADDR);
    char buf[1];
    buf[0] = which ? 0x0f : 0x0e;
    bcm2835_i2c_write(buf, 1);
    usleep(10000);
    buf[0] = control;
    bcm2835_i2c_write(buf, 1);
    bcm2835_i2c_end();
}

/**
 * Convert normal decimal numbers to binary coded decimal
 * @param val
 * @return
 */
uint8_t RTC::decToBcd(uint8_t val) {
    return (uint8_t)( (val/10*16) + (val%10) );
}

/**
 * Convert binary coded decimal to normal decimal numbers
 * @param val
 * @return
 */
uint8_t RTC::bcdToDec(uint8_t val) {
    return uint8_t( (val/16*10) + (val%16) );
}
