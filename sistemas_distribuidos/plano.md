## 🗂️ Plano de Ação para Implementação da KV Store Distribuída em Go

### 📌 Objetivo Geral
Desenvolver um sistema distribuído em Go com 3 servidores TCP e múltiplos clientes, mantendo uma key-value store com consistência e replicação.

---

## ✅ Etapas e Tarefas

### 1. Estrutura Inicial do Projeto
- [x] Criar estrutura de pastas:
  - `cliente/`
  - `servidor/`
  - `mensagem/`
  - `utils/`
- [x] Criar `go.mod` com `go mod init kvstore`

### 2. Módulo de Mensagens
- [x] Criar `mensagem.go` com struct `Mensagem` (JSON)
- [x] Definir tipos de mensagens: PUT, GET, PUT_OK, REPLICATION, etc.

### 3. Utilitários de Rede
- [x] Criar `utils/net.go` com funções para:
  - Enviar mensagem TCP com JSON
  - Receber mensagem TCP com JSON

### 4. Cliente
- [ ] Criar `cliente.go` com:
  - [ ] Menu interativo: INIT, PUT, GET
  - [ ] Captura IPs/portas dos servidores
  - [ ] Envio de PUT para servidor aleatório
  - [ ] Envio de GET com timestamp local
  - [ ] Armazenamento de timestamps por key
  - [ ] Recebimento e impressão de mensagens (GET, PUT_OK, WAIT_FOR_RESPONSE)

### 5. Servidor
- [ ] Criar `servidor.go` com:
  - [ ] Captura de IP/porta/localização do líder
  - [ ] Listener TCP
  - [ ] Tratamento simultâneo com goroutines
  - [ ] HashMap protegido por `sync.Mutex`
  - [ ] Lógica PUT (encaminha ou replica)
  - [ ] Lógica REPLICATION / REPLICATION_OK
  - [ ] Lógica GET com comparação de timestamps
  - [ ] Reenvio assíncrono para WAIT_FOR_RESPONSE

### 6. Main
- [ ] Criar `main.go` com prompt:
  - [ ] Rodar como Cliente ou Servidor
  - [ ] Redirecionar para `cliente.Run()` ou `servidor.Run()`

### 7. Testes Locais
- [ ] Abrir 3 terminais para servidores
- [ ] Abrir 2 terminais para clientes
- [ ] Testar PUT (com e sem ser no líder)
- [ ] Testar GET com resposta imediata e com WAIT_FOR_RESPONSE

---

## 🧪 Extras
- [ ] Logs formatados no terminal (para debug)
- [ ] Mensagens de erro amigáveis
- [ ] Verificação de erros de conexão TCP

---

## 📦 Entrega
- [ ] `main.go`, `cliente/`, `servidor/`, `mensagem/`, `utils/`
- [ ] Relatório explicando código e linhas-chave
- [ ] Link do vídeo com demonstração das 5 janelas
