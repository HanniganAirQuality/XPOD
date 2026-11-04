/*******************************************************************************
 * @project Hannigan Lab's Next Gen. Air Quality Pods
 *
 * @file    xpod_V4.4.0.ino
 * @version Percy's 4.4.0
 * @brief   XPOD Code Rewrite
 * 
 * @author  Percy Smith
 * @date 	  November 3, 2026
 * @log     GPS Time :)
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

#if GPS_ENABLED
  #include <TinyGPS++.h>
  TinyGPSPlus gps; 
  double gps_altitude = -99;
  double gps_longitude = -99;
  double gps_latitude = -99;
  #if GPS_SPD
    int32_t gps_speed_raw = -99;
    double gps_mps = -99;
  #endif //GPS_SPD
  #if GPS_TIME
    uint32_t gps_date = 0;
    uint32_t gps_time = 0;
  #endif //GPS_TIME 
  #if GPS_HDOP
    int32_t gps_hdop = 0;
    uint32_t num_sat = 0;
  #endif //HDOP
#endif //GPS_ENABLED

#if MET_ENABLED
  #include "wind_vane.h"
  long lastWindCheck = 0;
  volatile long lastWindIRQ = 0;
  volatile byte windClicks = 0;
  wind_vane windVane;
#endif

/******************  Functions  ******************/
#if MET_ENABLED
  void wspeedIRQ()
  {
    if(millis() - lastWindIRQ > 10)
    {
      lastWindIRQ = millis();
      windClicks++;
    }
  }
