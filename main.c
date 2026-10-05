```c
#include <stdio.h>
#include <string.h>

#include "conta.h"


void exibirMenu(void) {
    printf("\n===== SISTEMA BANCARIO =====\n");
    printf("1 - Criar conta\n");
    printf("2 - Depositar\n");
    printf("3 - Sacar\n");
    printf("4 - Transferir\n");
    printf("5 - Consultar saldo\n");
    printf("6 - Exibir extrato\n");
    printf("7 - Listar todas as contas\n");
    printf("0 - Salvar e sair\n");
    printf("Escolha uma opcao: ");
}


int main(void) {
    Conta contas[MAX_CONTAS];

    int total = carregarContas(contas);
    int opcao;

    printf(
        "Bem-vindo ao banco! %d conta(s) carregada(s).\n",
        total
    );

    do {
        exibirMenu();

        /*
         * Verifica se realmente foi digitado um inteiro.
         */
        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida. Digite um numero.\n");
            limparBufferEntrada();
            continue;
        }

        switch (opcao) {

            /*
             * ==================================================
             * CRIAR CONTA
             * ==================================================
             */
            case 1: {
                char titular[TAM_NOME];
                char cpf[TAM_CPF];
                float saldoInicial;

                printf("Nome do titular: ");
                limparBufferEntrada();

                if (
                    fgets(
                        titular,
                        sizeof(titular),
                        stdin
                    ) == NULL
                ) {
                    printf("Erro ao ler o nome.\n");
                    break;
                }

                titular[
                    strcspn(titular, "\n")
                ] = '\0';

                /*
                 * Se o nome ultrapassar o tamanho do buffer,
                 * descarta o restante da linha.
                 */
                if (
                    strchr(titular, '\0') != NULL &&
                    !strchr(titular, '\n')
                ) {
                    limparBufferEntrada();
                }

                printf(
                    "CPF (somente numeros ou 000.000.000-00): "
                );

                if (
                    fgets(
                        cpf,
                        sizeof(cpf),
                        stdin
                    ) == NULL
                ) {
                    printf("Erro ao ler o CPF.\n");
                    break;
                }

                cpf[
                    strcspn(cpf, "\n")
                ] = '\0';

                /*
                 * Descarta eventual sobra caso o usuario
                 * tenha digitado uma entrada maior que TAM_CPF.
                 */
                if (!strchr(cpf, '\n')) {
                    limparBufferEntrada();
                }

                printf(
                    "Deposito inicial (pode ser 0): R$ "
                );

                if (
                    scanf(
                        "%f",
                        &saldoInicial
                    ) != 1
                ) {
                    printf("Valor invalido.\n");
                    limparBufferEntrada();
                    break;
                }

                criarConta(
                    contas,
                    &total,
                    titular,
                    cpf,
                    saldoInicial
                );

                break;
            }


            /*
             * ==================================================
             * DEPOSITO
             * ==================================================
             */
            case 2: {
                int numero;
                float valor;

                printf("Numero da conta: ");

                if (
                    scanf(
                        "%d",
                        &numero
                    ) != 1
                ) {
                    printf("Numero invalido.\n");
                    limparBufferEntrada();
                    break;
                }

                Conta *conta = buscarConta(
                    contas,
                    total,
                    numero
                );

                if (conta == NULL) {
                    printf("Conta nao encontrada.\n");
                    break;
                }

                printf("Valor do deposito: R$ ");

                if (
                    scanf(
                        "%f",
                        &valor
                    ) != 1
                ) {
                    printf("Valor invalido.\n");
                    limparBufferEntrada();
                    break;
                }

                if (depositar(conta, valor)) {
                    printf(
                        "Deposito realizado. "
                        "Novo saldo: R$ %.2f\n",
                        conta->saldo
                    );
                }

                break;
            }


            /*
             * ==================================================
             * SAQUE
             * ==================================================
             */
            case 3: {
                int numero;
                float valor;

                printf("Numero da conta: ");

                if (
                    scanf(
                        "%d",
                        &numero
                    ) != 1
                ) {
                    printf("Numero invalido.\n");
                    limparBufferEntrada();
                    break;
                }

                Conta *conta = buscarConta(
                    contas,
                    total,
                    numero
                );

                if (conta == NULL) {
                    printf("Conta nao encontrada.\n");
                    break;
                }

                printf("Valor do saque: R$ ");

                if (
                    scanf(
                        "%f",
                        &valor
                    ) != 1
                ) {
                    printf("Valor invalido.\n");
                    limparBufferEntrada();
                    break;
                }

                if (sacar(conta, valor)) {
                    printf(
                        "Saque realizado. "
                        "Novo saldo: R$ %.2f\n",
                        conta->saldo
                    );
                }

                break;
            }


            /*
             * ==================================================
             * TRANSFERENCIA
             * ==================================================
             */
            case 4: {
                int numeroOrigem;
                int numeroDestino;
                float valor;

                printf("Conta de origem: ");

                if (
                    scanf(
                        "%d",
                        &numeroOrigem
                    ) != 1
                ) {
                    printf("Numero invalido.\n");
                    limparBufferEntrada();
                    break;
                }

                printf("Conta de destino: ");

                if (
                    scanf(
                        "%d",
                        &numeroDestino
                    ) != 1
                ) {
                    printf("Numero invalido.\n");
                    limparBufferEntrada();
                    break;
                }

                Conta *origem = buscarConta(
                    contas,
                    total,
                    numeroOrigem
                );

                Conta *destino = buscarConta(
                    contas,
                    total,
                    numeroDestino
                );

                if (origem == NULL) {
                    printf(
                        "Conta de origem nao encontrada.\n"
                    );
                    break;
                }

                if (destino == NULL) {
                    printf(
                        "Conta de destino nao encontrada.\n"
                    );
                    break;
                }

                printf("Valor a transferir: R$ ");

                if (
                    scanf(
                        "%f",
                        &valor
                    ) != 1
                ) {
                    printf("Valor invalido.\n");
                    limparBufferEntrada();
                    break;
                }

                if (
                    transferir(
                        origem,
                        destino,
                        valor
                    )
                ) {
                    printf(
                        "Transferencia concluida com sucesso.\n"
                    );

                    printf(
                        "Saldo da origem: R$ %.2f\n",
                        origem->saldo
                    );

                    printf(
                        "Saldo do destino: R$ %.2f\n",
                        destino->saldo
                    );
                }

                break;
            }


            /*
             * ==================================================
             * CONSULTAR SALDO
             * ==================================================
             */
            case 5: {
                int numero;

                printf("Numero da conta: ");

                if (
                    scanf(
                        "%d",
                        &numero
                    ) != 1
                ) {
                    printf("Numero invalido.\n");
                    limparBufferEntrada();
                    break;
                }

                Conta *conta = buscarConta(
                    contas,
                    total,
                    numero
                );

                if (conta == NULL) {
                    printf("Conta nao encontrada.\n");
                } else {
                    printf(
                        "\nConta: %d\n",
                        conta->numero
                    );

                    printf(
                        "Titular: %s\n",
                        conta->titular
                    );

                    printf(
                        "Saldo atual: R$ %.2f\n",
                        conta->saldo
                    );
                }

                break;
            }


            /*
             * ==================================================
             * EXTRATO
             * ==================================================
             */
            case 6: {
                int numero;

                printf("Numero da conta: ");

                if (
                    scanf(
                        "%d",
                        &numero
                    ) != 1
                ) {
                    printf("Numero invalido.\n");
                    limparBufferEntrada();
                    break;
                }

                Conta *conta = buscarConta(
                    contas,
                    total,
                    numero
                );

                if (conta == NULL) {
                    printf("Conta nao encontrada.\n");
                } else {
                    exibirExtrato(conta);
                }

                break;
            }


            /*
             * ==================================================
             * LISTAR CONTAS
             * ==================================================
             */
            case 7:
                listarContas(
                    contas,
                    total
                );
                break;


            /*
             * ==================================================
             * SAIR
             * ==================================================
             */
            case 0:
                salvarContas(
                    contas,
                    total
                );

                printf(
                    "Dados salvos. Ate mais!\n"
                );
                break;


            default:
                printf(
                    "Opcao invalida.\n"
                );
        }

    } while (opcao != 0);

    return 0;
}
```
