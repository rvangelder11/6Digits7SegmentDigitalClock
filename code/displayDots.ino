void displayDots(CRGB color, int secondHand, boolean onepoint) {
  if (dotsOn) {
    if (!onepoint) {
      LEDs[42] = ColorFromPalette(RainbowColors_p, 42 + 3 * secondHand, brightness, LINEARBLEND);
      LEDs[43] = ColorFromPalette(RainbowColors_p, 43 + 3 * secondHand, brightness, LINEARBLEND);
      LEDs[86] = ColorFromPalette(RainbowColors_p, 86 + 3 * secondHand, brightness, LINEARBLEND);
      LEDs[87] = ColorFromPalette(RainbowColors_p, 87 + 3 * secondHand, brightness, LINEARBLEND);
    }
    else{
      LEDs[42] = CRGB::Black;
      LEDs[43] = CRGB::Black;
      LEDs[86] = ColorFromPalette(RainbowColors_p, 86 + 3 * secondHand, brightness, LINEARBLEND);
      LEDs[87] = CRGB::Black;
    }
  } else {
    LEDs[42] = CRGB::Black;
    LEDs[43] = CRGB::Black;
    LEDs[86] = CRGB::Black;
    LEDs[87] = CRGB::Black;
  }

  //dotsOn = !dotsOn;   // knipper aan/uit
}
