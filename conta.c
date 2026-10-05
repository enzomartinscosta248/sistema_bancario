```c
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "conta.h"


/*
 * Limpa tudo que restou no buffer de entrada.
 */
void limparBufferEntrada(void) {
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta o restante da linha */
    }
}


/*
 * Gera o proximo numero de conta.
 *
 * As contas comecam em 1001.
 * Mesmo que uma conta seja removida, o proximo numero continua
 * sendo baseado no maior numero existente.
 */
static int proximoNumeroConta(
    const Conta contas[],
    int total
) {
    int maior = 1000;

    for (int i = 0; i < total; i++) {
        if (contas[i].numero > maior) {
            maior = contas[i].numero;
        }
    }

    return maior + 1;
}


/*
 * Verifica se o titular possui algum caractere alem de espacos.
 */
static int nomeValido(const char *nome) {
    if (nome == NULL) {
        return 0;
    }

    for (int i = 0; nome[i] != '\0'; i++) {
        if (!isspace((unsigned char)nome[i])) {
            return 1;
        }
    }

    return 0;
}


/*
 * Converte um CPF para somente os 11 digitos.
 *
 * Aceita:
 *   52998224725
 *
 * ou:
 *   529.982.247-25
 *
 * Qualquer outro caractere e rejeitado.
 */
static int normalizarCPF(
    const char *cpf,
    char cpfNumerico[TAM_CPF]
) {
    int quantidadeDigitos = 0;

    if (cpf == NULL) {
        return 0;
    }

    for (int i = 0; cpf[i] != '\0' && cpf[i] != '\n'; i++) {
        unsigned char caractere = (unsigned char)cpf[i];

        if (isdigit(caractere)) {
            if (quantidadeDigitos >= 11) {
                return 0;
            }

            cpfNumerico[quantidadeDigitos] = (char)caractere;
            quantidadeDigitos++;
        } else if (cpf[i] == '.' || cpf[i] == '-') {
            /*
             * Pontos e traco sao permitidos porque o usuario pode
             * digitar o CPF formatado.
             */
        } else if (isspace(caractere)) {
            /*
             * Permite espacos no final da entrada.
             */
            continue;
        } else {
            /*
             * Diferente da versao anterior, letras e outros
             * caracteres NAO sao simplesmente ignorados.
             */
            return 0;
        }
    }

    if (quantidadeDigitos != 11) {
        return 0;
    }

    cpfNumerico[11] = '\0';

    return 1;
}


/*
 * Verifica se o CPF ja esta cadastrado.
 */
static int cpfJaCadastrado(
    const Conta contas[],
    int total,
    const char *cpf
) {
    for (int i = 0; i < total; i++) {
        if (strcmp(contas[i].cpf, cpf) == 0) {
            return 1;
        }
    }

    return 0;
}


/*
 * Cria uma nova conta.
 */
int criarConta(
    Conta contas[],
    int *total,
    const char *titular,
    const char *cpf,
    float saldoInicial
) {
    char cpfNormalizado[TAM_CPF];

    if (total == NULL) {
        return -1;
    }

    if (*total >= MAX_CONTAS) {
        printf(
            "Limite de %d contas atingido.\n",
            MAX_CONTAS
        );
        return -1;
    }

    if (!nomeValido(titular)) {
        printf("Nome do titular invalido.\n");
        return -1;
    }

    if (!normalizarCPF(cpf, cpfNormalizado)) {
        printf(
            "CPF invalido. Digite 11 numeros ou no formato "
            "000.000.000-00.\n"
        );
        return -1;
    }

    if (!cpfValido(cpfNormalizado)) {
        printf("CPF invalido! Conta nao criada.\n");
        return -1;
    }

    if (cpfJaCadastrado(contas, *total, cpfNormalizado)) {
        printf("Este CPF ja possui uma conta cadastrada.\n");
        return -1;
    }

    if (saldoInicial < 0.0f) {
        printf("Saldo inicial nao pode ser negativo.\n");
        return -1;
    }

    Conta *nova = &contas[*total];

    nova->numero = proximoNumeroConta(contas, *total);

    strncpy(
        nova->titular,
        titular,
        TAM_NOME - 1
    );

    nova->titular[TAM_NOME - 1] = '\0';

    strcpy(nova->cpf, cpfNormalizado);

    nova->saldo = saldoInicial;

    (*total)++;

    /*
     * Se houver deposito inicial, registra no historico.
     */
    if (saldoInicial > 0.0f) {
        registrarHistorico(
            nova->numero,
            "Abertura de conta (deposito inicial)",
            saldoInicial,
            nova->saldo
        );
    }

    printf(
        "Conta criada! Numero: %d - Titular: %s\n",
        nova->numero,
        nova->titular
    );

    return nova->numero;
}


/*
 * Procura uma conta pelo numero.
 *
 * Retorna um ponteiro para a conta encontrada.
 * Retorna NULL se nao encontrar.
 */
Conta *buscarConta(
    Conta contas[],
    int total,
    int numero
) {
    for (int i = 0; i < total; i++) {
        if (contas[i].numero == numero) {
            return &contas[i];
        }
    }

    return NULL;
}


/*
 * Realiza um deposito.
 */
int depositar(
    Conta *conta,
    float valor
) {
    if (conta == NULL) {
        return 0;
    }

    if (valor <= 0.0f) {
        printf("Valor de deposito invalido.\n");
        return 0;
    }

    conta->saldo += valor;

    registrarHistorico(
        conta->numero,
        "Deposito",
        valor,
        conta->saldo
    );

    return 1;
}


/*
 * Realiza um saque.
 */
int sacar(
    Conta *conta,
    float valor
) {
    if (conta == NULL) {
        return 0;
    }

    if (valor <= 0.0f) {
        printf("Valor de saque invalido.\n");
        return 0;
    }

    if (valor > conta->saldo) {
        printf(
            "Saldo insuficiente. Saldo atual: R$ %.2f\n",
            conta->saldo
        );
        return 0;
    }

    conta->saldo -= valor;

    registrarHistorico(
        conta->numero,
        "Saque",
        valor,
        conta->saldo
    );

    return 1;
}


/*
 * Realiza transferencia entre duas contas.
 */
int transferir(
    Conta *origem,
    Conta *destino,
    float valor
) {
    if (origem == NULL || destino == NULL) {
        return 0;
    }

    if (origem == destino) {
        printf(
            "A conta de origem e a conta de destino "
            "nao podem ser iguais.\n"
        );
        return 0;
    }

    if (valor <= 0.0f) {
        printf("Valor de transferencia invalido.\n");
        return 0;
    }

    if (valor > origem->saldo) {
        printf(
            "Saldo insuficiente para transferencia. "
            "Saldo atual: R$ %.2f\n",
            origem->saldo
        );
        return 0;
    }

    origem->saldo -= valor;
    destino->saldo += valor;

    registrarHistorico(
        origem->numero,
        "Transferencia enviada",
        valor,
        origem->saldo
    );

    registrarHistorico(
        destino->numero,
        "Transferencia recebida",
        valor,
        destino->saldo
    );

    return 1;
}


/*
 * Exibe o extrato de uma conta.
 *
 * O arquivo de historico e compartilhado por todas as contas.
 * Por isso filtramos pelo numero da conta.
 */
void exibirExtrato(const Conta *conta) {
    if (conta == NULL) {
        return;
    }

    printf(
        "\n--- Extrato da conta %d (%s) ---\n",
        conta->numero,
        conta->titular
    );

    printf(
        "Saldo atual: R$ %.2f\n",
        conta->saldo
    );

    FILE *arquivo = fopen(
        ARQUIVO_HISTORICO,
        "r"
    );

    if (arquivo == NULL) {
        printf(
            "(sem operacoes registradas ainda)\n"
        );
        return;
    }

    char linha[256];
    int numeroLinha;
    int encontrou = 0;

    printf("Historico de operacoes:\n");

    while (fgets(
        linha,
        sizeof(linha),
        arquivo
    ) != NULL) {

        if (
            sscanf(linha, "%d", &numeroLinha) == 1 &&
            numeroLinha == conta->numero
        ) {
            printf("  %s", linha);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf(
            "  Nenhuma operacao registrada para esta conta.\n"
        );
    }

    fclose(arquivo);
}


/*
 * Lista todas as contas.
 */
void listarContas(
    const Conta contas[],
    int total
) {
    if (total == 0) {
        printf("Nenhuma conta cadastrada.\n");
        return;
    }

    printf(
        "\n%-8s %-25s %-15s %-12s\n",
        "Numero",
        "Titular",
        "CPF",
        "Saldo"
    );

    printf(
        "----------------------------------------------------------------\n"
    );

    for (int i = 0; i < total; i++) {
        printf(
            "%-8d %-25s %-15s R$ %8.2f\n",
            contas[i].numero,
            contas[i].titular,
            contas[i].cpf,
            contas[i].saldo
        );
    }
}


/*
 * Registra uma operacao no arquivo de historico.
 */
void registrarHistorico(
    int numeroConta,
    const char *operacao,
    float valor,
    float saldoFinal
) {
    FILE *arquivo = fopen(
        ARQUIVO_HISTORICO,
        "a"
    );

    if (arquivo == NULL) {
        printf(
            "Aviso: nao foi possivel registrar no historico.\n"
        );
        return;
    }

    time_t agora = time(NULL);
    struct tm *dataHora = localtime(&agora);

    if (dataHora == NULL) {
        fclose(arquivo);
        return;
    }

    fprintf(
        arquivo,
        "%d | %02d/%02d/%04d %02d:%02d | %-35s | "
        "R$ %8.2f | Saldo: R$ %8.2f\n",
        numeroConta,
        dataHora->tm_mday,
        dataHora->tm_mon + 1,
        dataHora->tm_year + 1900,
        dataHora->tm_hour,
        dataHora->tm_min,
        operacao,
        valor,
        saldoFinal
    );

    fclose(arquivo);
}


/*
 * Salva todas as contas em arquivo binario.
 */
void salvarContas(
    const Conta contas[],
    int total
) {
    FILE *arquivo = fopen(
        ARQUIVO_CONTAS,
        "wb"
    );

    if (arquivo == NULL) {
        printf("Erro ao salvar contas.\n");
        return;
    }

    if (fwrite(
        &total,
        sizeof(int),
        1,
        arquivo
    ) != 1) {
        printf("Erro ao salvar quantidade de contas.\n");
        fclose(arquivo);
        return;
    }

    if (
        total > 0 &&
        fwrite(
            contas,
            sizeof(Conta),
            (size_t)total,
            arquivo
        ) != (size_t)total
    ) {
        printf("Erro ao salvar os dados das contas.\n");
        fclose(arquivo);
        return;
    }

    fclose(arquivo);
}


/*
 * Carrega as contas do arquivo binario.
 *
 * Faz validacoes antes de usar o valor lido do arquivo para
 * evitar que um arquivo corrompido cause acesso fora do array.
 */
int carregarContas(
    Conta contas[]
) {
    FILE *arquivo = fopen(
        ARQUIVO_CONTAS,
        "rb"
    );

    if (arquivo == NULL) {
        return 0;
    }

    int total = 0;

    if (fread(
        &total,
        sizeof(int),
        1,
        arquivo
    ) != 1) {
        printf(
            "Erro: arquivo de contas invalido.\n"
        );
        fclose(arquivo);
        return 0;
    }

    if (total < 0 || total > MAX_CONTAS) {
        printf(
            "Erro: quantidade de contas no arquivo "
            "esta invalida.\n"
        );
        fclose(arquivo);
        return 0;
    }

    if (
        total > 0 &&
        fread(
            contas,
            sizeof(Conta),
            (size_t)total,
            arquivo
        ) != (size_t)total
    ) {
        printf(
            "Erro: arquivo de contas incompleto ou corrompido.\n"
        );
        fclose(arquivo);
        return 0;
    }

    fclose(arquivo);

    return total;
}


/*
 * Valida um CPF.
 *
 * Aceita apenas os 11 digitos numericos.
 * A normalizacao do formato (pontos/traco) acontece antes,
 * em normalizarCPF().
 */
int cpfValido(const char *cpf) {
    int digitos[11];

    if (cpf == NULL) {
        return 0;
    }

    if (strlen(cpf) != 11) {
        return 0;
    }

    for (int i = 0; i < 11; i++) {
        if (
            cpf[i] < '0' ||
            cpf[i] > '9'
        ) {
            return 0;
        }

        digitos[i] = cpf[i] - '0';
    }

    /*
     * Rejeita sequencias como:
     * 11111111111
     * 22222222222
     * etc.
     */
    int todosIguais = 1;

    for (int i = 1; i < 11; i++) {
        if (digitos[i] != digitos[0]) {
            todosIguais = 0;
            break;
        }
    }

    if (todosIguais) {
        return 0;
    }

    /*
     * Primeiro digito verificador.
     */
    int soma = 0;

    for (int i = 0; i < 9; i++) {
        soma += digitos[i] * (10 - i);
    }

    int resto = (soma * 10) % 11;

    if (resto == 10) {
        resto = 0;
    }

    if (resto != digitos[9]) {
        return 0;
    }

    /*
     * Segundo digito verificador.
     */
    soma = 0;

    for (int i = 0; i < 10; i++) {
        soma += digitos[i] * (11 - i);
    }

    resto = (soma * 10) % 11;

    if (resto == 10) {
        resto = 0;
    }

    if (resto != digitos[10]) {
        return 0;
    }

    return 1;
}
```
