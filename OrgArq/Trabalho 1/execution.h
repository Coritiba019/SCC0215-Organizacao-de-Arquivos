#include <stdio.h>

#ifndef H_EXECUTION_
#define H_EXECUTION_

typedef struct struct1
{
    char status;
    int RRNproxRegistro;
    int numeroRegistrosInseridos;
    int numeroRegistrosRemovidos;
    int numeroRegistrosAtualizados;
}REGISTRO_CABECALHO;

typedef struct struct2
{
    int tamcidadeMae;
    int tamcidadeBebe;
    char cidadeMae[100];
    char cidadeBebe[100];
    int idNascimento;
    int idadeMae;
    char dataNascimento[11];
    char sexoBebe;
    char estadoMae[3];
    char estadoBebe[3];
}REGISTRO_DADOS;

// Funções utilizadas na 1 funcionalidade do programa
void reset_cabecalho(REGISTRO_CABECALHO *cab);
void write_cabecalho(FILE *arqOutput, REGISTRO_CABECALHO *cab);
void update_cabecalho(FILE *arqOutput, REGISTRO_CABECALHO cab);
void reset_dados(REGISTRO_DADOS *dados);
void read_dados(FILE *arqInput, REGISTRO_DADOS *dados);
void write_dados(FILE *arqOutput, REGISTRO_DADOS dados, REGISTRO_CABECALHO *cab);

// Funções utilizadas na 2 funcionalidade do programa
int verifica_cabecalho_bin(FILE *arqOutput, REGISTRO_CABECALHO *cab);
void aux_imprimir_dados_bin(REGISTRO_DADOS *dados);
void imprimir_dados_bin(REGISTRO_DADOS dados);
int read_dados_bin(FILE *arqOutput, REGISTRO_DADOS *dados);

#endif