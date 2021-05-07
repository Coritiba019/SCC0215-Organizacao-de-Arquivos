#include "execution.h"
#include <stdio.h>
#include <string.h>

// Inicializa as variaveis que seram escritas no cabeçalho
void reset_cabecalho(REGISTRO_CABECALHO *cab)
{
    cab->status = '0';
    cab->RRNproxRegistro = 0;
    cab->numeroRegistrosInseridos = 0;
    cab->numeroRegistrosRemovidos = 0;
    cab->numeroRegistrosAtualizados = 0;

    return;
}

// Escreve o cabeçalho no arquivo binario
void write_cabecalho(FILE *arqOutput, REGISTRO_CABECALHO *cab)
{
    int i=0; // indice de um loop
    char lixo = '$'; // variavel usada para prencher o lixo do registro de cabeçalho

    // Reseta todos os valores do cabeçalho
    reset_cabecalho(cab);

    // Inicio da escrita dos dados de cabeçalho
    fwrite(&cab->status, sizeof(char), 1, arqOutput);
    fwrite(&cab->RRNproxRegistro, sizeof(int), 1, arqOutput);
    fwrite(&cab->numeroRegistrosInseridos, sizeof(int), 1, arqOutput);
    fwrite(&cab->numeroRegistrosRemovidos, sizeof(int), 1, arqOutput);
    fwrite(&cab->numeroRegistrosAtualizados, sizeof(int), 1, arqOutput);

    // Escreve o lixo nos 111 bytes restantes
    for(i=0;i<=110;i++)
    {
        fwrite(&lixo, sizeof(char), 1, arqOutput);
    }

    return ;
}

// Atualiza o cabeçalho após inserir os dados
void update_cabecalho(FILE *arqOutput, REGISTRO_CABECALHO cab)
{
    cab.status = '1';

    // Volta para o inicio do arquivo binario
    fseek(arqOutput, 0, SEEK_SET);

    // Atualiza os campos que foram modificados
    fwrite(&cab.status, sizeof(char), 1, arqOutput);
    fwrite(&cab.RRNproxRegistro, sizeof(int), 1, arqOutput);
    fwrite(&cab.numeroRegistrosInseridos, sizeof(int), 1, arqOutput);

    return ;
}

// Limpeza dos campos, para receber novos dados
void reset_dados(REGISTRO_DADOS *dados)
{
    dados->cidadeMae[0] = '\0';
    dados->cidadeBebe[0] = '\0';
    dados->idadeMae = -1;
    dados->dataNascimento[0] = '\0';
    dados->sexoBebe = '0';
    dados->estadoMae[0] = '\0';
    dados->estadoMae[1] = '$';
    dados->estadoBebe[0] = '\0';
    dados->estadoBebe[1] = '$';

    return;
}

// Leitura dos campos do arquivo de Entrada
void read_dados(FILE *arqInput, REGISTRO_DADOS *dados)
{
    // Variavel auxiliar para saber se o próximo campo é um número ou uma ","
    char c; 

    // Ignora a virgula
    fgetc(arqInput); 

    // Inicia a leitura dos dados
    fscanf(arqInput, "%[^,]", dados->cidadeBebe);
    fgetc(arqInput);

    fscanf(arqInput, "%d", &dados->idNascimento);
    fgetc(arqInput);

    c = fgetc(arqInput);
    if(c!=',')
    {
        fseek(arqInput, -1, SEEK_CUR);
        fscanf(arqInput, "%d", &dados->idadeMae);
        fgetc(arqInput);
    }

    fscanf(arqInput, "%[^,]", dados->dataNascimento);
    fgetc(arqInput);

    c = fgetc(arqInput);
    if(c!=',')
    {
        dados->sexoBebe = c;
        fgetc(arqInput);
    }

    fscanf(arqInput, "%[^,]", dados->estadoMae);
    fgetc(arqInput);

    fscanf(arqInput, "%[^\n]", dados->estadoBebe);
    fgetc(arqInput);

    dados->tamcidadeMae = strlen(dados->cidadeMae);
    dados->tamcidadeBebe = strlen(dados->cidadeBebe);

    return;
}

