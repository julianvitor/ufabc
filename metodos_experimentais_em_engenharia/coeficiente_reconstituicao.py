import numpy as np
import matplotlib.pyplot as plt

# 1. Entrada dos Dados Experimentais
n = np.arange(1, 11)
dt = np.array([600, 520, 440, 380, 340, 300, 280, 260, 260, 200])
U_dt = 20  # Incerteza expandida em ms (k=2)

# 2. Ajuste Exponencial (Linearização via Mínimos Quadrados)
# Equação teórica: ln(dt) = n*ln(epsilon) + ln(2t0)
coeficientes = np.polyfit(n, np.log(dt), 1)
ajuste_dt = np.exp(coeficientes[1]) * np.exp(coeficientes[0] * n)

# 3. Configuração Geral da Figura (Estilo Acadêmico ABNT)
plt.rcParams.update({'font.size': 12})
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

# --- Gráfico 1: Escala Linear ---
ax1.errorbar(n, dt, yerr=U_dt, fmt='bo', ecolor='red', capsize=5, 
             label='Dados experimentais', markersize=6)
ax1.plot(n, ajuste_dt, 'k--', label='Ajuste exponencial')
ax1.set_title('Gráfico 1 - $\Delta t_n$ em função de n')
ax1.set_xlabel('Número do impacto (n)')
ax1.set_ylabel('$\Delta t_n$ [ms]')
ax1.grid(True, linestyle=':', alpha=0.7)
ax1.legend()

# --- Gráfico 2: Escala Semi-log ---
ax2.errorbar(n, dt, yerr=U_dt, fmt='bo', ecolor='red', capsize=5, 
             label='Dados experimentais', markersize=6)
ax2.plot(n, ajuste_dt, 'k--', label='Ajuste exponencial')
ax2.set_yscale('log') # Transforma o eixo y em escala logarítmica
ax2.set_title('Gráfico 2 - Linearização (escala semi-log)')
ax2.set_xlabel('Número do impacto (n)')
ax2.set_ylabel('$\Delta t_n$ [ms]')
ax2.grid(True, which="both", linestyle=':', alpha=0.7)
ax2.legend()

# 4. Adição da Fonte (Exigência ABNT/Prof. Julio)
plt.figtext(0.5, 0.02, "Fonte: Elaborada pelos autores.", ha="center", fontsize=12)

# Ajuste de layout para dar espaço à legenda de fonte na parte inferior
plt.subplots_adjust(bottom=0.15)

# 5. Exportação e Exibição
plt.savefig('graficos_metodo_c.png', dpi=300, bbox_inches='tight')
print("Gráfico salvo com sucesso como 'graficos_metodo_c.png'!")
plt.show()