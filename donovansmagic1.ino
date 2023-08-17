#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LEDS 6  // On Trinket or Gemma, suggest changing this to 1

// For the light patterns buttons
#define PIN_DECREMENT 5
#define PIN_INCREMENT 4
        
// Pines analógicos para el joystick
#define PIN_JOY_X A0
#define PIN_JOY_Y A1

// Valores mínimos y máximos de lectura analógica del joystick
#define JOY_MIN 0
#define JOY_MAX 1023

// Brightness Pot
#define BRIGHTNESS A2 // For the leds brightness

//BUTTON that turns on and off the LEDS
#define ON_Off_BUTTON 7

// How many NeoPixels are attached to the Arduino?
//#define NUMPIXELS 100  // Popular NeoPixel ring size
#define NUMPIXELS 20  // For testing

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, LEDS, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 100  // Time (in milliseconds) to pause between pixels

#define R 0
#define G 1
#define B 2

int myMatrix[10][10] = {
  {7, 7, 0, 0, 0, 0, 1, 1, 1, 1},
  {7, 7, 0, 0, 0, 0, 1, 1, 1, 1},
  {7, 7, -1, -1, -1, -1, -1, -1, 2, 2},
  {7, 7, -1, -1, -1, -1, -1, -1, 2, 2},
  {6, 6, -1, -1, -1, -1, -1, -1, 2, 2},
  {6, 6, -1, -1, -1, -1, -1, -1, 2, 2},
  {6, 6, -1, -1, -1, -1, -1, -1, 3, 3},
  {6, 6, -1, -1, -1, -1, -1, -1, 3, 3},
  {5, 5, 5, 5, 4, 4, 4, 4, 3, 3},
  {5, 5, 5, 5, 4, 4, 4, 4, 3, 3}
};

int colorIndex = 1;
const int numColors = 8;

int colors[numColors][3] = { { 255, 255, 255 },
                     		 { 255,   0,   0 },
		                     { 250,  67,   0 },
							 { 255, 204,   0 },
							 {   0, 255,   0 },
		  					 {   0,   0, 255 },
  							 { 101,   6, 174 },
  							 { 255,   0, 200 }
};

// Initialize to white
int red   = colors[colorIndex][R];
int green = colors[colorIndex][G];
int blue  = colors[colorIndex][B];


// Number of leds to turn on
//const int numIncrements = 9;
const int numIncrements = 5;
int increments[numIncrements] = {  
  1,  // 100 all the leds are on
  2,  // 50 every 2
  4,  // 25 every 4
  5,  // 20 every 5
  10//, // 10 every 10
//  20, //  5 every 20, 1, 21, 41, 61, 81
//                            25, //  4 only the first, 26, 51, and 76
//                            50, //  2 only the first and the 51
//                            100
                                }; // 1 only the first led, the rest are off
    
// Get the number to increment
int incrementIndex = 0;
int increment = increments[incrementIndex];
int buttonUPCurrState;        // stores the button current value
int buttonUPPrevState;        // stores the button previous value
int buttonDWCurrState;        // stores the button current value
int buttonDWPrevState;        // stores the button previous value


//On Off Button state
int OnOffButtonCurrentState = 0;            // stores the button current value
int OnOffButtonPreviousState = 0;         // stores the button previous value
bool toggle = false;

// Brightness parameters
int brightnessPotState  = 0;

// Joystick state paramters
int joystickXState  = 0;
int joystickYState  = 0;

// debounce
unsigned long lastDebounceTime = 0;   // the last time the output pin was toggled
unsigned long debounceDelay = 50;    //* the debounce time; increase if the output flickers

// The code starts here...
void setup() 
{
  Serial.begin(9600);
  // Start pixels
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)}
  // OnOff  button
  pinMode(ON_Off_BUTTON, INPUT_PULLUP); // From joystick remove pullup?
  // Increment buttons
  pinMode(PIN_INCREMENT, INPUT_PULLUP);
  pinMode(PIN_DECREMENT, INPUT_PULLUP);
  Serial.println ("Starting...");
  
  // Set all leds to off
  turnOnLeds(0, 0, 0, 1); // 0, 0, 0 means all leds in black ie. off
}

void loop() 
{ 
  // On/Off swith
  turnOnOffSwitch();
  
  // Brightness
  updateBrightness();
  
  // Patterns
  updateIncrements();
  
  // New color
  updateColor();
}

// Only for testing purposes... delete it or do not call it on productive
void printStatus()
{
  Serial.print("Turning Leds On to ");
  Serial.print(red);
  Serial.print(" ");
  Serial.print(green);
  Serial.print(" ");
  Serial.print(blue);
  Serial.print(" ");
  Serial.println();
}

