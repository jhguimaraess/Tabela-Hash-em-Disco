#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct{
    int id;
    char nome[50];
    long prox;
}Cliente;

Cliente* leCliente(FILE* f);
void criaBaseCliente(FILE* f, int tam);
void embaralhaBase(int *vet, int tam);
void imprimeBase(FILE* f);
void imprimeTabelaHash(FILE* fTHash, FILE* fClientes, int  m);

void inicializaTabelaHash(FILE* arqTHash, int m){
    rewind(arqTHash);
    long vazio = -1;
    for (int i = 0; i < m; i++){
        fwrite(&vazio, sizeof(vazio), 1, arqTHash);
    }
}

int funcHash(int id, int m){
    return (id % m);
}

void tabelaHash(FILE* fTHash, FILE* fCliente, int m, int tam){

    inicializaTabelaHash(fTHash, m);
    printf("TABELA INICIALIZADA COM SUCESSO\n");

    rewind(fCliente);
    Cliente  c;
    for(int i = 0; i < tam; i++){

        long posicaoCliente = i * sizeof(Cliente);

        fseek(fCliente, posicaoCliente, SEEK_SET);
        fread(&c, sizeof(Cliente), 1, fCliente);
        int posicaoNaTabelaHash = funcHash(c.id, m);

        printf("\n\nInformacoes do primeiro cliete a ser analizado: id: %d \nposicao dele no arquivo de clientes: %ld \nprox dele %ld \nLocal que ele vai ficar na t hash: %d", c.id, posicaoCliente, c.prox, posicaoNaTabelaHash);

        long enderecoGuardadoNaTabela;
        fseek(fTHash, posicaoNaTabelaHash * sizeof(long), SEEK_SET);
        fread(&enderecoGuardadoNaTabela, sizeof(long), 1, fTHash);

        printf("\nEndereco que esta na posicao %d da tabela hash: %ld\n", posicaoNaTabelaHash, enderecoGuardadoNaTabela);

        if(enderecoGuardadoNaTabela == -1){

            fseek(fTHash, posicaoNaTabelaHash * sizeof(long), SEEK_SET);
            fwrite(&posicaoCliente, sizeof(long), 1, fTHash);

            printf("--Endereco para o cliente com id %d gravado na posicao %d\n", c.id, posicaoNaTabelaHash);
            
        }else{

            long valorAtual = enderecoGuardadoNaTabela;
            fseek(fCliente, valorAtual, SEEK_SET);
            fread(&c, sizeof(Cliente), 1, fCliente);

            printf("--chegamos ate o cliente que esta na posicao %ld do arquivo de clientes, ele tem id: %d e o prox e: %ld\n", valorAtual, c.id, c.prox);

            while(1){
                if(c.prox == -1){

                    c.prox = posicaoCliente;
                    printf("----agora o prox do cliente de id %d e o client que esta na posicao %ld no arquivo de clientes\n", c.id, posicaoCliente);

                    fseek(fCliente, valorAtual, SEEK_SET);
                    fwrite(&c, sizeof(Cliente), 1, fCliente);

                    break;

                }
                valorAtual = c.prox;

                fseek(fCliente, valorAtual, SEEK_SET);
                fread(&c, sizeof(Cliente), 1, fCliente);

                printf("----cliente a ser avaliado agora: id: %d, posicao: %ld, prox: %ld\n", c.id, valorAtual, c.prox);
            }
            
        }

    }

}

