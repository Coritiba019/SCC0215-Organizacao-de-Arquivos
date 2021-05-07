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

// Funções desenvolvidas no Trabalho 1
int verifica_cabecalho_bin(FILE *arqOutput, REGISTRO_CABECALHO *cab);
void reset_cabecalho(REGISTRO_CABECALHO *cab);
void reset_dados(REGISTRO_DADOS *dados);
int read_dados_bin(FILE *arqOutput, REGISTRO_DADOS *dados);
void aux_imprimir_dados_bin(REGISTRO_DADOS *dados);
void imprimir_dados_bin(REGISTRO_DADOS dados);
void update_cabecalho(FILE *arqOutput, REGISTRO_CABECALHO *cab);
void write_dados(FILE *arqOutput, REGISTRO_DADOS dados, REGISTRO_CABECALHO *cab);

// Funções desenvolvidas no Trabalho 2
void reset_dados2(REGISTRO_DADOS *dados);
void read_parametros(REGISTRO_DADOS *dados2, int m);
int cmp_dados(REGISTRO_DADOS dados, REGISTRO_DADOS dados2);
void remove_register(FILE *arqOutput, REGISTRO_CABECALHO *cab);
void read_input(REGISTRO_DADOS *dados2);
int read_dados_bin_rrn(FILE *arqOutput, REGISTRO_DADOS *dados, REGISTRO_CABECALHO *cab, int rrn);
void update_dados(FILE *arqOutput, REGISTRO_DADOS dados, REGISTRO_CABECALHO *cab, int rrn);
int convert_string_to_int(char str[100]);
void busca_parametrizada(FILE *arqOutput, REGISTRO_DADOS *dados, REGISTRO_DADOS *dados2, REGISTRO_CABECALHO *cab);
void busca_RRN(FILE *arqOutput, REGISTRO_DADOS *dados);
void remover_registros(FILE *arqOutput, REGISTRO_CABECALHO *cab, REGISTRO_DADOS *dados, REGISTRO_DADOS *dados2);
void inserir_registros(FILE *arqOutput, REGISTRO_CABECALHO *cab, REGISTRO_DADOS *dados2);
void atualizar_registros(FILE *arqOutput, REGISTRO_CABECALHO *cab, REGISTRO_DADOS *dados2);

#endif