// ON-OFF SWITCH Function
// This function turns on/off the leds and toggles true and false values of the main switch
void turnOnOffSwitch()
{
  OnOffButtonCurrentState = !digitalRead(ON_Off_BUTTON);
  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    //Serial.println(OnOffButtonCurrentState);
    if (OnOffButtonPreviousState != OnOffButtonCurrentState) 
    {
      lastDebounceTime = millis();

      if (OnOffButtonCurrentState == HIGH) // If push button is pressed
      {                                   	
       	toggle ^= true;
        Serial.print("toggle: ");
        Serial.println(toggle);
        
        if (toggle == true)
        {
          Serial.println("Turn all leds on with color, brightness and pattern"); 
          printStatus(); // For testing purposes only
          float alpha = getBrightness();
          turnOnLeds(red * alpha, green * alpha, blue * alpha, increment);
        }
        else
        {
          Serial.println("Turn all leds off");
          turnOnLeds(0, 0, 0, 1);
        }
      }
      OnOffButtonPreviousState = OnOffButtonCurrentState;
    }
  }
}

// Function that returns the current brightness pot value
float getBrightness()
{
  int brightnessPot = analogRead(BRIGHTNESS); // Get slider values
  return map(brightnessPot, 0, 1023, 40, 1) /100.0;
}

// Function to turn all the leds on/off according to a specific pattern
void turnOnLeds(int r_, int g_, int b_, int inc)
{
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for (int i = 0; i < NUMPIXELS; i+=inc) 
  {  
    pixels.setPixelColor(i, pixels.Color(r_, g_, b_));
  }
  pixels.show();    // Send the updated pixel colors to the hardware.
  delay(DELAYVAL);  // Pause before next pass through loop
}

// Function to update brightness in real-time
void updateBrightness()
{
  int brightnessPot = analogRead(BRIGHTNESS);
  if (brightnessPot != brightnessPotState)
  {
    if (toggle == true)
    {
    	float alpha = map(brightnessPot, 0, 1023, 40, 1) /100.0;
	    turnOnLeds(red * alpha, green * alpha, blue * alpha, increment);
    }
  }
}

// Auxiliar function to control the patterns increment index
void setIncrement(int incIndex)
{
  increment = increments[incIndex];
  Serial.print("Increment Index: ");
  Serial.println(incrementIndex);
}

// Function to update patterns in real-time
void updateIncrements()
{
  buttonUPCurrState = !digitalRead(PIN_INCREMENT);  // read button value from arduino
  buttonDWCurrState = !digitalRead(PIN_DECREMENT);  // read button value from arduino
  
  if(toggle == true)
  {
    if ((millis() - lastDebounceTime) > debounceDelay) 
    {	
      // Up control
      if (buttonUPPrevState != buttonUPCurrState) 
      {
        lastDebounceTime = millis();
        if (buttonUPCurrState == HIGH) // If push button is pressed
        {   
          if (incrementIndex < numIncrements-1)
          {
            incrementIndex++;
            setIncrement(incrementIndex);
            Serial.println("Up");
            turnOnLeds(0,0,0,1);
            float alpha = getBrightness();
          	turnOnLeds(red * alpha, green * alpha, blue * alpha, increment);
          }
        }
        buttonUPPrevState = buttonUPCurrState;
      }

      // Down control
      if (buttonDWPrevState != buttonDWCurrState) 
      {
        lastDebounceTime = millis();
        if (buttonDWCurrState == HIGH) // If push button is pressed
        {   
          if (incrementIndex > 0)
          {
            incrementIndex--;
            setIncrement(incrementIndex);
            Serial.println("Down");
            turnOnLeds(0,0,0,1);
            float alpha = getBrightness();
          	turnOnLeds(red * alpha, green * alpha, blue * alpha, increment);
          }
        }
        buttonDWPrevState = buttonDWCurrState;
      }
    }
  }
}

// Function to update colors in real-time from the joystick movement
void updateColor()
{
  int jx = analogRead(PIN_JOY_X); 
  int jy = analogRead(PIN_JOY_Y);
  
  if (toggle == true)
  {
    if (jx != joystickXState || jy != joystickYState)
    {
      int ix = map(jx, 0, 1023, 0, 9);  
      int iy = map(jy, 0, 1023, 0, 9);
      colorIndex = myMatrix[ix][iy];
      if (colorIndex != -1)
      {
	    red   = colors[colorIndex][R];
    	green = colors[colorIndex][G];
	    blue  = colors[colorIndex][B];
        
        //printStatus();
        float alpha = getBrightness();
	    turnOnLeds(red * alpha, green * alpha, blue * alpha, increment);
      }
    }
  } 
}