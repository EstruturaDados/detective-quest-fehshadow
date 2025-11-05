#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------
// Estruturas de dados
// ------------------------

/**
 * Representa uma sala da mansao.
 * Cada sala possui nome, ponteiros para esquerda e direita, e uma pista opcional.
 */
typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

/**
 * Representa um nó da BST de pistas.
 */
typedef struct PistaNode {
    char pista[100];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

/**
 * Pilha para controlar o caminho de volta.
 */
typedef struct Pilha {
    Sala* sala;
    struct Pilha* anterior;
} Pilha;

// ------------------------
// Funções auxiliares
// ------------------------

/**
 * Cria dinamicamente uma sala com nome e pista (pode ser vazia).
 */
Sala* criarSala(const char* nome, const char* pista) {
    Sala* nova = (Sala*)malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro ao alocar memoria para sala.\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esquerda = nova->direita = NULL;
    return nova;
}

/**
 * Cria dinamicamente um nó de pista e insere na BST.
 */
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (pista[0] == '\0') return raiz; // Ignora pistas vazias

    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*)malloc(sizeof(PistaNode));
        strcpy(novo->pista, pista);
        novo->esquerda = novo->direita = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (strcmp(pista, raiz->pista) > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    return raiz;
}

/**
 * Empilha uma sala na trilha de exploração.
 */
Pilha* empilhar(Pilha* topo, Sala* sala) {
    Pilha* novo = (Pilha*)malloc(sizeof(Pilha));
    novo->sala = sala;
    novo->anterior = topo;
    return novo;
}

/**
 * Desempilha e retorna a sala anterior.
 */
Pilha* desempilhar(Pilha* topo, Sala** salaAnterior) {
    if (!topo) {
        *salaAnterior = NULL;
        return NULL;
    }
    *salaAnterior = topo->sala;
    Pilha* anterior = topo->anterior;
    free(topo);
    return anterior;
}

/**
 * Percorre a BST em ordem e exibe as pistas coletadas.
 */
void exibirPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}

/**
 * Libera memória da BST de pistas.
 */
void liberarPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

/**
 * Libera memória da árvore de salas.
 */
void liberarSalas(Sala* raiz) {
    if (raiz == NULL) return;
    liberarSalas(raiz->esquerda);
    liberarSalas(raiz->direita);
    free(raiz);
}

/**
 * Controla a navegação entre salas e coleta de pistas.
 */
void explorarSalasComPistas(Sala* atual, PistaNode** arvorePistas) {
    Pilha* trilha = NULL;
    char escolha;

    while (atual != NULL) {
        printf("\n📍 Voce esta na sala: %s\n", atual->nome);
        if (strlen(atual->pista) > 0) {
            printf("🔎 Pista encontrada: \"%s\"\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
        } else {
            printf("🔍 Nenhuma pista nesta sala.\n");
        }

        printf("\nEscolha um caminho:\n");
        if (atual->esquerda) printf("  [e] Ir para a esquerda\n");
        if (atual->direita)  printf("  [d] Ir para a direita\n");
        if (trilha)          printf("  [a] Voltar para a sala anterior\n");
        printf("  [s] Sair da exploracao\n");
        printf("Opcao: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' && atual->esquerda) {
            trilha = empilhar(trilha, atual);
            atual = atual->esquerda;
        } else if (escolha == 'd' && atual->direita) {
            trilha = empilhar(trilha, atual);
            atual = atual->direita;
        } else if (escolha == 'a') {
            Sala* anterior = NULL;
            trilha = desempilhar(trilha, &anterior);
            if (anterior) {
                atual = anterior;
            } else {
                printf("Voce ja esta na sala inicial.\n");
            }
        } else if (escolha == 's') {
            printf("\n🔚 Exploracao encerrada pelo jogador.\n");
            break;
        } else {
            printf("Opcao invalida. Tente novamente.\n");
        }
    }

    // Libera a pilha
    while (trilha) {
        Sala* temp;
        trilha = desempilhar(trilha, &temp);
    }
}

// ------------------------
// Função principal
// ------------------------

int main() {
    // Criação do mapa fixo da mansão
    Sala* hall = criarSala("Hall de Entrada", "Pegadas molhadas no tapete");
    Sala* salaEstar = criarSala("Sala de Estar", "Um livro antigo com anotações");
    Sala* cozinha = criarSala("Cozinha", "");
    Sala* jardim = criarSala("Jardim", "Uma luva rasgada perto da fonte");
    Sala* biblioteca = criarSala("Biblioteca", "Uma carta escondida entre os livros");
    Sala* porao = criarSala("Porao", "Uma chave enferrujada no chão");

    // Conexões do mapa
    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = jardim;
    salaEstar->direita = biblioteca;
    cozinha->direita = porao;

    // Árvore de pistas inicial
    PistaNode* arvorePistas = NULL;

    // Introdução
    printf("🔍 Bem-vindo ao Detective Quest!\nExplore a mansao e colete pistas para resolver o misterio...\n");

    // Início da exploração
    explorarSalasComPistas(hall, &arvorePistas);

    // Exibição final das pistas
    printf("\n📜 Pistas coletadas (ordem alfabetica):\n");
    if (arvorePistas) {
        exibirPistas(arvorePistas);
    } else {
        printf("Nenhuma pista foi coletada.\n");
    }

    // Liberação de memória
    liberarSalas(hall);
    liberarPistas(arvorePistas);

    return 0;
}