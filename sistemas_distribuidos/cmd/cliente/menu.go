package main

import (
	"fmt"
	"net"
	"strings"
)

// Função para validar um endereço IP
func validarIP(ip string) bool {
	return net.ParseIP(ip) != nil
}

func MenuCLI() {
	var ip1, ip2, ip3 string
	var metodo, chave, valor string

	// --- Recebe os endereços IP ---
	fmt.Print("Digite o primeiro endereço IP: ")
	fmt.Scan(&ip1)

	fmt.Print("Digite o segundo endereço IP: ")
	fmt.Scan(&ip2)

	fmt.Print("Digite o terceiro endereço IP: ")
	fmt.Scan(&ip3)

	// --- Valida os endereços IP ---
	if !validarIP(ip1) || !validarIP(ip2) || !validarIP(ip3) {
		fmt.Println("\nErro: Um ou mais endereços IP são inválidos.")
		return
	}

	fmt.Println("\nEndereços IP validados com sucesso!")
	fmt.Println("------------------------------------")

	// --- Menu de método (PUT ou GET) ---
	fmt.Print("Escolha o método: PUT ou GET\n> ")
	fmt.Scan(&metodo)
	metodo = strings.ToUpper(strings.TrimSpace(metodo))

	// --- Lógica baseada no método escolhido ---
	switch metodo {
	case "PUT":
		fmt.Print("Digite a chave: ")
		fmt.Scan(&chave)

		fmt.Print("Digite o valor: ")
		fmt.Scan(&valor) // Atenção: fmt.Scan só lê até o primeiro espaço.

		fmt.Printf("\nExecutando PUT com Chave: '%s' e Valor: '%s'\n", chave, valor)
		// Aqui entraria a sua lógica para a operação PUT.

	case "GET":
		fmt.Print("Digite a chave: ")
		fmt.Scan(&chave)

		fmt.Printf("\nExecutando GET com Chave: '%s'\n", chave)
		// Aqui entraria a sua lógica para a operação GET.

	default:
		fmt.Println("Opção inválida. Por favor, escolha PUT ou GET.")
	}
}