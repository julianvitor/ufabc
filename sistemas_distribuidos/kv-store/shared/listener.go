package shared
<<<<<<< HEAD

import (
	"log"
	"net"
)

// StartListener inicia um listener TCP no endereço fornecido e passa as conexões para um handler.
func StartListener(addr string, handler func(net.Conn)) {
	listener, err := net.Listen("tcp", addr)
	if err != nil {
		log.Fatalf("Falha ao iniciar listener em %s: %v", addr, err)
	}
	defer listener.Close()
	log.Printf("Ouvindo em %s...", addr)
=======
import(
	"net"
	"log"
)

type RequestHandler func(conn net.Conn)

func StartListener(addr string, handler RequestHandler) {
	listener, err := net.Listen("tcp", clienteAddr)
	if err != nil {
		log.Fatalf("Erro ao iniciar em %s: %v", clienteAddr, err)
	}
	defer listener.Close()
	fmt.Printf("Ouvindo por respostas assíncronas em %s\n", clienteAddr)
>>>>>>> origin/main

	for {
		conn, err := listener.Accept()
		if err != nil {
<<<<<<< HEAD
			log.Printf("Erro ao aceitar conexão: %v", err)
			continue
		}
		// A função handler é chamada em uma nova goroutine para não bloquear o loop.
=======
			log.Printf("Erro ao aceitar conexão de servidor: %v", err)
			continue // ignora essa iteração que a conexão falhou
		}
		// Usa uma nova goroutine para cada resposta, para não bloquear o listener.
>>>>>>> origin/main
		go handler(conn)
	}
}