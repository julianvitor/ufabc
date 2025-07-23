package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"log"
	"net"
	"os"
	"sync"

	"github.com/julianvitor/ufabc/sistemas_distribuidos/internal/shared"
)
type registro struct{
	Valor string
	TimeStamp int64
}

var (
	Dados = make(map[string]registro)
	DadosMutex = &sync.Mutex{}
	SelfServerAddr string
	FatherServerAddr string
)

func Menu(){
	reader := bufio.NewReader(os.Stdin)

	for {

		fmt.Println("Insira o endereço do servidor.")
		SelfServerAddr, _ := reader.ReadString('\n')

		fmt.Println("Insira o endereço do servidor lider.")
		FatherServerAddr, _ := reader.ReadString('\n')
		
		if (FatherServerAddr == SelfServerAddr){
			FatherServer()
		} else {
			ChildServer()
		}
	}
}

func FatherServer(){
	fmt.Println("Iniciando servidor como líder")


// chamar listener > cria worker assincrono para requisições PUT e GET
}

func ChildServer(){
	fmt.Println("Iniciando servidor como filho")
}

func asyncRequestHandler(conn net.conn){
	defer conn.Close()
	Requisicao, err := bufio.NewReader(conn).ReadString('\n')
	if err != nil{
		log.Printf("Erro ao receber requisição assincrona: %v", err)
		return
	}

	var msgRequisicao shared.Mensagem
	json.Unmarshal([]byte(Requisicao), &msgRequisicao)

	switch msgRequisicao.Operacao{
		case "GET":
			//processGet(conn, msgRequisicao)
		case "PUT":
			//processatPut()
		case "REPLICATION_OK":
			//processReplicationOk()
	}
	
}

//TO_DO funcoes process