#-*- coding: utf-8 -*-
import os
import time
import matplotlib
matplotlib.use('Agg')  # Configura o modo 'Agg' para renderização sem GUI
import matplotlib.pyplot as plt
from serial_input import serial_input

plt.style.use('dark_background') #estilo

# Função para criar e salvar o gráfico como imagem PNG
def gerar_grafico(tempo_rodando = 5):
    tempo_inicial = time.time()
    tempos = []
    tensoes = []
    intervalo_millisegundos = 273 / 1000  # Converter para segundos

    while time.time() - tempo_inicial <= tempo_rodando:  # Roda por 6 segundos
        tempo_atual = time.time() - tempo_inicial

        if tempo_atual >= len(tempos) * intervalo_millisegundos:
            valor_tensao = serial_input()

            print("Tempo decorrido:", tempo_atual, "s")
            print("Valor de tensão:", valor_tensao)

            tempos.append(tempo_atual)
            tensoes.append(valor_tensao)

    plt.plot(tempos, tensoes)
    plt.xlabel('Tempo (s)')
    plt.ylabel('Tensão (V)')
    plt.title('Gráfico de Tensão em Função do Tempo')
    plt.grid()
    
    #caminho para exibir no html
    nome_arquivo = 'grafico_tensao.png'
    caminho_static = os.path.join(os.path.dirname(__file__), 'static')
    caminho_salvar = os.path.join(caminho_static, nome_arquivo)

    plt.savefig(caminho_salvar, dpi=300, bbox_inches='tight', format='png')
    plt.clf() #limpa a figura para evitar sobreprosição
