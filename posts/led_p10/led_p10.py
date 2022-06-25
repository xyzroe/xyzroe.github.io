#!/usr/bin/env python
# -*- coding: utf_8 -*-
import binascii
import codecs
import socket
import serial
from time import sleep
import sys
import os

file = '/media/itg/bid.txt'

UART_PORT = '/dev/tty.wchusbserial14130'
UART_SPEED = '9600'

TCP_IP = '192.168.5.61'
TCP_PORT = 9761


def restart_program():
    python = sys.executable
    os.execl(python, python, * sys.argv)

def sendDataUART(cmd):
    global error
    try:
        error = 0
        ser = serial.Serial(UART_PORT, UART_SPEED, timeout=0)
        ser.write(cmd)
        data = ser.read()
        ser.close()
        print ("Received data:", data)
    except:
        print('UART ERROR')
        error = 1

def sendDataTCP(cmd):
    global error
    try:
        BUFFER_SIZE = 4096
        error = 0
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((TCP_IP, TCP_PORT))
        s.send(cmd)
        data = s.recv(BUFFER_SIZE)
        s.close()
        print ("Received data:", data)
    except:
        print('TCP ERROR')
        error = 1

def makeCmd():
    global number
    #print(packet)
    while 1:
        number2 = ''
        try:
            f = open(file, 'r')
            number2 = f.readline()
            number2 = int(number2)
            print(number2)
            f.close()
        except:
            print('FILE OPEN ERROR')
            sleep(5)
            restart_program()
        if error == 1:
            number = 0
        if number2 != '':
            if number2 != number:
                packet = '0x07 0x30 0x20 0x20 0x20 0x20 0x20 0x20 0x20 0x20'
                packet = [int(x, 16) for x in packet.split(' ')]
                number = number2
                ones = number % 10
                tens = int((number % 100 - ones) / 10)
                hund = (number % 1000) // 100
                touz = number // 1000
                if touz > 0:
                    packet[6] = int(touz) + 48
                    packet[7] = int(hund) + 48
                    packet[8] = int(tens) + 48
                if hund > 0:
                    packet[7] = int(hund) + 48
                    packet[8] = int(tens) + 48
                if tens > 0:
                    packet[8] = int(tens) + 48
                packet[9] = int(ones) + 48
                checksum = 0
                for el in packet:
                    checksum ^= ord(chr(el))
                packet.append(checksum)
                cmd = ""
                for el in packet:
                    my = str.encode(chr(el))
                    cmd = cmd + chr(el)
                cmd = str.encode(cmd)
                #print(binascii.hexlify(cmd))
                print('Comand data:',cmd)
                sendDataTCP(cmd)
                #sendDataUART(cmd)
        sleep(5)

if __name__ == "__main__":
    global number, error
    number, error = 0, 0
    makeCmd()
