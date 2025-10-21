/*******************************************************************************
 * @project Hannigan Lab's Next Gen. Air Quality Pods
 *
 * @file    xpod_V4.1.0.ino
 * @version Percy's 4.1.0
 * @brief   XPOD Code Rewrite
 * 
 * @author  Percy Smith
 * @date 	  August 8, 2025 
 * @log     Completely re-did PM code to timeout when no response from plantower 
 *          Did initial visualization analysis to confirm consistent behavior (looks correct)
 ******************************************************************************/
#include "xpod_node.h"

// Communication Protocol Libraries
#include <Wire.h>
#include <SPI.h>

// Conditional Global Declarations
#if SD_ENABLED
  #include <SdFat.h>
  SdFat sd;
  File file;
  char fileName[] = "XPODID_YYYY_MM_DD.CSV";
#endif //SD_ENABLED

#if RTC_ENABLED
  #include <RTClib.h>
  RTC_DS3231 rtc;
  DateTime rtc_date_time;
  char bufftime[] = "YYYY-MM-DDThh:mm:ss";
  int Y,M,D,h,m,s;
#endif //RTC_ENABLED

#if ADS_ENABLED
  #include "ads_module.h"
  ADS_Module ads_module;
  ADS_Data ads_data;
#endif //ADS_ENABLED

#if CO2_ENABLED
  #include "co2_module.h"
  ELT_S300 CO2_module;
  uint16_t CO2 = 0;
#endif //CO2_ENABLED

#if BME_ENABLED
  #include "bme_module.h"
  BME_Module bme_module;
  BME_Data bme_data;
#endif //BME_ENABLED

#if QUAD_ENABLED
  #include "quad_module.h"
  QUAD_Module quad_module;
  QUAD_Data quadstat_data;
#endif //QUAD_ENABLED

#if PMS_ENABLED
  #include "PMS.h"
  PMS pms(Serial1);
  PMS::DATA pms_data;
#endif //PMS_ENABLED

#if THE_DAWG
  #include <avr/wdt.h>
#endif //THE_DAWG

/***************************************************************************************/
void setup() {
  /*    COMMUNICATIONS SETUP    */
  Wire.begin();
  SPI.begin();
  #if SERIAL_ENABLED
    Serial.begin(9600);
  #endif //SERIAL_ENABLED

  /*    MODULE INITIALIZE    */
  #if ADS_ENABLED
    if (!ads_module.begin())    {
      #if SERIAL_ENABLED
        Serial.println("Error: Failed to initialize one of the ADS1115 module!");
      #endif //SERIAL_ENABLED
    } //if (!ads_module.begin())   
  #endif //ADS_ENABLED
  #if CO2_ENABLED
    CO2_module.begin();
  #endif //CO2_ENABLED
  #if BME_ENABLED
    if(!bme_module.begin())  {
      #if SERIAL_LOG_ENABLED
        Serial.println("Error: Failed to initialize BME sensor!");
      #endif
    } //if (!bme_module.begin())
  #endif //BME_ENABLED
  #if QUAD_ENABLED
    if (!quad_module.begin())
    {
      #if SERIAL_ENABLED
        Serial.println("Error: Failed to initialize Quad Stat!");
      #endif //SERIAL_ENABLED
    }
  #endif //QUAD_ENABLED
  #if PMS_ENABLED
    Serial1.begin(9600);
  #endif //PMS_ENABLED

  /*    PIN DECLARATIONS    */
  pinMode(SD_CS, OUTPUT);
  // LEDs (internal & external)
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  /*  RTC INTIALIZE & SET DATETIME  */
  #if RTC_ENABLED 
    if (!rtc.begin())
    {
      #if SERIAL_ENABLED
        Serial.println("Error: Failed to initialize RTC module");
      #endif //SERIAL_ENABLED
    } else {
      #if ADJUST_DATETIME
        rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));    // Only run uncommented once to initialize RTC
        #if USE_UTC
          uint32_t unixrtc = rtc.now().unixtime();
          uint32_t rtc_utc = unixrtc + UTC_CONV*3600; //NEEDS TO BE MODIFIED DEPENDING ON MST/MDT
          rtc.adjust(DateTime(rtc_utc));     
        #endif //USE_UTC
        rtc_date_time = rtc.now();
      #endif //ADJUST_DATETIME
    }
  #endif //RTC_ENABLED

  /*  SD CARD & FILE SETUP  */
  #if SD_ENABLED
    digitalWrite(SD_CS, LOW);       //Pull SD_CS pin LOW to initialize SPI comms
    sd.begin(SD_CS);                //Initialize SD Card with relevant chip select pin
    // Establish contact with SD card - if initialization fails, run until success
    while (!sd.begin(SD_CS)) {
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      #if SERIAL_ENABLED
        Serial.println("insert sd card to begin");
      #endif  //SERIAL_ENABLED
      sd.begin(SD_CS);      //attempt to initialize again
    } //while(!sd.begin(SD_CS))

    #if RTC_ENABLED
      if(rtc.begin()) {
        //File Naming (FORMATTING HAS TO BE CONSISTENT WITH GLOBAL DECLARATION!!)
        DateTime now = rtc.now();     //pulls setup() time so we have one file name per run in a day
        Y = now.year();    M = now.month();    D = now.day();
        sprintf(fileName, "%s_%04u_%02u_%02u.CSV", XPODID, Y, M, D);    //char array for fileName
        delay(100);
      } //if(rtc.begin())
    #endif //RTC_ENABLED

    file.open(fileName, O_CREAT | O_APPEND | O_WRITE);  //open with create, append, write permissions
    delay(100);    
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
    file.close();                                       //close file, we opened so loop() is faster 
    digitalWrite(SD_CS, HIGH);    //release chip select on SD - allow other comm with SPI
  #endif //SD_ENABLED
  
  /*  MR. WATCHDOG ARRIVES  */
  #if THE_DAWG
    wdt_enable(WDTO_8S);
  #endif //THE_DAWG
} //void setup()

