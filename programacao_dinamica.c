#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_PLACA 8       // Tamanho máximo de uma placa de carro
#define MAX_CODIGO 14     // Tamanho máximo do código de um produto

// Estrutura para representar um carro
typedef struct {
    char placa[MAX_PLACA];  // Placa do carro
    int32_t peso;           // Peso máximo que o carro suporta
    int32_t volume;         // Volume máximo que o carro suporta
} Carro;

// Estrutura para representar um produto
typedef struct {
    char codigo[MAX_CODIGO]; // Código do produto
    float valor;             // Valor do produto
    int32_t peso;            // Peso do produto
    int32_t volume;          // Volume do produto
    int usado;               // Flag para indicar se o produto foi usado
    int posicao_original;    // Posição original do produto no array
} Produto;

// Função para retornar o máximo entre dois números
float maximo(float a, float b) {
    return (a > b) ? a : b;  // Retorna o maior valor
}

// Função para preencher a tabela DP (Programação Dinâmica)
void preencherTabelaDP(float ***dp, Carro carro, Produto *produtos, int32_t n) {
    for (int i = 0; i <= n; i++) {  // Loop sobre os produtos
        for (int j = 0; j <= carro.volume; j++) {  // Loop sobre o volume
            for (int k = 0; k <= carro.peso; k++) {  // Loop sobre o peso
                if (i == 0 || j == 0 || k == 0) {
                    dp[i][j][k] = 0;  // Caso base
                } else if (produtos[i - 1].volume <= j && produtos[i - 1].peso <= k && !produtos[i - 1].usado) {
                    dp[i][j][k] = maximo(
                        produtos[i - 1].valor + dp[i - 1][j - produtos[i - 1].volume][k - produtos[i - 1].peso],
                        dp[i - 1][j][k]
                    );  // Escolhe o máximo entre incluir ou não o produto
                } else {
                    dp[i][j][k] = dp[i - 1][j][k];  // Não inclui o produto
                }
            }
        }
    }
}

// Função para selecionar os produtos com base na tabela DP
void selecionarProdutosDP(float ***dp, Carro carro, Produto *produtosAux, int32_t n, FILE *output, int32_t *quantidade, Produto *produtosOriginal) {
    int pesoTotal = 0, volumeTotal = 0;
    char *selecionados[n];  // Array para armazenar os códigos dos produtos selecionados
    int count = 0;

    // Percorre a tabela DP de trás para frente
    for (int i = n, j = carro.volume, k = carro.peso; i > 0 && j > 0 && k > 0; i--) {
        if (dp[i][j][k] != dp[i - 1][j][k]) {  // Se o produto foi incluído
            selecionados[count++] = produtosAux[i - 1].codigo;  // Adiciona o código ao array
            pesoTotal += produtosAux[i - 1].peso;  // Atualiza o peso total
            volumeTotal += produtosAux[i - 1].volume;  // Atualiza o volume total
            produtosOriginal[produtosAux[i - 1].posicao_original].usado = 1;  // Marca o produto como usado
            j -= produtosAux[i - 1].volume;  // Atualiza o volume restante
            k -= produtosAux[i - 1].peso;    // Atualiza o peso restante
            (*quantidade)++;  // Incrementa a quantidade de produtos selecionados
        }
    }

    // Imprime os resultados no arquivo de saída
    fprintf(output, "[%s]R$%.2f,%dKG(%.0f%%),%dL(%.0f%%)->", carro.placa, dp[n][carro.volume][carro.peso],
            pesoTotal, (float) pesoTotal / carro.peso * 100,
            volumeTotal, (float) volumeTotal / carro.volume * 100);
    
    // Imprime os produtos selecionados na ordem correta
    for (int x = count - 1; x >= 0; x--) {
        fprintf(output, "%s%s", selecionados[x], (x > 0) ? "," : "\n");
    }
}

// Função para processar os carros e selecionar os produtos
void processarCarrosDP(Carro *carros, Produto *produtos, int32_t c, int32_t p, FILE *output) {
    int maxPeso = 0, maxVolume = 0;

    // Encontra o máximo peso e volume entre os carros
    for (int i = 0; i < c; i++) {
        if (carros[i].peso > maxPeso) maxPeso = carros[i].peso;
        if (carros[i].volume > maxVolume) maxVolume = carros[i].volume;
    }

    // Aloca a tabela DP com dimensões (p+1) x (maxVolume+1) x (maxPeso+1)
    float ***dp = malloc((p + 1) * sizeof(float **));
    for (int i = 0; i <= p; i++) {
        dp[i] = malloc((maxVolume + 1) * sizeof(float *));
        for (int j = 0; j <= maxVolume; j++) {
            dp[i][j] = malloc((maxPeso + 1) * sizeof(float));
        }
    }

    int quantidade = 0;
    for (int i = 0; i < c; i++) {
        // Cria um array auxiliar com produtos não usados
        int n_aux = 0;
        for (int j = 0; j < p; j++) {
            if (!produtos[j].usado) n_aux++;
        }
        Produto *produtosAux = malloc(n_aux * sizeof(Produto));
        int idx = 0;
        for (int j = 0; j < p; j++) {
            if (!produtos[j].usado) {
                produtosAux[idx++] = produtos[j];
            }
        }

        preencherTabelaDP(dp, carros[i], produtosAux, n_aux);
        selecionarProdutosDP(dp, carros[i], produtosAux, n_aux, output, &quantidade, produtos);

        free(produtosAux);
    }

    // Libera a memória da tabela DP
    for (int i = 0; i <= p; i++) {
        for (int j = 0; j <= maxVolume; j++) {
            free(dp[i][j]);
        }
        free(dp[i]);
    }
    free(dp);
}

// Função para calcular e imprimir os produtos pendentes
void calcularPendentesDP(Produto *produtos, int32_t p, FILE *output) {
    float valorPendente = 0;
    int pesoPendente = 0, volumePendente = 0;
    
    fprintf(output, "PENDENTE:");
    
    for (int i = 0; i < p; i++) {
        if (!produtos[i].usado) {
            valorPendente += produtos[i].valor;
            pesoPendente += produtos[i].peso;
            volumePendente += produtos[i].volume;
        }
    }
    
    fprintf(output, "R$%.2f,%dKG,%dL->", valorPendente, pesoPendente, volumePendente);
    int primeiro = 1;
    for (int i = 0; i < p; i++) {
        if (!produtos[i].usado) {
            if (!primeiro) fprintf(output, ",");
            fprintf(output, "%s", produtos[i].codigo);
            primeiro = 0;
        }
    }
    fprintf(output, "\n");
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    FILE *output = fopen(argv[2], "w");
    if (!output) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(input);
        return 1;
    }

    int c, p;
    fscanf(input, "%d", &c);
    Carro *carros = malloc(c * sizeof(Carro));
    for (int i = 0; i < c; i++) {
        fscanf(input, "%s %d %d", carros[i].placa, &carros[i].peso, &carros[i].volume);
    }

    fscanf(input, "%d", &p);
    Produto *produtos = malloc(p * sizeof(Produto));
    for (int i = 0; i < p; i++) {
        fscanf(input, "%s %f %d %d", produtos[i].codigo, &produtos[i].valor, &produtos[i].peso, &produtos[i].volume);
        produtos[i].usado = 0;
        produtos[i].posicao_original = i;
    }

    processarCarrosDP(carros, produtos, c, p, output);
    calcularPendentesDP(produtos, p, output);
    
    free(carros);
    free(produtos);
    fclose(input);
    fclose(output);
    return 0;
}