#endif

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

  #if MET_ENABLED
    pinMode(MET_SPD, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(MET_SPD), wspeedIRQ, FALLING);  // attaching wind speed interrupt to pin 3
  #endif

  #if GPS_ENABLED
    Serial2.begin(9600);
  #endif //GPS_ENABLED

  /*    PIN DECLARATIONS    */
  pinMode(SD_CS, OUTPUT);
  #if INPUTVOLT_ENABLED
    pinMode(IN_VOLT_PIN, INPUT);
  #endif
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
    #if GPS_ENABLED
      smartDelay(100);
    #else
      delay(100);
    #endif //GPS_ENABLED
    #if SD_ENABLED
      sprintf(fileName, "%s_%04u_%02u_%02u.CSV", XPODID, Y, M, D);    //char array for fileName
    #endif
  #endif //RTC_ENABLED
  
  #if INPUTVOLT_ENABLED
    float in_volt_val;
    in_volt_val = (analogRead(IN_VOLT_PIN) * 5.02 * 5) / 1023.0; //Follow up with rylee
  #endif

  #if ADS_ENABLED
    ads_data = ads_module.return_updated();
    #if GPS_ENABLED
      smartDelay(100);
    #else
      delay(100);
    #endif //GPS_ENABLED
  #endif //ADS_ENABLED

  #if CO2_ENABLED
    CO2 = CO2_module.getS300CO2();
    #if GPS_ENABLED
      smartDelay(100);
    #else
      delay(100);
    #endif //GPS_ENABLED
  #endif //CO2_ENABLED

  #if BME_ENABLED
    bme_data = bme_module.return_updated();
    #if GPS_ENABLED
      smartDelay(100);
    #else
      delay(100);
    #endif //GPS_ENABLED
  #endif //CO2_ENABLED

  #if QUAD_ENABLED
    quadstat_data = quad_module.return_updated();
    #if GPS_ENABLED
      smartDelay(100);
    #else
      delay(100);
    #endif //GPS_ENABLED
  #endif //QUAD_ENABLED
  

  #if PMS_ENABLED
  bool pm_returned;
    pms.requestRead();
    if (pms.readUntil(pms_data)) {
      pm_returned = true;
    } else {
      pm_returned = false;
    } //if (pms.readUntil(pms_data)) 
    #if GPS_ENABLED
      smartDelay(100);
    #else
      delay(100);
    #endif //GPS_ENABLED
  #endif //PMS_ENABLED

  #if MET_ENABLED
    float wind_speed = get_wind_speed();
    float wind_dir_volt = windVane.get_direction();
    float wind_dir_degree = windVane.degree_direction(wind_dir_volt);
    String wind_dir_cardinal = windVane.cardinal_direction(wind_dir_volt);
  #endif //MET_ENABLED Data Gathering
  
  #if GPS_ENABLED
    while(Serial2.available() > 0)    {
      gps.encode(Serial2.read());
    } //while(Serial2.available() > 0)
    
    //ALTITUDE (Default)
    bool new_alt = gps.altitude.isUpdated();
    int alt_age = gps.altitude.age();
    if(new_alt) {
      gps_altitude = gps.altitude.meters();
    } //if(new_alt)

    //LOCATION (Default)
    bool new_loc = gps.location.isUpdated();
    int loc_age = gps.location.age();
    if(new_loc) {
      gps_longitude = gps.location.lng();
      gps_latitude = gps.location.lat();
    } //if(new_loc)

    #if GPS_SPD
      bool new_spd = gps.speed.isUpdated();
      int spd_age = gps.speed.age();
      if(new_spd){
        gps_speed_raw = gps.speed.value();
        gps_mps = gps.speed.mps();
      } //if(new_spd)
    #endif //GPS_SPD
    
    #if GPS_TIME
      bool new_date = gps.date.isUpdated();
      int date_age = gps.date.age();
      if(new_date){
        gps_date = gps.date.value();
      } //if(new_time)      

      bool new_time = gps.time.isUpdated();
      int time_age = gps.time.age();
      if(new_time){
        gps_time = gps.time.value();
      } //if(new_time)      
    #endif //GPS_TIME

    #if GPS_HDOP
      bool new_hdop = gps.hdop.isUpdated();
      int hdop_age = gps.hdop.age();
      if(new_hdop){
        gps_hdop = gps.hdop.value();
      }
      bool new_sat = gps.satellites.isUpdated();
      int sat_age = gps.satellites.age();
      if(new_sat){
        num_sat = gps.satellites.value();
      }
    #endif //GPS_HDOP
  #endif //GPS_ENABLED

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
            file.print(F(","));
        #endif //RTC_ENABLED

        #if INPUTVOLT_ENABLED
          file.print(in_volt_val);
          file.print(F(","));
          #else
          file.print(F(","));
        #endif //INPUTVOLT_ENABLED

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
          file.print(ads_data.Worker);
          file.print(F(","));
          file.print(ads_data.Auxiliary);
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
          
        #if MET_ENABLED
          file.print(String(wind_speed));
          file.print(F(","));
          file.print(String(wind_dir_degree));
          file.print(F(","));
        #else
          file.print(F(",,"));
        #endif //MET_ENABLED

        #if GPS_ENABLED
          file.print(gps_longitude, 6);
          file.print(F(","));
          file.print(gps_latitude, 6);
          file.print(F(","));
          file.print(new_loc);
          file.print(F(","));
          file.print(loc_age);
          file.print(F(","));
          file.print(gps_altitude);
          file.print(F(","));
          file.print(new_alt);
          file.print(F(","));
          file.print(alt_age);
          file.print(F(","));
          
          #if GPS_SPD
            file.print(gps_speed_raw);
            file.print(F(","));
            file.print(gps_mps);
            file.print(F(","));
            file.print(new_spd);
            file.print(F(","));
            file.print(spd_age);
            file.print(F(","));
          #else 
            file.print(F(","));
            file.print(F(","));
            file.print(F(","));
            file.print(F(","));
          #endif //GPS_SPD

          #if GPS_TIME
            file.print(gps_date);
            file.print(F(","));
            file.print(new_date);
            file.print(F(","));
            file.print(date_age);
            file.print(F(","));
            file.print(gps_time);
            file.print(F(","));
            file.print(new_time);
            file.print(F(","));
            file.print(time_age);
            file.print(F(","));
          #else 
            file.print(F(","));
            file.print(F(","));
            file.print(F(","));
            file.print(F(","));
            file.print(F(","));
            file.print(F(","));
          #endif //GPS_TIME

          #if GPS_HDOP 
            file.print(gps_hdop);
            file.print(F(","));
            file.print(new_hdop);
            file.print(F(","));
            file.print(hdop_age);
            file.print(F(","));
            file.print(num_sat);
            file.print(F(","));
            file.print(new_sat);
            file.print(F(","));
            file.print(sat_age);
            file.print(F(","));
          #else 
            file.print(F(","));
            file.print(F(","));
            file.print(F(","));
            file.print(F(","));
            file.print(F(","));
            file.print(F(","));
          #endif //GPS_HDOP
        #else           
          file.print(F(",,,,,,,,,,,,,,,,,,,,,,,"));
        #endif //GPS_ENABLED

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
    #if INPUTVOLT_ENABLED
      Serial.print(in_volt_val);
      Serial.print(F(","));
    #endif
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
      Serial.print(ads_data.Worker);
      Serial.print(F(","));
      Serial.print(ads_data.Auxiliary);
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
    #if MET_ENABLED
      Serial.print(String(wind_speed));
      Serial.print(",");
      Serial.print(String(wind_dir_degree));
      Serial.print(",");
      Serial.print(String(wind_dir_volt));
      Serial.print(",");
    #else
      Serial.print(",,");
    #endif //MET_ENABLED

    #if GPS_ENABLED
      Serial.print(gps_longitude, 6);
      Serial.print(F(","));
      Serial.print(gps_latitude, 6);
      Serial.print(F(","));
      Serial.print(new_loc);
      Serial.print(F(","));
      Serial.print(loc_age);
      Serial.print(F(","));

      Serial.print(gps_altitude, 1); //doesn't seem to be writing anything other than the first dec?
      Serial.print(F(","));
      Serial.print(new_alt);
      Serial.print(F(","));
      Serial.print(alt_age);
      Serial.print(F(","));

      #if GPS_SPD
        Serial.print(gps_speed_raw);
        Serial.print(F(","));
        Serial.print(gps_mps, 1); //decimal has 1 out of 2 digits to prevent noise at 0 m/s
        Serial.print(F(","));
        Serial.print(new_spd);
        Serial.print(F(","));
        Serial.print(spd_age);
        Serial.print(F(","));
      #endif //GPS_SPD
      
      #if GPS_TIME
        Serial.print(gps_date);
        Serial.print(F(","));
        Serial.print(new_date);
        Serial.print(F(","));
        Serial.print(date_age);
        Serial.print(F(","));
        Serial.print(gps_time);
        Serial.print(F(","));
        Serial.print(time_age);
        Serial.print(F(","));
        Serial.print(new_time);
        Serial.print(F(","));
      #endif //GPS_TIME
      
      #if GPS_HDOP 
        Serial.print(gps_hdop);
        Serial.print(F(","));
        Serial.print(new_hdop);
        Serial.print(F(","));
        Serial.print(hdop_age);
        Serial.print(F(","));
        Serial.print(num_sat);
        Serial.print(F(","));
        Serial.print(new_sat);
        Serial.print(F(","));
        Serial.print(sat_age);
        Serial.print(F(","));
      #endif //GPS_HDOP
    #endif //GPS_ENABLED
  #endif //SERIAL_ENABLED
} //void loop()

#if MET_ENABLED
//Returns the instataneous wind speed
float get_wind_speed(){
  float deltaTime = millis() - lastWindCheck; //750ms

  deltaTime /= 1000.0; //Covert to seconds

  float windSpeed = (float)windClicks / deltaTime; //3 / 0.750s = 4

  windClicks = 0; //Reset and start watching for new wind
  lastWindCheck = millis();

  windSpeed *= 1.492; //4 * 1.492 = 5.968MPH

  return (windSpeed);
}
#endif

void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    #if GPS_ENABLED
      while (Serial2.available()) gps.encode(Serial2.read());
    #endif
  } while (millis() - start < ms);
}