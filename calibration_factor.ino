/*
 Calibration Technique
 By: Shan Yu Suen
*/

#include "HX711.h"

// Defines the pin numbers for the DOUT pin and SCK pin 
#define DOUT 5
#define CLK 6

// Create instance of HX711 amplifier to utilize functions
HX711 scale;

float calibration_factor = 11380; //worked with plastic piece and my water bottle (572 g, 1.26 lbs)

void setup() {
  Serial.begin(9600);
  while (!Serial) ; 
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  // Initialize the scale amplifier
  scale.begin(5, 6);
  //Serial.println("made it");
  scale.set_scale();
  //Serial.println("made it 2");
  scale.tare(); //Reset the scale to 0
  delay(2000);
  //Serial.println("made it 3");

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {
  //Adjust to this calibration factor
  scale.set_scale(calibration_factor); 

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 2);
  Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }
}
