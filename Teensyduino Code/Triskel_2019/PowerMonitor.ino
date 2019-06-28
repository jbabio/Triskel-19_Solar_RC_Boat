void PowerMonitor(){
  float vRaw = analogRead(panelVoltagePin);
  float aRaw = analogRead(PanelCurrentPin);
  //Conversion
  volts = vRaw / 19.97; //11,67v -> 233
  amps = map(aRaw, 0, 121, 0, 5);//0-121 fro 0-5amps with 45a attopilot
}