Cliente buscaClienteTabelaHash(FILE* fTHash, FILE* fCliente, int id, int m){
    
    int posicaoNaTabelaHash = funcHash(id, m);

    printf("\n\nIniciando busca - Posicao %d (de acordo com o resultado de %d modulo %d), na tabela hash\n", posicaoNaTabelaHash, id, m);

    long enderecoGuardadoNoTabela;
    fseek(fTHash, posicaoNaTabelaHash * sizeof(long), SEEK_SET);
    fread(&enderecoGuardadoNoTabela, sizeof(long), 1, fTHash);

    printf("endereco guardado na tabela %ld\n", enderecoGuardadoNoTabela);

    Cliente c;
    if(enderecoGuardadoNoTabela == -1){
        printf("Endereco indefinido\n");
        c.id = -1;
        return c;
    }

    fseek(fCliente, enderecoGuardadoNoTabela, SEEK_SET);
    fread(&c, sizeof(Cliente), 1, fCliente);

    long enderecoAtual = enderecoGuardadoNoTabela;
    while(enderecoAtual != -1){
        fseek(fCliente, enderecoAtual, SEEK_SET);
        fread(&c, sizeof(Cliente), 1, fCliente);

        printf("--Avaliando o cliente id: %d, que esta na posicao %ld no arquivo de clientes\n", c.id, enderecoAtual);

        if(c.id == id){
            printf("Cliente encontrado!\n");
            return c;
        }else{
            enderecoAtual = c.prox;
        }
    }

    printf("cliente nao encontrado\n");
        c.id = -1;
        return c;

}

int main(int argc, char const *argv[]){

    FILE *f = fopen("teste.bin", "wb+");
    FILE* arqTHash = fopen("tabelaHash.bin", "wb+");

    int tamBase = 20;
    criaBaseCliente(f, tamBase);

    int m = 7;
    tabelaHash(arqTHash, f, m, tamBase);

    Cliente c = buscaClienteTabelaHash(arqTHash, f, 15, m);
    printf("\n\nCLIENTE:\nID: %d \nNome: %s\n", c.id, c.nome);

    imprimeTabelaHash(arqTHash, f, m);

    fclose(f);
    fclose(arqTHash);

    return 0;
}









///////////////////////////////////////////////////////////////

Cliente* leCliente(FILE* f){
    Cliente *c = (Cliente*)malloc(sizeof(Cliente));
    if((fread(c, sizeof(Cliente), 1, f)) != 1){
        free(c);
        return NULL;
    }
    return c;
}

void criaBaseCliente(FILE* f, int tam){

    int ids[tam];
    int vazio = -1;
    for (int i = 0; i < tam; i++) ids[i] = i + 1;
    embaralhaBase(ids, tam);

    for(int i = 0; i < tam; i++){
        Cliente c = {ids[i], "cliente", vazio};
        fwrite(&c, sizeof(Cliente), 1, f);
    }
}

void embaralhaBase(int *vet, int tam) {
    srand(time(NULL));
    for (int i = 0; i < tam; i++) {
        int j = rand() % tam;
        int tmp = vet[i];
        vet[i] = vet[j];
        vet[j] = tmp;
    }
}

void imprimeBase(FILE* f){
    Cliente *c = (Cliente*)malloc(sizeof(Cliente));
    rewind(f);
    while((c = leCliente(f)) != NULL){
        printf("Cliente ID: %d \n", c->id);
        free(c);
    }
}


void imprimeTabelaHash(FILE* fTHash, FILE* fClientes, int  m){
    rewind(fTHash);

    printf("\n-------Tabela Hash--------\n");
    printf("|  posicao  |  endereco  |\n");
    for(int i = 0; i < m; i++){
        long posicao = i * sizeof(long);

        long endereco;
        fseek(fTHash, posicao, SEEK_SET);
        fread(&endereco, sizeof(long), 1, fTHash);

        printf("|     %d     |     %ld    ", i, endereco);

        Cliente c;
        fseek(fClientes, endereco, SEEK_SET);
        fread(&c, sizeof(Cliente), 1, fClientes);

        printf("|    %d  -> ", c.id);
        long enderecoAtual = c.prox;
        while(enderecoAtual != -1){

            fseek(fClientes, enderecoAtual, SEEK_SET);
            fread(&c, sizeof(Cliente), 1, fClientes);

            printf(" %d  ->  ", c.id);

            enderecoAtual = c.prox;
        }
        printf("  -1  \n");
    }

    printf("--------------------------\n");
}