#include <ArduinoJson.h>
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "HUAWEI-B525-4401";
char pass[] = "YMHNF7NDQDL";

const char* resource = "/bracers";
const char* server = "shrouded-spire-62169.herokuapp.com";

int status = WL_IDLE_STATUS;
WiFiClient client;

const unsigned long HTTP_TIMEOUT = 10000;  // max respone time from server
const size_t MAX_CONTENT_SIZE = 512;       // max size of the HTTP response

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
  Serial.begin(9600);
  while(!Serial) {
  }
  Serial.println("Serial ready");
 while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  delay(1000);
}

// Send the HTTP POST request to the server
bool sendRequest(const char* host, const char* resource){
  // Reserve memory space for your JSON data
  StaticJsonBuffer<200> jsonBuffer;
  
  // Build your own object tree in memory to store the data you want to send in the request
  JsonObject& root = jsonBuffer.createObject();
  root["sensor"] = "dht11";
  
  JsonObject& data = root.createNestedObject("data");
  // data.set("id": "2", "temperature": "100", "type": "point", "coordinates": [100,100], "status": true, "oxygen": 0);
  data.set("id", "2");
  data.set("temperature", "100");
  data.set("type", "point");
  data.set("coordinates", "[100,100]");
  data.set("status", true);
  data.set("oxygen", 0);
  
  // Generate the JSON string
  root.printTo(Serial);
  
  Serial.print("POST ");
  Serial.println(resource);

  client.print("POST ");
  client.print(resource);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.print(root.measureLength());
  client.print("\r\n");
  client.println();
  root.printTo(client);

  return true;
}

// Skip HTTP headers so that we are at the beginning of the response's body
bool skipResponseHeaders() {
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);

  if(!ok) {
    Serial.println("No response or invalid response!");
  }
  return ok;
}

// Close the connection with the HTTP server
void disconnect() {
  Serial.println("Disconnect");
  client.stop();
}

// Pause for a 1 minute
void wait() {
  Serial.println("Wait 60 seconds");
  delay(60000);
}

// Loop this thing
void loop() {
  if(client.connect(server, 443)) {
    if(sendRequest(server, resource) && skipResponseHeaders()) {
      Serial.print("HTTP POST request finished.");
    }
  }
  disconnect();
  wait();
}
