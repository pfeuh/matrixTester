
#define MATRIX_VERSION "1.00"

word oldPinState = 0xffff;
#define MAX_PINS_UNDER_TEST 32
int pinsUnderTest[MAX_PINS_UNDER_TEST];
byte nbPinsUnderTest = 0;

enum _state{configuring, testing} state = configuring;

word userWord = 0;

void configure()
{
    if(Serial.available())
    {
        char incomming_char = Serial.read();
        switch(incomming_char)
        {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                userWord = userWord * 10 + incomming_char - '0';
                break;
            case '\n':
                if(nbPinsUnderTest < MAX_PINS_UNDER_TEST)
                {
                    pinsUnderTest[nbPinsUnderTest++] = userWord;
                    pinMode(userWord, INPUT_PULLUP);
                    digitalWrite(userWord, 1);
                    Serial.print(F("Pin "));
                    Serial.print(userWord);
                    Serial.println(F(" added."));
                }
                userWord = 0;
                break;
            case '\r':
                break;
            default:
                state = testing;
                Serial.print(F("pins under test:"));
                for(byte x = 0; x < nbPinsUnderTest; x++)
                {
                    Serial.print(pinsUnderTest[x]);
                    Serial.write(' ');
                }
                Serial.write('\n');
                Serial.print(nbPinsUnderTest);
                Serial.println(F(" pins under test."));
                Serial.println(F("Process started! Hit keys to build the pinout."));
                break;
        }
    }
}

void test()
{
    word temp_state;
    for(byte x = 0; x < nbPinsUnderTest; x++)
    {
        pinMode(pinsUnderTest[x], OUTPUT);
        digitalWrite(pinsUnderTest[x], 0);
        for(byte y = 0; y < nbPinsUnderTest; y++)
        {
            if(y != x)
            {
                if(!digitalRead(pinsUnderTest[y]))
                {
                    if(x < y)
                        temp_state = ((word)x << 8) + y;
                    else
                        temp_state = ((word)y << 8) + x;
                    if(temp_state != oldPinState)
                    {
                        Serial.print(pinsUnderTest[temp_state >> 8]);
                        Serial.write(" <-> ");
                        Serial.print(pinsUnderTest[temp_state & 0xff]);
                        Serial.write(' ');
                        Serial.print(temp_state, HEX);
                        Serial.write('\n');
                        
                        oldPinState = temp_state;
                    }
                }
            }
        }
        digitalWrite(pinsUnderTest[x], 1);
        pinMode(pinsUnderTest[x], INPUT_PULLUP);
    }
}

void setup()
{
    Serial.begin(9600);
    Serial.println(F("Decode the Matrix v. " MATRIX_VERSION));
    Serial.println(F("Build " __DATE__ " " __TIME__));
    Serial.println(F("For each pin you want to test,"));
    Serial.println(F("please enter the pin number + RETURN."));
    Serial.println(F("Then enter any other key + RETURN to start the test"));

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    switch(state)
    {
        case configuring:
            configure();
            break;
        case testing:
            test();
            break;
    }

        // let's blink status led
    if(millis() & 0x200)
        digitalWrite(LED_BUILTIN, 0);
    else
        digitalWrite(LED_BUILTIN, 1);
}

