#define S2  D1 /*Define S2 Pin Number of ESP32*/
#define S3 D0 /*Define S3 Pin Number of ESP32*/
#define sensorOut D3 /*Define Sensor Output Pin Number of ESP32*/

unsigned long delaytime = 20;
/*Define int variables*/
int Red = 0;
int Green = 0;
int Blue = 0;
int Frequency = 0;

void setup() {
  pinMode(S2, OUTPUT); /*Define S2 Pin as a OUTPUT*/
  pinMode(S3, OUTPUT); /*Define S3 Pin as a OUTPUT*/
  pinMode(sensorOut, INPUT); /*Define Sensor Output Pin as a INPUT*/
  Serial.begin(115200); /*Set the baudrate to 115200*/
  Serial.println("Time (ms) Red Green Blue"); // Print header for clarity
}

void loop() {
  Red = getRed();
  delay(delaytime); 
  Green = getGreen();
  delay(delaytime); 
  Blue = getBlue();
  delay(delaytime); 

  // Get the current time in milliseconds
  unsigned long currentTime = millis();

  // Print the time and the frequency values in a format suitable for the Serial Plotter
  Serial.print(Red);
  Serial.print(" ");
  Serial.print(Green);
  Serial.print(" ");
  Serial.println(Blue); // Use println to end the line for the Serial Plotter
}

int getRed() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  Frequency = pulseIn(sensorOut, LOW); /*Get the Red Color Frequency*/
  return Frequency;
}

int getGreen() {
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  Frequency = pulseIn(sensorOut, LOW); /*Get the Green Color Frequency*/
  return Frequency;
}

int getBlue() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  Frequency = pulseIn(sensorOut, LOW); /*Get the Blue Color Frequency*/
  return Frequency;
}