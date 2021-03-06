
    
    

#include <ESP8266WiFi.h>
#include <E131.h>
#include <Wire.h> 


#include "SSD1306.h" // display 128x64
#include "OLEDDisplayUi.h"
 SSD1306  display(0x3c, 1, 3);


/*
const char ssid[] = "Robom";         // Replace with your SSID /
const char passphrase[] = "starbucksCOFFEE100";   // Replace with your WPA2 passphrase /
*/
const char ssid[] = "Saturel";         // Replace with your SSID /
const char passphrase[] = "saturel1203";   // Replace with your WPA2 passphrase /
/*
const char ssid[] = "SATUREL";         // Replace with your SSID /
const char passphrase[] = "4143020576571850";   // Replace with your WPA2 passphrase /
*/
 
E131 e131;  
    
    #include <Adafruit_NeoPixel.h>
    #define FASTLED_ALLOW_INTERRUPTS 0   //dolezite aby neblikali posledne
    #include <FastLED.h>
    #include "water_torture.h"
    #include <math.h>
    #include <SoftwareSerial.h>
    #define N_PIXELS   144 //52 // 144  // Number of pixels in strand
    #define N_PIXELS_HALF (N_PIXELS/2)
    #define MIC_PIN   A0  // Microphone is attached to this analog pin
    #define LED_PIN   12   // NeoPixel LED strand is connected to this pin GPIO 12 - D6  
    #define LED_PIN2   6   // FastLED ine oznacovane PINOV  D6 -  GPIO12
  //#define buttonPin  13 // the number of the pushbutton pin D7 - GPIO13
    #define buttonPin  14 // the number of the pushbutton pin D7 - GPIO13
 
    #define SAMPLE_WINDOW   10  // Sample window for average level
    #define PEAK_HANG 24 //Time of pause before peak dot falls
    #define PEAK_FALL 20 //Rate of falling peak dot
    #define PEAK_FALL2 8 //Rate of falling peak dot
    #define INPUT_FLOOR 10 //Lower range of analogRead input
    #define INPUT_CEILING 300 //Max range of analogRead input, the lower the value the more sensitive (1023 = max)300 (150)
    #define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
    #define NOISE     10  // Noise/hum/interference in mic signal
    #define SAMPLES   60  // Length of buffer for dynamic level adjustment
    #define TOP       (N_PIXELS + 2) // Allow dot to go slightly off scale
    #define SPEED .20       // Amount to increment RGB color by each cycle
    #define TOP2      (N_PIXELS + 1) // Allow dot to go slightly off scale
    #define LAST_PIXEL_OFFSET N_PIXELS-1
    #define PEAK_FALL_MILLIS 10  // Rate of peak falling dot
    //#define POT_PIN    2
    uint16_t pot = 0;
    #define BG 0
    #define LAST_PIXEL_OFFSET N_PIXELS-1
    #define SPARKING 50
    #define COOLING  55
    #define FRAMES_PER_SECOND 60
    #define NUM_BALLS         2                  // Number of bouncing balls you want (recommend < 7, but 20 is fun in its own way)
    #define GRAVITY           -9.81              // Downward (negative) acceleration of gravity in m/s^2
    #define h0                1                  // Starting height, in meters, of the ball (strip length)
    #if FASTLED_VERSION < 3001000
    #error "Requires FastLED 3.1 or later; check github for latest code."
    #endif
    #define BRIGHTNESS  255
    #define LED_TYPE    WS2812B     // Only use the LED_PIN for WS2812's
    #define COLOR_ORDER GRB
    #define COLOR_MIN           0
    #define COLOR_MAX         255
    #define DRAW_MAX          100
    #define SEGMENTS            4  // Number of segments to carve amplitude bar into
    #define COLOR_WAIT_CYCLES  10  // Loop cycles to wait between advancing pixel origin
    #define qsubd(x, b)  ((x>b)?b:0)     
    #define qsuba(x, b)  ((x>b)?x-b:0)                                              // Analog Unsigned subtraction macro. if result <0, then => 0. By Andrew Tuline.
    #define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
    



int16_t  audio;
uint32_t timer;
uint16_t cas;
uint16_t casE131;
uint16_t cas_print;
uint16_t cas_TL;
uint16_t cas_random = 100;
bool smer;
bool aut = true;
bool bit_TL;
bool prog;
uint16_t num_channels;
uint8_t skuska;
uint8_t pixel [N_PIXELS*3];
String IP_addr;
String program;
uint8_t program_E131;
bool hrana;
    
    //config for balls
    


float h[NUM_BALLS] ;                         // An array of heights
float vImpact0 = sqrt( -2 * GRAVITY * h0 );  // Impact velocity of the ball when it hits the ground if "dropped" from the top of the strip
float vImpact[NUM_BALLS] ;                   // As time goes on the impact velocity will change, so make an array to store those values
float tCycle[NUM_BALLS] ;                    // The time since the last time the ball struck the ground
int   pos[NUM_BALLS] ;                       // The integer position of the dot on the strip (LED index)
long  tLast[NUM_BALLS] ;                     // The clock time of the last ground strike
float COR[NUM_BALLS] ;                       // Coefficient of Restitution (bounce damping)
  
    struct CRGB leds[N_PIXELS];

    Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
 
    static uint16_t dist;         // A random number for noise generator.
    uint16_t scale = 30;          // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
    uint8_t maxChanges = 48;      // Value for blending between palettes.
 
    //CRGBPalette16 currentPalette(CRGB::Black);
    CRGBPalette16 currentPalette(OceanColors_p);
    CRGBPalette16 targetPalette(CloudColors_p);
// Water torture
WaterTorture water_torture = WaterTorture(&strip);

//new ripple vu
uint8_t timeval = 20;                                                           // Currently 'delay' value. No, I don't use delays, I use EVERY_N_MILLIS_I instead.
uint16_t loops = 0;                                                             // Our loops per second counter.
bool     samplepeak = 0;                                                        // This sample is well above the average, and is a 'peak'.
uint16_t oldsample = 0;                                                         // Previous sample is used for peak detection and for 'on the fly' values.
bool thisdir = 0;         
//new ripple vu

// Modes
enum 
{
} MODE;
bool reverze = true;
int BRIGHTNESS_MAX = 80;
int brightness = 20;


byte
//  peak      = 0,      // Used for falling dot
//  dotCount  = 0,      // Frame counter for delaying dot-falling speed
  volCount  = 0;      // Frame counter for storing past volume data
int
  reading,
  vol[SAMPLES],       // Collection of prior volume samples
  lvl       = 10,      // Current "dampened" audio level
  minLvlAvg = 0,      // For dynamic adjustment of graph low & high
  maxLvlAvg = 512;    
float
  greenOffset = 30,
  blueOffset = 150;
// cycle variables

int CYCLE_MIN_MILLIS = 2;
int CYCLE_MAX_MILLIS = 1000;
int cycleMillis = 20;
bool paused = false;
long lastTime = 0;
bool boring = true;
bool gReverseDirection = false;
int          myhue =   0;
//vu ripple
uint8_t colour; 
uint8_t myfade = 255;                                         // Starting brightness.
#define maxsteps 16                                           // Case statement wouldn't allow a variable.
int peakspersec = 0;
int peakcount = 0;
uint8_t bgcol = 0;   
int thisdelay = 20; 

