# Real Time Clock - DS3231
C++ class for the RealTime Clock module DS3231 for RaspberryPi.

In this early version I miss the Alarm feature and setting time in AM/PM format, only 24h. 

To set time use:

## USE
```C++
struct tm tmRTC;
RTC *rtc = new RTC();
rtc->parseTime(&tmRTC, "2018-07-22T15:25:00Z1");
rtc->setTime(&tmRTC);
```

where *2018-07-22T15:25:00Z1* mean *"22 July 2018, 15:25:00, Sunday"*

## Compile
```
gcc -v -o <YOUR-EXE> YOUR-MAIN.cpp RTC.cpp -L/usr/local/lib -lbcm2835 -lstdc++ -lrt
```