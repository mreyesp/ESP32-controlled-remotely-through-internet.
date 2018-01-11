/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	

	Software for "ESP32 as wireless temperature sensor and actuator 
	controlled through internet" system.
	
	Moises Reyes Pardo	|	12/2017	|	No Licence
	
	Source code working on ESP32 Devkit v1 device programed using
	Arduino IDE.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <WiFi.h>
#include <OneWire.h>

/* Global statements required for WiFi connection */
const char* ssid = "your_Wi-Fi_network";
const char* password =  "your_Wi-Fi_password";

/* Global statements required for a TCP server */
int TCP_port = 1234;  
WiFiServer server(TCP_port);
String data;

/* Global statements required for T° sensor and LED */
int ledPin = 5;
int DS18S20_Pin = 15;
OneWire ds(DS18S20_Pin);
int rec=0;
float temperature;

void setup() { 
  Serial.begin(115200);     // Satart serial port 
  pinMode(ledPin, OUTPUT);  // Set the LED´s pin as output

/* Set the convertion resolution on DS18B20 */
  ds.reset();
  ds.write(0xcc);
  ds.write(0x4E);         // write on scratchPad
  ds.write(0x00);         // User byte 0 - Unused
  ds.write(0x00);         // User byte 1 - Unused
  ds.write(0x1F);         // setup to 9 bits, 12 bits => (0x7F)

 /* Start the wi-fi connection */ 
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

 /* Start the TCP server */
  server.begin();
  delay(100); 
}

void loop() {
  WiFiClient client = server.available(); //TCP server waiting for a client
  if (client.connected()) {               //TCP client connected    
      while (client.connected()) {         
        data = client.readStringUntil('\r');     
        if (data == "led_on"){
          Serial.println("The LED connected to P5 is On");
          digitalWrite(ledPin, HIGH);     //LED turn on        
          temperature = getTemp();      //Get the temperature
          delay(20);      
          client.print(temperature);  //Send the T° to TCP client
          Serial.println(temperature);
        }
        else if (data == "led_off"){
          Serial.println("The LED connected to P5 is Off");
          digitalWrite(ledPin, LOW);      //LED turn off
        }
      }
      Serial.println("The client disconnected");
      delay(100);
  }    
}

/*Function to get the temperature DS18S20 in DEG Celsius*/
float getTemp(){
  byte data[12];
  byte addr[8];
  float tempRead;
  float TemperatureSum;
  byte MSB;
  byte LSB;
  byte SignBit;

  ds.reset();
  ds.write(0xcc); //Skyp the ROM address
  delay(5);
  ds.write(0x44); //start conversion, without parasite power
  delay(5);

  byte present = ds.reset();
  ds.write(0xcc); //Skyp the ROM address
  delay(5);   
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  MSB = data[1];
  LSB = data[0];
  
  /* Handling the data in function of the sign -/+ */
  SignBit = (MSB & 0xf8);  // separete the t° info from sign info (the first 5 bits in MSB Byte)

    if(SignBit == 0xf8){ // Check if the sign is -
      MSB = (~(MSB));
      LSB = (~(LSB));
      tempRead = ((MSB << 8) | LSB); //using two's compliment
      TemperatureSum = (tempRead / (16.0))*(-1);
    }
    else{  // if the sign is +
      tempRead = ((MSB << 8) | LSB); //using two's compliment
      TemperatureSum = tempRead / (16.0);
    }
  rec=0;  
  return TemperatureSum;
}
