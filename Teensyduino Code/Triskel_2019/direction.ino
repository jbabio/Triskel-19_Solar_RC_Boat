void ReadEncoder() {
  long newLeft, newRight;
  newLeft = leftTailEncoder.read();
  newRight = rightTailEncoder.read();
  if (newLeft != positionLeft || newRight != positionRight ) {
    positionLeft = newLeft;
    positionRight  = newRight;
  }
}

void CenterTails() {
  long calStartMillis = millis();       // Millis at calibration start.
  long calTimeout = 20000;      // Calibration timeout (milliseconds)
  digitalWrite(led, HIGH);              // Turn on led to indicate calibration mode
  Serial.println("Homing tails...");
  bool tailsCentered = LOW;      // Flag to escape centering routines.
  // Left variables
  bool leftTailCentered = LOW;
  long leftCenter = 0;
  long leftCenterIn = 0;
  long leftCenterOut = 0;
  //Right variables
  bool rightTailCentered = LOW;
  long rightCenter = 0;
  long rightCenterIn = 0;
  long rightCenterOut = 0;
  // Move the tails off center to allow a proper calibration
  while (digitalRead(leftTailCenterPin) == HIGH || digitalRead(rightTailCenterPin) == HIGH) {
    telemetry.send();                   // Keep sending telemetry inside the while
    motor1.drive(255);
    motor2.drive(255);
  }
  motor1.drive(0);
  motor1.drive(0);
  while (tailsCentered == LOW) {        //loop until the tails are homed.

    ReadEncoder();                      // Keep reading the encoders inside the while
    telemetry.send();                   // Keep sending telemetry inside the while
    bool leftTailCenterPinStatus = digitalRead(leftTailCenterPin);
    bool rightTailCenterPinStatus = digitalRead(rightTailCenterPin);
    if (leftTailCentered == LOW) {
      if (leftTailCenterPinStatus == LOW && leftCenterIn == 0) { //Center left tail
        motor1.drive(-255);
      } else if (leftTailCenterPinStatus == HIGH && leftCenterIn == 0) {
        leftCenterIn = positionLeft;
      } else if (leftTailCenterPinStatus == LOW && leftCenterIn != 0 && leftCenterOut == 0) {
        leftCenterOut = positionLeft;
      }
      if (leftCenterIn != 0 && leftCenterOut != 0) {
        leftCenter = leftCenterIn + ((leftCenterOut + (-leftCenterIn)) / 2);
        motor1.drive(200);
        if (positionLeft == leftCenter) {
          leftTailEncoder.write(0);
          positionLeft  = 0;
          motor1.drive(0);
          leftTailCentered = HIGH;
        }
      }
    }
    if (rightTailCentered == LOW) {
      if (rightTailCenterPinStatus == LOW && rightCenterIn == 0) { //Center right tail
        motor2.drive(-255);
      } else if (rightTailCenterPinStatus == HIGH && rightCenterIn == 0) {
        rightCenterIn = positionRight ;
      } else if (rightTailCenterPinStatus == LOW && rightCenterIn != 0 && rightCenterOut == 0) {
        rightCenterOut = positionRight  ;
      }
      if (rightCenterIn != 0 && rightCenterOut != 0) {
        rightCenter = rightCenterIn + ((rightCenterOut + (-rightCenterIn)) / 2);
        motor2.drive(200);
        if (positionRight == rightCenter) {
          rightTailEncoder.write(0);
          positionRight  = 0;
          motor2.drive(0);
          rightTailCentered = HIGH;
        }
      }
    }
    if (rightTailCentered == HIGH && leftTailCentered == HIGH) {
      tailsCentered = HIGH;         // Mark the  tails as homed to exit the while.
      digitalWrite(led, LOW);       // Turn off led to indicate calibration has ended.
      Serial.println("Tails homed");
    }else if (millis() - calStartMillis > calTimeout){
      tailsCentered = HIGH;
    }
  }
}

void TriggerCenterTails(){ // Center the tails with a trigger switch
  static bool switchFallingEdge = LOW; // Flag to trigger tail centering in the falling edge of the switch to avoid mltiple calibrations
  if(channels[2] > 1600 && switchFallingEdge == LOW){
    switchFallingEdge = HIGH;
    Serial.println(channels[2]);
  }else if (channels[2] < 1600 && switchFallingEdge == HIGH){
    switchFallingEdge = LOW;
    CenterTails();
  }
}

void SteerTails() { //channels[3]
  
  if (channels[0] > 980 && channels[0] < 1020 || failSafe == HIGH){
    dirSetPoint = 0;
        digitalWrite(led, LOW);
  } else {
        digitalWrite(led, HIGH);
    dirSetPoint = map(channels[0], 172, 1811, -2680, 2680);     // scale it to use it with the servo (value between 0 and 180)
  }
  // Rotate Tails 180º to go backwards
  if (channels[3] > 1000 ) {
    dirSetPoint = dirSetPoint + 5360;
  }
  m1Input = -positionLeft; // Change the sign to the encoder reading to turn both tails in the same directon.
  double m1gap = abs(dirSetPoint - m1Input); //distance away from setpoint
  if (m1gap < 10)
  {
    motor1PID.SetTunings(dirConsKp, dirConsKi, dirConsKd); //we're close to setpoint, use conservative tuning parameters
  } else {
    motor1PID.SetTunings(dirAggKp, dirAggKi, dirAggKd);    //we're far from setpoint, use aggressive tuning parameters
  }
  motor1PID.Compute();
  if (m1Output > -100 && m1Output < 100) { /// CREAR VARIABLE THRESHOLD???
    motor1.drive(0);
  } else {
    motor1.drive(-m1Output);
  }
  ///
  m2Input = positionRight; //map(positionRight, -2680, 2680, -255, 255);
  //constrain(m2Input, -2680, 2680);
  double m2gap = abs(dirSetPoint - m2Input); //distance away from setpoint
  if (m2gap < 10)
  {
    motor2PID.SetTunings(dirConsKp, dirConsKi, dirConsKd); //we're close to setpoint, use conservative tuning parameters
  } else {
    motor2PID.SetTunings(dirAggKp, dirAggKi, dirAggKd);    //we're far from setpoint, use aggressive tuning parameters
  }
  motor2PID.Compute();
  if (m2Output > -100 && m2Output < 100) {
    motor2.drive(0);
  } else {
    motor2.drive(m2Output);
  }
}
