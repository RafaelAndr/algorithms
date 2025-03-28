#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

// Estrutura para armazenar informações sobre uma doença
typedef struct Doenca {
    char codigo[9];       // Código da doença
    int numGenes;         // Número de genes associados à doença
    char **genes;         // Lista de genes
    int genesCompativeis; // Número de genes compatíveis com o DNA
    int percentual;       // Percentual de compatibilidade
    int posicao;          // Posição original da doença no arquivo de entrada
} Doenca;

// Estrutura para passar dados para as threads
typedef struct {
    Doenca *doencas;
    int inicio;
    int fim;
    char *dna;
    int tamanhoMinimo;
} ThreadData;

// Protótipos das funções
void buscaKMP(int *tabelaKMP, int *contador, char *texto, char *padrao, int tamanhoMinimo, int posicaoInicial);
void calcularTabelaKMP(int *tabelaKMP, char *padrao);
void processarDoencas(Doenca *doencas, int inicio, int fim, char *dna, int tamanhoMinimo);
void escreverResultados(FILE *arquivoSaida, Doenca *doencas, int quantidade);
int compararDoencas(const void *a, const void *b);
void mergeSort(Doenca *doencas, int inicio, int fim);
void intercalar(Doenca *doencas, int inicio, int meio, int fim);
void *threadFunction(void *arg);

int main(int argc, char **argv) {
    // Verifica se os argumentos foram passados corretamente
    if (argc < 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    // Abre os arquivos de entrada e saída
    FILE *arquivoEntrada = fopen(argv[1], "r");
    FILE *arquivoSaida = fopen(argv[2], "w");

    if (!arquivoEntrada || !arquivoSaida) {
        perror("Erro ao abrir arquivos");
        return 1;
    }

    // Lê o tamanho mínimo de compatibilidade e a sequência de DNA
    int tamanhoMinimo;
    char dna[50000];
    fscanf(arquivoEntrada, "%d %s", &tamanhoMinimo, dna);

    // Lê a quantidade de doenças
    int quantidade;
    fscanf(arquivoEntrada, "%d", &quantidade);

    // Aloca memória para as doenças
    Doenca *doencas = malloc(quantidade * sizeof(Doenca));

    // Lê as doenças do arquivo de entrada
    for (int i = 0; i < quantidade; i++) {
        fscanf(arquivoEntrada, "%s %d", doencas[i].codigo, &doencas[i].numGenes);
        
        doencas[i].genes = (char **)malloc(doencas[i].numGenes * sizeof(char *));
        doencas[i].genesCompativeis = 0;
        doencas[i].posicao = i;
        
        for (int j = 0; j < doencas[i].numGenes; j++) {
            char gene[1001];
            fscanf(arquivoEntrada, "%s", gene);
            
            size_t geneLen = strlen(gene);
            doencas[i].genes[j] = (char *)malloc((geneLen + 1) * sizeof(char));
            strcpy(doencas[i].genes[j], gene);
        }
    }

    // Fecha o arquivo de entrada
    fclose(arquivoEntrada);

    // Divide as doenças em 4 partes para processamento paralelo
    int parte = quantidade / 4;
    pthread_t threads[4];
    ThreadData threadData[4];

    for (int i = 0; i < 4; i++) {
        threadData[i].doencas = doencas;
        threadData[i].inicio = i * parte;
        threadData[i].fim = (i == 3) ? quantidade : (i + 1) * parte;
        threadData[i].dna = dna;
        threadData[i].tamanhoMinimo = tamanhoMinimo;

        pthread_create(&threads[i], NULL, threadFunction, &threadData[i]);
    }

    // Aguarda todas as threads terminarem
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    // Ordena as doenças com base no percentual e na posição original
    mergeSort(doencas, 0, quantidade - 1);

    // Escreve os resultados no arquivo de saída
    escreverResultados(arquivoSaida, doencas, quantidade);

    // Fecha o arquivo de saída
    fclose(arquivoSaida);

    // Libera a memória alocada
    for (int i = 0; i < quantidade; i++) {
        for (int j = 0; j < doencas[i].numGenes; j++) {
            free(doencas[i].genes[j]);
        }
        free(doencas[i].genes);
    }
    free(doencas);

    return 0;
}

// Função executada por cada thread
void *threadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    processarDoencas(data->doencas, data->inicio, data->fim, data->dna, data->tamanhoMinimo);
    return NULL;
}

