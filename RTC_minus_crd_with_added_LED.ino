

//Name: Iesan Jung karki
//Date: 3/19/2018

//Purpose: Take time from RTC and print of SD card
//Objectives completed: >

#include <Wire.h>     //connections 
#include <SPI.h>      
#include<EEPROM.h>    //For internal arduino memory
#include <RTClib.h>   //rtc library
#include <TimerOne.h> //LED timer library

//main Variables 
const int SIGNAL_THRESHOLD = 20; //maximum signal strength that triggers the data collection
unsigned long count = 0L;         //tally for total number of muons detected
const float cal[] = {9.038130548712698e-15, -1.4599909946491252e-12, -1.2626490003703338e-08, 1.1406923013075114e-05, -0.002891003749200569, 0.5003740521722947, 15.615470780166067};

//LED variables
int LED = 10; //pin used for LED blink codes
#define LED_TIMER 2000000 //LED blinks for 1 second per muon count
unsigned int T1;



//This calibration is for 499, 5pF, 10k
//const float cal[] = {4.0614902396231111e-14, -7.8414735932155768e-11, 5.6819206951234577e-08, -1.7873823684604088e-05, 0.0029455520399522652, 0.01701073093454207, 26.268331951783924};
//This calibration is for 499, 5pF, 10k
//const float cal[] = {9.038130548712698e-15, -1.4599909946491252e-12, -1.2626490003703338e-08, 1.1406923013075114e-05, -0.002891003749200569, 0.5003740521722947, 15.615470780166067};

//SD - card variable
const int chipSelect = 8;
//File muonFile;

//RTC Variables
RTC_DS3231 rtc;

DateTime start_time;
DateTime time_stamp;     
TimeSpan seconds_stamp;      
TimeSpan measurement_deadtime;
TimeSpan total_deadtime;

//SiPM Variables
float sipm_voltage = 0;
float last_sipm_voltage = 0;

void setup()
{
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2));    // clear prescaler bits
  ADCSRA |= bit (ADPS1);                                   // Set prescaler to 4  

  Serial.begin(9600);

  Serial.println("with RTC");

  if(! rtc.begin()){
    Serial.println("Couldn't find RTC");
      }

  if(rtc.lostPower())
  {
    Serial.println("Run Time Adjusting program again");
    delay(3000);
  }
  
  
  start_time = rtc.now();
  pinMode(LED,OUTPUT);

 Timer1.initialize(LED_TIMER); //initialize the LED timer
 Timer1.attachInterrupt(LEDisr);//attatch timer isr routine 

  
  }//end Setup()

void loop()
{
  
  while(1)
  {
    
    if(analogRead(A0) > SIGNAL_THRESHOLD)
    {     
      float avg_adc_measurement = (analogRead(A0) + analogRead(A0) ) / 2.;     //takes the average of 4 readings 
      digitalWrite(LED,HIGH);
      T1 = millis();
      count++;
      time_stamp = rtc.now(); //time to start the record
      seconds_stamp = time_stamp - start_time;
      
      sipm_voltage = get_sipm_voltage(avg_adc_measurement);
      
      Serial.println((String)count + " " + getDate(time_stamp)+ " " + avg_adc_measurement+ " " + sipm_voltage + " ");
      //while((analogRead(A0)+analogRead(A0)+analogRead(A0))/3. > (9)){continue;}
    }
   
  }
}

String getDate(const DateTime& dt) 
{
    String retDate = "";
    retDate += (String)dt.year();
    retDate += "/";
    retDate += (String)dt.month();
    retDate += "/";
    retDate += (String)dt.day();
    retDate += " ";
     retDate += " ";
    retDate += (String)dt.hour();
    retDate += ":";
    retDate += (String)dt.minute();
    retDate += ":";
    retDate += (String)dt.second();

    return retDate;
}
float get_sipm_voltage(float adc_value)
{
  float voltage = 0;
  for (int i = 0; i < (sizeof(cal)/sizeof(float)); i++) {
    voltage += cal[i] * pow(adc_value,(sizeof(cal)/sizeof(float)-i-1));
    }
    return voltage;
}
void LEDisr()
{
  interrupts();
  if(millis()-T1 >= 1000)
  {
  digitalWrite(LED,LOW);  
  }
  
}









