import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import linregress

# Dados
volume = np.array([17, 16, 15, 14, 13, 12, 11, 10, 9])
pressao = np.array([0, 0.02, 0.08, 0.13, 0.18, 0.24, 0.31, 0.37, 0.44])

# Calcular a regressão linear
slope, intercept, r_value, p_value, std_err = linregress(volume, pressao)

# Coeficientes
print(f"Coeficiente Angular (m): {slope}")
print(f"Coeficiente Linear (b): {intercept}")
print(f"Incerteza do Coeficiente Angular (σ_m): {std_err}")

# Função da linha de regressão
regressao = slope * volume + intercept

# Plotar o gráfico
plt.scatter(volume, pressao, label='Dados Observados', color='blue')
plt.plot(volume, regressao, label='Linha de Regressão', color='red')
plt.xlabel('Volume (ml)')
plt.ylabel('Pressão (kgf/cm²)')
plt.title('Gráfico de Dispersão com Linha de Regressão')
plt.legend()
plt.grid(True)
plt.show()
