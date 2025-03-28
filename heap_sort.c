#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

typedef struct Pacote {
    int numero_pacote;
    int tamanho;
    char conteudo[512][3];
} Pacote;

void trocar_pacote(Pacote* pacotes, uint32_t a, uint32_t b);
uint32_t esquerdo(uint32_t i);
uint32_t direito(uint32_t i);
void heapify_min(Pacote* pacotes, uint32_t n, uint32_t i);
void inserir_no_heap(Pacote* pacotes, uint32_t* tamanho_heap, Pacote novo_pacote);
Pacote remover_raiz(Pacote* pacotes, uint32_t* tamanho_heap);

int main(int argc, char* argv[]) {
    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");

    int total_pacotes, quantidade_ordenar;
    fscanf(input, "%d%d", &total_pacotes, &quantidade_ordenar);

    uint32_t lidos = 0;
    Pacote* heap = malloc(total_pacotes * sizeof(Pacote));
    int pacote_esperado = 0;
    uint32_t tamanho_heap = 0;

    for (int k = 0; k < ceil((double)total_pacotes / quantidade_ordenar); k++) {
        bool pacote_impresso = false; 

        for (int i = 0; i < quantidade_ordenar && lidos < total_pacotes; i++) {
            Pacote novo_pacote;
            fscanf(input, "%d %d", &novo_pacote.numero_pacote, &novo_pacote.tamanho);
            lidos++;

            for (int j = 0; j < novo_pacote.tamanho; j++) {
                fscanf(input, "%s", novo_pacote.conteudo[j]);
            }

            inserir_no_heap(heap, &tamanho_heap, novo_pacote);
        }

        while (tamanho_heap > 0 && heap[0].numero_pacote == pacote_esperado) {
            Pacote pacote_imprimir = remover_raiz(heap, &tamanho_heap);

            if (!pacote_impresso) {
                pacote_impresso = true;
            }

            fprintf(output, "|");
            for (int j = 0; j < pacote_imprimir.tamanho; j++) {
                if (j > 0) {
                    fprintf(output, ",");
                }
                fprintf(output, "%s", pacote_imprimir.conteudo[j]);
            }

            pacote_esperado++;
        }

        if (pacote_impresso) {
            fprintf(output, "|\n");
        }
    }

    fclose(input);
    fclose(output);
    free(heap);
    return 0;
}

void trocar_pacote(Pacote* pacotes, uint32_t a, uint32_t b) {
    Pacote temp = pacotes[a];
    pacotes[a] = pacotes[b];
    pacotes[b] = temp;
}

uint32_t esquerdo(uint32_t i) {
    return 2 * i + 1;
}

uint32_t direito(uint32_t i) {
    return 2 * i + 2;
}

void heapify_min(Pacote* pacotes, uint32_t n, uint32_t i) {
    uint32_t menor = i;
    uint32_t e = esquerdo(i);
    uint32_t d = direito(i);

    if (e < n && pacotes[e].numero_pacote < pacotes[menor].numero_pacote) {
        menor = e;
    }

    if (d < n && pacotes[d].numero_pacote < pacotes[menor].numero_pacote) {
        menor = d;
    }

    if (menor != i) {
        trocar_pacote(pacotes, i, menor);
        heapify_min(pacotes, n, menor);
    }
}

void inserir_no_heap(Pacote* pacotes, uint32_t* tamanho_heap, Pacote novo_pacote) {
    uint32_t i = (*tamanho_heap)++;
    pacotes[i] = novo_pacote;

    while (i > 0 && pacotes[(i - 1) / 2].numero_pacote > pacotes[i].numero_pacote) {
        trocar_pacote(pacotes, i, (i - 1) / 2);
        i = (i - 1) / 2;
    }
}

Pacote remover_raiz(Pacote* pacotes, uint32_t* tamanho_heap) {
    Pacote raiz = pacotes[0];
    pacotes[0] = pacotes[--(*tamanho_heap)];
    heapify_min(pacotes, *tamanho_heap, 0);
    return raiz;
}