// FOR SYLON ETC
uint8_t thisbeat =  23;
uint8_t thatbeat =  28;
uint8_t thisfade =   2;                                     // How quickly does it fade? Lower = slower fade rate.
uint8_t thissat = 255;                                     // The saturation, where 255 = brilliant colours.
uint8_t thisbri = 255; 

//FOR JUGGLE
uint8_t numdots = 4;                                          // Number of dots in use.
uint8_t faderate = 2;                                         // How long should the trails be. Very low value = longer trails.
uint8_t hueinc = 16;                                          // Incremental change in hue between each dot.
uint8_t thishue = 0;                                          // Starting hue.
uint8_t curhue = 0; 
uint8_t thisbright = 255;                                     // How bright should the LED/display be.
uint8_t basebeat = 5; 
uint8_t max_bright = 255;

// Twinkle
float redStates[N_PIXELS];
float blueStates[N_PIXELS];
float greenStates[N_PIXELS];
float Fade = 0.96;
unsigned int sample;

//Samples
#define NSAMPLES 64
unsigned int samplearray[NSAMPLES];
unsigned long samplesum = 0;
unsigned int sampleavg = 0;
int samplecount = 0;
//unsigned int sample = 0;
unsigned long oldtime = 0;
unsigned long newtime = 0;

//Ripple variables
int color;
int center = 0;
int step = -1;
int maxSteps = 16;
float fadeRate = 0.80;
int diff;

//vu 8 variables
int
  origin = 0,
  color_wait_count = 0,
  scroll_color = COLOR_MIN,
  last_intensity = 0,
  intensity_max = 0,
  origin_at_flip = 0;
uint32_t
    draw[DRAW_MAX];
boolean
  growing = false,
  fall_from_left = true;



//background color
uint32_t currentBg = random(256);
uint32_t nextBg = currentBg;
//CRGBPalette16 currentPalette;
//CRGBPalette16 targetPalette;
TBlendType    currentBlending;  
     


 //Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int pamatPushCounter = 55; 
int buttonState = 0;         // current state of the button
int lastButtonState = 0;

    
    byte peak = 16;      // Peak level of column; used for falling dots
//    unsigned int sample;
     
    byte dotCount = 0;  //Frame counter for peak dot
    byte dotHangCount = 0; //Frame counter for holding peak dot
     
void setup() {

      strip.begin();
      strip.show(); // all pixels to 'off'
       Wire.pins(1,3);    //GPIO  SDA  GPIO    SCL    Tx 1    Rx 3
       Wire.begin(1, 3);
       display.init();
       display.flipScreenVertically();
       display.setFont(ArialMT_Plain_10);
      //-------------------------------------------------------------------------
      program="inicializacia";

      text (program);  // text na terminal
     
    e131.beginMulticast(ssid, passphrase, 2); // via Multicast for Universe 2 
  
 
  
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  //  Serial.print(".");
    tries++;
    if (tries > 15) {
      break;
    }
  }
  IP_addr = WiFi.localIP().toString();   //prevod IP na string
    delay(500);
  
//-------------------------------------------------------------------------
program="start";
text (program);  // text na terminal


 for (int i = 0 ; i < NUM_BALLS ; i++) {    // Initialize variables
    tLast[i] = millis();
    h[i] = h0;
    pos[i] = 0;                              // Balls start on the ground
    vImpact[i] = vImpact0;                   // And "pop" up at vImpact0
    tCycle[i] = 0;
    COR[i] = 0.90 - float(i)/pow(NUM_BALLS,2);  
  
  }


//-------------------------    
pamatPushCounter = 55;
buttonPushCounter=1;

    // analogReference(EXTERNAL);
   aut=1;     
    pinMode(buttonPin, INPUT); 
    pinMode(2, OUTPUT);  
  //initialize the buttonPin as output
    pinMode(buttonPin,INPUT_PULLUP); 
     
  //Serial.begin(115200);
  //    strip.begin();
  //    strip.show(); // all pixels to 'off'

      //Serial.begin(57600);
      delay(100);
 
  LEDS.addLeds<LED_TYPE,LED_PIN2,COLOR_ORDER>(leds,N_PIXELS).setCorrection(TypicalLEDStrip); 
  LEDS.setBrightness(BRIGHTNESS);
  dist = random16(12345);          // A semi-random number for our noise generator

 
}

float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;
  


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  
  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

 
  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  { 
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
  
  }
    
  void loop() {
  
  //--------------------   E1.31 ---------------------------------
  delay(5);
   num_channels = e131.parsePacket();
   // Process channel data if we have it 
   
    if (num_channels) {casE131=0; prog=true; 
      
         if ( e131.data[511]==0 )
        {
         for (int i=0; i<N_PIXELS*3;i+=3)
          {
          strip.setPixelColor(i/3, strip.Color(e131.data[i],e131.data[i+1],e131.data[i+2])); 
          }
       
        strip.show();
        } 
        program_E131=e131.data[511];
      } 
   if (casE131>50) {prog=false;program_E131=0;}   
//---------------------------------------------------------------

 // Feed the data to the NeoPixel library

   
 /*   for (i=0;i<strip.numPixels();i++) {
      R[i]=(strip.getPixelColor(i) >> 16) & 0x7f;
      G[i]=(strip.getPixelColor(i) >>  8) & 0x7f;
      B[i]=(strip.getPixelColor(i)      ) & 0x7f;
    }
   */

/*   if (cas_print>=10) {cas_print=0;
  Serial.print("program:  ");
  Serial.println (buttonPushCounter);
  Serial.print("cas: ");
  Serial.println (cas);
   Serial.print("data1: ");
  Serial.println (skuska);
  
  }
*/  

   
   //for mic
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;
  // end mic
 
 //---------------------------------------------------------------
 if (millis() - timer >= 100) {
    timer = millis();  //cas 10ms
    cas++;cas_print++;cas_TL++;casE131++;
      }
 //-----------------------------------------------------------------



 
if (prog==false ||  program_E131>0)
{
 if (aut) {  
  if (cas>=50 && prog==false ) {cas=0; buttonPushCounter++;digitalWrite(2,!digitalRead(2));} 
  if (buttonPushCounter>24) {buttonPushCounter=1;}
 }
 if   (prog==false)
 {
  if (digitalRead(buttonPin)==false && bit_TL==false ) {
      buttonPushCounter++;aut=false;bit_TL=true;
      if (buttonPushCounter>24) {buttonPushCounter=1;}
      }
  if (digitalRead(buttonPin)==false) {cas_TL=0;} 
  if (cas_TL>=3) {bit_TL=false;} 
  
}
 if  (program_E131>0) {buttonPushCounter=program_E131;}
 pot=0;
 audio= map (analogRead(MIC_PIN),0,1024,512,0);
  
if (buttonPushCounter<25) {hrana=true;}

switch (buttonPushCounter){
      
     case 1: vu();program="VU";break;     
     case 2: vu1();program="VU1";break;
     case 3: vu2();program="VU2";break;
     case 4: Vu3();program="VU3";break;
     case 5: Vu4();program="VU4";break;  
     case 6: Vu5();program="VU5";break; 
     case 7: Vu6();program="VU6";break;
     case 8: vu7();program="VU7";break;
     case 9: vu8();program="VU8";break;
     case 10: vu9();program="VU9";break;
     case 11: vu10();program="VU10";break;
     case 12: vu11();program="VU11";break;
     case 13: vu12();program="VU12";break;
     case 14: vu13();program="VU13";break; 
     
     case 15: ripple();program="ripple";break; 
     case 16: ripple2();program="ripple2";break; 
     case 17: Twinkle();program="twinkle";break; 
     case 18:  pattern2();program="patern2";break; 
     case 19:  pattern3();program="patern3";break; 
     case 20: juggle2();program="juggle2";break; 
     case 21: blur();program="blur";break; 
     case 22: Drip();program="drip";break;
     case 23: fireblu();program="fireblu";break;
     case 24: fire();program="fire";break;
     case 25: colorWipe(strip.Color(0, 0, 0));  break; 
 //    case 24: Balls();program="balls";break; 
//     case 24: rainbow(5);program="rainbow";break;
//   colorWipe(strip.Color(0, 0, 0), 10);  break;}      
 } 
}
//if (bit_TL==true) {text(program);}
if (pamatPushCounter!=buttonPushCounter) {text(program);}



}   //end loop
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------


