void SendTelemetry() {
  fcs.setData(amps, volts);   // Temporally changed amps to mpptConsKp to calibrate PID
  rpm.setData(motorRpm,       // Rotations per minute
              (mpptConsKp*100), // Using Temp1 to calibrate the Kp parameter of the MPPT PID, since telem only sends integers
              //and the value is between 0 and 1, the param is multiplied by 100 to show two decimals.
              //solarPanelTemp, // Temperature #1 in degrees Celsuis (can be negative, will be rounded)
              mpptSetPoint);  // Using Temp2 to inform of the MPPT voltage set.

  telemetry.send();
}

void GetRPM() {
  static float sum1 = 0;
  static int count1 = 0;
  static elapsedMillis timeout;
  if (freq1.available()) {
    sum1 = sum1 + freq1.read();
    count1 = count1 + 1;
  }
  // print results every half second
  if (timeout > 500) {
    if (count1 > 0) {
      motorRpm = (freq1.countToFrequency(sum1 / count1)* 60);
    } else {
      motorRpm = 0.0;
    }
    sum1 = 0;
    count1 = 0;
    timeout = 0;
  }
}
