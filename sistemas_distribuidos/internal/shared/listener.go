package shared
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

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Printf("Erro ao aceitar conexão de servidor: %v", err)
			continue // ignora essa iteração que a conexão falhou
		}
		// Usa uma nova goroutine para cada resposta, para não bloquear o listener.
		go handler(conn)
	}
}