void colorWipe(uint32_t c) {
 if (hrana){
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
     }
   strip.show();  
 hrana=false;}
}     

void vu() {
 
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;
  
  n   = audio;                        // Raw reading from mic 
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)
 
  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);
 
  if(height < 0L)       height = 0;      // Clip output
  else if(height > TOP) height = TOP;
  if(height > peak)     peak   = height; // Keep 'peak' dot at top
 
 
  // Color pixels based on rainbow gradient
  for(i=0; i<N_PIXELS; i++) {
    if(i >= height)               strip.setPixelColor(i,   0,   0, 0);
    else strip.setPixelColor(i,Wheel(map(i,0,strip.numPixels()-1,30,150)));
    }
   
 
  // Draw peak dot  
  if(peak > 0 && peak <= N_PIXELS-1) strip.setPixelColor(peak,Wheel(map(peak,0,strip.numPixels()-1,30,150)));


   strip.show(); // Update strip
   num_channels = e131.parsePacket(); 


// Every few frames, make the peak pixel drop by 1:
 
    if(++dotCount >= PEAK_FALL) { //fall rate 
      
      if(peak > 0) peak--;
      dotCount = 0;
    } 
 
 
  vol[volCount] = n;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter
 
  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)
 
}
 
// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


void vu1() {
  
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;
 
 
 
  n   = audio;                        // Raw reading from mic 
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)
 
  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);
 
  if(height < 0L)       height = 0;      // Clip output
  else if(height > TOP) height = TOP;
  if(height > peak)     peak   = height; // Keep 'peak' dot at top
 
 
  // Color pixels based on rainbow gradient
  for(i=0; i<N_PIXELS_HALF; i++) {
    if(i >= height) {              
      strip.setPixelColor(N_PIXELS_HALF-i-1,   0,   0, 0);
      strip.setPixelColor(N_PIXELS_HALF+i,   0,   0, 0);
    }
    else {
      uint32_t color = Wheel(map(i,0,N_PIXELS_HALF-1,30,150));
      strip.setPixelColor(N_PIXELS_HALF-i-1,color);
      strip.setPixelColor(N_PIXELS_HALF+i,color);
    }
    
  } 
 
  // Draw peak dot  
  if(peak > 0 && peak <= N_PIXELS_HALF-1) {
    uint32_t color = Wheel(map(peak,0,N_PIXELS_HALF-1,30,150));
    strip.setPixelColor(N_PIXELS_HALF-peak-1,color);
    strip.setPixelColor(N_PIXELS_HALF+peak,color);
  }

  
  
 
   strip.show(); // Update strip
   num_channels = e131.parsePacket(); 

  
// Every few frames, make the peak pixel drop by 1:
 
    if(++dotCount >= PEAK_FALL) { //fall rate 
      
      if(peak > 0) peak--;
      dotCount = 0;
    }
 
 
 
  vol[volCount] = n;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter
 
  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)
 
}



void vu2() 
    {
      unsigned long startMillis= millis();  // Start of sample window
      float peakToPeak = 0;   // peak-to-peak level
     
      unsigned int signalMax = 0;
      unsigned int signalMin = 512;
      unsigned int c, y;
      
      while (millis() - startMillis < SAMPLE_WINDOW)
      {
        sample = audio;
        peakToPeak =512-audio;
       
      }

    
      
      for (int i=0;i<=N_PIXELS_HALF-1;i++){
        uint32_t color = Wheel(map(i,0,N_PIXELS_HALF-1,30,150));
        strip.setPixelColor(N_PIXELS-i,color);
        strip.setPixelColor(0+i,color);
      }

      
      c = fscale(INPUT_FLOOR, INPUT_CEILING, N_PIXELS_HALF, 0, peakToPeak, 2);
     
      if(c < peak) {
        peak = c;        // Keep dot on top
        dotHangCount = 0;    // make the dot hang before falling
      }
      if (c <= strip.numPixels()) { // Fill partial column with off pixels
        drawLine(N_PIXELS_HALF, N_PIXELS_HALF-c, strip.Color(0, 0, 0));
        drawLine(N_PIXELS_HALF, N_PIXELS_HALF+c, strip.Color(0, 0, 0));
      }
     
      


      y = N_PIXELS_HALF - peak;
      uint32_t color1 = Wheel(map(y,0,N_PIXELS_HALF-1,30,150));
      strip.setPixelColor(y-1,color1);
      //strip.setPixelColor(y-1,Wheel(map(y,0,N_PIXELS_HALF-1,30,150)));

      y = N_PIXELS_HALF + peak;
      strip.setPixelColor(y,color1);
      //strip.setPixelColor(y+1,Wheel(map(y,0,N_PIXELS_HALF+1,30,150)));
     
       strip.show();
      
     
      
      // Frame based peak dot animation
      if(dotHangCount > PEAK_HANG) { //Peak pause length
        if(++dotCount >= PEAK_FALL2) { //Fall rate 
          peak++;
          dotCount = 0;
        }
      } 
      else {
        dotHangCount++; 
      }
    }


