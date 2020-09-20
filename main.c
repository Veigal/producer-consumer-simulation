#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Struct produto
typedef struct Produto {
    char nome[20];
    int qualidade;
    char tamanho;
} TProduto;

void consumidor(int* file_pipes){
    int data_processed;
    TProduto *produtoRecebe;
    produtoRecebe = (TProduto*)malloc(2 * sizeof(TProduto));

    data_processed = read(file_pipes[0], produtoRecebe, sizeof(produtoRecebe));
    printf("Read %d bytes: %s\n", data_processed, produtoRecebe->nome);
    data_processed = read(file_pipes[0], produtoRecebe, sizeof(produtoRecebe));
    printf("Read %d bytes: %s\n", data_processed, produtoRecebe->nome);
    exit(EXIT_SUCCESS);
}

void produtor(int* file_pipes){
    int data_processed;
    TProduto *produto;
    produto = (TProduto*)malloc(2 * sizeof(TProduto));

    strcpy(produto->nome, "testess");
    data_processed = write(file_pipes[1], produto, sizeof(produto));
    printf("Wrote %d bytes\n", data_processed);
    strcpy(produto->nome, "testess2222");
    data_processed = write(file_pipes[1], produto, sizeof(produto));
    printf("Wrote %d bytes\n", data_processed);
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