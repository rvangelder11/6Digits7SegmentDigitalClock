void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    int colorindex = 0;
    for( int i = 0; i < NUM_LEDS; ++i) {
        LEDs[i] = ColorFromPalette( RainbowColors_p, colorIndex, brightness, LINEARBLEND);
        colorIndex += 3;
    }
}
