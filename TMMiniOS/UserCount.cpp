/*
    // user's timer trigger function
    //
    // In this function cannot use any function that will use the hardware signal "clock",
    // Such as:
    //  1. ClockHigh(),ClockLow(), ClockHighLow(),
    //  2. Any EEPROM functions.
    //  3. Any 5DigitLed functions.
    //  4. Any NVRAM function.
    //  5. Any RTC function.(GetTime(),SetTime(),GetDate(),SetDate())
    //
    // refer to demo9 for example code
*/

void UserCount(void)
{

}