void Vu3() {
  uint8_t i;
  uint16_t minLvl, maxLvl;
  int n, height;

  n = audio;             // Raw reading from mic
  n = abs(n - 512 - DC_OFFSET);        // Center on zero
  n = (n <= NOISE) ? 0 : (n - NOISE);  // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if (height < 0L)       height = 0;      // Clip output
  else if (height > TOP) height = TOP;
  if (height > peak)     peak   = height; // Keep 'peak' dot at top

  greenOffset += SPEED;
  blueOffset += SPEED;
  if (greenOffset >= 255) greenOffset = 0;
  if (blueOffset >= 255) blueOffset = 0;

  // Color pixels based on rainbow gradient
  for (i = 0; i < N_PIXELS; i++) {
    if (i >= height) {
      strip.setPixelColor(i, 0, 0, 0);
    } else {
      strip.setPixelColor(i, Wheel(
        map(i, 0, strip.numPixels() - 1, (int)greenOffset, (int)blueOffset)
      ));
    }
  }
  // Draw peak dot  
  if(peak > 0 && peak <= N_PIXELS-1) strip.setPixelColor(peak,Wheel(map(peak,0,strip.numPixels()-1,30,150)));
  
   strip.show(); // Update strip
 
// Every few frames, make the peak pixel drop by 1:
 
    if(++dotCount >= PEAK_FALL) { //fall rate 
      
      if(peak > 0) peak--;
      dotCount = 0;
    }
  strip.show();  // Update strip
 
 


  vol[volCount] = n;
  if (++volCount >= SAMPLES) {
    volCount = 0;
  }

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for (i = 1; i < SAMPLES; i++) {
    if (vol[i] < minLvl) {
      minLvl = vol[i];
    } else if (vol[i] > maxLvl) {
      maxLvl = vol[i];
    }
  }

  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if ((maxLvl - minLvl) < TOP) {
    maxLvl = minLvl + TOP;
  }
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)
}


void Vu4() {
    uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;
  
  n   = audio;                        // Raw reading from mic 
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)
 
  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);
 
  if(height < 0L)       height = 0;      // Clip output
  else if(height > TOP) height = TOP;
  if(height > peak)     peak   = height; // Keep 'peak' dot at top
  greenOffset += SPEED;
  blueOffset += SPEED;
  if (greenOffset >= 255) greenOffset = 0;
  if (blueOffset >= 255) blueOffset = 0;
 
  // Color pixels based on rainbow gradient
  for(i=0; i<N_PIXELS_HALF; i++) {
    if(i >= height) {              
      strip.setPixelColor(N_PIXELS_HALF-i-1,   0,   0, 0);
      strip.setPixelColor(N_PIXELS_HALF+i,   0,   0, 0);
    }
    else {
      uint32_t color = Wheel(map(i,0,N_PIXELS_HALF-1,(int)greenOffset, (int)blueOffset));
      strip.setPixelColor(N_PIXELS_HALF-i-1,color);
      strip.setPixelColor(N_PIXELS_HALF+i,color);
    }
    
  }
 
  // Draw peak dot  
  if(peak > 0 && peak <= N_PIXELS_HALF-1) {
    uint32_t color = Wheel(map(peak,0,N_PIXELS_HALF-1,30,150));
    strip.setPixelColor(N_PIXELS_HALF-peak-1,color);
    strip.setPixelColor(N_PIXELS_HALF+peak,color);
  }
  
   strip.show(); // Update strip
   
 
// Every few frames, make the peak pixel drop by 1:
 
    if(++dotCount >= PEAK_FALL) { //fall rate 
      
      if(peak > 0) peak--;
      dotCount = 0;
    }
 
 
  vol[volCount] = n;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter
 
  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)
 
}

void Vu5()
{
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;

  n   = audio;                        // Raw reading from mic 
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP2 * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if(height < 0L)       height = 0;      // Clip output
  else if(height > TOP2) height = TOP2;
  if(height > peak)     peak   = height; // Keep 'peak' dot at top


#ifdef CENTERED
 // Color pixels based on rainbow gradient
  for(i=0; i<(N_PIXELS/2); i++) {
    if(((N_PIXELS/2)+i) >= height)
    {
      strip.setPixelColor(((N_PIXELS/2) + i),   0,   0, 0);
      strip.setPixelColor(((N_PIXELS/2) - i),   0,   0, 0);
    }
    else
    {
      strip.setPixelColor(((N_PIXELS/2) + i),Wheel(map(((N_PIXELS/2) + i),0,strip.numPixels()-1,30,150)));
      strip.setPixelColor(((N_PIXELS/2) - i),Wheel(map(((N_PIXELS/2) - i),0,strip.numPixels()-1,30,150)));
    }
  }
  
  // Draw peak dot  
  if(peak > 0 && peak <= LAST_PIXEL_OFFSET)
  {
    strip.setPixelColor(((N_PIXELS/2) + peak),255,255,255); // (peak,Wheel(map(peak,0,strip.numPixels()-1,30,150)));
    strip.setPixelColor(((N_PIXELS/2) - peak),255,255,255); // (peak,Wheel(map(peak,0,strip.numPixels()-1,30,150)));
  }
#else
  // Color pixels based on rainbow gradient
  for(i=0; i<N_PIXELS; i++)
  {
    if(i >= height)
    {
      strip.setPixelColor(i,   0,   0, 0);
    }
    else
    {
      strip.setPixelColor(i,Wheel(map(i,0,strip.numPixels()-1,30,150)));
    }
  }

  // Draw peak dot  
  if(peak > 0 && peak <= LAST_PIXEL_OFFSET)
  {
    strip.setPixelColor(peak,255,255,255); // (peak,Wheel(map(peak,0,strip.numPixels()-1,30,150)));
  }
  
#endif  

  // Every few frames, make the peak pixel drop by 1:

  if (millis() - lastTime >= PEAK_FALL_MILLIS)
  {
    lastTime = millis();

    strip.show(); // Update strip

 

    //fall rate 
    if(peak > 0) peak--;
    }

  vol[volCount] = n;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++)
  {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < TOP2) maxLvl = minLvl + TOP2;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)
}

void Vu6()
{
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;

  n   = audio;                        // Raw reading from mic 
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP2 * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if(height < 0L)       height = 0;      // Clip output
  else if(height > TOP2) height = TOP2;
  if(height > peak)     peak   = height; // Keep 'peak' dot at top


#ifdef CENTERED
  // Draw peak dot  
  if(peak > 0 && peak <= LAST_PIXEL_OFFSET)
  {
    strip.setPixelColor(((N_PIXELS/2) + peak),255,255,255); // (peak,Wheel(map(peak,0,strip.numPixels()-1,30,150)));
    strip.setPixelColor(((N_PIXELS/2) - peak),255,255,255); // (peak,Wheel(map(peak,0,strip.numPixels()-1,30,150)));
  }
#else
  // Color pixels based on rainbow gradient
  for(i=0; i<N_PIXELS; i++)
  {
    if(i >= height)
    {
      strip.setPixelColor(i,   0,   0, 0);
    }
    else
    {
     }
  }

  // Draw peak dot  
  if(peak > 0 && peak <= LAST_PIXEL_OFFSET)
  {
    strip.setPixelColor(peak,0,0,255); // (peak,Wheel(map(peak,0,strip.numPixels()-1,30,150)));
  }
  
#endif  

  // Every few frames, make the peak pixel drop by 1:

  if (millis() - lastTime >= PEAK_FALL_MILLIS)
  {
    lastTime = millis();

    strip.show(); // Update strip

    if (num_channels) {casE131=0; prog=true; }  //prepnutie na E131 ak je prijem dat

    //fall rate 
    if(peak > 0) peak--;
    }

  vol[volCount] = n;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++)
  {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < TOP2) maxLvl = minLvl + TOP2;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)
}

void vu7() {

  EVERY_N_MILLISECONDS(1000) {
    peakspersec = peakcount;                                       // Count the peaks per second. This value will become the foreground hue.
    peakcount = 0;                                                 // Reset the counter every second.
  }

  soundmems();

  EVERY_N_MILLISECONDS(20) {
   ripple3();
  }

   show_at_max_brightness_for_power();

} // loop()


