"""
Geração de gráficos CORRIGIDOS para o Relatório de Luminosidade - Grupo 1
ESTO017-17 – Métodos Experimentais em Engenharia

CORRRECAO: d_real = H_tubo - d_barbante  (47,1 - d_barbante)
           O tubo é OPACO e absorvente. A lanterna desce para dentro do tubo.
           Quanto maior d_barbante, menor a distância real sensor-lanterna.
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch
import os
import warnings
warnings.filterwarnings('ignore')

# ─────────────────────────────────────────────────────────────────────
# DADOS EXPERIMENTAIS
# ─────────────────────────────────────────────────────────────────────
d_barbante = np.array([8.3, 13.3, 18.3, 23.3, 28.3, 33.3, 38.3])  # cm  (início tubo -> ponta lanterna)
H_tubo = 47.1   # cm  (altura do tubo)

# DISTÂNCIA REAL entre sensor (fundo do tubo) e lanterna:
d_real = H_tubo - d_barbante   # cm
# Resulta em: [38.8, 33.8, 28.8, 23.8, 18.8, 13.8, 8.8]

E_lab  = np.array([1632, 2041, 2685, 3473, 4641, 5712, 7067])  # lx
E_corr = np.array([1191, 2091, 2425, 2929, 3816, 5405, 7361])  # lx
E_arm  = np.array([ 876, 1644, 2230, 2563, 3559, 4955, 5788])  # lx

print("=== DISTÂNCIAS CORRIGIDAS ===")
print(f"{'d_barbante':>12} | {'d_real':>8} | {'E_lab':>7} | {'E_corr':>7} | {'E_arm':>7}")
print("-"*55)
for i in range(len(d_barbante)):
    print(f"{d_barbante[i]:>12.1f} | {d_real[i]:>8.1f} | {E_lab[i]:>7.0f} | {E_corr[i]:>7.0f} | {E_arm[i]:>7.0f}")

# Incertezas expandidas (k=2, 95%)
U_d = np.full_like(d_real, 0.10)           # cm
U_E_lab  = 0.115 * E_lab                   # lx
U_E_corr = 0.115 * E_corr                  # lx
U_E_arm  = 0.115 * E_arm                   # lx

out_dir = r"c:\Users\julian\Downloads\projeto luminosidade\graficos"
os.makedirs(out_dir, exist_ok=True)

# Paleta de cores
COR_LAB  = "#1f77b4"   # azul
COR_CORR = "#ff7f0e"   # laranja
COR_ARM  = "#2ca02c"   # verde
COR_TEO  = "#d62728"   # vermelho

# ─────────────────────────────────────────────────────────────────────
# FIGURA 1 — E vs d_real (escala linear, com barras de incerteza)
# Agora E DECRESCE com d_real, como esperado pela lei inverso quadrado
# ─────────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(9, 6), dpi=150)

ax.errorbar(d_real, E_lab,  xerr=U_d, yerr=U_E_lab,
            fmt='o-', color=COR_LAB,  capsize=5, capthick=1.5,
            linewidth=1.8, markersize=7, label='Laboratório', zorder=5)
ax.errorbar(d_real, E_corr, xerr=U_d, yerr=U_E_corr,
            fmt='s-', color=COR_CORR, capsize=5, capthick=1.5,
            linewidth=1.8, markersize=7, label='Corredor', zorder=5)
ax.errorbar(d_real, E_arm,  xerr=U_d, yerr=U_E_arm,
            fmt='^-', color=COR_ARM,  capsize=5, capthick=1.5,
            linewidth=1.8, markersize=7, label='Armário', zorder=5)

# Curva teórica ancorada no ponto médio
d_fit = np.linspace(7, 42, 300)
# Ancora no ponto 4 (d=23.8 cm, E_lab=3473)
k_teo = 3473 * 23.8**2
ax.plot(d_fit, k_teo / d_fit**2, '--', color=COR_TEO, linewidth=1.8,
        label='Referência teórica: E ∝ 1/d²', zorder=4, alpha=0.8)

ax.set_xlabel('Distância real sensor–lanterna  $d$ (cm)', fontsize=13)
ax.set_ylabel('Iluminância  $E$ (lx)', fontsize=13)
ax.set_title('Figura 1 — Iluminância vs. Distância Real (escala linear)\n'
             'Sensor STK3311-X | Incertezas expandidas k = 2 (95%)', fontsize=12)
ax.legend(fontsize=10, framealpha=0.9)
ax.grid(True, linestyle='--', alpha=0.5)
ax.tick_params(labelsize=11)
ax.set_xlim(5, 42)
ax.invert_xaxis()   # d pequeno (lanterna perto) à direita → E alto

ax.annotate('Lanterna próxima\ndo sensor → E alto',
            xy=(8.8, 7067), xytext=(14, 6500),
            fontsize=9, color='#333333',
            arrowprops=dict(arrowstyle='->', color='gray', lw=1.2))
ax.annotate('Lanterna distante\ndo sensor → E baixo',
            xy=(38.8, 1632), xytext=(30, 2500),
            fontsize=9, color='#333333',
            arrowprops=dict(arrowstyle='->', color='gray', lw=1.2))

plt.tight_layout()
fig.savefig(os.path.join(out_dir, 'Fig1_E_vs_d_linear.png'), bbox_inches='tight')
plt.close()
print("\nFigura 1 salva.")

# ─────────────────────────────────────────────────────────────────────
# MMQ — ajuste log-log
# ─────────────────────────────────────────────────────────────────────
def mmq_linear(X, Y):
    n = len(X)
    sumX  = X.sum(); sumY  = Y.sum()
    sumX2 = (X**2).sum(); sumXY = (X*Y).sum()
    D = n*sumX2 - sumX**2
    A = (n*sumXY - sumX*sumY) / D
    B = (sumY - A*sumX) / n
    Y_hat = A*X + B
    s2 = ((Y - Y_hat)**2).sum() / (n-2) if n > 2 else 0
    uA = np.sqrt(s2/D) if D != 0 else 0
    return A, B, uA, np.sqrt(s2)

lnD = np.log(d_real)
lnE_lab  = np.log(E_lab)
lnE_corr = np.log(E_corr)
lnE_arm  = np.log(E_arm)

A_lab,  B_lab,  uA_lab,  s_lab  = mmq_linear(lnD, lnE_lab)
A_corr, B_corr, uA_corr, s_corr = mmq_linear(lnD, lnE_corr)
A_arm,  B_arm,  uA_arm,  s_arm  = mmq_linear(lnD, lnE_arm)

print(f"\n=== EXPOENTES MMQ CORRIGIDOS ===")
print(f"Laboratório:  n = {A_lab:.4f}  ±  {uA_lab:.4f}  (u_A, k=1)")
print(f"Corredor:     n = {A_corr:.4f}  ±  {uA_corr:.4f}")
print(f"Armário:      n = {A_arm:.4f}  ±  {uA_arm:.4f}")
print(f"Teórico:      n = -2.0000")

# Z'-score
for nome, A, uA in [('Lab', A_lab, uA_lab), ('Corr', A_corr, uA_corr), ('Arm', A_arm, uA_arm)]:
    zs = abs(A - (-2.0)) / (2*uA)
    print(f"Z'-score {nome}: {zs:.2f}")

# ─────────────────────────────────────────────────────────────────────
# FIGURA 2 — ln(E) vs ln(d) com MMQ e curva teórica
# ─────────────────────────────────────────────────────────────────────
d_fit2 = np.linspace(7.5, 41, 300)
lnD_fit = np.log(d_fit2)

fig, ax = plt.subplots(figsize=(9, 6), dpi=150)

ax.scatter(lnD, lnE_lab,  color=COR_LAB,  marker='o', s=70, zorder=5, label='Lab. (dados)')
ax.scatter(lnD, lnE_corr, color=COR_CORR, marker='s', s=70, zorder=5, label='Corredor (dados)')
ax.scatter(lnD, lnE_arm,  color=COR_ARM,  marker='^', s=70, zorder=5, label='Armário (dados)')

ax.plot(lnD_fit, A_lab*lnD_fit  + B_lab,  '--', color=COR_LAB,  linewidth=1.8,
        label=f'MMQ Lab: n = {A_lab:.3f} ± {uA_lab:.3f}')
ax.plot(lnD_fit, A_corr*lnD_fit + B_corr, '--', color=COR_CORR, linewidth=1.8,
        label=f'MMQ Corredor: n = {A_corr:.3f} ± {uA_corr:.3f}')
ax.plot(lnD_fit, A_arm*lnD_fit  + B_arm,  '--', color=COR_ARM,  linewidth=1.8,
        label=f'MMQ Armário: n = {A_arm:.3f} ± {uA_arm:.3f}')

# Curva teórica ancorando no centróide dos dados
B_teo = np.mean(lnE_lab) - (-2.0)*np.mean(lnD)
ax.plot(lnD_fit, -2.0*lnD_fit + B_teo, '-', color=COR_TEO, linewidth=2.2,
        label='Lei teórica: n = −2')

ax.set_xlabel('ln($d$)  [ln(cm)]', fontsize=13)
ax.set_ylabel('ln($E$)  [ln(lx)]', fontsize=13)
ax.set_title('Figura 2 — Ajuste Log-Log por MMQ\n'
             'Expoente experimental vs. Lei do Inverso do Quadrado (n = −2)', fontsize=12)
ax.legend(fontsize=9.5, framealpha=0.9, loc='upper right')
ax.grid(True, linestyle='--', alpha=0.5)
ax.tick_params(labelsize=11)

# Anotação da declividade esperada
ax.annotate('n = −2\n(esperado)', xy=(np.log(15), -2.0*np.log(15)+B_teo),
            xytext=(np.log(20), -2.0*np.log(20)+B_teo+0.4),
            fontsize=9, color=COR_TEO,
            arrowprops=dict(arrowstyle='->', color=COR_TEO, lw=1))

plt.tight_layout()
fig.savefig(os.path.join(out_dir, 'Fig2_loglog_MMQ.png'), bbox_inches='tight')
plt.close()
print("Figura 2 salva.")

# ─────────────────────────────────────────────────────────────────────
# FIGURA 3 — Coeficientes de sensibilidade (diferença finita) CORRIGIDOS
# ─────────────────────────────────────────────────────────────────────
def grad_central(d, E):
    c = np.full(len(d), np.nan)
    for i in range(1, len(d)-1):
        c[i] = (E[i+1] - E[i-1]) / (d[i+1] - d[i-1])
    return c

# d_real é DECRESCENTE (38.8 → 8.8); para diferença finita em ordem natural:
c_lab  = grad_central(d_real, E_lab)
c_corr = grad_central(d_real, E_corr)
c_arm  = grad_central(d_real, E_arm)

# Coeficiente teórico: c = n*E/d (com n = -2, expoente ajustado)
c_teo_lab  = -2 * E_lab  / d_real
c_teo_corr = -2 * E_corr / d_real
c_teo_arm  = -2 * E_arm  / d_real

c_adj_lab  = A_lab  * E_lab  / d_real
c_adj_corr = A_corr * E_corr / d_real
c_adj_arm  = A_arm  * E_arm  / d_real

fig, axes = plt.subplots(1, 3, figsize=(14, 5.5), dpi=150)
ambientes = [
    ('Laboratório', d_real, E_lab,  c_lab,  c_teo_lab,  c_adj_lab,  COR_LAB),
    ('Corredor',    d_real, E_corr, c_corr, c_teo_corr, c_adj_corr, COR_CORR),
    ('Armário',     d_real, E_arm,  c_arm,  c_teo_arm,  c_adj_arm,  COR_ARM),
]

for ax, (nome, d, E, c_exp, c_teo, c_adj, cor) in zip(axes, ambientes):
    mask = ~np.isnan(c_exp)
    ax.bar(d[mask], c_exp[mask], width=3.2, color=cor, alpha=0.7,
           label='Dif. finita (exp.)', zorder=3, edgecolor='gray')
    ax.plot(d, c_teo, 'r--', linewidth=2, label='Deriv. analítica (n=−2)', zorder=4)
    ax.plot(d, c_adj, 'k:', linewidth=1.8, label=f'Deriv. analítica (n={A_lab:.2f})', zorder=4)
    ax.axhline(0, color='black', linewidth=0.8)
    ax.set_xlabel('$d$ (cm)', fontsize=11)
    ax.set_ylabel('$c_d$ = ∂E/∂d  (lx/cm)', fontsize=10)
    ax.set_title(nome, fontsize=12, fontweight='bold')
    ax.legend(fontsize=8.5)
    ax.grid(True, linestyle='--', alpha=0.4)
    ax.tick_params(labelsize=10)

fig.suptitle('Figura 3 — Coeficientes de Sensibilidade ∂E/∂d (lx/cm)\n'
             'Diferença Finita (experimental) vs. Derivada Analítica (teórica e ajustada)',
             fontsize=12, y=1.01)
plt.tight_layout()
fig.savefig(os.path.join(out_dir, 'Fig3_coef_sensibilidade.png'), bbox_inches='tight')
plt.close()
print("Figura 3 salva.")

# ─────────────────────────────────────────────────────────────────────
# FIGURA 4 — Comparação dos expoentes (barras + Z'-score)
# ─────────────────────────────────────────────────────────────────────
expoentes = [A_lab, A_corr, A_arm, -2.0]
incertezas_exp = [2*uA_lab, 2*uA_corr, 2*uA_arm, 0]
rotulos = ['Laboratório\n(MMQ)', 'Corredor\n(MMQ)', 'Armário\n(MMQ)', 'Teórico\n(n = −2)']
cores_bar = [COR_LAB, COR_CORR, COR_ARM, COR_TEO]

fig, ax = plt.subplots(figsize=(8, 5.5), dpi=150)
bars = ax.bar(rotulos, expoentes, yerr=incertezas_exp, color=cores_bar,
              capsize=8, error_kw={'linewidth':2, 'ecolor':'black'},
              alpha=0.85, edgecolor='black', linewidth=0.8, width=0.55, zorder=3)

ax.axhline(-2, color=COR_TEO, linestyle='--', linewidth=2.0,
           label='n = −2 (teórico)', zorder=2)
ax.axhline(0, color='gray', linestyle='-', linewidth=0.7, zorder=1)

# Valores nas barras
for bar, val in zip(bars, expoentes):
    ypos = val - 0.12 if val < 0 else val + 0.05
    ax.text(bar.get_x() + bar.get_width()/2, ypos,
            f'{val:.3f}', ha='center', va='top', fontsize=11, fontweight='bold', color='white' if val < -0.5 else 'black')

# Z'-score annotation
zscores = [abs(a-(-2))/(2*ua) if ua>0 else 0 for a, ua in zip(expoentes[:3], [uA_lab, uA_corr, uA_arm])]
for i, (bar, z) in enumerate(zip(bars[:3], zscores)):
    compativel = "incompat." if z > 3 else "compat." if z < 2 else "zona cinza"
    ax.text(bar.get_x() + bar.get_width()/2, -0.05,
            f"Z'={z:.1f}\n({compativel})", ha='center', va='top', fontsize=8.5,
            color='darkred' if z > 3 else 'darkgreen',
            bbox=dict(boxstyle='round,pad=0.2', facecolor='lightyellow',
                      edgecolor='darkred' if z>3 else 'darkgreen', alpha=0.85))

ax.set_ylabel('Expoente  n  (lei de potência  E = k·d^n)', fontsize=12)
ax.set_title('Figura 4 — Expoente Ajustado por Ambiente vs. Valor Teórico\n'
             '(Incertezas expandidas: k = 2; 95%)', fontsize=12)
ax.legend(fontsize=10)
ax.grid(axis='y', linestyle='--', alpha=0.5)
ax.tick_params(labelsize=11)
ax.set_ylim(min(expoentes) - 0.6, 0.3)

plt.tight_layout()
fig.savefig(os.path.join(out_dir, 'Fig4_expoentes_comparacao.png'), bbox_inches='tight')
plt.close()
print("Figura 4 salva.")

# ─────────────────────────────────────────────────────────────────────
# FIGURA 5 — Balanço de incertezas (ponto 4: d=23,8 cm; E=3473 lx)
# ─────────────────────────────────────────────────────────────────────
d4 = 23.8; E4 = 3473.0

u_reg  = 0.05/np.sqrt(3)   # incerteza da régua para d_barbante
u_marc = 0.05/np.sqrt(3)   # marcação barbante
u_H    = 0.05/np.sqrt(3)   # altura do tubo

# Coef. de sensibilidade: c_d = n*E/d (com n do ajuste)
c_d_adj = abs(A_lab * E4 / d4)   # lx/cm
c_d_teo = abs(-2    * E4 / d4)   # lx/cm

u_E_reg  = c_d_adj * u_reg
u_E_marc = c_d_adj * u_marc
u_E_H    = c_d_adj * u_H
u_E_tol  = 0.10 * E4 / np.sqrt(3)
u_E_res  = (0.027/2) / np.sqrt(3)

componentes = ['Régua\n(d_barbante)', 'Marcação\nbarbante', 'Altura\ndo tubo (H)',
               'Tolerância\nsensor (±10%)', 'Resolução\n(1 LSB = 0,027 lx)']
valores = [u_E_reg, u_E_marc, u_E_H, u_E_tol, u_E_res]
cores_comp = ['#5b9bd5', '#70ad47', '#ffd966', '#ed7d31', '#ae7bff']

u_c = np.sqrt(sum(v**2 for v in valores))

fig, ax = plt.subplots(figsize=(9.5, 5.5), dpi=150)
bars = ax.barh(componentes, valores, color=cores_comp, edgecolor='black',
               linewidth=0.7, alpha=0.88)

for bar, val in zip(bars, valores):
    ax.text(val + 0.3, bar.get_y() + bar.get_height()/2,
            f'{val:.2f} lx  ({val/E4*100:.3f}%)',
            va='center', ha='left', fontsize=10, fontweight='bold')

ax.axvline(u_c, color='red', linestyle='--', linewidth=2,
           label=f'u_c = {u_c:.2f} lx  (combinada)\nU_c = {2*u_c:.2f} lx  (k=2; 95%)')

ax.set_xlabel('Incerteza padrão  u  (lx)', fontsize=12)
ax.set_title(f'Figura 5 — Balanço de Incertezas Padrão\n'
             f'Ponto 4: d = {d4} cm,  E = {E4:.0f} lx  (Laboratório)', fontsize=11)
ax.legend(fontsize=10, loc='lower right')
ax.grid(axis='x', linestyle='--', alpha=0.5)
ax.tick_params(labelsize=10)
ax.set_xlim(0, max(valores)*1.55)

plt.tight_layout()
fig.savefig(os.path.join(out_dir, 'Fig5_balanco_incertezas.png'), bbox_inches='tight')
plt.close()
print("Figura 5 salva.")

# ─────────────────────────────────────────────────────────────────────
# DIAGRAMA DE ISHIKAWA — atualizado (tubo OPACO)
# ─────────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(15, 9), dpi=150)
ax.set_xlim(0, 15); ax.set_ylim(0, 9)
ax.axis('off')
ax.set_facecolor('#f5f6fa')
fig.patch.set_facecolor('#f5f6fa')

ax.set_title('Diagrama de Causa-Efeito (Ishikawa)\n'
             'Mensurando: Iluminância E (lx) — Sensor STK3311-X | ESTO017-17 — Grupo 1',
             fontsize=13, fontweight='bold', pad=14)

SPINE_Y = 4.5
ax.annotate('', xy=(13.2, SPINE_Y), xytext=(1.5, SPINE_Y),
            arrowprops=dict(arrowstyle='->', color='#2c3e50', lw=3.5))

rect = FancyBboxPatch((13.2, 3.7), 1.7, 1.6, boxstyle='round,pad=0.12',
                      facecolor='#e74c3c', edgecolor='#922b21', linewidth=2.5)
ax.add_patch(rect)
ax.text(14.05, SPINE_Y, 'E\n(lx)', ha='center', va='center',
        fontsize=13, fontweight='bold', color='white')

# Categorias e causas
cfg_top = {
    'FONTE DE LUZ': {'x': 3.2, 'y': 8.0, 'cor': '#2980b9',
        'causas': ['Variação de potência\nda lanterna',
                   'Espectro de emissão\n(lanterna não ideal)',
                   'Geometria da fonte\n(não ponto)']},
    'DISTÂNCIA': {'x': 6.5, 'y': 8.0, 'cor': '#27ae60',
        'causas': ['Resolução da régua\n(± 0,5 mm)',
                   'Marcação do barbante\n(± 0,5 mm)',
                   'Altura do tubo H\n(± 0,5 mm)']},
    'SENSOR / IMPLEMENTAÇÃO': {'x': 10.0, 'y': 8.0, 'cor': '#8e44ad',
        'causas': ['Tolerância ±10%\n(STK3311-X)',
                   'Filtros/média temporal\n(SO Android)',
                   'Configuração de ganho\n(driver Android)',
                   'Resposta espectral\n(pico 550 nm)']},
}

cfg_bot = {
    'MEIO — TUBO (OPACO)': {'x': 4.0, 'y': 1.2, 'cor': '#e67e22',
        'causas': ['Absorção da luz nas\nparedes do tubo',
                   'Reflexões parciais\n(interior do tubo)',
                   'Colimação do feixe\n(geometria cilíndrica)']},
    'ALINHAMENTO': {'x': 7.5, 'y': 1.2, 'cor': '#16a085',
        'causas': ['Desalinhamento axial\nda lanterna',
                   'Variação de posição\na cada medição']},
    'METODOLOGIA': {'x': 10.8, 'y': 1.2, 'cor': '#c0392b',
        'causas': ['Leitura via app\n(Device Info)',
                   'Tempo de estabilização\ndo sensor',
                   'Leitura única por\nponto (sem repetição)']},
}

def draw_bone(ax, x_junc, y_junc, causas, cor, side='top'):
    ax.annotate('', xy=(x_junc, SPINE_Y),
                xytext=(x_junc, y_junc + (-0.5 if side=='top' else 0.5)),
                arrowprops=dict(arrowstyle='->', color=cor, lw=2.5))
    bbox_c = dict(boxstyle='round,pad=0.3', facecolor=cor, edgecolor='none', alpha=0.92)
    ax.text(x_junc, y_junc + (0.05 if side=='top' else -0.05),
            list(cfg_top.keys() if side=='top' else cfg_bot.keys())[0],
            ha='center', va='bottom' if side=='top' else 'top',
            fontsize=8.5, fontweight='bold', color='white', bbox=bbox_c)

for nome, cfg in cfg_top.items():
    x, y, cor = cfg['x'], cfg['y'], cfg['cor']
    ax.annotate('', xy=(x, SPINE_Y), xytext=(x, y-0.5),
                arrowprops=dict(arrowstyle='->', color=cor, lw=2.5))
    ax.text(x, y+0.05, nome, ha='center', va='bottom', fontsize=8, fontweight='bold', color='white',
            bbox=dict(boxstyle='round,pad=0.28', facecolor=cor, edgecolor='none', alpha=0.92))
    for i, causa in enumerate(cfg['causas']):
        yc = y - 0.55 - i*0.75
        ax.annotate('', xy=(x, yc+0.12), xytext=(x-1.35, yc+0.35),
                    arrowprops=dict(arrowstyle='->', color=cor, lw=1.3, alpha=0.85))
        ax.text(x-1.45, yc+0.4, causa, ha='right', va='center', fontsize=7.5, color='#2c3e50',
                bbox=dict(boxstyle='round,pad=0.18', facecolor='white', edgecolor=cor, alpha=0.88, linewidth=1))

for nome, cfg in cfg_bot.items():
    x, y, cor = cfg['x'], cfg['y'], cfg['cor']
    ax.annotate('', xy=(x, SPINE_Y), xytext=(x, y+0.5),
                arrowprops=dict(arrowstyle='->', color=cor, lw=2.5))
    ax.text(x, y-0.05, nome, ha='center', va='top', fontsize=8, fontweight='bold', color='white',
            bbox=dict(boxstyle='round,pad=0.28', facecolor=cor, edgecolor='none', alpha=0.92))
    for i, causa in enumerate(cfg['causas']):
        yc = y + 0.55 + i*0.75
        ax.annotate('', xy=(x, yc-0.12), xytext=(x-1.35, yc-0.35),
                    arrowprops=dict(arrowstyle='->', color=cor, lw=1.3, alpha=0.85))
        ax.text(x-1.45, yc-0.4, causa, ha='right', va='center', fontsize=7.5, color='#2c3e50',
                bbox=dict(boxstyle='round,pad=0.18', facecolor='white', edgecolor=cor, alpha=0.88, linewidth=1))

ax.text(0.02, 0.02, 'ESTO017-17 – Métodos Experimentais em Engenharia | Grupo 1',
        ha='left', va='bottom', fontsize=8, color='gray', transform=ax.transAxes)

plt.tight_layout()
fig.savefig(os.path.join(out_dir, 'Diagrama_Ishikawa.png'), bbox_inches='tight')
plt.close()
print("Diagrama de Ishikawa salvo.")

# ─────────────────────────────────────────────────────────────────────
# RESUMO FINAL DOS RESULTADOS
# ─────────────────────────────────────────────────────────────────────
print("\n=== RESULTADOS FINAIS ===")
print(f"\nDistâncias reais (d = 47,1 - d_barbante):")
for i in range(len(d_barbante)):
    print(f"  d_barbante={d_barbante[i]:.1f} cm  ->  d_real={d_real[i]:.1f} cm  | E_lab={E_lab[i]} lx")
print(f"\nExpoentes MMQ (log-log):")
print(f"  Lab:  n = {A_lab:.4f}  +/-  {2*uA_lab:.4f}  (k=2, 95%)")
print(f"  Corr: n = {A_corr:.4f}  +/-  {2*uA_corr:.4f}")
print(f"  Arm:  n = {A_arm:.4f}  +/-  {2*uA_arm:.4f}")
print(f"  Teo:  n = -2.0000")
print(f"\nIncerteza da distância: U_d = 0,10 cm (k=2)")
print(f"Incerteza da iluminância: U_E = 11,5% do valor (k=2)")
print(f"\nGraficos salvos em: {out_dir}")
