package main

import (
    "fmt"
    "os"
    "cliente"
    "servidor"
)

func main() {
    var opcao int
    fmt.Println("Rodar como:")
    fmt.Println("1 - Cliente")
    fmt.Println("2 - Servidor")
    fmt.Scan(&opcao)

    switch opcao {
    case 1:
        cliente.Run()
    case 2:
        servidor.Run()
    default:
        fmt.Println("Opção inválida.")
        os.Exit(1)
    }
}
