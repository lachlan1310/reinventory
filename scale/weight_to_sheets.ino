#include <SPI.h>
#include <WiFiNINA.h>

const char WEBSITE[] = "api.pushingbox.com"; //Pushingbox API server
const String devid = "v02C76557EFAA620"; //Device ID on Pushingbox for our Scenario

const char* MY_SSID = "GTother";  // Name of WiFi
const char* MY_PWD =  "GeorgeP@1927"; // WiFi password

int status = WL_IDLE_STATUS;

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  
  // Check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  
  // Attempt to connect to Wifi network:
  while (status != WL_CONNECTED) 
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(MY_SSID);
    
    // Connect to WPA/WPA2 network.Change this line if using open/WEP network
    status = WiFi.begin(MY_SSID, MY_PWD);

    // wait 10 seconds for connection:
    delay(10000);
  }
  
  Serial.println("Connected to wifi.");
  printWifiStatus();
  
}

void loop() {
  delay(10000); // Wait between measurements.

  //prefer to use float, but package size or float conversion isnt working
  //will revise in future with a string fuction or float conversion function
  float weightData = 25.6;

  Serial.print("Weight: ");
  Serial.print(weightData);
  
Serial.println("\nSending Data to Server..."); 
  // if you get a connection, report back via serial:
WiFiClient client;  //Instantiate WiFi object

    //API service using WiFi Client through PushingBox then relayed to Google
    if (client.connect(WEBSITE, 80))
      { 
         delay(8000);
         
         // This is the request that pushes the data to the google sheet
         client.print("GET /pushingbox?devid=" + devid + "&weightData=" + (String) weightData);
         
         // HTTP 1.1 provides a persistent connection, allowing batched requests
         // or pipelined to an output buffer
         client.println(" HTTP/1.1"); 
         client.print("Host: ");
         client.println(WEBSITE);
         client.println("User-Agent: MKR1000/1.0");
         client.println();
         Serial.println("\nData Sent"); 
        
         delay(5000); // Allow delay to allow time for data to send.
         client.stop();  // Close connection.
      }
}

// Helper function to print WiFi information
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
