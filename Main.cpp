#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <system.h>
#include <DHT.h>

#define YES 1
#define NO  0

#define PIN_SERIAL_RX 0 // Placeholder for serial monitor comm
#define PIN_SERIAL_TX 1 // Placeholder for serial monitor comm
#define PIN_BUTTON    2 // Connect 2-pin button to this pin and GND
#define PIN_DHT11     3 // DHT11 data pin
#define PIN_LED       13

#define UPDATE_SYSTEM_TIME A00
#define UPDATE_DISPLAY     A01
#define READ_BUTTON        A02
#define READ_TEMP          A03
#define BUTTON_PUSHED      A04

#define ONE_SEC_TIMOUT      100
#define FIVE_SEC_TIMOUT     500
#define BUTTON_PUSH_TIMEOUT  30

#define ASCII_OFFSET 48
#define OLED_RESET 4
#define DHTTYPE DHT11   // DHT 11

#define DISPLAY_SYSTEM_TIME_BOTTOM_LINE 12
#define NOF_DISPLAY_VIEWS 3

#define TIMER_VACANT -2    /* The timer is not in use */
typedef void (*P_TIMER_FUNC)(void);

typedef enum
{
  SYSTEM_TIMER,
  DISPLAY_TIMER,
  DHT11_TIMER,
  BUTTON_TIMER,
  NOF_TIMERS
} TIMER_ID;

// Prototypes
Adafruit_SSD1306 display(OLED_RESET);
DHT dht(PIN_DHT11, DHTTYPE);
System cSystem;

extern void SystemTimer(void);
extern void DisplayTimer(void);
extern void Dht11Timer(void);
extern void ButtonTimer(void);

struct TimeUnits
{
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t year;
};

const P_TIMER_FUNC timer_func_array[NOF_TIMERS] =
{
  SystemTimer,
  DisplayTimer,
  Dht11Timer,
  ButtonTimer,
};

// bitmap created here: http://javl.github.io/image2cpp/
const unsigned char deer_jumping [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x01, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x01, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0x01, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x06, 0x20, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0x0c, 0x40, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xc3, 0x98, 0xc0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x41, 0xf1, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe1, 0xf6, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xfc, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe4, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfe, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xfe, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x3f, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf8, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x9f, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xf0, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xe0, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfc, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xe0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xf0, 0x00, 0x00, 0x03, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00, 0x03, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0xff, 0xfc, 0x07, 0x87, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x1f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00,
0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00,
0x00, 0x07, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf7, 0x00, 0x00, 0x00, 0x00,
0x00, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe7, 0x00, 0x00, 0x00, 0x00,
0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00,
0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00,
0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00,
0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

boolean HasButtonBeenPushed = NO;
boolean LedToggle = 0;
struct TimeUnits system_time;
static NOTIFY_ELEMENT_TYPE input;
static long timeoutarray[NOF_TIMERS];
uint8_t DisplayViewNo = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 100;
float HeatIndex = 0.0;

void HandleTimerTicks(void)
{
  byte count;

  for (count = 0; count < NOF_TIMERS; count++)
  {
    if(timeoutarray[count] > 0)
    {
      timeoutarray[count]--;
      if(timeoutarray[count] == 0)
      {
        timer_func_array[count]();
      }
    }
  }
}

ISR(TIMER1_COMPA_vect)
{

  HandleTimerTicks();
}

void StopTimer(TIMER_ID id)
{
  timeoutarray[id] = TIMER_VACANT;
}

void StartTimer(TIMER_ID id, long time_out)
{
  timeoutarray[id] = time_out;
}

void InitTimer()
{
  byte count;

  cSystem.DisableInterrupts();//stop interrupts

  for(count = 0; count < NOF_TIMERS; count++)
  {
    timeoutarray[count] = TIMER_VACANT;
  }

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 100hz increments
  OCR1A = 19999;// = (16*10^6) / (8*100) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  cSystem.EnableInterrupts();//allow interrupts
}

  // Set CS10 and CS12 bits for 1024 prescaler
  //TCCR1B |= (1 << CS12) | (1 << CS10);

void InitSystemTime(void)
{
  system_time.second = 0;
  system_time.minute = 0;
  system_time.hour = 0;
  system_time.day = 0;
  system_time.year = 0;
}

void UpdateSystemTime(void)
{
  //call every second
  system_time.second++;

  if (system_time.second > 59)
  {
    system_time.second = 0;
    system_time.minute++;
    if (system_time.minute > 59)
    {
      system_time.minute = 0;
      system_time.hour++;
      if (system_time.hour > 23)
      {
        system_time.hour = 0;
        system_time.day++;
        if (system_time.day > 365)
        {
          system_time.day = 0;
          system_time.year++;
        }
      }
    }
  }
}

void DisplaySystemInfo(void)
{
  uint8_t digit = 0;

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,3);
  display.print("Elapsed: ");
  digit = system_time.day/10;
  display.write(digit + ASCII_OFFSET);
  digit = system_time.day%10;
  display.write(digit + ASCII_OFFSET);
  display.print(":");
  digit = system_time.hour/10;
  display.write(digit + ASCII_OFFSET);
  digit = system_time.hour%10;
  display.write(digit + ASCII_OFFSET);
  display.print(":");
  digit = system_time.minute/10;
  display.write(digit + ASCII_OFFSET);
  digit = system_time.minute%10;
  display.write(digit + ASCII_OFFSET);
  display.print(":");
  digit = system_time.second/10;
  display.write(digit + ASCII_OFFSET);
  digit = system_time.second%10;
  display.write(digit + ASCII_OFFSET);
  //display time end
}

