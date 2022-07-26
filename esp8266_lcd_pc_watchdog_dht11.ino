#include "TickTwo.h" // https://github.com/sstaub/TickTwo
#include <LiquidCrystal_I2C.h> // https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include "DHT.h" // adafruit
#include <Wire.h>

#define DHTPIN 2     //D4 what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);


// hello screen
const char* hello_screen[4] = {
  "      DAW-MRO",
  " ",
  " PC Watchdog Module",
  "  Version: 220602"
};

// Global variables

// for lcd2004
String pc_hostname = "WaitingForRigName...";
String pc_uptime = "??d,??:??:??";
String pc_algo = "???";
String pc_hashrate = "???";
String Temperature = "??.??";
String Pressure = "????.??";
String Humidity = "??.??";
String sIAQ = "??.??";
int data_to_display_on_LCD = 1;

//pc watchdog variables
unsigned long pc_last_seen = 0;
int WD_time = 0;
int WD_max_time = 300;  

// pc watchdog pins
int pin_pc_watchdog = 15; //D8
bool pin_pc_watchdog_active = false;

// for dht11
float humidity_DHT11 = 40;
float temperature_DHT11 = 30;

// Core functions declarations
void updateLCDdisplay(void);
void check_pc_watchdog(void);
void takeMeasurementDHT11(void);

// Timer functions
TickTwo timerLCDdisplay(updateLCDdisplay, 4057, 0, MILLIS);
TickTwo timerWatchdog(check_pc_watchdog, 6211, 0, MILLIS); // checking watchdog every 6211ms
TickTwo timerMeasurementDHT11(takeMeasurementDHT11, 7687, 0, MILLIS); 

// Define lcd address and size
LiquidCrystal_I2C lcd(0x27, 20, 4);


void setup(void)
{
  Wire.begin(5, 4); // D1, D2
  Serial.begin(115200);
  pinMode(pin_pc_watchdog, OUTPUT);

  // prepare lcd
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  
  // display hello screen at boot
  lcd.setCursor(0, 0);
  lcd.print(hello_screen[0]);
  lcd.setCursor(0,1);
  lcd.print(hello_screen[1]);
  lcd.setCursor(0,2);
  lcd.print(hello_screen[2]);
  lcd.setCursor(0,3);
  lcd.print(hello_screen[3]);

  // start dht sensor
  dht.begin();

   // Starting timers
  timerLCDdisplay.start();
  timerWatchdog.start();
  timerMeasurementDHT11.start();
}
 
// Function that is looped forever
void loop(void)
{ 
  // Updating timers, checking if function should run now
  timerLCDdisplay.update();
  timerWatchdog.update();
  timerMeasurementDHT11.update();

  // Checking if there is some data on serial
  if(Serial.available() > 0)
  {
    // Reading all characters until ;
    String recieved = Serial.readStringUntil(';');

    //-------Handshake---------
    if (recieved == "who_are_you") 
    {
      pc_last_seen = millis();
      Serial.println("esp8266_lcd_pc_watchdog_dht11");
      //displayLCD_got_handshake();
    }
    
    //-------Hostname---------
    else if (recieved.substring(0,9) == "Hostname:") 
    {
      pc_last_seen = millis();
      pc_hostname = String(recieved.substring(9));
      Serial.println(pc_hostname);
    }
    
    //-------Uptime---------
    else if (recieved.substring(0,7) == "Uptime:") 
    {
      pc_last_seen = millis();
      pc_uptime = String(recieved.substring(7));
      Serial.println(pc_uptime);
    }

    //-------Algo---------
    else if (recieved.substring(0,5) == "Algo:") 
    {
      pc_last_seen = millis();
      pc_algo = String(recieved.substring(5));
      Serial.println(pc_algo);
    }

    //-------Hashrate---------
    else if (recieved.substring(0,9) == "Hashrate:") 
    {
      pc_last_seen = millis();
      pc_hashrate = String(recieved.substring(9));
      Serial.println(pc_hashrate);
    }
        
    //-------DHT11---------
    else if (recieved == "Get DHT11 temperature") 
    {
      pc_last_seen = millis();
      Serial.println(temperature_DHT11);
    }

        //-------DHT11---------
    else if (recieved == "Get DHT11 humidity") 
    {
      pc_last_seen = millis();
      Serial.println(humidity_DHT11);
    }
  }
}

double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
}

void updateLCDdisplay(void)
{
  if (data_to_display_on_LCD == 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(String(pc_hostname));
    lcd.setCursor(0,1);
    lcd.print(String("Uptime:" + pc_uptime));
    lcd.setCursor(0,2);
    lcd.print(String(pc_algo +":" + pc_hashrate));
    lcd.setCursor(0,3);
    if (WD_time > WD_max_time)
    {
      lcd.print(String("WD Timer: " + String(WD_time) + "/" + String(WD_max_time) + " !!"));
    }
    else
    {
      lcd.print(String("WD Timer: " + String(WD_time) + "/" + String(WD_max_time)));
    }
    
    data_to_display_on_LCD = 2;
  }
  else if (data_to_display_on_LCD == 2)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(String(pc_hostname));
    lcd.setCursor(0,1);
    lcd.print(String("Uptime:" + pc_uptime));
    lcd.setCursor(0,2);
    lcd.print(String("T:" + String(temperature_DHT11, 1) + "C, H:" + String(humidity_DHT11, 1) + "%"));
    lcd.setCursor(0,3);
    lcd.print(String("DewPoint:" + String(dewPointFast(temperature_DHT11, humidity_DHT11), 1) + "C"));
    data_to_display_on_LCD = 1;
  }
}

void check_pc_watchdog(void)
{
  if (pin_pc_watchdog_active == true)
  {
    digitalWrite(pin_pc_watchdog, LOW);
    pin_pc_watchdog_active = false;
  }
  if (WD_time > WD_max_time)
  {
    // using delay, ignoring TickTwo, restarting pc is more important now 
    
    // force shutdown pc
    digitalWrite(pin_pc_watchdog, HIGH);
    delay(5000);
    digitalWrite(pin_pc_watchdog, LOW);
    
    // wait for psu to cool down
    delay(30000);
    
    // start pc
    digitalWrite(pin_pc_watchdog, HIGH);
    delay(1000);
    digitalWrite(pin_pc_watchdog, LOW);
    
    // reset watchdog timer
    WD_time = 0;
    pc_last_seen = millis();
    pin_pc_watchdog_active = true;
  }
  else
  {
    digitalWrite(pin_pc_watchdog, LOW);
    pin_pc_watchdog_active = false;
    WD_time = (millis() - pc_last_seen)/1000;
  }
}

void takeMeasurementDHT11(void)
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  
  // Read humidity as %
  float humidity_DHT11_old = humidity_DHT11;
  humidity_DHT11 = dht.readHumidity();
  
  // Read temperature as Celsius
  float temperature_DHT11_old = temperature_DHT11;
  temperature_DHT11 = dht.readTemperature();
  
  // Check if any reads failed, use old values then (to try again later).
  if (isnan(humidity_DHT11) || isnan(temperature_DHT11)) 
  {
    humidity_DHT11 = humidity_DHT11_old;
    temperature_DHT11 = temperature_DHT11_old;
  }
}
