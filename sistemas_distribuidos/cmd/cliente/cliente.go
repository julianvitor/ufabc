package main

import (
	"bufio"
	"fmt"
	"math/rand"
	"net"
	"os"
	"strings"
	"time"
)

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
	// net.ParseIP retorna nil se o IP não for válido.
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
					continue // Pede o mesmo servidor novamente
				}

				ip := partes[0]
				// --- VALIDAÇÃO DO IP ---
				if !validarIP(ip) {
					fmt.Println("Erro: Endereço IP inválido. Tente novamente.")
					continue // Pede o mesmo servidor novamente
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

			// TO-DO capturar do teclado a key + value

			// realiza a conexão aleatoria
			conn, err := RandomConect(servidoresIP)
			if err != nil {
				fmt.Println("Erro de conexão:", err)
				continue // Volta para o menu
			}

			fmt.Println("Conectado com sucesso para o PUT!")

			// TODO: Usar a 'conn' para enviar a mensagem PUT

			conn.Close()

		case "3", "GET":
			if !iniciado {
				fmt.Println("Erro: Execute o INIT primeiro para configurar os servidores.")
				continue
			}
			fmt.Println("\n[Executando GET...]")

			// TODO capturar a chave

			// conecta
			conn, err := RandomConect(servidoresIP)
			if err != nil {
				fmt.Println("Erro de conexão:", err)
				continue // Volta para o menu
			}

			fmt.Println("Conectado com sucesso para o GET!")
			// TODO: Usar a 'conn' para enviar a mensagem GET
			conn.Close()

		case "4", "SAIR":
			fmt.Println("\nSaindo do cliente.")
			return

		default:
			fmt.Println("Opção inválida.")
		}
	}
}