void DisplayDegreeDays(void)
{
  float degree_days = 15.7;
  uint8_t digit = 0;

  //display degree days start
  display.setTextSize(5);
  display.setTextColor(WHITE);
  display.setCursor(6,12);
  digit = (uint8_t)degree_days/10.0;
  display.write(digit + ASCII_OFFSET);
  digit = (uint8_t)degree_days%10;
  display.write(digit + ASCII_OFFSET);
  display.print(",");
  digit = (uint8_t)(((uint8_t)(degree_days*10))%10);
  display.write(digit + ASCII_OFFSET);
  //display degree days end
}

void DisplayTemp(void)
{
  //float degree_days = 15.7;
  uint8_t digit = 0;
 // uint16_t u16test;
  //uint8_t u8modulo;
  //display degree days start
  display.setTextSize(5);
  display.setTextColor(WHITE);
  display.setCursor(6,12);
  digit = (uint8_t)HeatIndex/10.0;
  display.write(digit + ASCII_OFFSET);
  digit = (uint8_t)HeatIndex%10;
  display.write(digit + ASCII_OFFSET);
  display.print(",");
  //Serial.println("HeatIndex");
  //Serial.println(HeatIndex);
  //u16test = HeatIndex*10;
  //Serial.println("HeatIndex*10");
  //Serial.println(u16test);
  //u8modulo = u16test%10;
  //Serial.println("u16test%10");
  //Serial.println(u8modulo);
  digit = (uint8_t)(((uint16_t)(HeatIndex*10))%10);
  display.write(digit + ASCII_OFFSET);
  //display degree days end
}

void DisplayUpdate(uint8_t ViewNo)
{
  display.clearDisplay();
  //draw frame(s)
  display.drawRect(0, 0, display.width(), display.height(), WHITE);
  /*display.drawLine(0,               DISPLAY_SYSTEM_TIME_BOTTOM_LINE,
                   display.width(), DISPLAY_SYSTEM_TIME_BOTTOM_LINE, WHITE);*/
  /*display.drawLine(display.width()/2, DISPLAY_SYSTEM_TIME_BOTTOM_LINE,
                   display.width()/2, display.height(), WHITE);*/
  switch (ViewNo)
  {
    case 0:
    {
      DisplaySystemInfo();
    }
    break;

    case 1:
    {
      DisplayDegreeDays();
    }
    break;

    case 2:
    {
      DisplayTemp();
    }
    break;

    default:
    {

    }
    break;
  }
  display.display();
}


void iButtonPushed(void)
{
 // digitalWrite(PIN_LED, LOW);
 // SetButtonPush();
 //Serial.println("ISR");
  //cSystem.Notify(BUTTON_PUSHED);
  StartTimer(BUTTON_TIMER, BUTTON_PUSH_TIMEOUT);
}

void InitButton(void)
{
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  //pinMode(PIN_BUTTON, INPUT);
  //digitalWrite(PIN_BUTTON, HIGH); //Turn on internal 20K pullup resistor
  //button pressed: read = HIGH; button not pressed: read = LOW;
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), iButtonPushed, HIGH);
}

void UpdateDisplayViewNo(void)
{
  DisplayViewNo++;
  if (DisplayViewNo >= NOF_DISPLAY_VIEWS)
  {
    DisplayViewNo = 0;
  }
}
void Setup()
{
	  //byte count;
	  // put your setup code here, to run once:
	  Serial.begin(9600);
	  Serial.println("Start");
	  // initialize digital pin 13 as an output.
	  pinMode(PIN_LED, OUTPUT);
	  digitalWrite(PIN_LED, HIGH);
	  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
	  // Clear the buffer.
	  display.clearDisplay();
	  display.drawBitmap(0, 0, deer_jumping, 128, 64, 1);
	  display.display();

	  dht.begin();
	  InitSystemTime();
	  InitButton();
	  //DisplayUpdate(0);
	  //delay(1000); //to have a nice 1st time display update
	  InitTimer();
	  StartTimer(SYSTEM_TIMER, ONE_SEC_TIMOUT);
	  StartTimer(DHT11_TIMER, FIVE_SEC_TIMOUT);
	  //StartTimer(DISPLAY_TIMER, ONE_SEC_TIMOUT);
	  //StartTimer(BUTTON_TIMER, BUTTON_READ_TIMEOUT);
	  /*Serial.println("Timeout Array:");
	  for (count = 0; count < NOF_TIMERS; count++)
	  {
	    Serial.println(timeoutarray[count]);
	  }*/
	  /*Serial.println(timeoutarray[1]);
	  Serial.println(timeoutarray[2]);
	  Serial.println(timeoutarray[3]);*/
	  // init done
}

