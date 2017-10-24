/*
 * To operate the BME280 sensor, you need 4 basic APIs begin, setSensorConfig, setPowerMode, readSensorData. 
 * 
 * The sensor supports different power modes and sampling settings which can be configured based on the user need 
 * by using the following values given in the table,
 *-------------------------------------------------------------------------------------------------------------------
 *        power modes       |   Oversampling settings   |    Standby duration settings  |     Filter settings       |
 *--------------------------|---------------------------|-------------------------------|-------------------------- |
 *      BME280_SLEEP_MODE   | BME280_OVERSAMP_SKIPPED   |  BME280_STANDBY_TIME_1_MS     |  BME280_FILTER_COEFF_OFF  |
 *      BME280_FORCED_MODE  | BME280_OVERSAMP_1X        |  BME280_STANDBY_TIME_63_MS    |  BME280_FILTER_COEFF_2    |
 *      BME280_NORMAL_MODE  | BME280_OVERSAMP_2X        |  BME280_STANDBY_TIME_125_MS   |  BME280_FILTER_COEFF_4    |
 *                          | BME280_OVERSAMP_4X        |  BME280_STANDBY_TIME_250_MS   |  BME280_FILTER_COEFF_8    |
 *                          | BME280_OVERSAMP_8X        |  BME280_STANDBY_TIME_500_MS   |  BME280_FILTER_COEFF_16   |
 *                          | BME280_OVERSAMP_16X       |  BME280_STANDBY_TIME_1000_MS  |                           |
 *                          |                           |  BME280_STANDBY_TIME_10_MS    |                           |
 *                          |                           |  BME280_STANDBY_TIME_20_MS    |                           |
 *-------------------------------------------------------------------------------------------------------------------
 *
 */
#include <BME280_wr.h>

#include <ESP8266WiFi.h>
//#define WIFIENABLED

const char* ssid     = "BME280MAKEATHON";
const char* password = "123456789";
byte ip[] = { 192, 168, 43, 1};
WiFiClient client;

BME280 bme280;  //Constructor for I2c with device address (BME280 PIN4)SDO->GND Then Dev_Addr = 0x76

void setup()
{  
  Serial.begin(115200); 
  I2C.begin(2,0); 

  int8_t rslt = bme280.begin();
  if (rslt) {
    Serial.println("BME280 Sensor not connected properly ,Please Check your wiring!");
    while (1);
  }
  Serial.println("Init done");
  
  //  The below sensor settings can be modified based on user requirement as mentioned in
  //  "Section 5.5 - Recommended modes of operation" of BME280 datasheet available at,
  //  https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf
  bme280.humidityOverSampling = BME280_OVERSAMP_1X;
  bme280.pressureOverSampling = BME280_OVERSAMP_1X;
  bme280.temperatureOverSampling = BME280_OVERSAMP_1X;
  bme280.standbyDuration = BME280_STANDBY_TIME_1_MS;
  bme280.filterCoefficient = BME280_FILTER_COEFF_OFF;
  
  bme280.setSensorConfig(); // Configuring sensor's sampling, filter and standby duration settings
  bme280.setPowerMode(BME280_NORMAL_MODE); // Sets sensor to NORMAL mode.
  delay(10); //Wait for the first measurement to complete


#ifdef WIFIENABLED
  WiFi.begin(ssid, password);
  delay(10);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
  }
  
  const int Port = 8080;
  while (client.connect(ip, Port) != true) 
  {
    delay(500);
  }
#endif  
}

void loop()
{
  bme280.readSensorData();
  String data  = "Temperature data : ";
  data += ((bme280.temperature)*2);  // Temperature in °C
  data += ", Pressure data : ";
  data += bme280.pressure;  // Pressure in Pa 
  data += ", Humidity data : ";
  data += bme280.humidity;  // Humidity in %rH
  Serial.println(data);
  delay(200);
  
#ifdef WIFIENABLED
  // This will send the request to the server
    byte connection =  client.println(data);
    delay(30);

   if(connection == 0)
   {
      int Port = 8080;
      while (client.connect(ip, Port) != true) 
      {
        delay(500);
      }   
   }
#endif
}
