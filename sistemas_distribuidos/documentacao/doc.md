Documentação de Operações e JSONs
=================================

Sistema Distribuído kv

Estrutura Base da Mensagem (Go)
-------------------------------

Todas as mensagens JSON são derivadas desta struct. Campos opcionais são omitidos do JSON final quando não preenchidos.

    type Mensagem struct {
        Operacao    string json:"operacao"
        Chave       string json:"chave,omitempty"
        Valor       string json:"valor,omitempty"
        Timestamp   int64  json:"timestamp,omitempty"
        ClienteAddr string json:"cliente_addr,omitempty" // Endereço IP:Porta do cliente
        OrigemAddr  string json:"origem_addr,omitempty"  // Endereço IP:Porta do remetente (servidor)
    }

1\. Operações Cliente ➔ Servidor
--------------------------------

### PUT

*   **Propósito:** Solicitar a inserção ou atualização de um par chave-valor.
*   **De:** Cliente
*   **Para:** Servidor aleatório (Mãe ou Filho)

    {
      "operacao": "PUT",
      "chave": "user:123",
      "valor": "Maria Silva",
      "cliente_addr": "192.168.1.10:9901"
    }

### GET

*   **Propósito:** Solicitar o valor associado a uma chave.
*   **De:** Cliente
*   **Para:** Servidor aleatório (Mãe ou Filho)

    {
      "operacao": "GET",
      "chave": "user:123",
      "timestamp": 1678886400,
      "cliente_addr": "192.168.1.10:9901"
    }

2\. Operações Servidor ➔ Servidor (Internas)
--------------------------------------------

### REGISTER

*   **Propósito:** Um Filho se apresenta para a Mãe ao ser iniciado.
*   **De:** Filho
*   **Para:** Mãe

    {
      "operacao": "REGISTER",
      "origem_addr": "127.0.0.1:10098"
    }

### REPLICATION

*   **Propósito:** A Mãe envia a um Filho o novo dado para ser replicado.
*   **De:** Mãe
*   **Para:** Filho

    {
      "operacao": "REPLICATION",
      "chave": "user:123",
      "valor": "Maria Silva",
      "timestamp": 1678891234
    }

3\. Operações Servidor ➔ Cliente (Respostas)
--------------------------------------------

### PUT\_OK

*   **Propósito:** Confirmar ao cliente que o PUT foi concluído e replicado com sucesso.
*   **De:** Mãe
*   **Para:** Cliente

    {
      "operacao": "PUT_OK",
      "chave": "user:123",
      "timestamp": 1678891234
    }

### WAIT\_FOR\_RESPONSE

*   **Propósito:** Avisar ao cliente que o dado no servidor está desatualizado e que ele deve aguardar uma resposta futura.
*   **De:** Mãe ou Filho
*   **Para:** Cliente

    {
      "operacao": "WAIT_FOR_RESPONSE",
      "chave": "user:123"
    }