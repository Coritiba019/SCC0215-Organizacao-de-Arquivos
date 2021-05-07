// Nome : Fabio Dias da Cunha 
// N°USP: 11320874

#include <stdio.h>
#include <string.h>
#include "binarioNaTela.h"
#include "execution.h"

int main(void)
{
    REGISTRO_CABECALHO cab;
    REGISTRO_DADOS dados;
    FILE *arqInput, *arqOutput;
    char strAux[100], strInput[100], strOutput[100];
    int op=0, i=0, j=0;

    // Lê a operação digitada pelo usuario
    scanf("%d", &op);

    if(op==1)
    {
        // Lê quais arquivos o usuário deseja abrir
        scanf("%s%s", strInput, strOutput);

        // Abre o primeiro arquivo para leitura
        if((arqInput=fopen(strInput, "r"))==NULL)
        {
            printf("Falha no carregamento do arquivo.\n");

            return 0;
        }

        // Abre o segundo arquivo para escrita em binário
        if((arqOutput=fopen(strOutput, "wb"))==NULL)
        {
            printf("Falha no carregamento do arquivo.\n");

            fclose(arqInput);
            return 0;
        }

        // Escreve os dados do cabeçalho no inicio do arquivo
        write_cabecalho(arqOutput, &cab);

        // Pula a primeira que representa como os dados estão organizados
        fgets(strAux, 89, arqInput);

        // Reseta valores do arquivo de entrada
        reset_dados(&dados);

        // Percorre o arquivo de entrada até o fim
        while(fscanf(arqInput, "%[^,]", dados.cidadeMae)!=EOF)
        {
            // Le o arquivo de entrada
            read_dados(arqInput, &dados);
            
            // Escreve no arquivo de saida
            write_dados(arqOutput, dados, &cab);

            // Reseta valores do arquivo de entrada
            reset_dados(&dados);
        }

        // Atualiza as informações do cabeçalho
        update_cabecalho(arqOutput, cab);
        
        // Fecha os arquivos
        fclose(arqInput);
        fclose(arqOutput);

        // Mostra a saida do arquivo binario
        binarioNaTela(strOutput);
    }
    else if(op==2)
    {
        // Lê qual arquivo o usuário deseja abrir
        scanf("%s", strOutput);

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

        // Percorre o arquivo binario e imprimi as informações
        for(i=0;i<cab.numeroRegistrosInseridos;i++)
        {
            // Lê os dados do arquivo binario
            j = read_dados_bin(arqOutput, &dados);

            // Se o registro não estiver logicamente removido ele é impresso na tela
            if(j==1)
            {
                // Coloca o '\0' no final das strings para ser impresso na tela
                aux_imprimir_dados_bin(&dados);
            
                // Imprimi os dados na tela
                imprimir_dados_bin(dados);
            }
        }

        fclose(arqOutput);
    }
    
    return 0;
}