void soundmems() {                                                  // Rolling average counter - means we don't have to go through an array each time.
  newtime = millis();
  int tmp = audio - 512;
  sample = abs(tmp);

  int potin = map(pot, 0, 1023, 0, 60);

  samplesum = samplesum + sample - samplearray[samplecount];        // Add the new sample and remove the oldest sample in the array 
  sampleavg = samplesum / NSAMPLES;                                 // Get an average
  samplearray[samplecount] = sample;                                // Update oldest sample in the array with new sample
  samplecount = (samplecount + 1) % NSAMPLES;                       // Update the counter for the array

 // if (newtime > (oldtime + 200)) digitalWrite(13, LOW);             // Turn the LED off 200ms after the last peak.

  if ((sample > (sampleavg + potin)) && (newtime > (oldtime + 60)) ) { // Check for a peak, which is 30 > the average, but wait at least 60ms for another.
    step = -1;
    peakcount++;
 //   digitalWrite(13, HIGH);
    oldtime = newtime;
  }
}  // soundmems()



void ripple3() {
  for (int i = 0; i < N_PIXELS; i++) leds[i] = CHSV(bgcol, 255, sampleavg*2);  // Set the background colour.

  switch (step) {

    case -1:                                                          // Initialize ripple variables.
      center = random(N_PIXELS);
      colour = (peakspersec*10) % 255;                                             // More peaks/s = higher the hue colour.
      step = 0;
      bgcol = bgcol+8;
      break;

    case 0:
      leds[center] = CHSV(colour, 255, 255);                          // Display the first pixel of the ripple.
      step ++;
      break;

    case maxsteps:                                                    // At the end of the ripples.
      // step = -1;
      break;

    default:                                                             // Middle of the ripples.
      leds[(center + step + N_PIXELS) % N_PIXELS] += CHSV(colour, 255, myfade/step*2);       // Simple wrap from Marc Miller.
      leds[(center - step + N_PIXELS) % N_PIXELS] += CHSV(colour, 255, myfade/step*2);
      step ++;                                                         // Next step.
      break;  
  } // switch step
} // ripple()


void vu8() {
  int intensity = calculateIntensity();
  updateOrigin(intensity);
  assignDrawValues(intensity);
  writeSegmented();
  updateGlobals();
}

int calculateIntensity() {
  int      intensity;
  
  reading   = audio;                        // Raw reading from mic 
  reading   = abs(reading - 512 - DC_OFFSET); // Center on zero
  reading   = (reading <= NOISE) ? 0 : (reading - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + reading) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  intensity = DRAW_MAX * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  return constrain(intensity, 0, DRAW_MAX-1);
}

void updateOrigin(int intensity) {
  // detect peak change and save origin at curve vertex
  if (growing && intensity < last_intensity) {
    growing = false;
    intensity_max = last_intensity;
    fall_from_left = !fall_from_left;
    origin_at_flip = origin;
  } else if (intensity > last_intensity) {
    growing = true;
    origin_at_flip = origin;
  }
  last_intensity = intensity;
  
  // adjust origin if falling
  if (!growing) {
    if (fall_from_left) {
      origin = origin_at_flip + ((intensity_max - intensity) / 2);
    } else {
      origin = origin_at_flip - ((intensity_max - intensity) / 2);
    }
    // correct for origin out of bounds
    if (origin < 0) {
      origin = DRAW_MAX - abs(origin);
    } else if (origin > DRAW_MAX - 1) {
      origin = origin - DRAW_MAX - 1;
    }
  }
}

void assignDrawValues(int intensity) {
  // draw amplitue as 1/2 intensity both directions from origin
  int min_lit = origin - (intensity / 2);
  int max_lit = origin + (intensity / 2);
  if (min_lit < 0) {
    min_lit = min_lit + DRAW_MAX;
  }
  if (max_lit >= DRAW_MAX) {
    max_lit = max_lit - DRAW_MAX;
  }
  for (int i=0; i < DRAW_MAX; i++) {
    // if i is within origin +/- 1/2 intensity
    if (
      (min_lit < max_lit && min_lit < i && i < max_lit) // range is within bounds and i is within range
      || (min_lit > max_lit && (i > min_lit || i < max_lit)) // range wraps out of bounds and i is within that wrap
    ) {
      draw[i] = Wheel(scroll_color);
    } else {
      draw[i] = 0;
    }
  }
}

void writeSegmented() {
  int seg_len = N_PIXELS / SEGMENTS;

  for (int s = 0; s < SEGMENTS; s++) {
    for (int i = 0; i < seg_len; i++) {
      strip.setPixelColor(i + (s*seg_len), draw[map(i, 0, seg_len, 0, DRAW_MAX)]);
    }
  }
  strip.show();
}

uint32_t * segmentAndResize(uint32_t* draw) {
  int seg_len = N_PIXELS / SEGMENTS;
  
  uint32_t segmented[N_PIXELS];
  for (int s = 0; s < SEGMENTS; s++) {
    for (int i = 0; i < seg_len; i++) {
      segmented[i + (s * seg_len) ] = draw[map(i, 0, seg_len, 0, DRAW_MAX)];
    }
  }
  
  return segmented;
}

void writeToStrip(uint32_t* draw) {
  for (int i = 0; i < N_PIXELS; i++) {
    strip.setPixelColor(i, draw[i]);
  }
  strip.show();
}

void updateGlobals() {
  uint16_t minLvl, maxLvl;
    
  //advance color wheel
  color_wait_count++;
  if (color_wait_count > COLOR_WAIT_CYCLES) {
    color_wait_count = 0;
    scroll_color++;
    if (scroll_color > COLOR_MAX) {
      scroll_color = COLOR_MIN;
    }
  }
  
  vol[volCount] = reading;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(uint8_t i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < N_PIXELS) maxLvl = minLvl + N_PIXELS;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)
}




void vu9() {
  //currentBlending = LINEARBLEND;
  currentPalette = OceanColors_p;                             // Initial palette.
  currentBlending = LINEARBLEND; 
  EVERY_N_SECONDS(5) {                                        // Change the palette every 5 seconds.
    for (int i = 0; i < 16; i++) {
      targetPalette[i] = CHSV(random8(), 255, 255);
    }
  }
  
  EVERY_N_MILLISECONDS(100) {                                 // AWESOME palette blending capability once they do change.
    uint8_t maxChanges = 24; 
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
  }


  EVERY_N_MILLIS_I(thistimer,20) {                            // For fun, let's make the animation have a variable rate.
    uint8_t timeval = beatsin8(10,20,50);                     // Use a sinewave for the line below. Could also use peak/beat detection.
    thistimer.setPeriod(timeval);                             // Allows you to change how often this routine runs.
    fadeToBlackBy(leds, N_PIXELS, 16);                        // 1 = slow, 255 = fast fade. Depending on the faderate, the LED's further away will fade out.
    sndwave();
    soundble();
  }
  FastLED.setBrightness(max_bright);
  FastLED.show();

} // loop()



void soundble() {                                            // Quick and dirty sampling of the microphone.
  
  int tmp = audio - 512 - DC_OFFSET;
  sample = abs(tmp);
  
}  // soundmems()



