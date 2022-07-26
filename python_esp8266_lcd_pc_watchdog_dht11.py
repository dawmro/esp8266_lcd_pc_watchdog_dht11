#Python Code
import serial
import time
import ast
from datetime import datetime
import sqlite3
import socket
from random import randint
from uptime import uptime
import serial.tools.list_ports
import json

import os


board_name_on_comm_list = "CH340"
handshake_reply_desired = "esp8266_lcd_pc_watchdog_dht11"
serial_baudrate=115200
serial_timeout=5

        
def tStamp():

    return str("["+datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')+" UTC]")
 
 
def send_pc_hostname(hostname, serialcomm):

    to_send = str("Hostname:" + hostname + ";")
    print(to_send)
    print(tStamp() + " Sending hostname: " + str(serialcomm.write(to_send.encode())))
    response = ''
    try:
        response = serialcomm.readline().decode('utf-8').rstrip()
    except:
        response = serialcomm.readline()
    print(tStamp() + " Length of response: " + str(len(response)))
    print(response)
    
    
def send_pc_uptime(uptime, serialcomm):

    to_send = str("Uptime:" + uptime + ";")
    print(to_send)
    print(tStamp() + " Sending uptime: " + str(serialcomm.write(to_send.encode())))
    response = ''
    try:
        response = serialcomm.readline().decode('utf-8').rstrip()
    except:
        response = serialcomm.readline()
    print(tStamp() + " Length of response: " + str(len(response)))
    print(response)
    
    
def send_pc_algo(algo, serialcomm):

    to_send = str("Algo:" + algo + ";")
    print(to_send)
    print(tStamp() + " Sending algo: " + str(serialcomm.write(to_send.encode())))
    response = ''
    try:
        response = serialcomm.readline().decode('utf-8').rstrip()
    except:
        response = serialcomm.readline()
    print(tStamp() + " Length of response: " + str(len(response)))
    print(response)
    
    
def send_pc_hashrate(hashrate, hashrate_unit, serialcomm):

    to_send = str("Hashrate:" + hashrate + "" + hashrate_unit + ";")
    print(to_send)
    print(tStamp() + " Sending hashrate: " + str(serialcomm.write(to_send.encode())))
    response = ''
    try:
        response = serialcomm.readline().decode('utf-8').rstrip()
    except:
        response = serialcomm.readline()
    print(tStamp() + " Length of response: " + str(len(response)))
    print(response)
 
 
    
def scan_ports_and_send_handshahe(handshake_reply, serialcomm):
    arduino_ports = [
        p.device
        for p in serial.tools.list_ports.comports()
        if board_name_on_comm_list in p.description  # may need tweaking to match new arduinos
    ]
    if not arduino_ports:
        print(tStamp() + " No Arduino found :(")
    elif len(arduino_ports) > 1:
        print(tStamp() + " Multiple Arduinos found, will try each of them:")
    else:
        print(tStamp() + " Single Arduino found")
    
    for p in arduino_ports:
        print(tStamp() + " Trying to connect to: " + p)
        serialcomm = serial.Serial(port = str(p), baudrate = serial_baudrate)
        serialcomm.timeout = serial_timeout
    
        if serialcomm.isOpen():
            print(tStamp() + " Closing serial port: " + str(serialcomm.close()))
            
        print(tStamp() + " Opening serial port: " + str(serialcomm.open()))
        print(tStamp() + " Checking if serial port is open: " + str(serialcomm.isOpen()))
        
        to_send = "who_are_you;"
        print(to_send)
        print(tStamp() + " Sending handshake: " + str(serialcomm.write(to_send.encode())))
        response = ''
        try:
            response = serialcomm.readline().decode('utf-8').rstrip()
        except:
            response = serialcomm.readline()
        print(tStamp() + " Closing serial port: " + str(serialcomm.close()))    
        print(tStamp() + " Length of response: " + str(len(response)))
        print(response)
        print(handshake_reply_desired)

        if response == handshake_reply_desired:
            handshake_reply = response
            print(tStamp() + " Desired Arduino found on: " + p)
        
    return handshake_reply, serialcomm
    
    
def get_DHT11_humidity(serialcomm):

    print(tStamp() + " Sending Get DHT11 humidity request: " + str(serialcomm.write("Get DHT11 humidity;".encode())))
    response = ''
    print(tStamp() + " Starting getting response")
    try:
        response = serialcomm.readline().decode('utf-8').rstrip()
        print(tStamp() + " Got response")
    except:
        print(tStamp() + " Something fucked up with response")
        print(tStamp() + " Will try again in a while")
    
    if response != '':
        print(tStamp() + " Length of response: " + str(len(response)))
        print(response)    
        

def get_DHT11_temperature(serialcomm):

    print(tStamp() + " Sending Get DHT11 temperature request: " + str(serialcomm.write("Get DHT11 temperature;".encode())))
    response = ''
    print(tStamp() + " Starting getting response")
    try:
        response = serialcomm.readline().decode('utf-8').rstrip()
        print(tStamp() + " Got response")
    except:
        print(tStamp() + " Something fucked up with response")
        print(tStamp() + " Will try again in a while")
    
    if response != '':
        print(tStamp() + " Length of response: " + str(len(response)))
        print(response)    
      
 


def main():

    handshake_reply = ""
    serialcomm = ""
    

    
    while True:
        print("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n")
        
        
        
        # get hostname
        hostname = socket.gethostname()
        
        #get current algo
        #placeholder, todo get algo from on miner log file
        algo = "ETH"
        
        #get current hashrate
        #placeholder, todo read hashrate from miner log file
        hashrate = str(str(357) +"." + str(randint(10,99)))
        
        # get hashrate unit
        #placeholder, todo read hashrate unit from miner log file
        hashrate_unit = "Mh/s"
         
        t = int(uptime())
        # extracting hours, minutes, seconds & days from t
        # variable (which stores total time in seconds)
        mins, sec = divmod(t, 60)
        hour, mins = divmod(mins, 60)
        days, hour = divmod(hour, 24)
         
        # formatting the time in readable form
        # (format = x days, HH:MM:SS)
        pc_uptime = f"{days}d,{hour:02}:{mins:02}:{sec:02}"
        
        # sending handshake to arduino
        if handshake_reply != handshake_reply_desired:
            try:
                handshake_reply, serialcomm = scan_ports_and_send_handshahe(handshake_reply, serialcomm)
            except:
                print(tStamp() + " Unable to open port...")
                
        if handshake_reply == handshake_reply_desired:
        
            if serialcomm.isOpen():
                print(tStamp() + " Closing serial port: " + str(serialcomm.close()))
                
            try:    
                print(tStamp() + " Opening serial port: " + str(serialcomm.open()))
            except:
                handshake_reply = ""
                continue
            print(tStamp() + " Checking if serial port is open: " + str(serialcomm.isOpen()))
            
            try:
                send_pc_hostname(hostname, serialcomm)
                send_pc_uptime(pc_uptime, serialcomm)
                send_pc_algo(algo, serialcomm)
                send_pc_hashrate(hashrate, hashrate_unit, serialcomm)
                       
                get_DHT11_temperature(serialcomm)
                get_DHT11_humidity(serialcomm)
                
            except:
                print(tStamp() + " Communication error...")

            print(tStamp() + " Closing serial port: " + str(serialcomm.close()))
       
        print("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n")
        time.sleep(10)

    
if __name__ == '__main__':
    main()