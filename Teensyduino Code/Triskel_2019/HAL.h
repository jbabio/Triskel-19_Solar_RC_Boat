// Triskel MK1 board
// LED
// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0 & LC
const int led = 13;

// FRSKI Telemetry
FrSkySportSensorFcs fcs;          // Create FCS-40A sensor with default ID (use ID8 for FCS-150A)
FrSkySportSensorRpm rpm;          // Create RPM sensor with default ID
FrSkySportTelemetry telemetry;

//Power Monitor
const int panelVoltagePin = 20;
const int PanelCurrentPin = 21;

// S.Bus input
//sbusPin = 1;
SBUS x8r(Serial1);
// channel, fail safe, and lost frames data
uint16_t channels[16];
bool failSafe = 1; // Initialized to 1 to prevent issues if we turn on the remote after the boat
bool lostFrame;



// Tachometer
FreqMeasureMulti  freq1;
const int tachoInPin = 22;   // Right


// PWM output
const int pwmOutPin= 23;  // Left pin
Servo rcservoout;  // create servo object to control a servo

// DS18B20 SETUP
const int dsPin = 14; // on pin 26/A12/DAC (a 4.7K resistor is necessary)
OneWire ds(dsPin);

// Tail centering hall sensors
const int leftTailCenterPin = 17;
const int rightTailCenterPin = 16;

// Direction motor encoders
Encoder leftTailEncoder(9, 10);
Encoder rightTailEncoder(11, 12);

// TB6612FNG SETUP
const int STBY = 2;            //standby
//Motor1 (Left)
const int PWMA = 3;             //Speed control
const int AIN1 = 7;             //Direction
const int AIN2 = 8;             //Direction
//Motor2 (Right)
const int PWMB = 4;            //Speed control
const int BIN1 = 6;             //Direction
const int BIN2 = 5;            //Direction

// Motor direction
const int offsetA =  -1;     //Default motor direction, change values to make
const int offsetB =  -1;     //your motors run in the apropiate direction. Can be 1 or -1
// Initializing motors.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY); //motor1: Left tail direction motor
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY); //motor2: Right tail direction motor
