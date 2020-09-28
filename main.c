#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define QTD_PRODUTOS 6
#define QTD_QUALIDADE 5
#define QTD_TAMANHO 3
#define TAM_NOME 10

//Struct produto
typedef struct Produto {
    char tamanho;
    char nome[TAM_NOME];
    int qualidade;
} TProduto;


int geraNumeroAleatorio(int qtdValores) {
    time_t t;
    srand((unsigned) time(&t) + (getpid() * 5));
    return rand() % (qtdValores);
}

char* classificaProduto(TProduto* produto){
    if(strcmp(produto->nome, "moleton") || strcmp(produto->nome, "jeans") || strcmp(produto->nome, "casaco") ){
        if(produto->qualidade < 4){
            return "reprovado!";
        }
    }
    if(produto->tamanho == 'G'){
        if(produto->qualidade <3){
            return "reprovado!";
        }
    }
    if (produto->qualidade == 5){
        return "excelente!";
    }
    return "bom!";
}

void consumidor(int* file_pipes){
    int data_processed;
    TProduto *produtoRecebe;
    produtoRecebe = (TProduto*)malloc(2 * sizeof(TProduto));

    while(1){
        data_processed = read(file_pipes[0], produtoRecebe,sizeof(produtoRecebe->nome) + sizeof(produtoRecebe->qualidade) + sizeof(produtoRecebe->tamanho));
        sleep(1 + geraNumeroAleatorio(3)); //1 a 3 segundos
        classificaProduto(produtoRecebe);
        printf("\033[0;31m");//Vermelho 
        printf("---Consumindo---\nNome:%s\nTamanho:%c\nQualidade:%d\nEsse produto está %s\n", produtoRecebe->nome, produtoRecebe->tamanho, produtoRecebe->qualidade, classificaProduto(produtoRecebe));
    }
    exit(EXIT_SUCCESS);
}

void defineNomeProduto(char* nomeProduto){
    char nomesProdutos[QTD_PRODUTOS][TAM_NOME] = {"camiseta", "moleton", "jeans", "camisa", "casaco", "bermuda"};
    int i = geraNumeroAleatorio(QTD_PRODUTOS);
    strcpy(nomeProduto, nomesProdutos[i]);
}

char defineTamanhoProduto(){
    int i = geraNumeroAleatorio(QTD_TAMANHO);

    switch (i)
    {   
    case 0:
        return 'P';
        break;
    case 1:
        return 'M';
        break;
    case 2:
        return 'G';
        break;
    }
}

int defineQualidadeProduto(){
    int i = geraNumeroAleatorio(QTD_QUALIDADE) + 1;
    return (i);
}

void produtor(int* file_pipes){
    int data_processed;
    TProduto *produto;
    produto = (TProduto*)malloc(2 * sizeof(TProduto));

    while(1){
        sleep(4 + geraNumeroAleatorio(5));  //4 a 8 segundos
        defineNomeProduto(produto->nome);
        produto->tamanho = defineTamanhoProduto();
        produto->qualidade = defineQualidadeProduto();
        printf("\033[0m"); //Cor padrão
        printf("---Produzindo---\nNome:%s\nTamanho:%c\nQualidade:%d\n", produto->nome, produto->tamanho, produto->qualidade);
        data_processed = write(file_pipes[1], produto, sizeof(produto->nome) + sizeof(produto->tamanho) + sizeof(produto->qualidade) );
    }
    exit(EXIT_SUCCESS);
}

void criaFilhosConsumidores(int* file_pipes, int numProcessos){
    if(numProcessos == 0){
        return;
    } 
    pid_t fork_result;

    fork_result = fork();
    
    if(fork_result == 0){
        criaFilhosConsumidores(file_pipes, --numProcessos);
    } else{
        consumidor(file_pipes);
    }
}

void criaFilhosProdutores(int* file_pipes, int numProcessos){
    if(numProcessos == 0)
        return;

    pid_t fork_result;

    fork_result = fork();

    if(fork_result == 0){
        criaFilhosProdutores(file_pipes, --numProcessos);
    } else{
        produtor(file_pipes);
    }
}

int main() {
    int file_pipes[2];

    if(pipe(file_pipes) != 0)
        exit(EXIT_FAILURE);


    criaFilhosProdutores(file_pipes, 3);
    criaFilhosConsumidores(file_pipes, 1);
    consumidor(file_pipes);

    exit(EXIT_SUCCESS);
}