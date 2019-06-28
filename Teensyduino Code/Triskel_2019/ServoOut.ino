void InitESC() { //Activate the ESC
  rcservoout.writeMicroseconds(2000); //1000 = 1ms. Change it to 2000 if your ESC is activated with a 2ms pulse.
  delay(3000); //Esperar 5 segundos para hacer la activacion
  rcservoout.writeMicroseconds(1000); //1000 = 1ms. Change it to 2000 if your ESC is activated with a 2ms pulse.
  delay(3000); //Esperar 5 segundos para hacer la activacion
}

void MPPT() {
  mpptSetPoint = map(channels[4], 172, 1811, 0, 25);    // scaled to set a voltage value between 0v & 25v
  mpptConsKp = mapf(channels[5], 172, 1811, 0, 1);    // scaled  in to get a value between 0&100 and then /100 to get a decimal between 0 & 1
  mpptAggKp = mpptConsKp;
  int throttle = map(channels[1], 172, 1811, 1000, 2000);     // scaled to send a pulse between 1us & 2us
  Serial.print("throttle: ");
  Serial.print(throttle);
  mpptInput = volts;
  double mpptGap = abs(mpptSetPoint - mpptInput); //distance away from setpoint

  if (mpptGap > 0) {
    if (mpptGap < 1 ) {
      mpptPID.SetTunings(mpptConsKp, mpptConsKi, mpptConsKd); //we're close to setpoint, use conservative tuning parameters
    } else {
      mpptPID.SetTunings(mpptAggKp, mpptAggKi, mpptAggKd);    //we're far from setpoint, use aggressive tuning parameters
    }
    mpptPID.Compute();
    //throttle = throttle - (mpptOutput*100);
    throttle = constrain(throttle - (mpptOutput * 100), 1000, 2000);
  }
  Serial.print(",");
  Serial.print(mpptConsKp);
  Serial.print(", mpptIn: ");
  Serial.print(mpptInput);
  Serial.print(", SetPoint: ");
  Serial.print(mpptSetPoint);
  Serial.print(", Output: ");
  Serial.print(mpptOutput);
  Serial.print(", Modified throttle: ");
  Serial.println(throttle);

  rcservoout.writeMicroseconds(throttle);
}
