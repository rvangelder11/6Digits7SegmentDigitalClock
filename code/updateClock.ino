void updateClock() {
  byte d0, d1, d2, d3, d4, d5;
  int hour = myTZ.hour();
  int mins = myTZ.minute();
  int secs = myTZ.second();
  CRGB color = CRGB(r_val, g_val, b_val);
  if (secs % 30 < 20) {
    if (hourFormat == 12 && hour > 12) hour = hour - 12;
    d5 = hour / 10;
    d4 = hour % 10;
    d3 = mins / 10;
    d2 = mins % 10;
    d1 = secs / 10;
    d0 = secs % 10;
    displayDots(color, secs, false);
  }
  else if (TemperatureSensor) {
    tempSensors.requestTemperatures();                // start new temperature measurement if there are DS sensors detected
    double temp = tempSensors.getTempCByIndex(0);     // read temperature
    Serial.println("temperature = " + String(temp));  // print temperature
    temp *= 100;
    int itemp = int(temp);
    d0 = 12; //Celcius
    d1 = 11; //degrees
    d2 = itemp % 10;
    itemp /= 10;
    d3 = itemp % 10;
    itemp /= 10;
    d4 = itemp % 10;
    itemp /= 10;
    d5 = itemp % 10;
    displayDots(color, secs, true);
  }

  if (d5 > 0)
    displayNumber(d5, 5, color, secs);
  else
    displayNumber(10, 5, color, secs); // Blank

  displayNumber(d4, 4, color, secs);
  displayNumber(d3, 3, color, secs);
  displayNumber(d2, 2, color, secs);
  displayNumber(d1, 1, color, secs);
  displayNumber(d0, 0, color, secs);

}