// Função para processar as doenças
void processarDoencas(Doenca *doencas, int inicio, int fim, char *dna, int tamanhoMinimo) {
    for (int i = inicio; i < fim; i++) {
        for (int j = 0; j < doencas[i].numGenes; j++) {
            int *tabelaKMP = (int *)malloc(strlen(doencas[i].genes[j]) * sizeof(int));
            int contador = 0;
            
            buscaKMP(tabelaKMP, &contador, dna, doencas[i].genes[j], tamanhoMinimo, 0);
            
            if (contador >= (int)(strlen(doencas[i].genes[j]) * 0.9)) {
                doencas[i].genesCompativeis++;
            }
            
            free(tabelaKMP);
        }
        
        doencas[i].percentual = (int)round((double)doencas[i].genesCompativeis / doencas[i].numGenes * 100);
    }
}

// Função para escrever os resultados no arquivo de saída
void escreverResultados(FILE *arquivoSaida, Doenca *doencas, int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        fprintf(arquivoSaida, "%s->%d%%\n", doencas[i].codigo, doencas[i].percentual);
    }
}

// Função de busca usando o algoritmo KMP
void buscaKMP(int *tabelaKMP, int *contador, char *texto, char *padrao, int tamanhoMinimo, int posicaoInicial) {
    int tamanhoTexto = strlen(texto);
    int tamanhoPadrao = strlen(padrao);
    int soma = 0;
    
    calcularTabelaKMP(tabelaKMP, padrao);

    for (int i = posicaoInicial, j = -1; i < tamanhoTexto; i++) {
        while (j >= 0 && padrao[j + 1] != texto[i]) {
            j = tabelaKMP[j];
            soma = j + 1;

            if (j < tamanhoMinimo - 1) {
                continue;
            }
            
            int restantePadrao = tamanhoPadrao - j - 1;
            if (restantePadrao >= tamanhoMinimo) {
                char subpadrao[restantePadrao + 1];
                strncpy(subpadrao, padrao + j + 1, restantePadrao);
                subpadrao[restantePadrao] = '\0';
                *contador += soma;
                buscaKMP(tabelaKMP, contador, texto, subpadrao, tamanhoMinimo, i);
                return;
            }
            
            *contador += soma;
            return;
        }

        if (padrao[j + 1] == texto[i]) {
            j++;
            soma++;
        }

        if (j == tamanhoPadrao - 1) {
            *contador += soma;
            return;
        }
    }
}

// Função para calcular a tabela KMP
void calcularTabelaKMP(int *tabelaKMP, char *padrao) {
    int tamanhoPadrao = strlen(padrao);
    tabelaKMP[0] = -1;
    for (int i = 1, j = -1; i < tamanhoPadrao; i++) {
        while (j >= 0 && padrao[j + 1] != padrao[i]) {
            j = tabelaKMP[j];
        }
        if (padrao[j + 1] == padrao[i]) {
            j++;
        }
        tabelaKMP[i] = j;
    }
}

// Função para intercalar duas metades ordenadas
void intercalar(Doenca *doencas, int inicio, int meio, int fim) {
    int tamanhoEsquerda = meio - inicio + 1;
    int tamanhoDireita = fim - meio;

    // Cria arrays temporários
    Doenca *esquerda = malloc(tamanhoEsquerda * sizeof(Doenca));
    Doenca *direita = malloc(tamanhoDireita * sizeof(Doenca));

    // Copia os dados para os arrays temporários
    for (int i = 0; i < tamanhoEsquerda; i++) {
        esquerda[i] = doencas[inicio + i];
    }
    for (int j = 0; j < tamanhoDireita; j++) {
        direita[j] = doencas[meio + 1 + j];
    }

    // Intercala os arrays temporários de volta ao array principal
    int i = 0, j = 0, k = inicio;
    while (i < tamanhoEsquerda && j < tamanhoDireita) {
        if (esquerda[i].percentual > direita[j].percentual ||
            (esquerda[i].percentual == direita[j].percentual && esquerda[i].posicao < direita[j].posicao)) {
            doencas[k++] = esquerda[i++];
        } else {
            doencas[k++] = direita[j++];
        }
    }

    // Copia os elementos restantes de esquerda (se houver)
    while (i < tamanhoEsquerda) {
        doencas[k++] = esquerda[i++];
    }

    // Copia os elementos restantes de direita (se houver)
    while (j < tamanhoDireita) {
        doencas[k++] = direita[j++];
    }

    // Libera a memória alocada
    free(esquerda);
    free(direita);
}

// Função principal do Merge Sort
void mergeSort(Doenca *doencas, int inicio, int fim) {
    if (inicio < fim) {
        int meio = inicio + (fim - inicio) / 2;
        mergeSort(doencas, inicio, meio);      // Ordena a metade esquerda
        mergeSort(doencas, meio + 1, fim);    // Ordena a metade direita
        intercalar(doencas, inicio, meio, fim); // Intercala as duas metades
    }
}