 /*******************************************************************************
 * @project Hannigan Lab's Next Gen. Air Quality Pods
 *
 * @file    xpod_V4.0.0.ino
 * @version Percy's 4.0.0
 * @brief   Main XPOD .ino - Gathers & Prints Data
 *
 * @author 	Percy Smith
 * @date 	  October 09, 2024
 * @log     UPDATES & REMOVES UNUSED LIBRARIES
 *          Fixes random huge readings in PM 
 *          Pulled the MET firmware due to interrupts being weird...
 *          Re-writes CO2 firmware to mirror YPOD CO2 firmware for the vibes
 *          Also still uses the WDT but now... u can disable it .. big day.
 * @test    RUNNING TEST 10/08/2024 TO CONFIRM PM FIX - confirmed 10/18
 ******************************************************************************/
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <SdFat.h>

#include "xpod_node.h"

/*************  Global Declarations  *************/
SdFat sd;
File file;
char fileName[] = "YPODID_YYYY_MM_DD.CSV";
char bufftime[] = "YYYY-MM-DDThh:mm:ss";
  int Y,M,D,h,m,s;

/*************  Conditional Global Declarations  *************/
#if IC_POTS
  #include "digipot.h"
#endif //IC_POTS

#if THE_DAWG
  #include <avr/wdt.h>
#endif //THE_DAWG

#if RTC_ENABLED
  #include <RTClib.h>
  RTC_DS3231 rtc;
  DateTime rtc_date_time;
#endif //RTC_ENABLED

#if ADS_ENABLED
  #include "ads_module.h"
  ADS_Module ads_module;
#endif //ADS_ENABLED

#if CO2_ENABLED
  #include "co2_module.h"
  ELT_S300 CO2_module;
#endif //CO2_ENABLED

#if BME_ENABLED
  #include "bme_module.h"
  BME_Module bme_module;
#endif //BME_ENABLED

#if MQ_ENABLED
  #include "mq_module.h"
  MQ_Module mq_module;
#endif //MQ_ENABLED

#if QUAD_ENABLED
  #include "quad_module.h"
  QUAD_Module quad_module;
#endif

#if PMS_ENABLED
  #include "plantower_module.h"
  PMS5003 pms5003;
  pms5003data pmdata;
#endif //PMS_ENABLED


/***************************************************************************************/
void setup() {
  /*    FOUNDATIONS OF FW    */
  #if SERIAL_ENABLED
    Serial.begin(9600);
  #endif //SERIAL_ENABLED
  Wire.begin();
  SPI.begin();

  /*    PIN DECLARATIONS    */
  // Really Critical Pins  
  pinMode(IN_VOLT_PIN, INPUT);
  pinMode(SD_CS, OUTPUT);
  // LEDs (internal & external)
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  #if EXTERNAL_LED
    pinMode(EXT_GREEN, OUTPUT);
    pinMode(EXT_RED, OUTPUT);
  #endif //EXTERNAL_LED

  #if IC_POTS
    initpots();
    DownPot(0);
    DownPot(1);
    //DownPot(2);
    SetPotLevel(2, 80);
  #endif //IC_POTS

  #if ADS_ENABLED
    if (!ads_module.begin())
    {
      #if SERIAL_ENABLED
        Serial.println("Error: Failed to initialize one of the ADS1115 module!");
      #endif //SERIAL_ENABLED
    }
  #endif //ADS_ENABLED

  #if CO2_ENABLED
    CO2_module.begin();
  #endif //CO2_ENABLED

  #if BME_ENABLED
    if (!bme_module.begin())
    {
      #if SERIAL_ENABLED
        Serial.println("Error: Failed to initialize BME sensor!");
      #endif //SERIAL_ENABLED
    }
  #endif //BME_ENABLED

  #if QUAD_ENABLED
    if (!quad_module.begin())
    {
      #if SERIAL_ENABLED
        Serial.println("Error: Failed to initialize Quad Stat!");
      #endif //SERIAL_ENABLED
    }
  #endif //QUAD_ENABLED
  
  #if MQ_ENABLED
    if (!mq_module.begin())
    {
      #if SERIAL_ENABLED
        Serial.println("Error: Failed to initialize MQ sensor!");
      #endif
    }
  #endif //MQ_ENABLED
  
  #if PMS_ENABLED
    Serial1.begin(9600);
  #endif //PMS_ENABLED

  /*  RTC Initializing & Setting DT  */
  #if RTC_ENABLED 
    if (!rtc.begin())
    {
      #if SERIAL_ENABLED
        Serial.println("Error: Failed to initialize RTC module");
      #endif //SERIAL_ENABLED
    }
    else{
      #if ADJUST_DATETIME
        rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));    // Only run uncommented once to initialize RTC
        #if USE_UTC
          uint32_t unixrtc = rtc.now().unixtime();
          uint32_t rtc_utc = unixrtc + 6*3600; //NEEDS TO BE MODIFIED DEPENDING ON MST/MDT
          rtc.adjust(DateTime(rtc_utc));     
        #endif //USE_UTC
        rtc_date_time = rtc.now();
      #endif //ADJUST_DATETIME
    }
  #endif //RTC_ENABLED

  /*  SD Card & File Setup  */
  digitalWrite(SD_CS, LOW);       //Pull SD_CS pin LOW to initialize SPI comms
  sd.begin(SD_CS);                //Initialize SD Card with relevant chip select pin
  // Establish contact with SD card - if initialization fails, run until success
  while (!sd.begin(SD_CS)) 
  {
    #if SERIAL_ENABLED
      Serial.println("insert sd card to begin");
      #if EXTERNAL_LED
        digitalWrite(EXT_RED, HIGH);
      #endif //EXTERNAL_LED
    #endif  //SERIAL_ENABLED
    sd.begin(SD_CS);      //attempt to initialize again
  } //while(!sd.begin(SD_CS))
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);      //if we exit the while loop, blink green LED once to indicate success
  #if EXTERNAL_LED
    digitalWrite(EXT_RED, LOW);
    digitalWrite(EXT_GREEN, HIGH);
  #endif //EXTERNAL_LED
  //File Naming (FORMATTING HAS TO BE CONSISTENT WITH GLOBAL DECLARATION!!)
  DateTime now = rtc.now();     //pulls setup() time so we have one file name per run in a day
    Y = now.year();
    M = now.month();
    D = now.day();
  sprintf(fileName, "%s_%04u_%02u_%02u.CSV", xpodID, Y, M, D);    //char array for fileName
  delay(100);   
  file.open(fileName, O_CREAT | O_APPEND | O_WRITE);  //open with create, append, write permissions
  file.close();                                       //close file, we opened so loop() is faster 
  digitalWrite(SD_CS, HIGH);    //release chip select on SD - allow other comm with SPI
  digitalWrite(GREEN_LED, LOW);     //turn off green LED (file is closed)
  #if EXTERNAL_LED
    digitalWrite(EXT_GREEN, LOW);
  #endif //EXTERNAL_LED
  
  #if THE_DAWG
    wdt_enable(WDTO_8S);
  #endif //THE_DAWG
} //void setup()


