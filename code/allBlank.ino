void allBlank() {
  for (int i = 0; i < NUM_LEDS; i++) {
    LEDs[i] = CRGB::Black;
  }
  FastLED.show();
}
