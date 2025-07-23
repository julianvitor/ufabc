package shared

// validarIP verifica se a string do IP (sem a porta) é válida.
func ValidarIP(ip string) bool {
	return net.ParseIP(ip) != nil
}