/***************************************************************************************/
void loop() {
  /*  SETTING UP LOOP  */
  digitalWrite(RED_LED, HIGH);
  #if THE_DAWG
    wdt_reset();
  #endif //THE_DAWG

  /*  COLLECT DATA  */
  #if RTC_ENABLED
    DateTime now = rtc.now();
    Y = now.year();  M = now.month();  D = now.day();  h = now.hour();  m = now.minute();  s = now.second();
    sprintf(bufftime, "%04u-%02u-%02uT%02u:%02u:%02u", Y, M, D, h, m, s); //normal timestamp format?
    delay(100);
    #if SD_ENABLED
      sprintf(fileName, "%s_%04u_%02u_%02u.CSV", XPODID, Y, M, D);    //char array for fileName
    #endif
  #endif //RTC_ENABLED
  
  #if ADS_ENABLED
    ads_data = ads_module.return_updated();
    delay(100);
  #endif //ADS_ENABLED

  #if CO2_ENABLED
    CO2 = CO2_module.getS300CO2();
    delay(100);
  #endif //CO2_ENABLED

  #if BME_ENABLED
    bme_data = bme_module.return_updated();
    delay(100);
  #endif //CO2_ENABLED

  #if QUAD_ENABLED
    quadstat_data = quad_module.return_updated();
    delay(100);
  #endif //QUAD_ENABLED
  

  #if PMS_ENABLED
  bool pm_returned;
    pms.requestRead();
    if (pms.readUntil(pms_data)) {
      pm_returned = true;
    } else {
      pm_returned = false;
    } //if (pms.readUntil(pms_data)) 
    delay(100);
  #endif 

  /*  PRINT TO SD  */
  #if SD_ENABLED
    digitalWrite(SD_CS, LOW);
    sd.begin(SD_CS);
    // beginning sd object to then open file
    while (!sd.begin(SD_CS)) {
      #if SERIAL_ENABLED
        Serial.println("error in loop");
      #endif  //SERIAL_ENABLED
      sd.begin(SD_CS);
    } //while (!sd.begin(SD_CS))
    if(sd.begin(SD_CS)){
      file.open(fileName, O_CREAT | O_APPEND | O_WRITE); 
      delay(100);

      if(file.isOpen()){
        digitalWrite(GREEN_LED, HIGH);
        file.println();

        #if RTC_ENABLED
          file.print(bufftime);
          file.print(F(","));
          #else
            file.print(F(",");
        #endif //RTC_ENABLED

        #if ADS_ENABLED
          file.print(ads_data.Fig1);
          file.print(F(","));
          file.print(ads_data.Fig2);
          file.print(F(","));
          file.print(ads_data.Fig3);
          file.print(F(","));
          file.print(ads_data.Fig3_heater);
          file.print(F(","));
          file.print(ads_data.Fig4);
          file.print(F(","));
          file.print(ads_data.Fig4_heater);
          file.print(F(","));
          #if MQ_ENABLED
            file.print(ads_data.Mq);
            file.print(F(","));
          #endif //MQ_ENABLED
          #if PID_ENABLED
            file.print(ads_data.Pid);
          #endif //PID_ENABLED
          file.print(F(","));
          file.print(ads_data.Misc2611);
          file.print(F(","));
          file.print(ads_data.Auxiliary);
          file.print(F(","));
          file.print(ads_data.Worker);
          file.print(F(","));
          #else 
            file.print(F(",,,,,,,,,,")); //10 commas
        #endif //ADS_ENABLED

        #if CO2_ENABLED
          file.print(CO2);
          file.print(F(","));
          #else
            file.print(F(","));
        #endif //CO2_ENABLED

        #if BME_ENABLED
          file.print(bme_data.T);
          file.print(F(","));
          file.print(bme_data.P / 100.0);
          file.print(F(","));
          file.print(bme_data.RH);
          file.print(F(","));
          file.print(bme_data.GR / 1000.0);
          file.print(F(","));
          #else
            file.print(F(",,,,"));
        #endif //BME_ENABLED

        #if QUAD_ENABLED
          file.print(quadstat_data.QS1_C1);
          file.print(F(","));
          file.print(quadstat_data.QS1_C2);
          file.print(F(","));
          file.print(quadstat_data.QS2_C1);
          file.print(F(","));
          file.print(quadstat_data.QS2_C2);
          file.print(F(","));
          file.print(quadstat_data.QS3_C1);
          file.print(F(","));
          file.print(quadstat_data.QS3_C2);
          file.print(F(","));
          file.print(quadstat_data.QS4_C1);
          file.print(F(","));
          file.print(quadstat_data.QS4_C2);
          file.print(F(","));
          #else
            file.print(F(",,,,,,,,"));
        #endif //QUAD_ENABLED

        #if PMS_ENABLED
        if(pm_returned) {
          file.print(pms_data.pm10_env);
          file.print(F(","));
          delay(100);
          file.print(pms_data.pm25_env);
          file.print(F(","));
          delay(100);
          file.print(pms_data.pm100_env);
          file.print(F(","));
          delay(100);
          #if INCLUDE_STANDARD
            file.print(pms_data.pm10_standard);
            file.print(F(","));
            file.print(pms_data.pm25_standard);
            file.print(F(","));
            file.print(pms_data.pm100_standard);
            file.print(F(","));
          #else
            file.print(F(",,,"));
          #endif //INCLUDE_STANDARD
          delay(100);
          #if INCLUDE_PARTICLES
            if(pms_data.hasParticles) {
              file.print(pms_data.particles_03um);
              file.print(F(","));
              file.print(pms_data.particles_05um);
              file.print(F(","));
              file.print(pms_data.particles_10um);
              file.print(F(","));
              file.print(pms_data.particles_25um);
              file.print(F(","));
              file.print(pms_data.particles_50um);
              file.print(F(","));
              file.print(pms_data.particles_100um);
              file.print(F(","));
            } // if(pms_data.hasParticles)
          #else 
            file.print(F(",,,,,,"));
          #endif  //INCLUDE_PARTICLES
          delay(100);
        } else {
          file.print(F(",,,,,,,,,,,,"));
        } //if(pm_returned)
        #else 
          file.print(F(",,,,,,,,,,,,"));
        #endif //PMS_ENABLED
        delay(50);
        file.sync();
        file.close();
      } //if(file.isOpen())
    } //if(sd.begin(SD_CS))
    digitalWrite(SD_CS, HIGH);
    digitalWrite(GREEN_LED, LOW);
  #endif //SD_ENABLED

  /*  PRINT TO SERIAL  */
  #if SERIAL_ENABLED
    Serial.println();
    #if RTC_ENABLED
      Serial.print(bufftime);
      Serial.print(F(","));
    #endif //RTC_ENABLED
    #if ADS_ENABLED
      Serial.print(ads_data.Fig1);
      Serial.print(F(","));
      Serial.print(ads_data.Fig2);
      Serial.print(F(","));
      Serial.print(ads_data.Fig3);
      Serial.print(F(","));
      Serial.print(ads_data.Fig3_heater);
      Serial.print(F(","));
      Serial.print(ads_data.Fig4);
      Serial.print(F(","));
      Serial.print(ads_data.Fig4_heater);
      Serial.print(F(","));
      #if MQ_ENABLED
        Serial.print(ads_data.Mq);
        Serial.print(F(","));
      #endif //MQ_ENABLED
      #if PID_ENABLED
        Serial.print(ads_data.Pid);
        Serial.print(F(","));
      #endif //PID_ENABLED
      Serial.print(ads_data.Misc2611);
      Serial.print(F(","));
      Serial.print(ads_data.Auxiliary);
      Serial.print(F(","));
      Serial.print(ads_data.Worker);
      Serial.print(F(","));
    #endif //ADS_ENABLED
    #if CO2_ENABLED
      Serial.print(CO2);
      Serial.print(F(","));
    #endif //CO2_ENABLED
    #if BME_ENABLED
      Serial.print(bme_data.T);
      Serial.print(F(","));
      Serial.print(bme_data.P / 100.0);
      Serial.print(F(","));
      Serial.print(bme_data.RH);
      Serial.print(F(","));
      Serial.print(bme_data.GR / 1000.0);
      Serial.print(F(","));
    #endif //BME_ENABLED
    #if QUAD_ENABLED
      Serial.print(quadstat_data.QS1_C1);
      Serial.print(F(","));
      Serial.print(quadstat_data.QS1_C2);
      Serial.print(F(","));
      Serial.print(quadstat_data.QS2_C1);
      Serial.print(F(","));
      Serial.print(quadstat_data.QS2_C2);
      Serial.print(F(","));
      Serial.print(quadstat_data.QS3_C1);
      Serial.print(F(","));
      Serial.print(quadstat_data.QS3_C2);
      Serial.print(F(","));
      Serial.print(quadstat_data.QS4_C1);
      Serial.print(F(","));
      Serial.print(quadstat_data.QS4_C2);
      Serial.print(F(","));
    #endif //QUAD_ENABLED
    #if PMS_ENABLED
    if(pm_returned){
      Serial.print(pms_data.pm10_env);
      Serial.print(F(","));
      delay(100);
      Serial.print(pms_data.pm25_env);
      Serial.print(F(","));
      delay(100);
      Serial.print(pms_data.pm100_env);
      Serial.print(F(","));
      delay(100);
      #if INCLUDE_STANDARD
        Serial.print(pms_data.pm10_standard);
        Serial.print(F(","));
        Serial.print(pms_data.pm25_standard);
        Serial.print(F(","));
        Serial.print(pms_data.pm100_standard);
        Serial.print(F(","));
      #endif //INCLUDE_STANDARD
      delay(100);
      #if INCLUDE_PARTICLES
        if(pms_data.hasParticles) {
          Serial.print(pms_data.particles_03um);
          Serial.print(F(","));
          Serial.print(pms_data.particles_05um);
          Serial.print(F(","));
          Serial.print(pms_data.particles_10um);
          Serial.print(F(","));
          Serial.print(pms_data.particles_25um);
          Serial.print(F(","));
          Serial.print(pms_data.particles_50um);
          Serial.print(F(","));
          Serial.print(pms_data.particles_100um);
          Serial.print(F(","));
        } //if(pms_data.hasParticles)
      #endif  //INCLUDE_PARTICLES
      delay(100);
    } else {
      Serial.print(F("Error reaching PMS5003"));
    } //if(pm_returned)
    #endif //PMS_ENABLED
  #endif //SERIAL_ENABLED
} //void loop()