void sndwave() {
  
  leds[N_PIXELS/2] = ColorFromPalette(currentPalette, sample, sample*2, currentBlending); // Put the sample into the center
  
  for (int i = N_PIXELS - 1; i > N_PIXELS/2; i--) {       //move to the left      // Copy to the left, and let the fade do the rest.
    leds[i] = leds[i - 1];
  }

  for (int i = 0; i < N_PIXELS/2; i++) {                  // move to the right    // Copy to the right, and let the fade to the rest.
    leds[i] = leds[i + 1];
  }
  addGlitter(sampleavg);
}

void vu10() {
  
EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
    static uint8_t baseC = random8();                         // You can use this as a baseline colour if you want similar hues in the next line.
    
    for (int i = 0; i < 16; i++) {
      targetPalette[i] = CHSV(random8(), 255, 255);
    }
  }
  
  EVERY_N_MILLISECONDS(100) {
    uint8_t maxChanges = 24; 
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
  }
  
  EVERY_N_MILLISECONDS(thisdelay) {                           // FastLED based non-blocking delay to update/display the sequence.
    soundtun();
    FastLED.setBrightness(max_bright);
    FastLED.show();
  }
} // loop()



void soundtun() {

  int n;
  n = audio;                                    // Raw reading from mic
  n = qsuba(abs(n-512), 10);                                  // Center on zero and get rid of low level noise
  CRGB newcolour = ColorFromPalette(currentPalette, constrain(n,0,255), constrain(n,0,255), currentBlending);
  nblend(leds[0], newcolour, 128);
  
  for (int i = N_PIXELS-1; i>0; i--) {
    leds[i] = leds[i-1];
  }
  
} // soundmems()


void vu11() {

  EVERY_N_MILLISECONDS(1000) {
    peakspersec = peakcount;                                  // Count the peaks per second. This value will become the foreground hue.
    peakcount = 0;                                            // Reset the counter every second.
  }

  soundrip();

  EVERY_N_MILLISECONDS(20) {
   rippled();
  }

   FastLED.show();

} // loop()


void soundrip() {                                            // Rolling average counter - means we don't have to go through an array each time.

  newtime = millis();
  int tmp = audio - 512;
  sample = abs(tmp);

  int potin = map(pot, 0, 1023, 0, 60);

  samplesum = samplesum + sample - samplearray[samplecount];  // Add the new sample and remove the oldest sample in the array 
  sampleavg = samplesum / NSAMPLES;                           // Get an average

 

  
  samplearray[samplecount] = sample;                          // Update oldest sample in the array with new sample
  samplecount = (samplecount + 1) % NSAMPLES;                 // Update the counter for the array

  if (newtime > (oldtime + 200)) digitalWrite(13, LOW);       // Turn the LED off 200ms after the last peak.

  if ((sample > (sampleavg + potin)) && (newtime > (oldtime + 60)) ) { // Check for a peak, which is 30 > the average, but wait at least 60ms for another.
    step = -1;
    peakcount++;
    oldtime = newtime;
  }
  
}  // soundmems()



void rippled() {

  fadeToBlackBy(leds, N_PIXELS, 64);                          // 8 bit, 1 = slow, 255 = fast
  
  switch (step) {

    case -1:                                                  // Initialize ripple variables.
      center = random(N_PIXELS);
      colour = (peakspersec*10) % 255;                        // More peaks/s = higher the hue colour.
      step = 0;
      break;

    case 0:
      leds[center] = CHSV(colour, 255, 255);                  // Display the first pixel of the ripple.
      step ++;
      break;

    case maxsteps:                                            // At the end of the ripples.
      // step = -1;
      break;

    default:                                                  // Middle of the ripples.
      leds[(center + step + N_PIXELS) % N_PIXELS] += CHSV(colour, 255, myfade/step*2);       // Simple wrap from Marc Miller.
      leds[(center - step + N_PIXELS) % N_PIXELS] += CHSV(colour, 255, myfade/step*2);
      step ++;                                                // Next step.
      break;  
  } // switch step
  
} // ripple()


 //Used to draw a line between two points of a given color
    void drawLine(uint8_t from, uint8_t to, uint32_t c) {
      uint8_t fromTemp;
      if (from > to) {
        fromTemp = from;
        from = to;
        to = fromTemp;
      }
      for(int i=from; i<=to; i++){
        strip.setPixelColor(i, c);
      }
    }

void setPixel(int Pixel, byte red, byte green, byte blue) {
    strip.setPixelColor(Pixel, strip.Color(red, green, blue));

}


void setAll(byte red, byte green, byte blue) {

  for(int i = 0; i < N_PIXELS; i++ ) {

    setPixel(i, red, green, blue); 

  }

  strip.show();

}
void vu12() {

  EVERY_N_MILLISECONDS(1000) {
    peakspersec = peakcount;                                  // Count the peaks per second. This value will become the foreground hue.
    peakcount = 0;                                            // Reset the counter every second.
  }

  soundripped();

  EVERY_N_MILLISECONDS(20) {
  rippvu();
  }

   FastLED.show();

} // loop()


void soundripped() {                                            // Rolling average counter - means we don't have to go through an array each time.

  newtime = millis();
  int tmp = audio - 512;
  sample = abs(tmp);

  int potin = map(pot, 0, 1023, 0, 60);

  samplesum = samplesum + sample - samplearray[samplecount];  // Add the new sample and remove the oldest sample in the array 
  sampleavg = samplesum / NSAMPLES;                           // Get an average



  
  samplearray[samplecount] = sample;                          // Update oldest sample in the array with new sample
  samplecount = (samplecount + 1) % NSAMPLES;                 // Update the counter for the array

  if (newtime > (oldtime + 200)) digitalWrite(13, LOW);       // Turn the LED off 200ms after the last peak.

  if ((sample > (sampleavg + potin)) && (newtime > (oldtime + 60)) ) { // Check for a peak, which is 30 > the average, but wait at least 60ms for another.
    step = -1;
    peakcount++;
    oldtime = newtime;
  }
  
}  // soundmems()
void rippvu() {                                                                 // Display ripples triggered by peaks.

fadeToBlackBy(leds, N_PIXELS, 64);                          // 8 bit, 1 = slow, 255 = fast
  
  switch (step) {

    case -1:                                                  // Initialize ripple variables.
      center = random(N_PIXELS);
      colour = (peakspersec*10) % 255;                        // More peaks/s = higher the hue colour.
      step = 0;
      break;

    case 0:
      leds[center] = CHSV(colour, 255, 255);                  // Display the first pixel of the ripple.
      step ++;
      break;

    case maxsteps:                                            // At the end of the ripples.
      // step = -1;
      break;

    default:                                                  // Middle of the ripples.
      leds[(center + step + N_PIXELS) % N_PIXELS] += CHSV(colour, 255, myfade/step*2);       // Simple wrap from Marc Miller.
      leds[(center - step + N_PIXELS) % N_PIXELS] += CHSV(colour, 255, myfade/step*2);
      step ++;                                                // Next step.
      break;  
  } // switch step
   addGlitter(sampleavg);
} // ripple()