// Escreve os dados no arquivo binario
void write_dados(FILE *arqOutput, REGISTRO_DADOS dados, REGISTRO_CABECALHO *cab)
{
    int i=0, byteoffset=0;
    char lixo = '$';

    // Calcula onde o registro deve ser inserido
    byteoffset = 128 + (cab->RRNproxRegistro)*128;
    fseek(arqOutput, byteoffset, SEEK_SET);

    // Escreve no arquivo binario
    fwrite(&dados.tamcidadeMae, sizeof(int), 1, arqOutput);
    fwrite(&dados.tamcidadeBebe, sizeof(int), 1, arqOutput);
            
    if(dados.tamcidadeMae>0)
    {
        fwrite(dados.cidadeMae, sizeof(char), dados.tamcidadeMae, arqOutput);
    }

    if(dados.tamcidadeBebe>0)
    {
        fwrite(dados.cidadeBebe, sizeof(char), dados.tamcidadeBebe, arqOutput);
    }

    if((dados.tamcidadeMae+dados.tamcidadeBebe)<97)
    {
        for(i=(dados.tamcidadeMae+dados.tamcidadeBebe);i<97;i++)
        {
            fwrite(&lixo, sizeof(char), 1, arqOutput);
        }
    }

    fwrite(&dados.idNascimento, sizeof(int), 1, arqOutput);
    fwrite(&dados.idadeMae, sizeof(int), 1, arqOutput);
            
    if(dados.dataNascimento[0]=='\0')
    {
        fwrite(&dados.dataNascimento[0], sizeof(char), 1, arqOutput);

        for(i=0;i<9;i++)
        { 
            fwrite(&lixo, sizeof(char), 1, arqOutput);
        }
    }
    else
    {
        fwrite(dados.dataNascimento, sizeof(char), 10, arqOutput);
    }

    fwrite(&dados.sexoBebe, sizeof(char), 1, arqOutput);

    fwrite(dados.estadoMae, sizeof(char), 2, arqOutput);

    fwrite(dados.estadoBebe, sizeof(char), 2, arqOutput);

    // Incrementa os dados do cabeçalho
    cab->RRNproxRegistro += 1;
    cab->numeroRegistrosInseridos += 1;

    return ;
}

// Faz a verificação do status do arquivo e do numero de registros inseridos
// para saber se o arquivo encontra-se consistente.
int verifica_cabecalho_bin(FILE *arqOutput, REGISTRO_CABECALHO *cab)
{
    fread(&cab->status, sizeof(char), 1, arqOutput);
    fread(&cab->RRNproxRegistro, sizeof(int), 1, arqOutput);
    fread(&cab->numeroRegistrosInseridos, sizeof(int), 1, arqOutput);

    if(cab->status=='0')
    {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    if(cab->numeroRegistrosInseridos==0)
    {
        printf("Registro inexistente.\n");
        return 0;
    }

    // Se as informações de cabeçalho forem válidas ele pula o inicio para ler os dados
    fseek(arqOutput, 128, SEEK_SET);

    return 1;
}

// Após a leitura dos dados do arquivo binário, a função coloca o '\n' 
// nas strings que precisam ser impressas na tela
void aux_imprimir_dados_bin(REGISTRO_DADOS *dados)
{

    dados->cidadeBebe[dados->tamcidadeBebe] = '\0';
    dados->dataNascimento[10] = '\0';
    dados->estadoMae[2] = '\0';
    dados->estadoBebe[2] = '\0';

    return ;
}

// Imprimi os dados do arquivo binário na tela
void imprimir_dados_bin(REGISTRO_DADOS dados)
{
    if(dados.tamcidadeBebe>0)
    {
        printf("Nasceu em %s/", dados.cidadeBebe);
    }
    else
    {
        printf("Nasceu em -/");
    }

    if(dados.estadoBebe[0]!='\0')
    {
        printf("%s, ", dados.estadoBebe);
    }
    else
    {
        printf("-, ");
    }

    if(dados.dataNascimento[0]!='\0')
    {
        printf("em %s, ", dados.dataNascimento);
    }
    else
    {
        printf("em -, ");
    }
 
    if(dados.sexoBebe=='1')
    {
        printf("um bebê de sexo MASCULINO.\n");
    }
    else if(dados.sexoBebe=='2')
    {
        printf("um bebê de sexo FEMININO.\n");
    }   
    else
    {
        printf("um bebê de sexo IGNORADO.\n");
    }

    return ;
}

// Lê os dados do arquivo binario e armazena na variavel para ser impresso
int read_dados_bin(FILE *arqOutput, REGISTRO_DADOS *dados)
{
    int i=0;
    char aux[100];

    fread(&dados->tamcidadeMae, sizeof(int), 1, arqOutput);
    fread(&dados->tamcidadeBebe, sizeof(int), 1, arqOutput);

    // Se o registro encontra-se logicamente removido ele pula para o proximo
    if(dados->tamcidadeMae==(-1))
    {
        fseek(arqOutput, 128, SEEK_CUR);

        return 0;
    }

    if(dados->tamcidadeMae>0)
    {
        fread(dados->cidadeMae, dados->tamcidadeMae, 1, arqOutput);
    }   
    if(dados->tamcidadeBebe>0)
    {
        fread(dados->cidadeBebe, dados->tamcidadeBebe, 1, arqOutput);
    }
        
    for(i=(dados->tamcidadeMae+dados->tamcidadeBebe);i<97;i++)
    {
        fread(aux, sizeof(char), 1, arqOutput);
    }        

    fread(&dados->idNascimento, sizeof(int), 1, arqOutput);
    fread(&dados->idadeMae, sizeof(int), 1, arqOutput);
    fread(dados->dataNascimento, 10, 1, arqOutput);
    fread(&dados->sexoBebe, sizeof(char), 1, arqOutput);
    fread(dados->estadoMae, 2, 1, arqOutput);
    fread(dados->estadoBebe, 2, 1, arqOutput);

    return 1;
}