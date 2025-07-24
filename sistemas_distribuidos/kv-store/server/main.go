package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"log"
	"net"
	"os"
	"strings"
	"sync"
	"time"

	"github.com/julianvitor/ufabc/sistemas_distribuidos/kv-store/shared" 
)

// registro armazena o valor e o timestamp de uma chave.
type registro struct {
	Valor     string
	Timestamp int64
}

// pendingGet armazena informações de um cliente aguardando uma atualização.
type pendingGet struct {
	clienteAddr string
	clienteTs   int64
}

// Servidor contém o estado e a lógica do servidor.
type Servidor struct {
	addr         string
	leaderAddr   string
	isLeader     bool
	otherServers []string

	dados       map[string]registro
	dadosMutex  sync.Mutex // Protege o acesso concorrente

	pendingGets      map[string][]pendingGet
	pendingGetsMutex sync.Mutex // Protege o acesso concorrente
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Digite o endereço deste servidor (ex: 127.0.0.1:10097): ")
	myAddr, _ := reader.ReadString('\n')
	myAddr = strings.TrimSpace(myAddr)

	fmt.Print("Digite o endereço do servidor líder (pode ser o mesmo acima): ")
	leaderAddr, _ := reader.ReadString('\n')
	leaderAddr = strings.TrimSpace(leaderAddr)

	allServers := make([]string, 3)
	fmt.Println("Digite o endereço de todos os 3 servidores do cluster:")
	for i := 0; i < 3; i++ {
		fmt.Printf("Endereço do servidor %d: ", i+1)
		addr, _ := reader.ReadString('\n')
		allServers[i] = strings.TrimSpace(addr)
	}

	// Filtra o próprio endereço para obter a lista dos outros servidores
	var otherServers []string
	for _, addr := range allServers {
		if addr != myAddr {
			otherServers = append(otherServers, addr)
		}
	}

	srv := &Servidor{
		addr:         myAddr,
		leaderAddr:   leaderAddr,
		isLeader:     myAddr == leaderAddr,
		otherServers: otherServers,
		dados:        make(map[string]registro),
		pendingGets:  make(map[string][]pendingGet),
	}

	if srv.isLeader {
		log.Printf("Iniciando servidor como LÍDER em %s", srv.addr)
	} else {
		log.Printf("Iniciando servidor como SEGUIDOR em %s, líder é %s", srv.addr, srv.leaderAddr)
	}

	// Inicia o listener para receber conexões de clientes e outros servidores
	shared.StartListener(srv.addr, srv.handleConnection)
}

// handleConnection é chamado para cada nova conexão recebida.
func (s *Servidor) handleConnection(conn net.Conn) {
	defer conn.Close()
	log.Printf("Nova conexão de %s", conn.RemoteAddr().String())

	request, err := bufio.NewReader(conn).ReadString('\n')
	if err != nil {
		log.Printf("Erro ao ler da conexão: %v", err)
		return
	}

	var msg shared.Mensagem
	if err := json.Unmarshal([]byte(request), &msg); err != nil {
		log.Printf("Erro ao decodificar JSON: %v", err)
		return
	}

	// Tratamento de Requisições
	switch msg.Operacao {
	case "PUT":
		s.processPut(conn, msg)
	case "GET":
		s.processGet(conn, msg)
	case "REPLICATION":
		s.processReplication(conn, msg)
	case "REPLICATION_OK":
		// Esta operação é tratada no fluxo síncrono do líder, não aqui.
		log.Printf("Recebido REPLICATION_OK (geralmente tratado em outra parte do fluxo)")
	default:
		log.Printf("Operação desconhecida recebida: %s", msg.Operacao)
	}
}

// Funcionalidade: Recebe Requisição PUT
func (s *Servidor) processPut(conn net.Conn, msg shared.Mensagem) {
	if !s.isLeader {
		// Se não for o líder, encaminha a requisição para o líder.
		log.Printf("Encaminhando PUT key:[%s] value:[%s] para o líder %s", msg.Chave, msg.Valor, s.leaderAddr)
		leaderConn, err := net.Dial("tcp", s.leaderAddr)
		if err != nil {
			log.Printf("Falha ao conectar ao líder para encaminhar PUT: %v", err)
			return
		}
		defer leaderConn.Close()
		json.NewEncoder(leaderConn).Encode(msg)
		return
	}

	// Se for o líder:
	log.Printf("Cliente [%s] PUT key:[%s] value:[%s]", msg.ClienteAddr, msg.Chave, msg.Valor)

	// Insere a informação e atualiza o timestamp.
	s.dadosMutex.Lock()
	newTimestamp := time.Now().UnixNano()
	s.dados[msg.Chave] = registro{
		Valor:     msg.Valor,
		Timestamp: newTimestamp,
	}
	s.dadosMutex.Unlock()

	// Replica a informação para os outros servidores.
	var wg sync.WaitGroup
	wg.Add(len(s.otherServers))

	replicationMsg := shared.Mensagem{
		Operacao:  "REPLICATION",
		Chave:     msg.Chave,
		Valor:     msg.Valor,
		Timestamp: newTimestamp,
		Origem:    s.addr,
	}
	jsonMsg, _ := json.Marshal(replicationMsg)

	for _, serverAddr := range s.otherServers {
		go func(addr string) {
			defer wg.Done()
			replConn, err := net.Dial("tcp", addr)
			if err != nil {
				log.Printf("Falha ao conectar com seguidor %s para replicação: %v", addr, err)
				return
			}
			defer replConn.Close()

			// Envia REPLICATION e espera por REPLICATION_OK na mesma conexão.
			replConn.Write(append(jsonMsg, '\n'))

			// Recebe Requisição REPLICATION_OK
			response, err := bufio.NewReader(replConn).ReadString('\n')
			if err != nil {
				log.Printf("Falha ao receber REPLICATION_OK de %s: %v", addr, err)
				return
			}
			var ackMsg shared.Mensagem
			json.Unmarshal([]byte(response), &ackMsg)
			if ackMsg.Operacao != "REPLICATION_OK" {
				log.Printf("Resposta inesperada de %s durante replicação: %s", addr, ackMsg.Operacao)
			}
		}(serverAddr)
	}

	wg.Wait() // Espera todos os seguidores confirmarem.

	//  Envie PUT_OK para o cliente.
	log.Printf("Enviando PUT_OK ao Cliente [%s] da key:[%s] ts:[%d]", msg.ClienteAddr, msg.Chave, newTimestamp)
	clientConn, err := net.Dial("tcp", msg.ClienteAddr)
	if err != nil {
		log.Printf("Falha ao conectar com cliente %s para enviar PUT_OK: %v", msg.ClienteAddr, err)
		return
	}
	defer clientConn.Close()

	putOkMsg := shared.Mensagem{
		Operacao:  "PUT_OK",
		Chave:     msg.Chave,
		Timestamp: newTimestamp,
	}
	json.NewEncoder(clientConn).Encode(putOkMsg)

	// Após um PUT bem-sucedido, verifica se há clientes aguardando por esta chave.
	s.checkPendingGets(msg.Chave, msg.Valor, newTimestamp)
}

