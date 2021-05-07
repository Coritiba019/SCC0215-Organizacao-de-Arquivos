#include "execution.h"
#include "binarioNaTela.h"
#include <stdio.h>
#include <string.h>

// Faz a verificação do status do arquivo e do numero de registros inseridos
// para saber se o arquivo encontra-se consistente.
int verifica_cabecalho_bin(FILE *arqOutput, REGISTRO_CABECALHO *cab)
{
    // Reseta todos os valores do cabeçalho
    reset_cabecalho(cab);

    // Inicio da leitura dos dados de cabeçalho
    fread(&cab->status, sizeof(char), 1, arqOutput);
    fread(&cab->RRNproxRegistro, sizeof(int), 1, arqOutput);
    fread(&cab->numeroRegistrosInseridos, sizeof(int), 1, arqOutput);
    fread(&cab->numeroRegistrosRemovidos, sizeof(int), 1, arqOutput);
    fread(&cab->numeroRegistrosAtualizados, sizeof(int), 1, arqOutput);

    if(cab->status=='0')
    {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    if(cab->numeroRegistrosInseridos==0)
    {
        printf("Registro Inexistente.\n");
        return 0;
    }

    // Se as informações de cabeçalho forem válidas ele pula o inicio para ler os dados
    fseek(arqOutput, 128, SEEK_SET);

    return 1;
}

// Inicializa as variáveis que serão escritas no cabeçalho
void reset_cabecalho(REGISTRO_CABECALHO *cab)
{
    cab->status = '0';
    cab->RRNproxRegistro = 0;
    cab->numeroRegistrosInseridos = 0;
    cab->numeroRegistrosRemovidos = 0;
    cab->numeroRegistrosAtualizados = 0;

    return;
}

// Limpeza dos campos, para receber novos dados
void reset_dados(REGISTRO_DADOS *dados)
{
    dados->tamcidadeMae = 0;
    dados->tamcidadeBebe = 0;
    dados->cidadeMae[0] = '\0';
    dados->cidadeBebe[0] = '\0';
    dados->idNascimento = -1;
    dados->idadeMae = -1;
    dados->dataNascimento[0] = '\0';
    dados->sexoBebe = '0';
    dados->estadoMae[0] = '\0';
    dados->estadoMae[1] = '$';
    dados->estadoBebe[0] = '\0';
    dados->estadoBebe[1] = '$';

    return;
}

// Lê os dados do arquivo binario e armazena na variavel para ser impresso
int read_dados_bin(FILE *arqOutput, REGISTRO_DADOS *dados)
{
    int i=0;
    char aux[100];

    fread(&dados->tamcidadeMae, sizeof(int), 1, arqOutput);

    // Se o registro encontra-se logicamente removido ele pula para o proximo
    if(dados->tamcidadeMae==(-1))
    {
        fseek(arqOutput, 124, SEEK_CUR);

        return 0;
    }

    fread(&dados->tamcidadeBebe, sizeof(int), 1, arqOutput);

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

// Atualiza o cabeçalho após inserir os dados
void update_cabecalho(FILE *arqOutput, REGISTRO_CABECALHO *cab)
{
    int i=0; // indice de um loop
    char lixo = '$'; // variavel usada para prencher o lixo do registro de cabeçalho

    fseek(arqOutput, 0, SEEK_SET);

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


// Funções do Trabalho 2

// Limpeza dos campos, para receber novos dados
void reset_dados2(REGISTRO_DADOS *dados)
{
    dados->tamcidadeMae = 0;
    dados->tamcidadeBebe = 0;
    dados->cidadeMae[0] = '\0';
    dados->cidadeBebe[0] = '\0';
    dados->idNascimento = -1;
    dados->idadeMae = -1;
    dados->dataNascimento[0] = '\0';
    dados->sexoBebe = '-';
    dados->estadoMae[0] = '\0';
    dados->estadoMae[1] = '$';
    dados->estadoBebe[0] = '\0';
    dados->estadoBebe[1] = '$';

    return;
}

// Função responsável por ler os parâmetros informados pelo usuário e armazenar tudo em uma variável
void read_parametros(REGISTRO_DADOS *dados2, int m)
{
    char aux1[100]="\0", aux2[100]="\0";
    int i=0, k=0;

    for(i=0;i<m;i++)
    {
        scanf("%s", aux1);

        if(strcmp(aux1, "cidadeMae")==0)
        {
            scan_quote_string(dados2->cidadeMae);
            dados2->tamcidadeMae = strlen(dados2->cidadeMae);
        }
        else if(strcmp(aux1, "cidadeBebe")==0)
        {
            scan_quote_string(dados2->cidadeBebe);
            dados2->tamcidadeBebe = strlen(dados2->cidadeBebe);
        }
        else if(strcmp(aux1, "idNascimento")==0)
        {
            scanf("%d", &dados2->idNascimento);
        }
        else if(strcmp(aux1, "idadeMae")==0)
        {
            scanf("%s", aux2);

            if(aux2[0]!='N')
            {
                k = convert_string_to_int(aux2);

                if(k!=(-1))
                {
                    dados2->idadeMae = k;
                }
            }
            else
            {
                dados2->idadeMae = -1;
            }
        }
        else if(strcmp(aux1, "dataNascimento")==0)
        {
            scan_quote_string(dados2->dataNascimento);
        }
        else if(strcmp(aux1, "sexoBebe")==0)
        {
            scan_quote_string(&dados2->sexoBebe);
        }
        else if(strcmp(aux1, "estadoMae")==0)
        {
            scan_quote_string(dados2->estadoMae);
        }
        else if(strcmp(aux1, "estadoBebe")==0)
        {
            scan_quote_string(dados2->estadoBebe);
        }

    }

    return ;
}

// Essa função compara um registro com os parâmetros informados pelo usuário
int cmp_dados(REGISTRO_DADOS dados, REGISTRO_DADOS dados2)
{
    int i=0;

    if(dados2.tamcidadeMae > 0)
    {
        if(dados.tamcidadeMae>0)
        {
            dados.cidadeMae[dados.tamcidadeMae] = '\0';

            if(strcmp(dados2.cidadeMae, dados.cidadeMae)==0)
            {
                i++;
            }    
        }  
    }

    if(dados2.tamcidadeBebe > 0)
    {
        if(dados.tamcidadeBebe>0)
        {
            dados.cidadeBebe[dados.tamcidadeBebe] = '\0';

            if(strcmp(dados2.cidadeBebe, dados.cidadeBebe)==0)
            {
                i++;
            }
        }
    }

    if(dados2.idNascimento != (-1))
    {
        if(dados2.idNascimento==dados.idNascimento)
        {
            i++;
        }
    }

    if(dados2.idadeMae != (-1))
    {
        if(dados2.idadeMae==dados.idadeMae)
        {
            i++;
        }
    }

    if(dados2.dataNascimento[0] != '\0')
    {
        if(dados.dataNascimento[0] != '\0')
        {
            dados.dataNascimento[10] = '\0';

            if(strcmp(dados2.dataNascimento, dados.dataNascimento)==0)
            {
                i++;
            }
        }
    }


    if(dados2.sexoBebe!='-')
    {
        if(dados2.sexoBebe==dados.sexoBebe)
        {
            i++;
        }
    }

    if(dados2.estadoMae[0]!='\0')
    {
        if(dados2.estadoMae[0]==dados.estadoMae[0] && dados2.estadoMae[1]==dados.estadoMae[1])
        {
            i++;
        }
    }

    if(dados2.estadoBebe[0]!='\0')
    {
        if(dados2.estadoBebe[0]==dados.estadoBebe[0] && dados2.estadoBebe[1]==dados.estadoBebe[1])
        {
            i++;
        }
    }

    return i;
}

// Essa função é chamada para fazer a remoção lógica de um registro 
void remove_register(FILE *arqOutput, REGISTRO_CABECALHO *cab)
{
    int num = -1;

    fseek(arqOutput, -128, SEEK_CUR);
    fwrite(&num, sizeof(int), 1, arqOutput);
    fseek(arqOutput, 124, SEEK_CUR);
    cab->numeroRegistrosInseridos--;
    cab->numeroRegistrosRemovidos++;

    return ;
}

// Função responsável por ler os dados que serão informados pelo usuário para que possam ser inseridas no arquivo
void read_input(REGISTRO_DADOS *dados2)
{
    char aux2[100];
    int k=0;

    scan_quote_string(dados2->cidadeMae);

    dados2->tamcidadeMae = strlen(dados2->cidadeMae);

    scan_quote_string(dados2->cidadeBebe);

    dados2->tamcidadeBebe = strlen(dados2->cidadeBebe);

    scanf("%d", &dados2->idNascimento);

    scanf("%s", aux2);

    if(aux2[0]!='N')
    {
        k = convert_string_to_int(aux2);

        if(k!=(-1))
        {
            dados2->idadeMae = k;
        }
    }
    else
    {
        dados2->idadeMae = -1;
    }

    scan_quote_string(dados2->dataNascimento);

    scan_quote_string(&dados2->sexoBebe);
    if(dados2->sexoBebe=='\0')
    {
        dados2->sexoBebe = '0';
    }

    scan_quote_string(dados2->estadoMae);

    scan_quote_string(dados2->estadoBebe);

    return ;
}

// Essa função recupera dados de um registro , de acordo com o RRN informado pelo usuário
int read_dados_bin_rrn(FILE *arqOutput, REGISTRO_DADOS *dados, REGISTRO_CABECALHO *cab, int rrn)
{
    int i=0, byteoffset=0;
    char aux[100];

    if(rrn>cab->RRNproxRegistro)
    {
        return 0;
    }

    // Calcula onde o registro deve ser inserido
    byteoffset = 128 + ((rrn)*128);
    fseek(arqOutput, byteoffset, SEEK_SET);

    fread(&dados->tamcidadeMae, sizeof(int), 1, arqOutput);

    // Se o registro encontra-se logicamente removido ele pula para o proximo
    if(dados->tamcidadeMae==(-1))
    {
        return 0;
    }

    fread(&dados->tamcidadeBebe, sizeof(int), 1, arqOutput);

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

// Função responsável pela atualização dos dados, a partir de um RRN
void update_dados(FILE *arqOutput, REGISTRO_DADOS dados, REGISTRO_CABECALHO *cab, int rrn)
{
    int i=0, byteoffset=0;
    char lixo = '$';

    // Calcula onde o registro deve ser inserido
    byteoffset = 128 + ((rrn)*128);
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

    fseek(arqOutput, byteoffset+105, SEEK_SET);

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

    cab->numeroRegistrosAtualizados += 1;

    return ;
}

// Função auxiliar, para converter uma string em um número inteiro
int convert_string_to_int(char str[100])
{
    int i=0,total=1,result=0,n=0;

    n = strlen(str)-1;

    if(n==(-1))
    {
        return -1;
    }

    while(n>=0) 
    {
        i = (str[n] - 48) * total;
        result += i;
        total *= 10;
        n--;
    }

    return result;
}

// Percorre o arquivo binario e imprimi os registros que serão selecionados de acordo com os parâmetross
void busca_parametrizada(FILE *arqOutput, REGISTRO_DADOS *dados, REGISTRO_DADOS *dados2, REGISTRO_CABECALHO *cab)
{
    int i=0, j=0, control=0, m=0,numPrint=0;

    // Reseta valores do arquivo de entrada
    reset_dados2(dados2);

    // Lê a quantidade de parâmetros
    scanf("%d", &m);

    // Lê quais são os parâmetros que o usuário vai digitar
    read_parametros(dados2, m);

    for(i=0;i<cab->RRNproxRegistro;i++)
    {
        // Reseta valores do arquivo de entrada
        reset_dados(dados);

        // Lê os dados do arquivo binario
        j = read_dados_bin(arqOutput, dados);

        // Se o registro não estiver logicamente removido ele é verificado 
        if(j==1)
        {
            // Recebe a quantidade de parâmetros que foram validados
            control = cmp_dados(*dados, *dados2);

            // Comparação para saber se todos os dados do registro estão de acordo com os parâmetros informados
            if(control==m)
            {
                // Coloca o '\0' no final das strings para ser impresso na tela
                aux_imprimir_dados_bin(dados);

                // Imprimi os dados na tela
                imprimir_dados_bin(*dados);

                // Variável que controla o número de arquivos que serão impressos
                numPrint++;
            }
        }
    }

    if(numPrint==0)
    {
        printf("Registro Inexistente.\n");
    }

    return ;
}

// Procura um registro pelo RRN e imprimi o registro
void busca_RRN(FILE *arqOutput, REGISTRO_DADOS *dados)
{
    int j=0,rrn=0;

    //Lê o RRN do registro que vai ser buscado
    scanf("%d", &rrn);

    // Pula para a posição onde devemos buscar o registro
    fseek(arqOutput, 128*rrn, SEEK_CUR);

    // Lê os dados do arquivo binario
    j = read_dados_bin(arqOutput, dados);

    // Se o registro não estiver logicamente removido ele é impresso na tela
    if(j==1)
    {
        // Coloca o '\0' no final das strings para ser impresso na tela
        aux_imprimir_dados_bin(dados);
            
        // Imprimi os dados na tela
        imprimir_dados_bin(*dados);
    }
    else
    {
        printf("Registro Inexistente.\n");
    }

    return ;
}

// Função responsável por remover diversos registros do arquivo binário
void remover_registros(FILE *arqOutput, REGISTRO_CABECALHO *cab, REGISTRO_DADOS *dados, REGISTRO_DADOS *dados2)
{
    int i=0, j=0, k=0, n=0, m=0, control=0, num = -1;

    // Atualiza o estado do arquivo
    cab->status = '0';
    update_cabecalho(arqOutput, cab);

    // Lê a quantidade de registros que serão removidos
    scanf("%d", &n);

    for(k=0;k<n;k++)
    {
        // Reseta valores do arquivo de entrada
        reset_dados2(dados2);

        // Lê a quantidade de parâmetros
        scanf("%d", &m);

        // Lê quais são os parâmetros que o usuário vai digitar
        read_parametros(dados2, m);

        // Percorre o arquivo binario buscando os registros que satisfazem os parâmetros
        for(i=0;i<cab->RRNproxRegistro;i++)
        {
            // Reseta valores do arquivo de entrada
            reset_dados(dados);

            // Lê os dados do arquivo binario
            j = read_dados_bin(arqOutput, dados);

            // Se o registro não estiver logicamente removido ele é impresso na tela
            if(j==1)
            {
                // Recebe a quantidade de parâmetros que foram validados
                control = cmp_dados(*dados, *dados2);

                // Comparação para saber se todos os dados do registro estão de acordo com os parâmetros informados
                if(control==m)
                {
                    // Função responsável por remover o registro que foi encontrado
                    remove_register(arqOutput, cab);
                }
            }
        }

        // Retorna para o inicio do arquivo de dados
        fseek(arqOutput, 128, SEEK_SET);
    }

    // Atualiza o estado do arquivo
    cab->status = '1';
    update_cabecalho(arqOutput, cab);

    return ;
}

// Função responsável por inserir diversos registros no arquivo binário
void inserir_registros(FILE *arqOutput, REGISTRO_CABECALHO *cab, REGISTRO_DADOS *dados2)
{
    int i=0,n=0;

    // Atualiza o estado do arquivo
    cab->status = '0';
    update_cabecalho(arqOutput, cab);

    // Lê a quantidade de registros que serão inseridos
    scanf("%d", &n);

    for(i=0;i<n;i++)
    {
        // Reseta valores do arquivo de entrada
        reset_dados2(dados2);

        // Leitura dos campos do registro que será inserido 
        read_input(dados2);

        // Escreve no arquivo binario
        write_dados(arqOutput, *dados2, cab);
    }

    // Atualiza o estado do arquivo
    cab->status = '1';
    update_cabecalho(arqOutput, cab);

    return ;
}

// Função responsável por atualizar diversos registros no arquivo binário, dado o RRN do registro
void atualizar_registros(FILE *arqOutput, REGISTRO_CABECALHO *cab, REGISTRO_DADOS *dados2)
{
    int i=0,j=0,n=0,m=0,rrn=0;

    // Atualiza o estado do arquivo
    cab->status = '0';
    update_cabecalho(arqOutput, cab);

    // Lê a quantidade de registros que serão atualizados
    scanf("%d", &n);

    for(i=0;i<n;i++)
    {
        // Lê o RRN do registro que será atualizado
        scanf("%d", &rrn);

        // Reseta valores do arquivo de entrada
        reset_dados(dados2);

        // Lê os campos do registro que será atualizado e verifica se o registro está removido
        j = read_dados_bin_rrn(arqOutput, dados2, cab, rrn);

        // Lê a quantidade de parametros
        scanf("%d", &m);

        // Lê os parâmetros que o usuário vai digitar
        read_parametros(dados2, m);

        // Se o registro não estiver removido logicamente , ele é atualizado com as novas informações
        if(j!=0)
        {
            // Atualiza os campos que foram informados
            update_dados(arqOutput, *dados2, cab, rrn);
        }

    }

    // Atualiza o estado do arquivo
    cab->status = '1';
    update_cabecalho(arqOutput, cab);

    return ;
}