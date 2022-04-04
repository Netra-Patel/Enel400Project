
#include <ESP8266WiFi.h> 

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  //WiFi.hostname("ESP-host")
  WiFi.begin("TELUS46DA", "XK4sGDbashLh");             // Connect to the network
  Serial.print("Connecting to ");
  Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { 
    Serial.println("waiting");// Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}

void loop() { }
