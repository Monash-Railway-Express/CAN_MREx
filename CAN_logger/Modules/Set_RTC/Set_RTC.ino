    /*!
     * @file getTimefromNTP.ino
     * @brief Get time from ntpServer and show current time 
     * @n Experiment phenomenon: read data every second and print it on serial port. 
     * @n                        This demo only works on mpython
     *
     * @copyright	Copyright (c) 2010 DFRobot Co.Ltd (https://www.dfrobot.com)
     * @licence     The MIT License (MIT)
     * @author [LuoYufeng](yufeng.luo@dfrobot.com)
     * @version  V0.1
     * @date  2019-08-19
     * @url https://github.com/DFRobot/DFRobot_DS3231M
     */
    #include "DFRobot_DS3231M.h"
    #include <WiFi.h>
    #include "time.h"
    
    DFRobot_DS3231M rtc;
    
    const char* ssid       = "dfrobotYanfa";//wlan information
    const char* password   = "hidfrobot";
    
    const char* ntpServer = "ntp.ntsc.ac.cn";//local ntp server
    const long  gmtOffset_sec = 10*3600;  //GMT+10:00
    const int   daylightOffset_sec = 3600; //Daylight savings
    
    
    void getTimeFromNTP()
    {
        struct tm timeinfo;
        if(!getLocalTime(&timeinfo)){
            Serial.println("Failed to obtain time");
            return;
        }
        
        rtc.setYear(timeinfo.tm_year - 100);
        rtc.setMonth(timeinfo.tm_mon + 1);
        rtc.setDate(timeinfo.tm_mday);
        rtc.setHour(timeinfo.tm_hour, e24hours);
        rtc.setMinute(timeinfo.tm_min);
        rtc.setSecond(timeinfo.tm_sec);
        rtc.adjust();
        
    }
    
    void setup()
    {
        Serial.begin(9600);
        while(rtc.begin() != true){
            Serial.println("Failed to init chip, please check if the chip connection is fine. ");
            delay(1000);
        }
        //connect to WiFi
        Serial.printf("Connecting to %s ", ssid);
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println(" CONNECTED");
        
        //init and get the time
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        getTimeFromNTP();
        //disconnect WiFi as it's no longer needed
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }
    
    void loop()
    {
        rtc.getNowTime();
        Serial.print(rtc.year(), DEC);//year
        Serial.print('/');
        Serial.print(rtc.month(), DEC);//month
        Serial.print('/');
        Serial.print(rtc.day(), DEC);//date
        Serial.print(" (");
        Serial.print(rtc.getDayOfTheWeek());//day of week
        Serial.print(") ");
        Serial.print(rtc.hour(), DEC);//hour
        Serial.print(':');
        Serial.print(rtc.minute(), DEC);//minute
        Serial.print(':');
        Serial.print(rtc.second(), DEC);//second
        Serial.println();
        delay(1000);
        
    }
