#include <DFRobot_ESP_PH_WITH_ADC.h>
#include <EEPROM.h>
#include <GravityTDS.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#define VREF 3.3
#define SCOUNT  30
#define TdsSensorPin 36
GravityTDS gravityTds ;
DFRobot_ESP_PH_WITH_ADC ph;
#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value
#define PH_PIN 39    //the esp gpio data pin number
OneWire oneWire1(32);
DallasTemperature sensorAir(&oneWire1);
OneWire oneWire2(33);
DallasTemperature sensorUdara(&oneWire2);
float voltage, phValue, temperature = 25;
float tempC, suhuudara;
const int tur_pin = 35;
double sensorValue, voltagetur, voltage_konversi;
float suhuair, tdsval;

void setup() {
  Serial.begin(9600);
  EEPROM.begin(32);//needed to permit storage of calibration value in eeprom
  ph.begin();
  sensorAir.begin();
  sensorUdara.begin();
  pinMode(tur_pin,INPUT);
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(3.3);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(4096);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
}

void loop() 
{
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) //time interval: 1s
  {
    timepoint = millis();
    //voltage = rawPinValue / esp32ADC * esp32Vin
    voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE; // read the voltage
    Serial.print("voltage:");
    Serial.println(voltage, 4);
    
    //temperature = readTemperature();  // read your temperature sensor to execute temperature compensation
    Serial.print("temperature:");
    Serial.print(temperature, 1);
    Serial.println("ºC");

    phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
    Serial.print("pH:");
    Serial.println(phValue, 4);

    sensorUdara.requestTemperatures();
    suhuudara = sensorUdara.getTempCByIndex(0); 
    Serial.print("Suhu Udara: ");
    Serial.print(suhuudara);
    Serial.println("ºC");

    sensorAir.requestTemperatures();  //add your temperature sensor and read it
    suhuair = sensorAir.getTempCByIndex(0);
    gravityTds.setTemperature(suhuair);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate
    tdsval = gravityTds.getTdsValue();  // then get the value
    Serial.print("suhu air:");
    Serial.print(suhuair, 1); 
    Serial.println("ºC");
    Serial.print("TDS :");
    Serial.print(tdsval,0);
    Serial.println("ppm");

    sensorValue = analogRead(tur_pin);// read the input on analog pin 0:
    voltagetur = sensorValue *3.3 / 4095; // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    //voltage_konversi = voltagetur * (4.5 / 2 ) ; // konversi agar range voltage mendekati nilai yang ada pada jurnal
    Serial.print("Turbidity: ");
    Serial.println(voltagetur); // print out the value you read:
  }
  ph.calibration(voltage, temperature); // calibration process by Serail CMD
}
