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
        printf("Inserido: ID=%d Nome=%s\n", id, nome);
    } else {
        printf("Banco cheio!\n");
    }
}

void deletar(int id) {
    for (int i = 0; i < cont; i++) {
        if (banco[i].id == id) {
            banco[i] = banco[cont - 1];
            cont--;
            printf("Deletado ID=%d\n", id);
            return;
        }
    }
    printf("ID %d não encontrado.\n", id);
}

void selecionar(int id) {
    for (int i = 0; i < cont; i++) {
        if (banco[i].id == id) {
            printf("Encontrado: ID=%d Nome=%s\n", banco[i].id, banco[i].nome);
            return;
        }
    }
    printf("ID %d não encontrado.\n", id);
}

void atualizar(int id, const char *novo_nome) {
    for (int i = 0; i < cont; i++) {
        if (banco[i].id == id) {
            strcpy(banco[i].nome, novo_nome);
            printf("Atualizado: ID=%d Novo Nome=%s\n", id, novo_nome);
            return;
        }
    }
    printf("ID %d não encontrado para atualizar.\n", id);
}

void *processar(void *arg) {
    char *requisicao = (char *)arg;
    printf("Requisição recebida: %s\n", requisicao);
    pthread_mutex_lock(&m_banco);

    int op;
    sscanf(requisicao, "%d", &op);

    switch (op) {
        case 1: { // INSERT
            int id;
            char nome[50];
            if (sscanf(requisicao, "1 id=%d nome='%49[^']'", &id, nome) == 2) {
                inserir(id, nome);
            } else {
                printf("Comando INSERT inválido.\n");
            }
            break;
        }

        case 2: { // DELETE
            int id;
            if (sscanf(requisicao, "2 id=%d", &id) == 1) {
                deletar(id);
            } else {
                printf("Comando DELETE inválido.\n");
            }
            break;
        }

        case 3: { // SELECT
            int id;
            if (sscanf(requisicao, "3 id=%d", &id) == 1) {
                selecionar(id);
            } else {
                printf("Comando SELECT inválido.\n");
            }
            break;
        }

        case 4: { // UPDATE
            int id;
            char nome[50];
            if (sscanf(requisicao, "4 id=%d nome='%49[^']'", &id, nome) == 2) {
                atualizar(id, nome);
            } else {
                printf("Comando UPDATE inválido.\n");
            }
            break;
        }

        default:
            printf("Código de operação inválido: %d\n", op);
    }

    print_banco();

    pthread_mutex_unlock(&m_banco);
    free(requisicao);
    return NULL;
}


int main() {
    char *myfifo = "/tmp/myfifo";
    mkfifo(myfifo, 0666);
    char buffer[256];

    while (1) {
        int fd = open(myfifo, O_RDONLY);
        if (fd == -1) continue;

        while (read(fd, buffer, sizeof(buffer)) > 0) {
            char *req = strdup(buffer);
            pthread_t thread;
            pthread_create(&thread, NULL, processar, req);
            pthread_detach(thread);
        }

        close(fd);
    }

    unlink(myfifo);
    return 0;
}