/***************************************************************************************/
void loop() {
  #if THE_DAWG
    wdt_reset();
  #endif //THE_DAWG
  float in_volt_val;
  in_volt_val = (analogRead(IN_VOLT_PIN) * 5.02 * 5) / 1023.0; //Follow up with rylee

  #if CO2_ENABLED
    float CO2 = CO2_module.getS300CO2();
    delay(100);
  #endif //CO2_ENABLED

  #if PMS_ENABLED
  //poll for plantower data first cause it has to run a while loop and i'm paranoid !!
    bool pm_available = 0;
    while(!pm_available)
    {
      pm_available = pms5003.readPMSdata(&Serial1);
      delay(100);
      if(pm_available) { pmdata = pms5003.returnPMdataset(); }
    } //while(!pm_available)
    delay(100);
  #endif //PMS_ENABLED

  digitalWrite(GREEN_LED, LOW);

  //open SPI SD 
  digitalWrite(SD_CS, LOW);

  while (!sd.begin(SD_CS)) {
    #if SERIAL_ENABLED
      Serial.println("error in loop");
    #endif  //SERIAL_ENABLED
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    #if EXTERNAL_LED
      digitalWrite(EXT_GREEN, LOW);
      digitalWrite(EXT_RED, HIGH);
    #endif //EXTERNAL_LED
    sd.begin(SD_CS);
  }

  delay(100);
  DateTime now = rtc.now();
  Y = now.year();  M = now.month();  D = now.day();  h = now.hour();  m = now.minute();  s = now.second();
  sprintf(bufftime, "%04u-%02u-%02uT%02u:%02u:%02u", Y, M, D, h, m, s);
  delay(100); 
  sprintf(fileName, "%s_%04u_%02u_%02u.CSV", xpodID, Y, M, D);    //char array for fileName

  if(sd.begin(SD_CS)){
    delay(100);
    file.open(fileName, O_CREAT | O_APPEND | O_WRITE); 
    delay(100);
    if(file.isOpen())
    {
      digitalWrite(GREEN_LED, HIGH);
      #if EXTERNAL_LED
        digitalWrite(EXT_RED, LOW);
        digitalWrite(EXT_GREEN, HIGH);
      #endif //EXTERNAL_LED

      file.print(bufftime);
      file.print(",");
      delay(100); 

      file.print(in_volt_val);
      file.print(",");
      delay(100);
      
      #if ADS_ENABLED
        file.print(ads_module.read4sd_raw());
        file.print(",");
        delay(100);
      #else
        file.print(",,,,,,,,");
      #endif //ADS_ENABLED

      #if CO2_ENABLED
        file.print(CO2);
        delay(100);
        file.print(",");
      #else
        file.print(",");
      #endif //CO2_ENABLED

      #if BME_ENABLED
        file.print(bme_module.read4sd());
        file.print(",");
        delay(100);
      #else
        file.print(",,,,,");
      #endif //BME_ENABLED
          
      #if QUAD_ENABLED
        file.print(quad_module.read());
        file.print(",");
      #else
        file.print(",,,,,,,,");
      #endif //QUAD_ENABLED
    
      #if MET_ENABLED
        file.print(String(wind_speed));
        file.print(",");
        file.print(String(wind_dir_degree));
        file.print(",");
      #else
        file.print(",,");
      #endif //MET_ENABLED

      #if MQ_ENABLED
        file.print(mq_module.read4sd());
        file.print(",");
      #else
        file.print(",");
      #endif //MQ_ENABLED

      #if PMS_ENABLED
        file.print(pmdata.pm10_env);
        file.print(",");
        file.print(pmdata.pm25_env);
        file.print(",");
        file.print(pmdata.pm100_env);
        file.print(",");
        delay(100);
        file.print(pmdata.particles_03um);
        file.print(",");
        file.print(pmdata.particles_05um);
        file.print(",");
        file.print(pmdata.particles_10um);
        file.print(",");
        file.print(pmdata.particles_25um);
        file.print(",");
        file.print(pmdata.particles_50um);
        file.print(",");
        file.print(pmdata.particles_100um);
        file.print(",");
        delay(100); 
        file.print(pmdata.pm10_standard);
        file.print(",");
        file.print(pmdata.pm25_standard);
        file.print(",");
        file.print(pmdata.pm100_standard);
        file.print(",");
        delay(100); 
      #else
        file.print(",,,");
        file.print(",,,,,,");
        file.print(",,,");
      #endif //PMS_ENABLED

      file.print("\n");
      delay(100);

      file.sync();
      file.close();
    } else {
      #if SERIAL_ENABLED
        Serial.println("File not opening");
      #endif //SERIAL_ENABLED
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      #if EXTERNAL_LED
        digitalWrite(EXT_GREEN, LOW);
        digitalWrite(EXT_RED, HIGH);
      #endif //EXTERNAL_LED
      file.close();
    }
  } else {
    #if SERIAL_ENABLED
      Serial.println("SD begin failed");
    #endif //SERIAL_ENABLED
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    #if EXTERNAL_LED
      digitalWrite(EXT_GREEN, LOW);
      digitalWrite(EXT_RED, HIGH);
    #endif //EXTERNAL_LED
  }
  digitalWrite(SD_CS, HIGH);
  digitalWrite(GREEN_LED, LOW);

  #if SERIAL_ENABLED
    Serial.print(bufftime);
    Serial.print(",");
    delay(100);

    Serial.print("Volt:");
    Serial.print(in_volt_val);
    Serial.print(","); 
    delay(100);

    #if ADS_ENABLED
      Serial.print(ads_module.read4print_raw());
      Serial.print(",");
      delay(100);  
    #endif //ADS_ENABLED

    #if CO2_ENABLED
      Serial.print("CO2:");
      Serial.print(CO2);
      Serial.print(",");
      delay(100);
    #endif //CO2_ENABLED

    #if BME_ENABLED
      Serial.print(bme_module.read4print());
      Serial.print(",");
      delay(100);
    #endif //BME_ENABLED

    #if QUAD_ENABLED
      Serial.print(quad_module.read());
      Serial.print(",");
      delay(100);
    #endif //QUAD_ENABLED

    #if MQ_ENABLED
      Serial.print(mq_module.read4print());
      Serial.print(",");
    #endif //MQ_ENABLED    

    #if PMS_ENABLED
      Serial.print("PM1.0_ENV:");
      Serial.print(pmdata.pm10_env);
      Serial.print(",PM2.5_ENV:");
      Serial.print(pmdata.pm25_env);
      Serial.print(",PM10.0_ENV:");
      Serial.print(pmdata.pm100_env);
      delay(100);
      Serial.print(",PM_0.3um:");
      Serial.print(pmdata.particles_03um);
      Serial.print(",PM_0.5um:");
      Serial.print(pmdata.particles_05um);
      Serial.print(",PM_1.0um:");
      Serial.print(pmdata.particles_10um);
      Serial.print(",PM_2.5um:");
      Serial.print(pmdata.particles_25um);
      Serial.print(",PM_5.0um:");
      Serial.print(pmdata.particles_50um);
      Serial.print(",PM_10.0um:");
      Serial.print(pmdata.particles_100um);
      delay(100);
      Serial.print(",PM1.0_STND:");
      Serial.print(pmdata.pm10_standard);
      Serial.print(",PM2.5_STND:");
      Serial.print(pmdata.pm25_standard);
      Serial.print(",PM10.0_STND:");
      Serial.print(pmdata.pm100_standard);
      delay(100);
    #endif //PMS_ENABLED
    Serial.print("\n");
  #endif //SERIAL_ENABLED
} //void loop()
