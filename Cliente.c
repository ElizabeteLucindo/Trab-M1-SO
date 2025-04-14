#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int id;
char nome[50];

void enviar_requisicao(const char *pipe, const char *buffer) {
    int fd = open(pipe, O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir pipe");
        return;
    }
    
    write(fd, buffer, strlen(buffer) + 1);  // Envia a mensagem para a pipe
    close(fd);
}

int main()
{
 
    char buffer[256];

    printf("Conectado ao servidor. Selecione um comando:\n");

    while (1) {

        int opcao;
        printf("\n==== MENU ====\n");
        printf("1 - INSERIR\n");
        printf("2 - DELETAR\n");
        printf("3 - SELECIONAR\n");
        printf("4 - ATUALIZAR\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar(); 

        if (opcao == 0) {
            printf("Encerrando cliente...\n");
            break;
        }

        switch (opcao) {
            case 1: // INSERIR
                printf("Digite o ID: ");
                scanf("%d", &id);
                getchar();
                printf("Digite o nome: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0'; // remove \n
                snprintf(buffer, sizeof(buffer), "1 id=%d nome='%s'", id, nome);
                enviar_requisicao("/tmp/pipe_inserir", buffer);
                break;

            case 2: // DELETAR
                printf("Digite o ID para deletar: ");
                scanf("%d", &id);
                getchar();
                snprintf(buffer, sizeof(buffer), "2 id=%d", id);
                enviar_requisicao("/tmp/pipe_deletar", buffer);
                break;

            case 3: // SELECIONAR
                printf("Digite o ID para buscar: ");
                scanf("%d", &id);
                getchar();
                snprintf(buffer, sizeof(buffer), "3 id=%d", id);
                enviar_requisicao("/tmp/pipe_selecionar", buffer);
                break;

            case 4: // ATUALIZAR
                printf("Digite o ID para atualizar: ");
                scanf("%d", &id);
                getchar();
                printf("Digite o novo nome: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';
                snprintf(buffer, sizeof(buffer), "4 id=%d nome='%s'", id, nome);
                enviar_requisicao("/tmp/pipe_atualizar", buffer);
                break;

            default:
                printf("Opção inválida!\n");
                continue;
        }

        //Resposta do servidor
        char resposta[256];
        int fd1 = open("/tmp/pipe_resposta", O_RDONLY);
        if (fd1 == -1) {
            perror("Erro ao abrir pipe de resposta");
        }
        read(fd1, resposta, sizeof(resposta));
        printf("%s",resposta);
        close(fd1);
    }
    return 0;
}
