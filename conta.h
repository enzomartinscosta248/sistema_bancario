#ifndef CONTA_H
#define CONTA_H

#define MAX_CONTAS 100
#define TAM_NOME 50
#define TAM_CPF 15

#define ARQUIVO_CONTAS "contas.dat"
#define ARQUIVO_HISTORICO "historico.txt"

typedef struct {
    int numero;
    char titular[TAM_NOME];
    char cpf[TAM_CPF]; /* armazenado somente com os 11 digitos */
    float saldo;
} Conta;

/* Contas */
int criarConta(
    Conta contas[],
    int *total,
    const char *titular,
    const char *cpf,
    float saldoInicial
);

Conta *buscarConta(Conta contas[], int total, int numero);

void listarContas(
    const Conta contas[],
    int total
);

/* Operacoes bancarias */
int depositar(Conta *conta, float valor);
int sacar(Conta *conta, float valor);
int transferir(Conta *origem, Conta *destino, float valor);

/* Extrato */
void exibirExtrato(const Conta *conta);

void registrarHistorico(
    int numeroConta,
    const char *operacao,
    float valor,
    float saldoFinal
);

/* Persistencia */
void salvarContas(
    const Conta contas[],
    int total
);

int carregarContas(Conta contas[]);

/* Validacoes */
int cpfValido(const char *cpf);

/* Utilidades */
void limparBufferEntrada(void);

#endif
