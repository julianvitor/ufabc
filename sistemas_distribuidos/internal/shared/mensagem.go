package mensagem

type Mensagem struct {
    Tipo       string `json:"tipo"` // "PUT", "GET", "PUT_OK", etc.
    Key        string `json:"key,omitempty"`
    Value      string `json:"value,omitempty"`
    Timestamp  int    `json:"timestamp,omitempty"`
    ClienteIP  string `json:"cliente_ip,omitempty"`
    ClientePorta string `json:"cliente_porta,omitempty"`
}
