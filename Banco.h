////pode ter 2 pipes um para inserir e outro delete
#ifndef BANCO_H
#define BANCO_H
#define TAM 100

typedef struct{
 int id;
 char nome[50];
} Registro;

extern Registro banco [TAM];
extern pthread_mutex_t m_banco;




///assinaturas das funções



#endif