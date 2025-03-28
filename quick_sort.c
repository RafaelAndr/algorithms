#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

struct data {
    int32_t valor;
    char *tipo;
    int32_t prioridade;
};

void quicksort_lomuto (int32_t * V , int32_t i , int32_t j, int32_t *cont);
void quicksort_lomutoMediana (int32_t * V , int32_t i , int32_t j, int32_t *cont);
void quicksort_lomutoAleatorio (int32_t * V , int32_t i , int32_t j, int32_t *cont);
void quicksort_hoarePadrao (int32_t * V , int32_t i , int32_t j, int32_t *cont);
void quicksort_hoareMediana (int32_t * V , int32_t i , int32_t j, int32_t *cont);
void quicksort_hoareAleatorio (int32_t * V , int32_t i , int32_t j, int32_t *cont);

int32_t lomuto (int32_t * V , int32_t i , int32_t j, int32_t *cont);
int32_t lomuto_mediana ( int32_t * V , int32_t i , int32_t j, int32_t *cont);
int32_t lomuto_aleatorio ( int32_t * V , int32_t i , int32_t j, int32_t *cont);
int32_t hoare ( int32_t * V , int32_t i , int32_t j, int32_t *cont);
int32_t hoare_mediana ( int32_t * V , int32_t i , int32_t j, int32_t *cont);
int32_t hoare_aleatorio ( int32_t * V , int32_t i , int32_t j, int32_t *cont);

int32_t mediana(int32_t *V, int32_t i, int32_t j);
int32_t partition(struct data *lista, int32_t i, int32_t j);

void trocar (int32_t *x, int32_t *y);
void quickSortOutput(struct data *lista, int32_t i, int32_t j);
void swap(struct data *a, struct data *b);

struct data lista[6];

int32_t main(int argc, char* argv[]) {
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");
  
    int quant;
    fscanf(input, "%d", &quant);

    char *tipos[] = {"LP", "LM", "LA", "HP", "HM", "HA"};
    void (*sort_functions[])(int32_t *, int32_t, int32_t, int32_t *) = {
        quicksort_lomuto, quicksort_lomutoMediana, quicksort_lomutoAleatorio,
        quicksort_hoarePadrao, quicksort_hoareMediana, quicksort_hoareAleatorio
    };

    for (int i = 0; i < quant; i++) {
        for (int j = 0; j < 6; j++) {
            lista[j].tipo = tipos[j];
            lista[j].valor = 1;
            lista[j].prioridade = j;
        }

        int32_t n;
        fscanf(input, "%d", &n);
        int32_t *v = malloc(n * sizeof(int32_t));
    

        for (int j = 0; j < n; j++) {
            fscanf(input, "%d", &v[j]);
        }

        int32_t *vAux = malloc(n * sizeof(int32_t));

        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < n; k++) {
                vAux[k] = v[k];
            }
            sort_functions[j](vAux, 0, n - 1,   &lista[j].valor);
        }

        quickSortOutput(lista, 0, 5);

        fprintf(output, "%d:N(%d)", i, n);
        for (int j = 0; j < 6; j++) {
            fprintf(output, ",%s(%d)", lista[j].tipo, lista[j].valor);
            
        }
        if (i < quant - 1) {
            fprintf(output, "\n");
        }

        free(v);
        free(vAux);
    }

    fclose(input);
    fclose(output);
    return 0;
}

void quickSortOutput(struct data *lista, int32_t i, int32_t j) {
    if (i < j) {
        int32_t pivot = partition(lista, i, j);
        quickSortOutput(lista, i, pivot - 1);
        quickSortOutput(lista, pivot + 1, j);
    }
}

int32_t partition(struct data *lista, int32_t i, int32_t j) {
    struct data pivot = lista[j]; 
    int32_t x = i;              

    for (int32_t y = i; y < j; y++) { 

        if (lista[y].valor < pivot.valor || 
           (lista[y].valor == pivot.valor && lista[y].prioridade < pivot.prioridade)) {
            swap(&lista[x], &lista[y]); 
            x++; 
        }
    }

    swap(&lista[x], &lista[j]);

    return x; 
}

void swap(struct data *a, struct data *b) {
    struct data temp = *a;
    *a = *b;
    *b = temp;
}

void trocar(int32_t *x, int32_t *y) {
    int32_t aux;
    aux = *x;
    *x = *y;
    *y = aux;
}

int32_t lomuto(int32_t* V, int32_t i, int32_t j, int32_t *cont) {
    int32_t P = V [j] , x = i - 1 , y = i;
  
    for (y = i; y < j; y++)
    if (V[y] <= P) {
        trocar(&V[++x], &V[y]);
        *cont = *cont + 1;
    }
 
    trocar (&V[++x], &V[j]);
    *cont = *cont + 1;
 
    return x ;
}

