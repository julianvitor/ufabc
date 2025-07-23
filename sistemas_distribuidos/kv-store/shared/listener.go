package shared

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

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Printf("Erro ao aceitar conexão: %v", err)
			continue
		}
		// A função handler é chamada em uma nova goroutine para não bloquear o loop.
		go handler(conn)
	}
}