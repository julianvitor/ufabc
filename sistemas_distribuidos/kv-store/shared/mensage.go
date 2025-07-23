package shared

type Mensagem struct {
	Operacao    string   `json:"operacao"`
	Chave       string   `json:"chave"`
	Valor       string   `json:"valor,omitempty"`
	Timestamp   int64    `json:"timestamp,omitempty"`
	ClienteAddr string   `json:"cliente_addr,omitempty"` // Endereço para respostas assíncronas
	Origem      string   `json:"origem,omitempty"`       // Endereço do remetente (servidor)
	Servidores  []string `json:"servidores,omitempty"`   // Lista de todos os servidores
}