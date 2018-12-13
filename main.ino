#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

#include <TinyGPS++.h> //GPS library
#include <SoftwareSerial.h> //Serial library

/*------ Touchscreen ------*/
#include <TouchScreen.h> //Touchscreen library

//Touchscreen boundaries
#define TS_MINX 120
#define TS_MAXX 900
#define TS_MINY 70
#define TS_MAXY 920

#define YP A3  // must be an analog pin
#define XM A2  // must be an analog pin
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define MINPRESSURE 10
#define MAXPRESSURE 1000

/*------ LCD ------*/
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

/*------ GPS ------*/
static const int RXPin = A5, TXPin = 1;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps; // The TinyGPS++ object
SoftwareSerial ss(RXPin, TXPin);// The serial connection to the GPS device

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Custom Colours
#define ACCENT  0x8010
#define MAIN  0x87E0

//Other Constants
#define W 320 //Width of screen
#define H 240 //Height of screen

//Current state
char type[] = {'1', '2', '3', '4', '5'}; //Used to switch between screens
int ofx;
int curScreen = 0;  //Stores value of current screen
bool inLogo = true; //Determines whether we are in the logo screen or not. 
                    //Logo is only visible at the start. 
                    //Once user clicks anything, it disappears forever.


//GPS Data
double _long, _lat, _altitudeVal, _speedVal;      //Raw GPS data
int sec, oldSec;                                  //Used to store time and update the screen every second
int _day, _month, _year, _hour, _minute, _second; //Raw GPS data
String _date, _time, _speed, _altitude;           //Pretty printed data for display (e.g. "20 Km/h" rather than "20")

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup() {
  Serial.begin(9600);

  ss.begin(GPSBaud);        //Begin serial communication

  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(1);       //Rotate to landscape
  drawUI('0');              //Draws logo at the beginning
  sec = gps.time.second();  //Used to update data every second
  oldSec = gps.time.second();
}

void loop() {
  while (ss.available() > 0) {//When serial is available
    sec = gps.time.second();
    if (gps.encode(ss.read()) && sec > oldSec ) { //If serial is reading data which is correctly encoded and one second has passed
      updateGPS();    //Update the GPS data variables
      if (!inLogo) { //If not in the logo screen
        printData(type[curScreen]);//Print updated values, overwriting old ones
      }
    }
  }
  oldSec = sec;

  //Enable choosing between screens
  button(0, W, 0, H / 2, 'l');//Left
  button(0, W, H - H / 2, H, 'r');//Right
}


void drawUI(char screen) {//This method clears the screen and then draws the content
  //This method is called once whenever the user changes screen, to clear it from old data
  //And display the new one. (Refreshing values every second is done by another method)
  switch (screen) {
    case '0':
      ofx = 4;
      inLogo = true;
      pinMode(XM, OUTPUT);//We are sending new data to the LCD
      pinMode(YP, OUTPUT);

      tft.fillScreen(MAIN);//Clean the screen

      tft.setCursor(0, H / 4);//Set the cursor
      tft.setTextSize(ofx);//Set text size (1-5)
      tft.setTextColor(ACCENT);//Set colour (just foreground)
      tft.println("   EXTREME \n   SPORTS \n   JACKET");//\n draws a new line, tabs and spaces are also considered
      break;

    case '1':
      ofx = 4;

      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      tft.fillScreen(MAIN);

      tft.setCursor(0, H / 8);
      tft.setTextSize(ofx);
      tft.setTextColor(ACCENT);
      tft.println(">>ALTITUDE<<");
      tft.setCursor(0, H / 8 + H / 8 + ofx);
      tft.println("\n " + _altitude);
      break;

    case '2':
      ofx = 4;

      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      tft.fillScreen(MAIN);

      tft.setCursor(0, H / 8);
      tft.setTextSize(ofx);
      tft.setTextColor(ACCENT);
      tft.println(">>>>TIME<<<<");
      tft.setCursor(0, H / 8 + H / 8 + ofx);
      tft.println("\n   " + _time);
      break;

    case '3':
      ofx = 4;

      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      tft.fillScreen(MAIN);

      tft.setCursor(0, H / 8);
      tft.setTextSize(ofx);
      tft.setTextColor(ACCENT);
      tft.println(">>>>DATE<<<<");
      tft.setCursor(0, H / 8 + H / 8 + ofx);
      tft.println("\n   " + _date);
      break;

    case '4':
      ofx = 4;

      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      tft.fillScreen(MAIN);

      tft.setCursor(0, H / 8);
      tft.setTextSize(ofx);
      tft.setTextColor(ACCENT);
      tft.println(">>>>SPEED<<<<");
      tft.setCursor(0, H / 8 + H / 8 + ofx);
      tft.println("\n   " + _speed);
      break;


    case '5':
      ofx = 4;

      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      tft.fillScreen(MAIN);

      tft.setCursor(0, H / 8);
      tft.setTextSize(ofx);
      tft.setTextColor(ACCENT);
      tft.println(">>LATITUDE<<");
      tft.setCursor(ofx * 4, H / 8 + H / 8 + ofx);
      tft.println(_lat, 6);

      tft.setCursor(0, H - H / 2);
      tft.println(">>LONGITUDE<<");
      tft.setCursor(ofx * 4, H - H / 2 + H / 8 + ofx);
      tft.println(_long, 6);
      break;

  }
}

