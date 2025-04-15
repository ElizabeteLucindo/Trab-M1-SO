#ifndef BANCO_H
#define BANCO_H
#define TAM 100

typedef struct{
 int id;
 char nome[50];
} Registro;

extern Registro banco [TAM];
extern pthread_mutex_t m_banco;

void inserir(int id, const char *nome);
void deletar(int id, char *resposta, size_t tamanho);
void atualizar(int id, const char *novo_nome,  char *resposta, size_t tamanho);
void selecionar(int id, char *resposta, size_t tamanho);

#endif