void vu13() {                                                                   // The >>>>>>>>>> L-O-O-P <<<<<<<<<<<<<<<<<<<<<<<<<<<<  is buried here!!!11!1!

 
  EVERY_N_MILLISECONDS(1000) {
    peakspersec = peakcount;                                  // Count the peaks per second. This value will become the foreground hue.
    peakcount = 0;                                            // Reset the counter every second.
  }

  soundripper();

  EVERY_N_MILLISECONDS(20) {
  jugglep();
  }

   FastLED.show();

} // loop()


void soundripper() {                                            // Rolling average counter - means we don't have to go through an array each time.

  newtime = millis();
  int tmp = audio - 512;
  sample = abs(tmp);

  int potin = map(pot, 0, 1023, 0, 60);

  samplesum = samplesum + sample - samplearray[samplecount];  // Add the new sample and remove the oldest sample in the array 
  sampleavg = samplesum / NSAMPLES;                           // Get an average

  

  
  samplearray[samplecount] = sample;                          // Update oldest sample in the array with new sample
  samplecount = (samplecount + 1) % NSAMPLES;                 // Update the counter for the array

  if (newtime > (oldtime + 200)) digitalWrite(13, LOW);       // Turn the LED off 200ms after the last peak.

  if ((sample > (sampleavg + potin)) && (newtime > (oldtime + 60)) ) { // Check for a peak, which is 30 > the average, but wait at least 60ms for another.
    step = -1;
    peakcount++;
    oldtime = newtime;
                                                    // Change the current pattern function periodically.
    jugglep();
  }

} // loop()


void jugglep() {                                                                // Use the juggle routine, but adjust the timebase based on sampleavg for some randomness.

// Persistent local variables
  static uint8_t thishue=0;

  timeval = 40;                                                                 // Our EVERY_N_MILLIS_I timer value.

  leds[0] = ColorFromPalette(currentPalette, thishue++, sampleavg, LINEARBLEND);
  
  for (int i = N_PIXELS-1; i >0 ; i-- ) leds[i] = leds[i-1];

  addGlitter(sampleavg/2);                                                      // Add glitter based on sampleavg. By Andrew Tuline.
  
} // matrix()


void Balls() {
  for (int i = 0 ; i < NUM_BALLS ; i++) {
    tCycle[i] =  millis() - tLast[i] ;     // Calculate the time since the last time the ball was on the ground

    // A little kinematics equation calculates positon as a function of time, acceleration (gravity) and intial velocity
    h[i] = 0.5 * GRAVITY * pow( tCycle[i]/1000 , 2.0 ) + vImpact[i] * tCycle[i]/1000;

    if ( h[i] < 0 ) {                      
      h[i] = 0;                            // If the ball crossed the threshold of the "ground," put it back on the ground
      vImpact[i] = COR[i] * vImpact[i] ;   // and recalculate its new upward velocity as it's old velocity * COR
      tLast[i] = millis();

      if ( vImpact[i] < 0.01 ) vImpact[i] = vImpact0;  // If the ball is barely moving, "pop" it back up at vImpact0
    }
    pos[i] = round( h[i] * (N_PIXELS - 1) / h0);       // Map "h" to a "pos" integer index position on the LED strip
  }

  //Choose color of LEDs, then the "pos" LED on
  for (int i = 0 ; i < NUM_BALLS ; i++) leds[pos[i]] = CHSV( uint8_t (i * 40) , 255, 255);
  FastLED.show();
  //Then off for the next loop around
  for (int i = 0 ; i < NUM_BALLS ; i++) {
    leds[pos[i]] = CRGB::Black;
  }
}





 void ripple() {
 
    if (currentBg == nextBg) {
      nextBg = random(256);
    }
    else if (nextBg > currentBg) {
      currentBg++;
    } else {
      currentBg--;
    }
    for(uint16_t l = 0; l < N_PIXELS; l++) {
      leds[l] = CHSV(currentBg, 255, 50);         // strip.setPixelColor(l, Wheel(currentBg, 0.1));
    }
 
  if (step == -1) {
    center = random(N_PIXELS);
    color = random(256);
    step = 0;
  }
 
  if (step == 0) {
    leds[center] = CHSV(color, 255, 255);         // strip.setPixelColor(center, Wheel(color, 1));
    step ++;
  }
  else {
    if (step < maxSteps) {
      
 
      leds[wrap(center + step)] = CHSV(color, 255, pow(fadeRate, step)*255);       //   strip.setPixelColor(wrap(center + step), Wheel(color, pow(fadeRate, step)));
      leds[wrap(center - step)] = CHSV(color, 255, pow(fadeRate, step)*255);       //   strip.setPixelColor(wrap(center - step), Wheel(color, pow(fadeRate, step)));
      if (step > 3) {
        leds[wrap(center + step - 3)] = CHSV(color, 255, pow(fadeRate, step - 2)*255);     //   strip.setPixelColor(wrap(center + step - 3), Wheel(color, pow(fadeRate, step - 2)));
        leds[wrap(center - step + 3)] = CHSV(color, 255, pow(fadeRate, step - 2)*255);     //   strip.setPixelColor(wrap(center - step + 3), Wheel(color, pow(fadeRate, step - 2)));
      }
      step ++;
    }
    else {
      step = -1;
    }
  }
 
  LEDS.show();
  delay(50);
}
 
 
int wrap(int step) {
  if(step < 0) return N_PIXELS + step;
  if(step > N_PIXELS - 1) return step - N_PIXELS;
  return step;
}
 
 
void one_color_allHSV(int ahue, int abright) {                // SET ALL LEDS TO ONE COLOR (HSV)
  for (int i = 0 ; i < N_PIXELS; i++ ) {
    leds[i] = CHSV(ahue, 255, abright);
  }
}

 
void ripple2() {
  if (BG){
    if (currentBg == nextBg) {
      nextBg = random(256);
    } 
    else if (nextBg > currentBg) {
      currentBg++;
    } else {
      currentBg--;
    }
    for(uint16_t l = 0; l < N_PIXELS; l++) {
      strip.setPixelColor(l, Wheel(currentBg, 0.1));
    }
  } else {
    for(uint16_t l = 0; l < N_PIXELS; l++) {
      strip.setPixelColor(l, 0, 0, 0);
    }
  }
 
 
  if (step == -1) {
    center = random(N_PIXELS);
    color = random(256);
    step = 0;
  }
 
 
 
  if (step == 0) {
    strip.setPixelColor(center, Wheel(color, 1));
    step ++;
  } 
  else {
    if (step < maxSteps) {
      strip.setPixelColor(wrap(center + step), Wheel(color, pow(fadeRate, step)));
      strip.setPixelColor(wrap(center - step), Wheel(color, pow(fadeRate, step)));
      if (step > 3) {
        strip.setPixelColor(wrap(center + step - 3), Wheel(color, pow(fadeRate, step - 2)));
        strip.setPixelColor(wrap(center - step + 3), Wheel(color, pow(fadeRate, step - 2)));
      }
      step ++;
    } 
    else {
      step = -1;
    }
  }
  
  strip.show();
  delay(50);
}

