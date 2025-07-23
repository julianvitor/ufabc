package shared

type Mensagem struct {
	Operacao    string `json:"operacao"`
	Chave       string `json:"chave"`
	Valor       string `json:"valor,omitempty"`
	Timestamp   int64  `json:"timestamp,omitempty"`
	ClienteAddr string `json:"cliente_addr,omitempty"`
}