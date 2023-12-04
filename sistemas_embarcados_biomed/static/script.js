// Função para fazer o download da imagem
function downloadImage() {
    // Capturar o conteúdo da página em uma imagem
    html2canvas(document.body).then(function(canvas) {
        // Criar um elemento de link para o download
        var link = document.createElement('a');
        link.href = canvas.toDataURL();
        link.download = 'relotorio.png';
        link.click();
    });
}

// Associar a função de download ao clique do botão
document.getElementById('downloadButton').addEventListener('click', downloadImage);
