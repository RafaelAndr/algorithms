#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

typedef struct containers {
    char codigo[12];
    char cnpj[19];
    char cnpj_aux[19];
    int32_t peso;
    int32_t pos;
    int32_t erro;
    int32_t diferenca_peso;
    int32_t diferenca_percentual;
} containers;

void mergesort_posicoes(containers *S, containers *E, int32_t i, int32_t j);
void intercalar_posicoes(containers *S, containers *E, int32_t i, int32_t m, int32_t j);
int32_t busca_binaria(containers *S, int32_t n, char *codigo);
void mergesort(containers *S, containers *E, int32_t i, int32_t j);
void intercalar(containers *S, containers *E, int32_t i, int32_t m, int32_t j);
void copiar(containers *S, containers *E, int32_t n);
void mergesort_posicoes_percentual(containers *S, containers *E, int32_t i, int32_t j);
void intercalar_posicoes_percentual(containers *S, containers *E, int32_t i, int32_t m, int32_t j);
void copiar_posicoes_percentual(containers *S, containers *E, int32_t n);


int main(int argc, char *argv[]){
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");

    int32_t n, m;
    int32_t erro1 = 0;
    int32_t erro2 = 0;

    fscanf(input, "%d", &n);
    containers *container = malloc(n * sizeof(containers));
    containers *containers_sort = malloc(n * sizeof(containers));

    for (int i = 0; i < n; i++){
        fscanf(input, "%s %s %d\n", container[i].codigo, container[i].cnpj, &container[i].peso);
        container[i].pos = i;
    }
    mergesort(containers_sort, container, 0, n-1);

    fscanf(input, "%d", &m);
    containers containers_inspecionados[m];
    for (int i = 0; i < m; i++){
        fscanf(input, "%s %s %d\n", containers_inspecionados[i].codigo, containers_inspecionados[i].cnpj, &containers_inspecionados[i].peso);
        int32_t index = busca_binaria(containers_sort, n, containers_inspecionados[i].codigo);
        if (index != -1){
            if (strcmp(containers_inspecionados[i].cnpj, containers_sort[index].cnpj) != 0){
                containers_inspecionados[i].erro = 1;
                strcpy(containers_inspecionados[i].cnpj_aux, containers_sort[index].cnpj);
                containers_inspecionados[i].pos = containers_sort[index].pos;
                erro1 ++;
            }
            else if((abs((double)containers_inspecionados[i].peso - (double)containers_sort[index].peso) * 100 / (double)containers_sort[index].peso) > 10){
                containers_inspecionados[i].erro = 2;
                containers_inspecionados[i].diferenca_peso = abs(containers_sort[index].peso - containers_inspecionados[i].peso);
                containers_inspecionados[i].diferenca_percentual = (int) round(abs(containers_sort[index].peso - containers_inspecionados[i].peso)*100/ (float) containers_sort[index].peso);
                containers_inspecionados[i].pos = containers_sort[index].pos;
                erro2++;
            }
            else{
                containers_inspecionados[i].erro = 3;
                containers_inspecionados[i].pos = containers_sort[index].pos;
            }
        }
    }

    containers *containers_inspecionados_erros1 = malloc(erro1 * sizeof(containers));
    containers *containers_inspecionados_erros2 = malloc(erro2 * sizeof(containers));
    containers *containers_inspecionados_sort_erros1 = malloc(erro1 * sizeof(containers));
    containers *containers_inspecionados_sort_erros2 = malloc(erro2 * sizeof(containers));

    int indexErro1 = 0, indexErro2 = 0;
    for (int i = 0; i < m; i++) {
        if (containers_inspecionados[i].erro == 1) {
            containers_inspecionados_erros1[indexErro1].pos = containers_inspecionados[i].pos;
            strcpy(containers_inspecionados_erros1[indexErro1].codigo, containers_inspecionados[i].codigo);
            strcpy(containers_inspecionados_erros1[indexErro1].cnpj, containers_inspecionados[i].cnpj);
            strcpy(containers_inspecionados_erros1[indexErro1].cnpj_aux, containers_inspecionados[i].cnpj_aux);
            indexErro1++;

        } else if (containers_inspecionados[i].erro == 2) {
            containers_inspecionados_erros2[indexErro2].pos = containers_inspecionados[i].pos;
            strcpy(containers_inspecionados_erros2[indexErro2].codigo, containers_inspecionados[i].codigo);
            containers_inspecionados_erros2[indexErro2].diferenca_peso = containers_inspecionados[i].diferenca_peso;
            containers_inspecionados_erros2[indexErro2].diferenca_percentual = containers_inspecionados[i].diferenca_percentual;            
            indexErro2++;
        }
    }

    mergesort_posicoes(containers_inspecionados_sort_erros1, containers_inspecionados_erros1, 0, erro1-1);
    mergesort_posicoes_percentual(containers_inspecionados_sort_erros2, containers_inspecionados_erros2, 0, erro2-1);

    for (int i = 0; i < erro1; i++){
            fprintf(output, "%s:%s<->%s\n", containers_inspecionados_erros1[i].codigo, containers_inspecionados_erros1[i].cnpj_aux, containers_inspecionados_erros1[i].cnpj);   
    }
    
    for (int i = 0; i < erro2; i++){
        if (containers_inspecionados_erros2[i].diferenca_percentual >= 11){
            fprintf(output, "%s:%dkg(%d%%)\n", containers_inspecionados_erros2[i].codigo, containers_inspecionados_erros2[i].diferenca_peso, containers_inspecionados_erros2[i].diferenca_percentual);   
        }
    }

    free(container);
    free(containers_sort);
    free(containers_inspecionados_erros1);
    free(containers_inspecionados_erros2);
    free(containers_inspecionados_sort_erros1);
    free(containers_inspecionados_sort_erros2);

    fclose(input);
    fclose(output);
}

