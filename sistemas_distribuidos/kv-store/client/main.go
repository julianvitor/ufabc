package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"log"
	"math/rand"
	"net"
	"os"
	"strings"
	"sync"
	"time"
	"github.com/julianvitor/ufabc/sistemas_distribuidos/kv-store/shared"
)

var (
	timestamps = make(map[string]int64)
	tsMutex    = &sync.Mutex{}


	clienteAddr string
)

func main() {
	// 1. Alocação Dinâmica do Endereço do Cliente
	listener, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		log.Fatalf("Falha ao encontrar uma porta livre para o cliente: %v", err)
	}
	
	// Descobrimos qual endereço completo (com a porta) foi alocado.
	clienteAddr = listener.Addr().String()
	log.Printf("Cliente iniciado. Ouvindo por respostas em: %s", clienteAddr)

	// Iniciamos o listener em uma goroutine para lidar com as respostas assíncronas.
	go func() {
		for {
			conn, err := listener.Accept()
			if err != nil {
				log.Printf("Listener do cliente falhou: %v", err)
				return
			}
			go handleAsyncResponse(conn)
		}
	}()

	var servidoresIP []string
	var iniciado bool

	reader := bufio.NewReader(os.Stdin)
	rand.Seed(time.Now().UnixNano())


	for {
		fmt.Println("\n--- MENU DO CLIENTE ---")
		fmt.Println("1. INIT (Configurar servidores)")
		fmt.Println("2. PUT  (Inserir chave-valor)")
		fmt.Println("3. GET  (Buscar valor por chave)")
		fmt.Println("4. SAIR")
		fmt.Print("Escolha uma opção: ")

		opcao, _ := reader.ReadString('\n')
		opcao = strings.ToUpper(strings.TrimSpace(opcao))

		switch opcao {
		case "1", "INIT":
			fmt.Println("\n[Executando INIT...]")
			ips := make([]string, 3)
			for i := 0; i < 3; i++ {
				fmt.Printf("Digite o IP e Porta do Servidor %d (ex: 127.0.0.1:10097): ", i+1)
				input, _ := reader.ReadString('\n')
				ips[i] = strings.TrimSpace(input)
			}
			servidoresIP = ips
			iniciado = true
			fmt.Println("Servidores configurados com sucesso:", servidoresIP)

		case "2", "PUT":
			if !iniciado {
				fmt.Println("Erro: Execute o INIT primeiro para configurar os servidores.")
				continue
			}
			fmt.Println("\n[Executando PUT...]")

			fmt.Print("Digite a chave: ")
			chave, _ := reader.ReadString('\n')
			chave = strings.TrimSpace(chave)

			fmt.Print("Digite o valor: ")
			valor, _ := reader.ReadString('\n')
			valor = strings.TrimSpace(valor)

			msgPUT := shared.Mensagem{
				Operacao:    "PUT",
				Chave:       chave,
				Valor:       valor,
				ClienteAddr: clienteAddr,
			}
			jsonMsg, _ := json.Marshal(msgPUT)

			conn, err := randomConnect(servidoresIP)
			if err != nil {
				fmt.Println("Erro de conexão:", err)
				continue
			}
			log.Println("Aguardando PUT_OK")
			conn.Write(append(jsonMsg, '\n'))
			conn.Close()

		case "3", "GET":
			if !iniciado {
				fmt.Println("Erro: Execute o INIT primeiro para configurar os servidores.")
				continue
			}
			fmt.Println("\n[Executando GET...]")

			fmt.Print("Digite a chave: ")
			chave, _ := reader.ReadString('\n')
			chave = strings.TrimSpace(chave)

			tsMutex.Lock()
			ultimoTimestamp := timestamps[chave]
			tsMutex.Unlock()

			msgGET := shared.Mensagem{
				Operacao:    "GET",
				Chave:       chave,
				Timestamp:   ultimoTimestamp,
				ClienteAddr: clienteAddr,
			}
			jsonMsg, _ := json.Marshal(msgGET)

			conn, err := randomConnect(servidoresIP)
			if err != nil {
				fmt.Println("Erro de conexão:", err)
				continue
			}

			conn.Write(append(jsonMsg, '\n'))
			respostaServidor, err := bufio.NewReader(conn).ReadString('\n')
			conn.Close()
			if err != nil {
				fmt.Println("Erro ao ler resposta do servidor:", err)
				continue
			}
			handleSyncResponse(respostaServidor, conn.RemoteAddr().String(), ultimoTimestamp)

		case "4", "SAIR":
			fmt.Println("\nSaindo do cliente.")
			listener.Close() // Fecha o listener ao sair.
			return

		default:
			fmt.Println("Opção inválida.")
		}
	}
}


