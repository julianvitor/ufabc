#-*- coding: utf-8 -*-
from flask import Flask, render_template
import random
from hr_calc import hr_calc
from gerar_grafico import gerar_grafico

app = Flask(__name__)



# Rota para exibir a página web com o valor da frequência cardíaca
@app.route('/')
def index():
    
    gerar_grafico(4)
    heart_rate = hr_calc()
    #heart_rate = random.randrange(40, 220)
    
    return render_template('index.html', heart_rate=heart_rate)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