void mergesort_posicoes(containers *S, containers *E, int32_t i, int32_t j) {
    if (i < j) {
        int32_t m = i + (j - i) / 2;
        mergesort_posicoes(S, E, i, m);
        mergesort_posicoes(S, E, m + 1, j);
        intercalar_posicoes(S, E, i, m, j);
    }
}

void intercalar_posicoes(containers *S, containers *E, int32_t i, int32_t m, int32_t j) {
    int32_t i1 = i, i2 = m + 1, k = i;

    while (i1 <= m && i2 <= j) {
        if (E[i1].pos < E[i2].pos) {
            S[k] = E[i1++]; 
        } else {
            S[k] = E[i2++]; 
        }
        k++;
    }
    while (i1 <= m) {
        S[k++] = E[i1++];
    }
    while (i2 <= j) {
        S[k++] = E[i2++];
    }
    for (int32_t x = i; x <= j; x++) {
        E[x] = S[x];
    }
}

int32_t busca_binaria(containers *S, int32_t n, char *codigo) {
    int32_t i = 0, j = n - 1, m;
    while (i <= j) {
        m = i + (j - i) / 2;
        if (strcmp(S[m].codigo, codigo) == 0) return m;
        else if (strcmp(S[m].codigo, codigo) < 0) i = m + 1;
        else j = m - 1;
    }
    return -1;
}

void mergesort(containers *S, containers *E, int32_t i, int32_t j) {
    if (i < j) {
        int32_t m = i + (j - i) / 2;
        mergesort(S, E, i, m);
        mergesort(S, E, m + 1, j);
        intercalar(S, E, i, m, j);
    }
}

void intercalar(containers *S, containers *E, int32_t i, int32_t m, int32_t j) {    
    int32_t i1 = i, i2 = m + 1, k = i;

    while (i1 <= m && i2 <= j) {
        if (strcmp(E[i1].codigo, E[i2].codigo) <= 0) {
            S[k] = E[i1++];
        } else {
            S[k] = E[i2++];
        }
        k++;
    }
    while (i1 <= m) {
        S[k++] = E[i1++];
    }
    while (i2 <= j) {
        S[k++] = E[i2++];
    }
    for (int32_t x = i; x <= j; x++) {
        E[x] = S[x];
    }
}

void mergesort_posicoes_percentual(containers *S, containers *E, int32_t i, int32_t j) {
    if (i < j) {
        int32_t m = i + (j - i) / 2;
        mergesort_posicoes_percentual(S, E, i, m);
        mergesort_posicoes_percentual(S, E, m + 1, j);
        intercalar_posicoes_percentual(S, E, i, m, j);
    }
}

void intercalar_posicoes_percentual(containers *S, containers *E, int32_t i, int32_t m, int32_t j) {
    int32_t i1 = i, i2 = m + 1, k = i;

    while (i1 <= m && i2 <= j) {
        if (E[i1].diferenca_percentual == E[i2].diferenca_percentual) {
            if (E[i1].pos < E[i2].pos) {
                S[k] = E[i1++];
            } else {
                S[k] = E[i2++];
            }
        }
        else if (E[i1].diferenca_percentual > E[i2].diferenca_percentual) {
            S[k] = E[i1++];
        } else {
            S[k] = E[i2++];
        }
        k++;
    }

    while (i1 <= m) {
        S[k++] = E[i1++];
    }
    while (i2 <= j) {
        S[k++] = E[i2++];
    }
    for (int32_t x = i; x <= j; x++) {
        E[x] = S[x];
    }
}