void fire(){
#define FRAMES_PER_SECOND 20
//random16_add_entropy( N_PIXELS);

  
// Array of temperature readings at each simulation cell
  static byte heat[N_PIXELS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < N_PIXELS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / N_PIXELS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= N_PIXELS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < N_PIXELS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow,  CRGB::White), colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (N_PIXELS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;

    }
    
    FastLED.show();
  }


  void fireblu(){
#define FRAMES_PER_SECOND 40
//random16_add_entropy( N_PIXELS);

  
// Array of temperature readings at each simulation cell
  static byte heat[N_PIXELS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < N_PIXELS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / N_PIXELS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= N_PIXELS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < N_PIXELS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White), colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (N_PIXELS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;

    }
    FastLED.show();
  }


void Drip()
{
 MODE_WATER_TORTURE: 
 if (cycle())
        {
        strip.setBrightness(255); // off limits
        water_torture.animate(reverze);
        strip.show();
        //strip.setBrightness(brightness); // back to limited
        
        }
  }

      
bool cycle()
{
  if (paused)
  {
    return false;
  }
  
  if (millis() - lastTime >= cycleMillis)
  {
    lastTime = millis();
    return true;
  }
  return false;
}

 


   void pattern2() {
      
       sinelon();                                                  // Call our sequence.
  show_at_max_brightness_for_power();                         // Power managed display of LED's.
 
} // loop()


void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, N_PIXELS, thisfade);
  delay(10);
  int pos1 = beatsin16(thisbeat,0,N_PIXELS);
  int pos2 = beatsin16(thatbeat,0,N_PIXELS);
    leds[(pos1+pos2)/2] += CHSV( myhue++/64, thissat, thisbri);
}
// Pattern 3 - JUGGLE
    void pattern3() {
       ChangeMe();
      
  juggle();
  
  show_at_max_brightness_for_power();// Power managed display of LED's.
  
} // loop()


void juggle() {                                               // Several colored dots, weaving in and out of sync with each other
  curhue = thishue;                                          // Reset the hue values.
  fadeToBlackBy(leds, N_PIXELS, faderate);
  delay(10);
  for( int i = 0; i < numdots; i++) {
    leds[beatsin16(basebeat+i+numdots,0,N_PIXELS)] += CHSV(curhue, thissat, thisbright);   //beat16 is a FastLED 3.1 function
    curhue += hueinc;
  }
} // juggle()


void ChangeMe() {                                             // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.
  uint8_t secondHand = (millis() / 1000) % 30;                // IMPORTANT!!! Change '30' to a different value to change duration of the loop.
  static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
  if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    if (secondHand ==  0)  {numdots=1; faderate=2;}  // You can change values here, one at a time , or altogether.
    if (secondHand == 10)  {numdots=4; thishue=128; faderate=8;}
    if (secondHand == 20)  {hueinc=48; thishue=random8();}                               // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
  }
} // ChangeMe()

void juggle2() {                                               // Several colored dots, weaving in and out of sync with each other

  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, N_PIXELS, 20);
  
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,N_PIXELS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
FastLED.show();

  
}


void addGlitter( fract8 chanceOfGlitter) {                                      // Let's add some glitter, thanks to Mark
  
  if( random8() < chanceOfGlitter) {
    leds[random16(N_PIXELS)] += CRGB::White;
  }

} // addGlitter()




void Twinkle () {
   if (random(25) == 1) {
      uint16_t i = random(N_PIXELS);
      if (redStates[i] < 1 && greenStates[i] < 1 && blueStates[i] < 1) {
        redStates[i] = random(256);
        greenStates[i] = random(256);
        blueStates[i] = random(256);
      }
    }
    
    for(uint16_t l = 0; l < N_PIXELS; l++) {
      if (redStates[l] > 1 || greenStates[l] > 1 || blueStates[l] > 1) {
        strip.setPixelColor(l, redStates[l], greenStates[l], blueStates[l]);
        
        if (redStates[l] > 1) {
          redStates[l] = redStates[l] * Fade;
        } else {
          redStates[l] = 0;
        }
        
        if (greenStates[l] > 1) {
          greenStates[l] = greenStates[l] * Fade;
        } else {
          greenStates[l] = 0;
        }
        
        if (blueStates[l] > 1) {
          blueStates[l] = blueStates[l] * Fade;
        } else {
          blueStates[l] = 0;
        }
        
      } else {
        strip.setPixelColor(l, 0, 0, 0);
      }
    }
    strip.show();
     delay(10);
  
}

void blur() {

  uint8_t blurAmount = dim8_raw( beatsin8(3,64, 192) );       // A sinewave at 3 Hz with values ranging from 64 to 192.
  blur1d( leds, N_PIXELS, blurAmount);                        // Apply some blurring to whatever's already on the strip, which will eventually go black.
  delay(5);
  uint8_t  i = beatsin8(  9, 0, N_PIXELS);
  uint8_t  j = beatsin8( 7, 0, N_PIXELS);
  uint8_t  k = beatsin8(  5, 0, N_PIXELS);
  
  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();  
  leds[(i+j)/2] = CHSV( ms / 29, 200, 255);
  leds[(j+k)/2] = CHSV( ms / 41, 200, 255);
  leds[(k+i)/2] = CHSV( ms / 73, 200, 255);
  leds[(k+i+j)/3] = CHSV( ms / 53, 200, 255);
  
  FastLED.show();
  
} 

void citaj () {
    for(int i=0; i<60; i++) {     
     pixel[3*i] = (strip.getPixelColor(i)>>16);
     pixel[3*i+1] = (strip.getPixelColor(i)>>8) ;
     pixel[3*i+2] = (strip.getPixelColor(i)) ;
       }  
}


 

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j = 0; j < 256; j++) // 1 cycles of all colors on wheel
  {
    for(i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
   if (digitalRead(buttonPin)==false && bit_TL==false ) {buttonPushCounter++;bit_TL=true;return;}
   cas_TL++;
              
    delay(wait);
  }
  cas++;  
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, float opacity) {
  
  if(WheelPos < 85) {
    return strip.Color((WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color((255 - WheelPos * 3) * opacity, 0, (WheelPos * 3) * opacity);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, (WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity);
  }
}




void colorWipe2(uint32_t c, uint8_t wait) //postupne zapal
{
  //open colowipe
  for(uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}//close colorWipe function

void theaterChase(uint32_t c, uint8_t wait)  //striedavo  vpred
{
  //open theaterchase function
  for (int j = 0; j < 3; j++) //do 3 cycles of chasing
  {
    for (int q = 0; q < 3; q++)
    {
      for (int i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}//close theater chase function

//----------------------  display ------------------------------------
void text (String prog) {
  
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(5, 0, "   NEOPIXEL ");
    display.setFont(ArialMT_Plain_10);              //velkost textu
    display.drawString(108,2,String(program_E131));
    display.drawString(10, 23, "IP:");
    display.drawString(32,23,String(IP_addr));
    display.drawString(0, 45, "Prog:");
    display.setFont(ArialMT_Plain_16);
    display.drawString(32,45,(prog));
   
    display.display();
}




//vypnutie vystupov od led
void nulujPixel (int od_Pixel)
 {   uint16_t k;
   for (k=od_Pixel; k<512; k++)
   {strip.setPixelColor(k,0,0,0);}
  }

