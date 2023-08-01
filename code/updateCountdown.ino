void updateCountdown() {

  if (countdownMilliSeconds == 0 && endCountDownMillis == 0)
    return;

  unsigned long restMillis = endCountDownMillis - millis();
  unsigned long hours = ((restMillis / 1000) / 60) / 60;
  unsigned long minutes = (restMillis / 1000) / 60;
  unsigned long seconds = restMillis / 1000;
  int remSeconds = seconds - (minutes * 60);
  int remMinutes = minutes - (hours * 60);

  Serial.print(restMillis);
  Serial.print(" ");
  Serial.print(hours);
  Serial.print(" ");
  Serial.print(minutes);
  Serial.print(" ");
  Serial.print(seconds);
  Serial.print(" | ");
  Serial.print(remMinutes);
  Serial.print(" ");
  Serial.println(remSeconds);

  byte h1 = hours / 10;
  byte h2 = hours % 10;
  byte m1 = remMinutes / 10;
  byte m2 = remMinutes % 10;
  byte s1 = remSeconds / 10;
  byte s2 = remSeconds % 10;

  CRGB color = countdownColor;
  if (restMillis <= 60000) {
    color = CRGB::Red;
  }

  
    // hh:mm:ss
    displayNumber(h1, 5, color,0);
    displayNumber(h2, 4, color,0);
    displayNumber(m1, 3, color,0);
    displayNumber(m2, 2, color,0);
    displayNumber(s1, 1, color,0);
    displayNumber(s2, 0, color,0);
  

  displayDots(color,0,false);

  if (hours <= 0 && remMinutes <= 0 && remSeconds <= 0) {
    Serial.println("Countdown timer ended.");
    //endCountdown();
    countdownMilliSeconds = 0;
    endCountDownMillis = 0;
    digitalWrite(COUNTDOWN_OUTPUT, HIGH);
    return;
  }
}
