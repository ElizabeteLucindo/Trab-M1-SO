#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int id;
char nome[50];

int main()
{
    int fd;
 
    // FIFO file path
    char * myfifo = "/tmp/myfifo";
 
    char buffer[256];

    printf("Conectado ao servidor. Selecione um comando:\n");

    while (1) {

        int opcao;
        printf("\n==== MENU ====\n");
        printf("1 - INSERT\n");
        printf("2 - DELETE\n");
        printf("3 - SELECT\n");
        printf("4 - UPDATE\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar(); 

        if (opcao == 0) {
            printf("Encerrando cliente...\n");
            break;
        }

        switch (opcao) {
            case 1: // INSERT
                printf("Digite o ID: ");
                scanf("%d", &id);
                getchar();
                printf("Digite o nome: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0'; // remove \n
                snprintf(buffer, sizeof(buffer), "1 id=%d nome='%s'", id, nome);
                break;

            case 2: // DELETE
                printf("Digite o ID para deletar: ");
                scanf("%d", &id);
                getchar();
                snprintf(buffer, sizeof(buffer), "2 id=%d", id);
                break;

            case 3: // SELECT
                printf("Digite o ID para buscar: ");
                scanf("%d", &id);
                getchar();
                snprintf(buffer, sizeof(buffer), "3 id=%d", id);
                break;

            case 4: // UPDATE
                printf("Digite o ID para atualizar: ");
                scanf("%d", &id);
                getchar();
                printf("Digite o novo nome: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';
                snprintf(buffer, sizeof(buffer), "4 id=%d nome='%s'", id, nome);
                break;

            default:
                printf("Opção inválida!\n");
                continue;
        }

        //Envia para servidor
        fd = open(myfifo, O_WRONLY);
        write(fd, buffer, strlen(buffer) + 1);
        close(fd);
    }

    return 0;
}