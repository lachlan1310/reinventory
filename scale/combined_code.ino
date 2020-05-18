/* Reinventory SmartScale using Arduino Nano 33 IoT, HX711, load cells, PushingBox,
and Google Sheets.

Created by Shan Yu Suen / https://github.com/shanyusuen/reinventory

*/

#include <SPI.h>
#include <WiFiNINA.h>
#include <Q2HX711.h>

//----------------------------------------------------------------------------//
// INITIALIZED VARIABLES FOR WEIGHT SENSING //
// Pinouts for amplifier 1
const byte DOUT1 = 5;
const byte CLK1 = 6;

// Pinouts for amplifier 2
const byte DOUT2 = 2;
const byte CLK2 = 3;

/*
Calibration is calculated linear function (y = mx + b). These values are used
to calculate the mass. Read link below for more:
https://makersportal.com/blog/2019/5/12/arduino-weighing-scale-with-load-cell-and-hx711
*/
float y = 566.99; // Calibrated mass to be added
long x1 = 0L;
long x0 = 0L;
float avg_size = 10.0; // Number of averages for each mass measurement

// Initialize and construct HX711 amplifier
Q2HX711 amplifier1(DOUT1, CLK1);
Q2HX711 amplifier2(DOUT2, CLK2);

//----------------------------------------------------------------------------//
// INITIALIZED VARIABLES FOR WIRELESS DATA TRANSFER //
// Website URL and device ID from Pushingbox for our Scenario
const char WEBSITE[] = "api.pushingbox.com";
const String dev_id = "v02C76557EFAA620";

// Name of WiFi network and WiFi password (change these accordingly)
const char* MY_SSID = "GTother";
const char* MY_PWD =  "GeorgeP@1927";

int status = WL_IDLE_STATUS;

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // Wait for serial port to connect.
      // Needed for native USB port only (includes Arduino Nano 33 IoT)
  }
  calibrate(amplifier1);



  //----------------------------------------------------------------------------//
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
  float weightData = weigh(amplifier1);

  Serial.println("\nSending Data to Server...");
  // if you get a connection, report back via serial:
  WiFiClient client;  //Instantiate WiFi object

  //API service using WiFi Client through PushingBox then relayed to Google
  if (client.connect(WEBSITE, 80))
    {
       delay(8000);

       // This is the request that pushes the data to the google sheet
       client.print("GET /pushingbox?devid=" + dev_id + "&weightData=" + (String) weightData);

       // HTTP 1.1 provides a persistent connection, allowing batched requests
       // or pipelined to an output buffer
       client.println(" HTTP/1.1");
       client.print("Host: ");
       client.println(WEBSITE);
       client.println("User-Agent: MKR1000/1.0");
       client.println();
       Serial.println("\nData Sent");
       delay(5000);
       client.stop();
    }
}

// Helper function to calibrate load cells
void calibrate(Q2HX711 amplifier) {
  for (int i = 0; i < int(avg_size); i++){
    delay(10);
    x0 += amplifier.read();
  }
  x0 /= long(avg_size);
  Serial.println("Add Calibrated Mass");
  // Calibration procedure (mass should be added equal to y)
  int i = 1;
  while(true){
    if (amplifier.read() < (x0 + 10000)){
    } else {
      i++;
      delay(2000);
      for (int j = 0; j < int(avg_size); j++){
        x1 += amplifier.read();
      }
      x1 /= long(avg_size);
      break;
    }
  }
  Serial.println("Calibration Complete");
}

// Helper function for weighing after calibration
float weigh(Q2HX711 amplifier) {
  delay(4000);
  long reading = 0;
  for (int j = 0; j < int(avg_size); j++){
    reading += amplifier.read();
  }
  reading /= long(avg_size);
  // calculating mass based on calibration and linear fit
  float ratio_1 = (float) (reading - x0);
  float ratio_2 = (float) (x1 - x0);
  float ratio = ratio_1 / ratio_2;
  float mass = y * ratio;
  float weight = mass * 0.00220462; // 1 gram = 0.00220462 lbs

  Serial.print("Raw: ");
  Serial.print(reading);
  Serial.print(", ");
  Serial.println(mass);
  Serial.print("Weight: ");
  Serial.println(weight);

  return weight;
}

// Helper function to print WiFi diagnostics
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
