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
type registro struct{
	Valor string
	TimeStamp int64
}

var (
	Dados = make(map[string]registro)
	DadosMutex = &sync.Mutex{}
	SelfServerAddr := ""  
	FatherServerAddr := ""
)

func Menu(){
	reader := bufio.NewReader(os.Stdin)

	for {

		fmt.Println("Insira o endereço do servidor.")
		SelfServerAddr, _ := reader.ReadString('\n')

		fmt.Println("Insira o endereço do servidor lider.")
		FatherServerAddr, _ := reader.Readstring('\n')
		
		if (FatherServerAddr == SelfServerAddr){
			fmt.Println("Iniciando servidor como líder")
			FatherServer()
		} else {
			fmt.Println("Iniciando servidor como filho")
			ChildServer()
		}
	}
}

func FatherServer(){
// chamar listener > cria worker assincrono para requisições PUT e GET
}

func ChildServer(){
}


