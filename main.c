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
    srand((unsigned) time(&t));
    return rand() % (qtdValores);
}

void consumidor(int* file_pipes){
    int data_processed;
    TProduto *produtoRecebe;
    produtoRecebe = (TProduto*)malloc(2 * sizeof(TProduto));
    int sleepTime;


    while(1){
        sleepTime = 1;
        data_processed = read(file_pipes[0], produtoRecebe,sizeof(produtoRecebe->nome) + sizeof(produtoRecebe->qualidade) + sizeof(produtoRecebe->tamanho));

        if (produtoRecebe->tamanho == 'G'){
            sleepTime ++;
        }
        sleep(sleepTime);
        printf("Read %d bytes: %s - %c - %d\n", data_processed, produtoRecebe->nome, produtoRecebe->tamanho, produtoRecebe->qualidade);
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
        sleep(1);
        return 'P';
        break;
    case 1:
        sleep(1);
        return 'M';
        break;
    case 2:
        sleep(2);
        return 'G';
        break;
    }
}

int defineQualidadeProduto(){
    int i = geraNumeroAleatorio(QTD_QUALIDADE) + 1;

    switch (i)
    {
    case 4:
        sleep(1);
        break;
    case 5:
        sleep(2);
        break;
    default:
        break;
    }

    return (i);
}

void produtor(int* file_pipes){
    int data_processed;
    TProduto *produto;
    produto = (TProduto*)malloc(2 * sizeof(TProduto));

    while(1){
        defineNomeProduto(produto->nome);
        produto->tamanho = defineTamanhoProduto();
        produto->qualidade = defineQualidadeProduto();    
        data_processed = write(file_pipes[1], produto, sizeof(produto->nome) + sizeof(produto->tamanho) + sizeof(produto->qualidade) );
        printf("Wrote %d bytes\n", data_processed);
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