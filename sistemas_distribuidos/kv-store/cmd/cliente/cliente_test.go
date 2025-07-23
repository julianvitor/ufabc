
package main

import (
	"encoding/json"
	"net"
	"testing"
	"time"

	"github.com/julianvitor/ufabc/sistemas_distribuidos/internal/shared"
)

// TestValidarIP testa a função de validação de IP com múltiplos casos.
func TestValidarIP(t *testing.T) {
	// Estrutura para os casos de teste
	testCases := []struct {
		name     string
		ip       string
		expected bool
	}{
		{"IPv4 Válido", "127.0.0.1", true},
		{"IPv4 Inválido", "256.0.0.1", false},
		{"IPv6 Válido", "::1", true},
		{"String Vazia", "", false},
		{"Texto Aleatório", "nao-e-um-ip", false},
	}

	for _, tc := range testCases {
		// t.Run agrupa testes, facilitando a identificação de falhas
		t.Run(tc.name, func(t *testing.T) {
			result := validarIP(tc.ip)
			if result != tc.expected {
				t.Errorf("Para o IP '%s', esperado %v, mas obteve %v", tc.ip, tc.expected, result)
			}
		})
	}
}

// TestRandomConect testa a função de conexão aleatória.
func TestRandomConect(t *testing.T) {
	// Inicia um servidor TCP falso que apenas aceita e fecha conexões.
	mockServer, err := net.Listen("tcp", "127.0.0.1:0") // 0 significa porta aleatória
	if err != nil {
		t.Fatalf("Não foi possível criar o servidor mock: %v", err)
	}
	defer mockServer.Close()

	// Goroutine para aceitar a conexão do teste
	go func() {
		conn, _ := mockServer.Accept()
		if conn != nil {
			conn.Close()
		}
	}()

	t.Run("Conexão bem-sucedida", func(t *testing.T) {
		servers := []string{mockServer.Addr().String()}
		conn, err := RandomConect(servers)
		if err != nil {
			t.Fatalf("Esperava uma conexão bem-sucedida, mas obteve erro: %v", err)
		}
		if conn != nil {
			conn.Close()
		}
	})

	t.Run("Lista de servidores vazia", func(t *testing.T) {
		_, err := RandomConect([]string{})
		if err == nil {
			t.Fatal("Esperava um erro para lista de servidores vazia, mas não obteve nenhum.")
		}
	})

	t.Run("Servidor inalcançável (timeout)", func(t *testing.T) {
		// Usa um endereço que provavelmente não existe e não será roteado
		// para forçar um timeout em vez de 'connection refused'.
		servers := []string{"10.255.255.1:1"}
		_, err := RandomConect(servers)
		if err == nil {
			t.Fatal("Esperava um erro de timeout, mas a conexão foi bem-sucedida.")
		}
		// Verifica se o erro é de timeout
		if netErr, ok := err.(net.Error); !ok || !netErr.Timeout() {
			t.Errorf("Esperava um erro de timeout, mas obteve: %v", err)
		}
	})
}

// TestHandleAsyncResponse testa o processamento da resposta assíncrona.
func TestHandleAsyncResponse(t *testing.T) {
	// Usa net.Pipe para criar uma conexão em memória, sem precisar de rede real.
	serverConn, clientConn := net.Pipe()

	// Prepara a mensagem de resposta que o servidor falso vai enviar.
	chaveTeste := "minha-chave-async"
	novoTimestamp := time.Now().Unix()
	msg := shared.Mensagem{
		Operacao:  "GET_RESPONSE",
		Chave:     chaveTeste,
		Valor:     "novo-valor",
		Timestamp: novoTimestamp,
	}
	jsonMsg, _ := json.Marshal(msg)

	// Inicia a função a ser testada em uma goroutine, passando a conexão do cliente.
	go handleAsyncResponse(clientConn)

	// O servidor falso escreve a mensagem na sua ponta da conexão.
	_, err := serverConn.Write(append(jsonMsg, '\n'))
	if err != nil {
		t.Fatalf("Erro ao escrever no pipe do servidor: %v", err)
	}

	// Dá um pequeno tempo para a goroutine processar a mensagem.
	time.Sleep(100 * time.Millisecond)

	// Verifica se o mapa global de timestamps foi atualizado corretamente.
	tsMutex.Lock()
	defer tsMutex.Unlock()

	if timestamps[chaveTeste] != novoTimestamp {
		t.Errorf("Timestamp incorreto. Esperado %d, obteve %d", novoTimestamp, timestamps[chaveTeste])
	}
}

