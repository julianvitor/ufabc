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

	"github.com/julianvitor/ufabc/sistemas_distribuidos/internal/shared"
)

var (
	timestamps = make(map[string]int64)
	tsMutex    = &sync.Mutex{}
)

// Endereço deste cliente para o servidor poder retornar chamadas (WAIT_FOR_RESPONSE).
const clienteAddr = "127.0.0.1:9901"

// iniciarListener abre uma porta TCP para receber respostas assíncronas dos servidores.
// Esta função deve ser executada em uma goroutine.
func iniciarListener() {
	listener, err := net.Listen("tcp", clienteAddr)
	if err != nil {
		log.Fatalf("Erro ao iniciar listener do cliente em %s: %v", clienteAddr, err)
	}
	defer listener.Close()
	fmt.Printf("Cliente ouvindo por respostas assíncronas em %s\n", clienteAddr)

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Printf("Erro ao aceitar conexão de servidor: %v", err)
			continue
		}
		// Usa uma nova goroutine para cada resposta, para não bloquear o listener.
		go handleAsyncResponse(conn)
	}
}

func handleAsyncResponse(conn net.Conn) {
	defer conn.Close()

	respostaServidor, err := bufio.NewReader(conn).ReadString('\n')
	if err != nil {
		log.Printf("Erro ao ler resposta assíncrona: %v", err)
		return
	}

	var msgResposta shared.Mensagem
	json.Unmarshal([]byte(respostaServidor), &msgResposta)

	if msgResposta.Operacao == "GET_RESPONSE" {
		tsMutex.Lock() // Bloqueia o acesso ao mapa de timestamps
		ultimoTimestamp := timestamps[msgResposta.Chave]
		timestamps[msgResposta.Chave] = msgResposta.Timestamp
		tsMutex.Unlock() // Libera o acesso

		// Imprime a resposta na tela. O \n no início ajuda a não bagunçar o menu.
		fmt.Printf("\n[RESPOSTA ASSÍNCRONA RECEBIDA]\n")
		fmt.Printf("GET key: [%s] value: [%s] obtido do servidor [%s], meu timestamp era [%d] e o novo é [%d]\n",
			msgResposta.Chave, msgResposta.Valor, conn.RemoteAddr().String(), ultimoTimestamp, msgResposta.Timestamp)
		fmt.Print("Escolha uma opção: ") // Reexibe o prompt do menu
	}
}

func RandomConect(servidoresIP []string) (net.Conn, error) {
	if len(servidoresIP) == 0 {
		return nil, fmt.Errorf("lista de servidores está vazia")
	}
	indiceAleatorio := rand.Intn(len(servidoresIP))
	servidorEscolhido := servidoresIP[indiceAleatorio]

	fmt.Printf("Tentando conectar ao servidor aleatório: %s\n", servidorEscolhido)

	conn, err := net.DialTimeout("tcp", servidorEscolhido, 5 * time.Second)
	if err != nil {
		return nil, err //Erro
	}

	return conn, nil // Sucesso
}


// validarIP verifica se a string do IP (sem a porta) é válida.
func validarIP(ip string) bool {
	return net.ParseIP(ip) != nil
}

func cliente() {
	var servidoresIP []string
	var iniciado bool
	
	reader := bufio.NewReader(os.Stdin)

	for {
		fmt.Println("\n1. INIT (Configurar servidores)")
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
			ipsValidos := 0 // Contador para IPs válidos

			// Loop até termos 3 IPs válidos
			for ipsValidos < 3 {
				fmt.Printf("Digite o IP e Porta do Servidor %d (ex: 127.0.0.1:10097): ", ipsValidos+1)
				input, _ := reader.ReadString('\n')
				enderecoCompleto := strings.TrimSpace(input)

				// Separa o IP da Porta
				partes := strings.Split(enderecoCompleto, ":")
				if len(partes) != 2 {
					fmt.Println("Formato inválido. Use o formato IP:Porta.")
					continue 
				}

				ip := partes[0]

				if !validarIP(ip) {
					fmt.Println("Erro: Endereço IP inválido. Tente novamente.")
					continue 
				}

				// Se for válido, armazena e avança o contador
				ips[ipsValidos] = enderecoCompleto
				ipsValidos++
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

			// Monta a mensagem PUT
			msgPUT := shared.Mensagem{
				Operacao:    "PUT",
				Chave:       chave,
				Valor:       valor,
				ClienteAddr: clienteAddr,
			}
			jsonMsg, _ := json.Marshal(msgPUT)

			// Conecta e envia
			conn, err := RandomConect(servidoresIP)
			if err != nil {
				fmt.Println("Erro de conexão:", err)
				continue
			}

			conn.Write(append(jsonMsg, '\n')) // Envia JSON

			// Lê e processa a resposta
			respostaServidor, err := bufio.NewReader(conn).ReadString('\n')
			conn.Close() // Fecha a conexão após receber a resposta
			if err != nil {
				fmt.Println("Erro ao ler resposta do servidor:", err)
				continue
			}

			var msgResposta shared.Mensagem //Struct mensagem
			json.Unmarshal([]byte(respostaServidor), &msgResposta)

			if msgResposta.Operacao == "PUT_OK" {
				timestamps[msgResposta.Chave] = msgResposta.Timestamp
				fmt.Printf("PUT_OK key: [%s] value [%s] timestamp [%d] realizada no servidor [%s]\n", msgResposta.Chave, valor, msgResposta.Timestamp, conn.RemoteAddr().String())
			} else {
				fmt.Println("Resposta inesperada do servidor:", respostaServidor)
			}

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

			conn, err := RandomConect(servidoresIP)
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

			var msgResposta shared.Mensagem
			json.Unmarshal([]byte(respostaServidor), &msgResposta)

			switch msgResposta.Operacao {
			case "GET_RESPONSE":
				tsMutex.Lock()
				timestamps[msgResposta.Chave] = msgResposta.Timestamp
				tsMutex.Unlock()
				fmt.Printf("GET key: [%s] value: [%s] obtido do servidor [%s], meu timestamp [%d] e do servidor [%d]\n",
					msgResposta.Chave, msgResposta.Valor, conn.RemoteAddr().String(), ultimoTimestamp, msgResposta.Timestamp)

			case "WAIT_FOR_RESPONSE":
				fmt.Printf("WAIT_FOR_RESPONSE para a chave [%s]. Aguardando atualização assíncrona do servidor [%s].\n", msgResposta.Chave, conn.RemoteAddr().String())

			default:
				fmt.Println("Resposta inesperada do servidor:", respostaServidor)
			}


		case "4", "SAIR":
			fmt.Println("\nSaindo do cliente.")
			return

		default:
			fmt.Println("Opção inválida.")
		}
	}
}

