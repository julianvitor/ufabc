import pdfplumber
import os

pasta = r"c:\Users\julian\Downloads\projeto luminosidade"
saida_dir = r"C:\Users\julian\.gemini\antigravity-ide\brain\31fb9cb7-b8be-4d48-810a-63e53f0e489b"

pdfs = [
    "Teoria2_2026.2-revisadov3.pdf",
    "Teoria4_2026.2-revisado.pdf",
    "Teoria5_2025_2ComStudent.pdf",
    "Aula05_2025_2.pdf",
    "Aula09TestesDeHipotese_2025.pdf",
    "Aula_CorreçãoRelatorioJulio2026.pdf",
    "SensoreseTransdutores_2025_2.pdf",
    "STK3311-X.pdf",
    "dados - relatório final_260730_112029 (1).pdf",
]

os.makedirs(saida_dir, exist_ok=True)

for nome in pdfs:
    caminho = os.path.join(pasta, nome)
    if not os.path.exists(caminho):
        print(f"[AVISO] Arquivo não encontrado: {nome}")
        continue
    
    nome_saida = nome.replace(".pdf", ".txt").replace(" ", "_").replace("(", "").replace(")", "").replace("ç", "c").replace("ã", "a")
    caminho_saida = os.path.join(saida_dir, nome_saida)
    
    print(f"Extraindo: {nome}...")
    try:
        with pdfplumber.open(caminho) as pdf:
            texto_total = []
            for i, pagina in enumerate(pdf.pages):
                texto = pagina.extract_text()
                if texto:
                    texto_total.append(f"\n--- PÁGINA {i+1} ---\n{texto}")
                # Também extrair tabelas
                tabelas = pagina.extract_tables()
                for t, tabela in enumerate(tabelas):
                    texto_total.append(f"\n[TABELA {t+1} - pág {i+1}]")
                    for linha in tabela:
                        texto_total.append(" | ".join(str(c) if c else "" for c in linha))
            
            conteudo = "\n".join(texto_total)
            with open(caminho_saida, "w", encoding="utf-8") as f:
                f.write(conteudo)
            
            print(f"  -> Salvo em: {caminho_saida} ({len(pdf.pages)} páginas, {len(conteudo)} chars)")
    except Exception as e:
        print(f"  [ERRO] {nome}: {e}")

print("\nExtração concluída!")
