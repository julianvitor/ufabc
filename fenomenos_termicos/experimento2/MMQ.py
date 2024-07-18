import numpy as np

# Dados fornecidos
ln_R_R0 = np.array([-2.20, -2.02, -1.90, -1.80, -1.65, -1.58, -1.41, -1.30, -1.20, -0.97])
inv_T = np.array([0.002894, 0.002923, 0.002941, 0.002958, 0.002976, 0.003003, 0.003039, 0.003076, 0.003095, 0.003144])
sigma_ln_R_R0 = np.array([0.01] * 10)

# Função de ajuste linear
def linear_func(x, a, b):
    return a * x + b

# Ajuste linear usando polyfit com peso (1/erro^2)
weights = 1 / sigma_ln_R_R0**2
params, covariance = np.polyfit(ln_R_R0, inv_T, 1, w=weights, cov=True)
A, B = params
std_A, std_B = np.sqrt(np.diag(covariance))

# Imprimindo os resultados
print(f"A: {A:.6e} ± {std_A:.6e}")
print(f"B: {B:.6e} ± {std_B:.6e}")

# Expressão de temperatura em função da resistência
# 1/T = A * ln(R/R0) + B
# T = 1 / (A * ln(R/R0) + B)

# Desenhar a reta média no gráfico
plt.figure(figsize=(10, 6))
plt.errorbar(inv_T, ln_R_R0, yerr=sigma_ln_R_R0, xerr=sigma_inv_T, fmt='o', label='Dados experimentais', ecolor='r', capsize=5)
plt.plot(inv_T, linear_func(ln_R_R0, A, B), label=f'Ajuste linear: 1/T = {A:.2e} * ln(R/R0) + {B:.2e}', color='blue')
plt.xlabel('1/T (K$^{-1}$)')
plt.ylabel('ln(R/R0)')
plt.title('Gráfico de ln(R/R0) vs 1/T')
plt.legend()
plt.grid(True)
plt.show()
