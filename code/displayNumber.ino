void displayNumber(byte number, byte segment, CRGB color, int secondHand) {
  /*

       __ __ __        __ __ __         __ __ __        __ __ __          __ __ __        12 13 14
     __        __    __        __     __        __    __        __      __        __    11        15
     __        __    __        __     __        __    __        __      __        __    10        16
     __        __    __        __ 86  __        __    __        __  42  __        __    _9        17
       __ __ __        __ __ __         __ __ __        __ __ __          __ __ __        20 19 18
     __        109   __        88 87  __        65    __        44  43  __        21    _8        _0
     __        __    __        __     __        __    __        __      __        __    _7        _1
     __        __    __        __     __        __    __        __      __        __    _6        _2
       __ __ __        __ __ __         __ __ __        __ __ __           __ __ __       _5 _4 _3

  */
  //Serial.print(number);
  //if (segment == 0) Serial.println();
  // segment from left to right: 5, 4, 3, 2, 1, 0
  byte startindex = 0;
  switch (segment) {
    case 0:
      startindex = 0;
      break;
    case 1:
      startindex = 21;
      break;
    case 2:
      startindex = 44;
      break;
    case 3:
      startindex = 65;
      break;
    case 4:
      startindex = 88;
      break;
    case 5:
      startindex = 109;
      break;
  }

  for (byte i = 0; i < 21; i++) {
    yield();
    color = ColorFromPalette(RainbowColors_p, i + startindex+secondHand*3, brightness, LINEARBLEND);
    LEDs[i + startindex] = ((numbers[number] & 1 << i) == 1 << i) ? color : alternateColor;
  }
}