void SetButtonPush(void)
{
  HasButtonBeenPushed = YES;
}

void ClearButtonPush(void)
{
  HasButtonBeenPushed = NO;
}

boolean GetHasButtonBeenPushed(void)
{
  return (HasButtonBeenPushed);
}

void Main(void)
{
  Setup();

  while(1)
  {
	  input = cSystem.GetNotifyList();
	  /*if (input != 0)
	  {
	    Serial.println("Get");
	    Serial.print(input);
	  }*/

	  switch (input)
	  {
	    case UPDATE_SYSTEM_TIME://SystemTimer notify
	    {
	      UpdateSystemTime();
	      if (LedToggle == 1)
	      {
	        digitalWrite(PIN_LED, HIGH);
	        LedToggle = 0;
	      }
	      else
	      {
	        digitalWrite(PIN_LED, LOW);
	        LedToggle = 1;
	      }
	      cSystem.Notify(UPDATE_DISPLAY);
	      cSystem.ClearNotify(UPDATE_SYSTEM_TIME);
	    }
	    break;

	    case UPDATE_DISPLAY: //DisplayUpdate notify
	    {
	      if (GetHasButtonBeenPushed() == YES)
	      {
	        UpdateDisplayViewNo();
	        ClearButtonPush();
	      }
	      DisplayUpdate(DisplayViewNo);
	      cSystem.ClearNotify(UPDATE_DISPLAY);
	    }
	    break;

	    case READ_BUTTON: //Button notify
	    {
	 /*     if (digitalRead(PIN_BUTTON) == HIGH)
	      {
	        HasButtonBeenPushed = YES;
	      }*/
	      cSystem.Notify(UPDATE_DISPLAY);
	      cSystem.ClearNotify(READ_BUTTON);
	    }
	    break;

	    case BUTTON_PUSHED: // Button has been pushed
	    {
	      Serial.println("Pushed");
	      if (digitalRead(PIN_BUTTON) == HIGH)
	      {
	        Serial.println("HIGH");
	        // button still pushed
	        //lastDebounceTime = millis();
	        StartTimer(BUTTON_TIMER, BUTTON_PUSH_TIMEOUT);
	      }
	      else // button has been releashed for 50 ms
	      {
	        /*unsigned long milisec = millis();

	        Serial.println(milisec);
	        Serial.println(lastDebounceTime);*/
	        // button released or bouncing
	        //if ((millis() - lastDebounceTime) > debounceDelay)
	        //{
	          // button released
	          Serial.println("Released");
	          SetButtonPush();
	          cSystem.Notify(UPDATE_DISPLAY);
	          cSystem.ClearNotify(BUTTON_PUSHED);
	       // }
	        // else do nothing - still debouncing
	      }

	      case READ_TEMP:
	      {
	        float Temp;
	        float Humidity;

	        Humidity = dht.readHumidity();
	        Temp = dht.readTemperature();
	        HeatIndex = dht.computeHeatIndex(Temp, Humidity, false);
	        //Serial.print(HeatIndex);
	        cSystem.Notify(UPDATE_DISPLAY);
	        cSystem.ClearNotify(READ_TEMP);
	      }
	    }
	    break;

	    default:
	    {

	    }
	    break;
	  }
  }
}

void SystemTimer(void)
{
  cSystem.Notify(UPDATE_SYSTEM_TIME);
  StartTimer(SYSTEM_TIMER, ONE_SEC_TIMOUT);
}

void DisplayTimer(void)
{
//  cSystem.Notify(UPDATE_DISPLAY);
  StartTimer(DISPLAY_TIMER, ONE_SEC_TIMOUT);
}

void Dht11Timer(void)
{
  cSystem.Notify(READ_TEMP);
  StartTimer(DHT11_TIMER, FIVE_SEC_TIMOUT);
}

void ButtonTimer(void)
{
  cSystem.Notify(BUTTON_PUSHED);
 // StartTimer(BUTTON_TIMER, BUTTON_READ_TIMEOUT);
}
