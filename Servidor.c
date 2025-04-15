#include <stdio.h>
#include <stdlib.h>     
#include <string.h>
#include <pthread.h>    
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Banco.h"

Registro banco[TAM];
int cont=0;
pthread_mutex_t m_banco = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_resposta = PTHREAD_MUTEX_INITIALIZER;

void *processar(void *arg);

void print_banco() {
    printf("\n Banco Atual:\n");
    for (int i = 0; i < cont; i++) {
        printf("  ID: %d | Nome: %s\n", banco[i].id, banco[i].nome);
    }
    printf("--------------------\n");
}

void inserir(int id, const char *nome) {
    if (cont < TAM) {
        banco[cont].id = id;
        strcpy(banco[cont].nome, nome);
        cont++;
   }
}

void *th_inserir(void *arg){
    char buffer[256];
    int fd;
    while(1){
        fd = open ("/tmp/pipe_inserir",O_RDONLY);
        while (read(fd,buffer,sizeof(buffer))>0){
        char *msg = strdup(buffer);
        processar((void*)msg);
    }
    close(fd);
}
return NULL;
}

void deletar(int id, char *resposta, size_t tamanho) {
    int encontrado = 0;
    for (int i = 0; i < cont; i++) {
        if (banco[i].id == id) {
            banco[i] = banco[cont - 1];
            cont--;
            snprintf(resposta, tamanho, "Deletado com sucesso: ID = %d", id);
            encontrado = 1;
            break;
        }
    }if (!encontrado){
        snprintf(resposta, tamanho, "Erro: ID %d não encontrado no banco.", id);
    }
}

void *th_deletar(void *arg) {
    char buffer[256];
    int fd;
    while (1) {
        fd = open("/tmp/pipe_deletar", O_RDONLY);
        while (read(fd, buffer, sizeof(buffer)) > 0) {
            char *msg = strdup(buffer);
            processar((void *)msg);
        }
        close(fd);
    }
    return NULL;

}

void selecionar(int id, char *resposta, size_t tamanho) {
    int encontrado = 0;
    for (int i = 0; i < cont; i++) {
        if (banco[i].id == id) {
            snprintf(resposta, tamanho, "Selecionado com sucesso: ID = %d, NOME = %s", banco[i].id, banco[i].nome);
            encontrado = 1;
            break;
        }
    }
    if (!encontrado){
        snprintf(resposta, tamanho, "Erro: ID %d não encontrado no banco.", id);
    }
}

void *th_selecionar(void *arg) {
    char buffer[256];
    int fd;
    while (1) {
        fd = open("/tmp/pipe_selecionar", O_RDONLY);
        while (read(fd, buffer, sizeof(buffer)) > 0) {
            char *msg = strdup(buffer);
            processar((void *)msg);
        }
        close(fd);
    }
    return NULL;
}

void atualizar(int id, const char *novo_nome,  char *resposta, size_t tamanho) {
    int encontrado = 0;
    for (int i = 0; i < cont; i++) {
        if (banco[i].id == id) {
            strcpy(banco[i].nome, novo_nome);
            encontrado = 1;
            snprintf(resposta, tamanho, "Atualizado com Sucesso ID %d", id);
            break;
        }
    }
    if (!encontrado){
        snprintf(resposta, tamanho, "Erro: ID %d não encontrado no banco.", id);
    }
}

void *th_atualizar(void *arg) {
    char buffer[256];
    int fd;
    while (1) {
        fd = open("/tmp/pipe_atualizar", O_RDONLY);
        while (read(fd, buffer, sizeof(buffer)) > 0) {
            char *msg = strdup(buffer);
            processar((void *)msg);
        }
        close(fd);
    }
    return NULL;
}


void *processar(void *arg) {
    char *requisicao = (char *)arg;
    char resposta[256];
    memset(resposta, 0, sizeof(resposta));
    printf("Requisição recebida: %s\n", requisicao);

    pthread_mutex_lock(&m_banco);

    int id;
    char nome[50];

    int op;
    sscanf(requisicao, "%d", &op);

    switch (op) {
        case 1: { // INSERIR
            if (sscanf(requisicao, "1 id=%d nome='%49[^']'", &id, nome) == 2) {
                inserir(id, nome);
                snprintf(resposta,sizeof(resposta),"Inserido com sucesso: ID = %d, NOME =%s",id,nome);
            } else {
                snprintf(resposta,sizeof(resposta),"Falha! Banco cheio.");
            }
            break;
        }

        case 2: { // DELETAR
            if (sscanf(requisicao, "2 id=%d", &id) == 1) {
                deletar(id, resposta, sizeof(resposta));
            } else {
                printf("Comando deletar inválido.\n");
            }
            break;
        }

        case 3: {
            if (sscanf(requisicao, "3 id=%d", &id) == 1) {
                selecionar(id, resposta, sizeof(resposta));
            } else {
                snprintf(resposta,sizeof(resposta),"Comando inválido para seleção.");
            }
            break;
        }

        case 4: { // ATUALIZAR
            if (sscanf(requisicao, "4 id=%d nome='%49[^']'", &id, nome) == 2) {
                atualizar(id, nome, resposta, sizeof(resposta));
            }else {
                snprintf(resposta,sizeof(resposta),"Comando inválido para atualização.");
            }
            break;
        }

        default:
            printf("Código de operação inválido: %d\n", op);
    }

    print_banco();

    pthread_mutex_unlock(&m_banco);

    pthread_mutex_lock(&m_resposta);
    
    int fd_resposta = open("/tmp/pipe_resposta", O_WRONLY);
    write(fd_resposta, resposta, strlen(resposta) + 1);
    close(fd_resposta);
    pthread_mutex_unlock(&m_resposta);

    free(requisicao);
    return NULL;
}


int main() {
    pthread_t t_inserir, t_deletar, t_selecionar, t_atualizar;
    
    mkfifo("/tmp/pipe_inserir", 0666);
    mkfifo("/tmp/pipe_deletar", 0666);
    mkfifo("/tmp/pipe_selecionar", 0666);
    mkfifo("/tmp/pipe_atualizar", 0666);
    mkfifo("/tmp/pipe_resposta",0666);

    
    pthread_create(&t_inserir, NULL, th_inserir, NULL);
    pthread_create(&t_deletar, NULL, th_deletar, NULL);
    pthread_create(&t_selecionar, NULL, th_selecionar, NULL);
    pthread_create(&t_atualizar, NULL, th_atualizar, NULL);
 
     // Aguarda as threads
    pthread_join(t_inserir, NULL);
    pthread_join(t_deletar, NULL);
    pthread_join(t_selecionar, NULL);
    pthread_join(t_atualizar, NULL);
 
    return 0;
}
