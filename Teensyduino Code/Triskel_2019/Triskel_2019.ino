//*************************************  RC Solar boat by J. Alberto Babío Pérez  ************************************************************//
//  V1.0 - This is a work in progress project.                                                                                               //
//  All the code is licensed under GPL V3 or later.                                                                                           //
//  The Copyright of the included libraries, belongs to their respective authors.                                                             //
//                                                                                                                                            //
//                                                                                                                                            //
//  Libraries:                                                                                                                                //
//    FreqMeasureMulti.h  https://github.com/PaulStoffregen/FreqMeasureMulti                                                                  //
//    Encoder.h           https://www.pjrc.com/teensy/td_libs_Encoder.html                                                                    //
//                        https://github.com/PaulStoffregen/Encoder                                                                           //
//    OneWire.h           https://www.pjrc.com/teensy/td_libs_OneWire.html                                                                    //
//                        https://github.com/PaulStoffregen/OneWire                                                                           //
//    SBUS.h              https://github.com/bolderflight/SBUS                                                                                //
//    FrSky Telemetry     https://www.rcgroups.com/forums/showthread.php?2245978-FrSky-S-Port-telemetry-library-easy-to-use-and-configurable  //
//    SparkFun_TB6612.h   https://github.com/sparkfun/SparkFun_TB6612FNG_Arduino_Library/blob/master/src/SparkFun_TB6612.h                    //
//    PID_v1.h            https://playground.arduino.cc/Code/PIDLibrary/                                                                      //
//********************************************************************************************************************************************//

// FRSKI Telemetry
#include "FrSkySportSensor.h"
#include "FrSkySportSensorFcs.h"    // Current / Voltage
#include "FrSkySportSensorRpm.h"    // RPM/temperature
#include "FrSkySportSingleWireSerial.h"
#include "FrSkySportTelemetry.h"

#include <SparkFun_TB6612.h>
#include <PID_v1.h>

//#include <EEPROM.h>
#include <OneWire.h>
#include <Encoder.h>
#include <SBUS.h>
#include <Servo.h>
#include <FreqMeasureMulti.h>
#include "HAL.h"

//Function declaration
void PowerMonitor();
void ReadSbusIn();
void MPPT();
void ReadEncoder();
void CenterTails();
void TriggerCenterTails();
void SteerTails();
void SendTelemetry();
void GetRPM();
float mapf();
//void GetSolarPanelTemp();

// Tail Centering
long positionLeft  = -999;
long positionRight = -999;


float solarPanelTemp = 0;
float amps = 0;
float volts = 0;
int motorRpm = 0;

// PID CONFIGURATION
// Direction control PID
double dirSetPoint, m1Input, m1Output, m2Input, m2Output ;
double dirAggKp=1, dirAggKi=0.00, dirAggKd=0.00;        //Define the aggressive Tuning Parameters
double dirConsKp=1, dirConsKi=0.00, dirConsKd=0.00;     //Define the conservative Tuning Parameters

// MPPT control PID
double mpptSetPoint, mpptInput, mpptOutput;
double mpptAggKp=1, mpptAggKi=0.00, mpptAggKd=0.00;     //Define the aggressive Tuning Parameters
double mpptConsKp=1, mpptConsKi=0.00, mpptConsKd=0.00;  //Define the conservative Tuning Parameters

//Specify the links and initial tuning parameters
PID motor1PID(&m1Input, &m1Output, &dirSetPoint, dirConsKp, dirConsKi, dirConsKd, REVERSE);     //Left direction motor PID
PID motor2PID(&m2Input, &m2Output, &dirSetPoint, dirConsKp, dirConsKi, dirConsKd, DIRECT);      //Left direction motor PID
PID mpptPID(&mpptInput, &mpptOutput, &mpptSetPoint, mpptConsKp, mpptConsKi, mpptConsKd, DIRECT);//MPPT PID


void setup() {
  Serial.begin(115200);
  InitESC();
  freq1.begin(tachoInPin);
  x8r.begin();
  rcservoout.attach(pwmOutPin,1000,2000);
  pinMode(leftTailCenterPin, INPUT);
  pinMode(rightTailCenterPin, INPUT);

  //PID
  motor1PID.SetMode(AUTOMATIC);
  motor1PID.SetSampleTime(1);
  motor1PID.SetOutputLimits(-255, 255);
  motor2PID.SetMode(AUTOMATIC);
  motor2PID.SetSampleTime(1);
  motor2PID.SetOutputLimits(-255, 255);
  mpptPID.SetMode(AUTOMATIC);
  mpptPID.SetSampleTime(1);
  mpptPID.SetOutputLimits(0, 1000);

  pinMode(led, OUTPUT);
  //CenterTails();
  Serial2.setTX(31); //Use the alternate Serial2 TX Port
  Serial2.setRX(26); //Use the alternate Serial2 RX Port
  telemetry.begin(FrSkySportSingleWireSerial::SERIAL_2, &fcs, &rpm);
  CenterTails();
}

void loop()
{
  ReadSbusIn();
  TriggerCenterTails(); // Center the tails with a trigger switch during operation.
  GetSolarPanelTemp();
  ReadEncoder();
  SteerTails();
  ReadSbusIn();
  MPPT();
  PowerMonitor();
  GetRPM();
  SendTelemetry();
  

}

float mapf(float value, float istart, float istop, float ostart, float ostop) {
return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}
