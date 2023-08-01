void endCountdown() {
  allBlank();
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i > 0)
      LEDs[i - 1] = CRGB::Black;

    LEDs[i] = CRGB::Red;
    FastLED.show();
    delay(25);
  }
}
