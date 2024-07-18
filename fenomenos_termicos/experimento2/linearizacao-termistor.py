import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

# Dados fornecidos
medidas = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
ln_R_R0 = np.array([-2.20, -2.02, -1.90, -1.80, -1.65, -1.58, -1.41, -1.30, -1.20, -0.97])
sigma_ln_R_R0 = np.array([0.01] * 10)
inv_T = np.array([0.002894, 0.002923, 0.002941, 0.002958, 0.002976, 0.003003, 0.003039, 0.003076, 0.003095, 0.003144])
sigma_inv_T = np.array([0.000004, 0.000004, 0.000004, 0.000004, 0.000004, 0.000004, 0.000005, 0.000005, 0.000005, 0.000005])

# Definindo a função de linearização simples
def linear_func(x, a, b):
    return a * x + b

# Ajustando a curva
params, covariance = curve_fit(linear_func, inv_T, ln_R_R0, sigma=sigma_ln_R_R0)

# Valores ajustados
a, b = params
lin_fit = linear_func(inv_T, a, b)

# Gráfico
plt.figure(figsize=(10, 6))
plt.errorbar(inv_T, ln_R_R0, yerr=sigma_ln_R_R0, xerr=sigma_inv_T, fmt='o', label='Dados experimentais', ecolor='r', capsize=5)
plt.plot(inv_T, lin_fit, label=f'Ajuste linear: ln(R/R0) = {a:.2e} * 1/T + {b:.2e}', color='blue')
plt.xlabel('1/T (K$^{-1}$)')
plt.ylabel('ln(R/R0)')
plt.title('Gráfico de ln(R/R0) vs 1/T')
plt.legend()
plt.grid(True)
plt.show()
