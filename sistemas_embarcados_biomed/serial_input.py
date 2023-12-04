#-*- coding: utf-8 -*-
import serial
import random

#porta serial do Arduino 
#SERIAL_PORT = '/dev/ttyACM0'  # ou 'COMX' no Windows
#BAUD_RATE = 9600

#def serialinput(SERIAL_PORT = '/dev/ttyACM0', BAUD_RATE = 9600):
#    serialvoltage = int(arduino.readline().decode().strip())    # Lê o valor da frequência cardíaca do Arduino
#    return tensao_atual


# Função para simular a obtenção de dados de tensão do Arduino (substitua pela função real quando em hardware) 
def serial_input():
    # Suponhamos que você tem uma variável chamada "tensao_atual" que contém o valor de tensão
    # Substitua este trecho pela leitura real da porta serial do Arduino
    tensao_atual = random.randint(1500, 5000)  # Supondo valores entre 1.5 e 5 volts
    return tensao_atual