// handleSyncResponse processa respostas síncronas (GET_RESPONSE, WAIT_FOR_RESPONSE).
func handleSyncResponse(response, serverAddr string, clientTs int64) {
	var msgResposta shared.Mensagem
	json.Unmarshal([]byte(response), &msgResposta)

	switch msgResposta.Operacao {
	case "GET_RESPONSE":
		tsMutex.Lock()
		timestamps[msgResposta.Chave] = msgResposta.Timestamp
		tsMutex.Unlock()
		fmt.Printf("GET key: [%s] value: [%s] obtido do servidor [%s], meu timestamp [%d] e do servidor [%d]\n",
			msgResposta.Chave, msgResposta.Valor, serverAddr, clientTs, msgResposta.Timestamp)

	case "WAIT_FOR_RESPONSE":
		fmt.Printf("GET key: [%s]... Resposta: WAIT_FOR_RESPONSE. \n", msgResposta.Chave)

	default:
		fmt.Println("Resposta inesperada do servidor:", response)
	}
}

// handleAsyncResponse processa respostas que chegam de forma assíncrona.
func handleAsyncResponse(conn net.Conn) {
	defer conn.Close()

	respostaServidor, err := bufio.NewReader(conn).ReadString('\n')
	if err != nil {
		log.Printf("Erro ao ler resposta assíncrona: %v", err)
		return
	}

	var msgResposta shared.Mensagem
	json.Unmarshal([]byte(respostaServidor), &msgResposta)

	// A resposta pode ser um PUT_OK (após um PUT) ou um GET_RESPONSE (após um WAIT).
	switch msgResposta.Operacao {
	case "PUT_OK":
		tsMutex.Lock()
		timestamps[msgResposta.Chave] = msgResposta.Timestamp
		tsMutex.Unlock()
		fmt.Printf("\n[CONFIRMAÇÃO RECEBIDA]\nPUT_OK key: [%s] timestamp [%d] realizada com sucesso.\n", msgResposta.Chave, msgResposta.Timestamp)

	case "GET_RESPONSE":
		tsMutex.Lock()
		ultimoTimestamp := timestamps[msgResposta.Chave]
		timestamps[msgResposta.Chave] = msgResposta.Timestamp
		tsMutex.Unlock()
		fmt.Printf("\n[RESPOSTA ASSÍNCRONA RECEBIDA]\n")
		fmt.Printf("GET key: [%s] value: [%s] obtido do servidor [%s], meu timestamp era [%d] e o novo é [%d]\n",
			msgResposta.Chave, msgResposta.Valor, conn.RemoteAddr().String(), ultimoTimestamp, msgResposta.Timestamp)
	}
	fmt.Print("Escolha uma opção: ") // Reexibe o prompt do menu
}

// randomConnect escolhe e conecta a um servidor aleatório da lista.
func randomConnect(servidoresIP []string) (net.Conn, error) {
	if len(servidoresIP) == 0 {
		return nil, fmt.Errorf("lista de servidores está vazia")
	}
	servidorEscolhido := servidoresIP[rand.Intn(len(servidoresIP))]

	fmt.Printf("Conectando ao servidor aleatório: %s\n", servidorEscolhido)
	return net.DialTimeout("tcp", servidorEscolhido, 5*time.Second)
}