// Funcionalidade: Recebe Requisição REPLICATION
func (s *Servidor) processReplication(conn net.Conn, msg shared.Mensagem) {
	log.Printf("REPLICATION key:[%s] value:[%s] ts:[%d] de [%s]", msg.Chave, msg.Valor, msg.Timestamp, msg.Origem)

	// Insere na sua tabela de hash local.
	s.dadosMutex.Lock()
	s.dados[msg.Chave] = registro{
		Valor:     msg.Valor,
		Timestamp: msg.Timestamp,
	}
	s.dadosMutex.Unlock()

	// Responde para o líder com REPLICATION_OK.
	ackMsg := shared.Mensagem{Operacao: "REPLICATION_OK"}
	json.NewEncoder(conn).Encode(ackMsg)

	// Após replicar, verifica se algum cliente local estava aguardando por esta chave.
	s.checkPendingGets(msg.Chave, msg.Valor, msg.Timestamp)
}
func (s *Servidor) processGet(conn net.Conn, msg shared.Mensagem) {
	s.dadosMutex.Lock()
	record, exists := s.dados[msg.Chave]
	s.dadosMutex.Unlock()

	log.Printf("Cliente [%s] GET key:[%s] ts:[%d]. Meu ts é [%d]", msg.ClienteAddr, msg.Chave, msg.Timestamp, record.Timestamp)

	if (!exists && msg.Timestamp > 0) || (exists && record.Timestamp < msg.Timestamp) {
		log.Printf("... meu estado é desatualizado. Devolvendo WAIT_FOR_RESPONSE")
		responseMsg := shared.Mensagem{Operacao: "WAIT_FOR_RESPONSE", Chave: msg.Chave}
		json.NewEncoder(conn).Encode(responseMsg)

		// Armazena a requisição para notificação futura.
		s.pendingGetsMutex.Lock()
		s.pendingGets[msg.Chave] = append(s.pendingGets[msg.Chave], pendingGet{
			clienteAddr: msg.ClienteAddr,
			clienteTs:   msg.Timestamp,
		})
		s.pendingGetsMutex.Unlock()
	} else {
		
		valorDeResposta := "NULL"
		tsDeResposta := int64(0)
		
		if exists {
			valorDeResposta = record.Valor
			tsDeResposta = record.Timestamp
		}
		
		log.Printf("... portanto devolvendo [%s]", valorDeResposta)
		responseMsg := shared.Mensagem{
			Operacao:  "GET_RESPONSE",
			Chave:     msg.Chave,
			Valor:     valorDeResposta,
			Timestamp: tsDeResposta,
		}
		json.NewEncoder(conn).Encode(responseMsg)
	}
}

// checkPendingGets verifica e notifica clientes que aguardavam por uma atualização.
func (s *Servidor) checkPendingGets(chave, valor string, newTimestamp int64) {
	s.pendingGetsMutex.Lock()
	defer s.pendingGetsMutex.Unlock()

	waitingClients := s.pendingGets[chave]
	if len(waitingClients) == 0 {
		return
	}

	var remainingClients []pendingGet
	for _, clientReq := range waitingClients {
		// Se o novo timestamp for maior, envia a resposta.
		if newTimestamp >= clientReq.clienteTs {
			log.Printf("Enviando atualização assíncrona para %s da chave %s", clientReq.clienteAddr, chave)
			go func(req pendingGet) {
				clientConn, err := net.Dial("tcp", req.clienteAddr)
				if err != nil {
					log.Printf("Falha ao conectar com cliente %s para resposta assíncrona: %v", req.clienteAddr, err)
					return
				}
				defer clientConn.Close()

				responseMsg := shared.Mensagem{
					Operacao:  "GET_RESPONSE",
					Chave:     chave,
					Valor:     valor,
					Timestamp: newTimestamp,
				}
				json.NewEncoder(clientConn).Encode(responseMsg)
			}(clientReq)
		} else {
			// mantém o cliente na lista de espera.
			remainingClients = append(remainingClients, clientReq)
		}
	}

	// Atualiza a lista de pendências, removendo os que foram notificados.
	if len(remainingClients) > 0 {
		s.pendingGets[chave] = remainingClients
	} else {
		delete(s.pendingGets, chave)
	}
}