void button(int x0, int x1, int y0, int y1, char side) {//This method enable to have 'buttons'
  //There are 3 (although we use just two in this final sketch) buttons:
  //Left, to go left. Right, to go right. Center, to refresh the current view.
  TSPoint p = ts.getPoint();
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {//If the pressure is between our boundaries
    inLogo = false; //User cliked, hence we are not in the logo anymore
    p.x = tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0); //Mapping x and y of p (the point where the user clicked)
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);              //Taking into account that we rotated the display by 90°
    if (p.x > x0 && p.x < x1 && p.y > y0 && p.y < y1) {//If the click is inside our rectangle
      if (side == 'r') {//If "right" button is clicked
        if (curScreen <= 4) {//Go right
          drawUI(type[++curScreen]);
          Serial.println(F("Going right"));
          Serial.print(curScreen);
        } else {//Unless you're at the end, then go back to 0 (circular array)09
          curScreen = 0;
          drawUI(type[curScreen]);
        }
      } else if (side == 'l') {//Same thing for left but inverted.
        if (curScreen > 0) {
          drawUI(type[--curScreen]);
          Serial.print(curScreen);
          Serial.println(F("Going left"));
        } else {
          curScreen = 5;
          drawUI(type[curScreen]);
        }

      } else if (side == 'c') {//Center
        drawUI(type[curScreen]);//Refresh current screen
      }
    }

  }
}

void updateGPS() {//This method updates the values only if they are valid (else display old values)
  if (gps.location.isValid())  {//Lat and Long
    _lat = (gps.location.lat());
    _long = (gps.location.lng());
  }

  if (gps.date.isValid())  {//Date
    _month = (gps.date.month());
    _day = (gps.date.day());
    _year = (gps.date.year());
    _date = String(_day) + "/" + String(_month) + "/" + String(_year); //Pretty printed date e.g. "20/09/1992"
  }

  if (gps.time.isValid()) {
    _hour = (gps.time.hour()); // Hour (0-23) (u8)
    _minute = (gps.time.minute()); // Minute (0-59) (u8)
    _second = (gps.time.second()); // Second (0-59) (u8)
    //Very unegantly pretty printing the time without introducing another variable
    //Basically adding 0's when <10 (e.g. 1 sec will turn into 01)
    _time = _hour < 10 ? "0" +  String(_hour) : String(_hour);
    _time = _time + ":";
    _time = _minute < 10 ? _time + "0" + String(_minute) : _time + String(_minute);
    _time = _time + ":";
    _time = _second < 10 ? _time + "0" + String(_second) : _time + String(_second);
    //Final result will look like 10:42:05
  }

  if (gps.altitude.isValid())  {//Altitude in meters
    _altitudeVal = (gps.altitude.meters());
    _altitude = String(_altitudeVal) + " meters";
  }

  if (gps.speed.isValid())  {//Speed in km/h
    _speedVal = (gps.speed.kmph());
    _speed = String(_speedVal) + " km/h";
  }


}

void printData(char screen) {//This method is run every second to redraw the data.
  //To make everything run smoothly, instead of painting the whole screen which would
  //Take 2-5 seconds, it draws just what's needed.
  //Another clever thing is that by setting the text colour to (ACCENT,MAIN);
  //It actually clears the background before drawing the foreground, thus
  //resulting in a smooth transition.
  switch (screen) {
    case '1':
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      tft.setTextColor(ACCENT, MAIN);
      tft.setCursor(0, H / 8 + H / 8 + ofx);
      tft.println("\n " + _altitude);
      break;

    case '2':
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      tft.setTextColor(ACCENT, MAIN);
      tft.setCursor(0, H / 8 + H / 8 + ofx);
      tft.println("\n   " + _time);
      break;

    case '3':
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      tft.setTextColor(ACCENT, MAIN);
      tft.setCursor(0, H / 8 + H / 8 + ofx);
      tft.println("\n   " + _date);
      break;

    case '4':
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      tft.setTextColor(ACCENT, MAIN);
      tft.setCursor(0, H / 8 + H / 8 + ofx);
      tft.println("\n   " + _speed);
      break;


    case '5':
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      tft.setTextColor(ACCENT, MAIN);
      tft.setCursor(ofx * 4, H / 8 + H / 8 + ofx);
      tft.println(_lat, 6);

      tft.setCursor(0, H - H / 2);
      tft.setCursor(ofx * 4, H - H / 2 + H / 8 + ofx);
      tft.println(_long, 6);
      break;
  }
}
