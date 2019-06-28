void ReadSbusIn() {
  // look for a good SBUS packet from the receiver
  x8r.read(&channels[0], &failSafe, &lostFrame);
}
