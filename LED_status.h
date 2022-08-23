#pragma once

/*
ESP8266 Blink
Blink the blue LED on the ESP8266 module
*/
#define CONFIG_LED_PIN D3          
#define CONFIG_LED_NUMPIXELS 28   //These two values are from config, but including config seems to interfere with the mqtt and causes the Green LED Loop of death

Adafruit_NeoPixel strip(CONFIG_LED_NUMPIXELS, CONFIG_LED_PIN, NEO_GRB + NEO_KHZ800);

#define LED 2 //Define blinking LED pin
#define WIPETIME 25

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.begin();  
    strip.setBrightness(10);
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}



int dir = 1;
int bright = 0;

void Pulse(uint32_t color,int s)  
{
  strip.fill(color);                                 //fill strip with color blueFade
  strip.show();                                         //send data to strip
  bright = bright + dir;                                //change bright by +1 or -1
  if(bright > 100 || bright < 1) dir = dir * -1;        //set dir to +1 or -1
  delay(s);                                            //speed of fade faster with lower delay value
}

void blink1() {
    digitalWrite(LED, LOW);delay(250);
    digitalWrite(LED, HIGH);
  }

void blink2() {
    digitalWrite(LED, LOW);delay(250);
    digitalWrite(LED, HIGH);delay(250);
    
    digitalWrite(LED, LOW);delay(250);
    digitalWrite(LED, HIGH);
  }

void blink3() {
    digitalWrite(LED, LOW);delay(250);
    digitalWrite(LED, HIGH);delay(250);
    
    digitalWrite(LED, LOW);delay(250);
    digitalWrite(LED, HIGH);delay(250);
    
    digitalWrite(LED, LOW);delay(250);
    digitalWrite(LED, HIGH);
  }

void blink4() {
    digitalWrite(LED, LOW);delay(250);
    digitalWrite(LED, HIGH);delay(250);
    
    digitalWrite(LED, LOW);delay(250);
    digitalWrite(LED, HIGH);delay(250);
    
    digitalWrite(LED, LOW);delay(250);
    digitalWrite(LED, HIGH);delay(250);
    
    digitalWrite(LED, LOW);delay(250);
    digitalWrite(LED, HIGH);
  }

uint32_t blueFade = strip.ColorHSV(43650,255,bright); //set blueFade color to variable bright
void PulseBlue() {
Pulse(blueFade,100);
}

uint32_t greenFade = strip.ColorHSV(43650,150,bright); //set blueFade color to variable bright
void PulseGreen() {
Pulse(greenFade, 100);  
}

  void WipeRed() {
    colorWipe(strip.Color(255,   0,   0), WIPETIME);
}

  void WipeOrange() {
    colorWipe(strip.Color(255,   20,   0), WIPETIME);
  }

  void WipeYellow() {
    colorWipe(strip.Color(255,   250,   10), WIPETIME);
  }

void WipePurple() {
    colorWipe(strip.Color(255,   155,   0), WIPETIME);
}

void WipeBlue() {
    colorWipe(strip.Color(0,   0,   255), WIPETIME);
}

void WipeTeal() {
    colorWipe(strip.Color(0,   255,   255), WIPETIME);
}

void WipeGreen() {
    colorWipe(strip.Color(0,   255,   0), WIPETIME);
}

void WipeBlack() {
    colorWipe(strip.Color(0,   0,   0), WIPETIME);
}

void WipePink() {
    colorWipe(strip.Color(255,   100,   200), WIPETIME);
}

void WipeWhite() {
    colorWipe(strip.Color(255,   230,   230), WIPETIME);    
}
