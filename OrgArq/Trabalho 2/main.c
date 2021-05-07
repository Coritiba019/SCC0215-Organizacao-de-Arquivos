// Nome : Fabio Dias da Cunha 
// N°USP: 11320874

#include <stdio.h>
#include <string.h>
#include "binarioNaTela.h"
#include "execution.h"

int main(void)
{
    REGISTRO_CABECALHO cab;
    REGISTRO_DADOS dados, dados2;
    FILE *arqOutput;
    char strOutput[100];
    int op=0, j=0, m=0;

    // Lê a operação digitada pelo usuário
    scanf("%d", &op);

    // Lê qual arquivo o usuário deseja abrir
    scanf("%s", strOutput);

    if(op==3) //Realiza uma busca parametrizada
    {
        // Abre o arquivo binario para leitura
        if((arqOutput=fopen(strOutput, "rb"))==NULL)
        {
            printf("Falha no processamento do arquivo.\n");

            return 0;
        }

        // Verifica se tem registros e se o status é igual a 0
        j=verifica_cabecalho_bin(arqOutput, &cab);

        // Se a função retornar 0, ocorreu um erro, sendo assim é encerrado o programa
        if(j==0)
        {
            fclose(arqOutput);
            return 0;
        }

        // Percorre o arquivo binario e imprimi os registros que serão selecionados de acordo com os parâmetros
        busca_parametrizada(arqOutput, &dados, &dados2, &cab);

        fclose(arqOutput);
    }
    else if(op==4) // Busca utilizando o RRN como parâmetro
    {
        // Abre o arquivo binario para leitura
        if((arqOutput=fopen(strOutput, "rb"))==NULL)
        {
            printf("Falha no processamento do arquivo.\n");

            return 0;
        }

        // Verifica se tem registros e se o status é igual a 0
        j=verifica_cabecalho_bin(arqOutput, &cab);

        // Se a função retornar 0, ocorreu um erro, sendo assim é encerrado o programa
        if(j==0)
        {
            fclose(arqOutput);
            return 0;
        }

        // Procura um registro pelo RRN e imprimi o registro
        busca_RRN(arqOutput, &dados);
    }
    else if(op==5) // Remoção lógica de registros
    {
        // Abre o arquivo binario para escrita e leitura
        if((arqOutput=fopen(strOutput, "rb+"))==NULL)
        {
            printf("Falha no processamento do arquivo.\n");

            return 0;
        }

        // Verifica se tem registros e se o status é igual a 0
        j=verifica_cabecalho_bin(arqOutput, &cab);

        // Se a função retornar 0, ocorreu um erro, sendo assim é encerrado o programa
        if(j==0)
        {
            fclose(arqOutput);
            return 0;
        }

        // Função responsável por remover diversos registros do arquivo binário
        remover_registros(arqOutput, &cab, &dados, &dados2);

        fclose(arqOutput);

        // Mostra a saida do arquivo binario
        binarioNaTela(strOutput);
    }
    else if(op==6) // Inserção de registros adicionais
    {
        // Abre o arquivo binario para escrita e leitura
        if((arqOutput=fopen(strOutput, "rb+"))==NULL)
        {
            printf("Falha no processamento do arquivo.\n");

            return 0;
        }

        // Verifica se tem registros e se o status é igual a 0
        j=verifica_cabecalho_bin(arqOutput, &cab);

        // Se a função retornar 0, ocorreu um erro, sendo assim é encerrado o programa
        if(j==0)
        {
            fclose(arqOutput);
            return 0;
        }

        // Função responsável por inserir diversos registros no arquivo binário
        inserir_registros(arqOutput, &cab, &dados2);

        fclose(arqOutput);

        // Mostra a saida do arquivo binario
        binarioNaTela(strOutput);
    }
    else if(op==7) // Atualização de campos de um registro identificado por seu RRN
    {
        // Abre o arquivo binario para escrita e leitura
        if((arqOutput=fopen(strOutput, "rb+"))==NULL)
        {
            printf("Falha no processamento do arquivo.\n");

            return 0;
        }

        // Verifica se tem registros e se o status é igual a 0
        j=verifica_cabecalho_bin(arqOutput, &cab);

        // Se a função retornar 0, ocorreu um erro, sendo assim é encerrado o programa
        if(j==0)
        {
            fclose(arqOutput);
            return 0;
        }

        // Função responsável por atualizar diversos registros no arquivo binário, dado o RRN do registro
        atualizar_registros(arqOutput, &cab, &dados2);

        fclose(arqOutput);

        // Mostra a saida do arquivo binario
        binarioNaTela(strOutput);
    }
    
    return 0;
}