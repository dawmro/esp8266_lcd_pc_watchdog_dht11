# esp8266_lcd_pc_watchdog_dht11
PC hardware watchdog with LCD display and atmospheric parameters reading capability. Based on the esp8266 module,  the 2004LCD display and the dht11 sensor.

- Hardware PC watchdog
- Reads temperature and humidity 
- Integrated 20x4 LCD display
- Restarts frozen PC
- Receives data from PC
- Sends data to PC 
- Shows data on LCD display.


![alt text](https://github.com/dawmro/esp8266_lcd_pc_watchdog_dht11/blob/main/images/3_displaying_data.png?raw=true)


## Schematic
![alt text](https://github.com/dawmro/esp8266_lcd_pc_watchdog_dht11/blob/main/schematic/schematic_esp8266_lcd_pc_watchdog_dht11.PNG?raw=true)


## Usage

1. Assemble hardware
2. Upload sketch to esp8266
3. Run Python script


## Bill of Materials

1x ESP8266 WeMos D1 mini ESP-12F WiFi

1x 2004 lcd + i2c

1x dht11 temperature and humidity sensor

1x pc817

1x 390 ohm resistor

1x 10k resistor

1x some wires