void quicksort_lomuto (int32_t * V, int32_t i, int32_t j, int32_t *cont) {
    if(i < j){

        int32_t pivo = lomuto(V, i , j, cont);

        quicksort_lomuto(V, i, pivo - 1, cont);
        *cont = *cont + 1;
        quicksort_lomuto(V, pivo + 1, j, cont);
        *cont = *cont + 1;
    }
}

int32_t lomuto_mediana(int32_t * V , int32_t i , int32_t j, int32_t *cont) {
    int32_t med = mediana(V, i, j);
    
    trocar (&V[j], &V[med]);
    *cont = *cont + 1;

    return lomuto (V , i , j, cont);
}

void quicksort_lomutoMediana (int32_t * V , int32_t i , int32_t j, int32_t *cont) {
    if(i < j) {
    
        int32_t pivo = lomuto_mediana (V , i , j, cont) ;

        quicksort_lomutoMediana(V , i , pivo - 1, cont);
        *cont = *cont + 1;
        quicksort_lomutoMediana(V , pivo + 1 , j, cont);
        *cont = *cont + 1;
    }
}

int32_t lomuto_aleatorio(int32_t * V , int32_t i , int32_t j, int32_t *cont) {
    trocar(&V[j] , &V[i + abs(V[i]) % (j-i+1)]);
    *cont = *cont + 1;

    return lomuto(V, i , j, cont) ;
}

void quicksort_lomutoAleatorio(int32_t * V, int32_t i , int32_t j, int32_t *cont) {
    if(i < j){
  
        int32_t pivo = lomuto_aleatorio(V, i , j, cont);

        quicksort_lomutoAleatorio(V, i , pivo - 1, cont);
        *cont = *cont + 1;
        quicksort_lomutoAleatorio(V, pivo + 1 , j, cont);
        *cont = *cont + 1;
    }
}

int32_t hoare(int32_t * V , int32_t i , int32_t j, int32_t *cont){
    int32_t P = V [i], x = i - 1, y = j + 1;

    while (1) {
        while(V[--y] > P);
        while(V[++x] < P);
        if(x < y) {
            trocar (&V[x] ,&V[y]);
            *cont = *cont + 1;
        }
        else return y;
    }
}

void quicksort_hoarePadrao(int32_t* V, int32_t i, int32_t j, int32_t *cont){
    if(i < j) {  

        int32_t pivo = hoare(V, i , j, cont);

        quicksort_hoarePadrao(V, i , pivo, cont);
        *cont = *cont + 1;
        quicksort_hoarePadrao(V, pivo + 1 , j, cont);
        *cont = *cont + 1;
    }
}

int32_t hoare_mediana(int32_t * V, int32_t i , int32_t j, int32_t *cont){
    int32_t med = mediana(V, i, j);

    trocar (&V[i], &V[med]);
    *cont = *cont + 1;

    return hoare (V, i , j, cont) ;
}

void quicksort_hoareMediana(int32_t* V, int32_t i, int32_t j, int32_t *cont){
    if(i < j) {

        int32_t pivo = hoare_mediana(V , i , j, cont);
 
        quicksort_hoareMediana(V , i , pivo, cont);
        *cont = *cont + 1;
        quicksort_hoareMediana(V , pivo + 1 , j, cont);
        *cont = *cont + 1;
    }
}

int32_t hoare_aleatorio(int32_t* V, int32_t i, int32_t j, int32_t *cont){
 
    trocar(&V[ i ], &V[i+abs(V[i]) % (j-i+1)]);
    *cont = *cont + 1;

    return hoare (V, i, j, cont) ;
}

void quicksort_hoareAleatorio(int32_t* V, int32_t i, int32_t j, int32_t *cont){
    if(i < j) {

        int32_t pivo = hoare_aleatorio(V, i, j, cont);

        quicksort_hoareAleatorio(V, i, pivo, cont);
        *cont = *cont + 1;
        quicksort_hoareAleatorio(V, pivo + 1 , j, cont);
        *cont = *cont + 1;
    }
}

int32_t mediana(int32_t *V, int32_t i, int32_t j){
    int32_t pos1 = i + (j - i + 1) / 4;
    int32_t pos2 = i + (j - i + 1) / 2;
    int32_t pos3 = i + (j - i + 1) * 3 / 4;

    int32_t v1 = V[pos1];
    int32_t v2 = V[pos2];
    int32_t v3 = V[pos3];

    if (v1 == v3 || v1 == v2) {
        return pos1;
    } else if (v2 == v3) {
        return pos2;
    }

    if (v1 < v2) {
        if (v1 > v3) {
            return pos1;
        } else if (v2 < v3) {
            return pos2;
        } else {
            return pos3;
        }
    }

    else {
        if (v2 > v3) {
            return pos2;
        } else if (v1 < v3) {
            return pos1;
        } else {
            return pos3;
        }
    }
}