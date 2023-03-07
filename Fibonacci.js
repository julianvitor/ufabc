const readline = require('readline');

// Configurando entrada de dados pelo terminal
const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

// Função para gerar a sequência de Fibonacci
function fibonacci(n) {
  if (n <= 1) {
    return n;
  }
  return fibonacci(n - 1) + fibonacci(n - 2);
}

// Pedindo ao usuário a quantidade de números da sequência de Fibonacci
rl.question('Quantos números da Sequência de Fibonacci deseja gerar? ', (num) => {
  for (let i = 0; i < num; i++) {
    console.log(fibonacci(i));
  }
  
  // Fechando a entrada de dados
  rl.close();
});
