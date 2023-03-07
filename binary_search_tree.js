// Definindo a classe Node, que representa cada nó da árvore
class Node {
    constructor(value) {
      this.value = value;
      this.left = null;
      this.right = null;
    }
  }
  
  // Definindo a classe BinarySearchTree, que representa a árvore binária de busca
  class BinarySearchTree {
    constructor() {
      this.root = null;
    }
  
    // Método para inserir um novo nó na árvore
    insert(value) {
      const newNode = new Node(value);
  
      if (this.root === null) {
        this.root = newNode;
      } else {
        this.insertNode(this.root, newNode);
      }
    }
  
    insertNode(node, newNode) {
      if (newNode.value < node.value) {
        if (node.left === null) {
          node.left = newNode;
        } else {
          this.insertNode(node.left, newNode);
        }
      } else {
        if (node.right === null) {
          node.right = newNode;
        } else {
          this.insertNode(node.right, newNode);
        }
      }
    }
  
    // Método para buscar um nó na árvore
    search(value) {
      return this.searchNode(this.root, value);
    }
  
    searchNode(node, value) {
      if (node === null) {
        return null;
      } else if (value < node.value) {
        return this.searchNode(node.left, value);
      } else if (value > node.value) {
        return this.searchNode(node.right, value);
      } else {
        return node;
      }
    }
  
    // Método para percorrer a árvore em ordem
    inOrder(node) {
      if (node !== null) {
        this.inOrder(node.left);
        console.log(node.value);
        this.inOrder(node.right);
      }
    }
  
    // Método para percorrer a árvore em pré-ordem
    preOrder(node) {
      if (node !== null) {
        console.log(node.value);
        this.preOrder(node.left);
        this.preOrder(node.right);
      }
    }
  
    // Método para percorrer a árvore em pós-ordem
    postOrder(node) {
      if (node !== null) {
        this.postOrder(node.left);
        this.postOrder(node.right);
        console.log(node.value);
      }
    }
  }
  
  // Criando uma nova instância da árvore
  const tree = new BinarySearchTree();
  
  // Pedindo ao usuário que insira os valores a serem adicionados à árvore
  const values = prompt("Insira os valores a serem adicionados separados por vírgula:").split(",");
  
  // Inserindo os valores na árvore
  for (let i = 0; i < values.length; i++) {
    tree.insert(parseInt(values[i]));
  }
  
  // Imprimindo a árvore em ordem
  console.log("Árvore em ordem:");
  tree.inOrder(tree.root);
  
  // Imprimindo a árvore em pré-ordem
  console.log("Árvore em pré-ordem:");
  tree.preOrder(tree.root);
  
  // Imprimindo a árvore em pós-ordem
  console.log("Árvore em pós-ordem:");
  tree.postOrder(tree.root);
  
  // Pedindo ao usuário que insira o valor a ser buscado na árvore
  const searchValue = parseInt(prompt("Insira o valor a ser buscado na árvore:"));

  // Buscando o valor na árvore e imprimindo o resultado
  const result = tree.search(searchValue);
  if (result === null) {
  console.log(O valor ${searchValue} não foi encontrado na árvore.);
  } else {
  console.log(O valor ${searchValue} foi encontrado na árvore.);
  }
  