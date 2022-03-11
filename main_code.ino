
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
#endif
//#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include "DHT.h"
#include <ESP8266WiFi.h> 


// Default Threshold Temperature Value
String inputMessage = "25.0";
String lastTemperature;
String enableArmChecked = "checked";
String inputMessage2 = "true";

// HTML web page to handle 2 input fields (threshold_input, enable_arm_input)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Temperature Threshold Output Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h2>DS18B20 Temperature</h2> 
  <h3>%TEMPERATURE% &deg;C</h3>
  <h2>ESP Arm Trigger</h2>
  <form action="/get">
    Temperature Threshold <input type="number" step="0.1" name="threshold_input" value="%THRESHOLD%" required><br>
    Arm Trigger <input type="checkbox" name="enable_arm_input" value="true" %ENABLE_ARM_INPUT%><br><br>
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

//
//void notFound(AsyncWebServerRequest *request) {
//  request->send(404, "text/plain", "Not found");
//}

//AsyncWebServer server(80);

WiFiServer server(80);


// Replaces placeholder with DS18B20 values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return lastTemperature;
  }
  else if(var == "THRESHOLD"){
    return inputMessage;
  }
  else if(var == "ENABLE_ARM_INPUT"){
    return enableArmChecked;
  }
  return String();
}

// Flag variable to keep track if triggers was activated or not
bool triggerActive = false;

const char* PARAM_INPUT_1 = "threshold_input";
const char* PARAM_INPUT_2 = "enable_arm_input";

// Interval between sensor readings. Learn more about ESP32 timers: https://RandomNerdTutorials.com/esp32-pir-motion-sensor-interrupts-timers/
unsigned long previousMillis = 0;     
const long interval = 5000;    

// GPIO where the output is connected to
#define DHTPIN D5 
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

// GPIO where the output is connected to
const int output = 2;






void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  Serial.println('\n');
  WiFi.begin("TELUS46DA", "XK4sGDbashLh");             // Connect to the network
  Serial.print("Connecting to Wifi");
  Serial.println(" ...");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { 
    Serial.println("waiting");                   // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); 
    Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}


  dht.begin(); //Sensor turn on

  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);

  Serial.println();
  Serial.print("ESP IP Address: http://");
  Serial.println(WiFi.localIP());



 // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Receive an HTTP GET request at <ESP_IP>/get?threshold_input=<inputMessage>&enable_arm_input=<inputMessage2>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET threshold_input value on <ESP_IP>/get?threshold_input=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      // GET enable_arm_input value on <ESP_IP>/get?enable_arm_input=<inputMessage2>
      if (request->hasParam(PARAM_INPUT_2)) {
        inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
        enableArmChecked = "checked";
      }
      else {
        inputMessage2 = "false";
        enableArmChecked = "";
      }
    }
    Serial.println(inputMessage);
    Serial.println(inputMessage2);
    request->send(200, "text/html", "HTTP GET request sent to your ESP.<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() { 
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    float t = dht.readTemperature();
    Serial.print(temperature);
    Serial.println(" *C");
    lastTemperature = String(temperature);
  
  // Check if temperature is above threshold and if it needs to trigger output
    if(temperature > inputMessage.toFloat() && inputMessage2 == "true" && !triggerActive){
      String message = String("Temperature above threshold. Current temperature: ") + 
                            String(temperature) + String("C");
      Serial.println(message);
      triggerActive = true;
      digitalWrite(output, HIGH);
    }
    // Check if temperature is below threshold and if it needs to trigger output
    else if((temperature < inputMessage.toFloat()) && inputMessage2 == "true" && triggerActive) {
      String message = String("Temperature below threshold. Current temperature: ") + 
                            String(temperature) + String(" C");
      Serial.println(message);
      triggerActive = false;
      digitalWrite(output, LOW);
    }
  }
}
  
  
  
  
  }
