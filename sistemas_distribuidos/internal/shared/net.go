func EnviarMensagem(conn net.Conn, msg mensagem.Mensagem) error
func ReceberMensagem(conn net.Conn) (mensagem.Mensagem, error)
