#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura que representa uma sala da mansao
typedef struct Sala {
    char nome[50];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

// Estrutura de pilha para armazenar o caminho anterior
typedef struct Pilha {
    Sala* sala;
    struct Pilha* anterior;
} Pilha;

// Cria uma nova sala dinamicamente
Sala* criarSala(const char* nome) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro ao alocar memoria para a sala.\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

// Empilha a sala atual na trilha
Pilha* empilhar(Pilha* topo, Sala* sala) {
    Pilha* novo = (Pilha*)malloc(sizeof(Pilha));
    novo->sala = sala;
    novo->anterior = topo;
    return novo;
}

// Desempilha e retorna a sala anterior
Pilha* desempilhar(Pilha* topo, Sala** salaAnterior) {
    if (topo == NULL) {
        *salaAnterior = NULL;
        return NULL;
    }
    *salaAnterior = topo->sala;
    Pilha* anterior = topo->anterior;
    free(topo);
    return anterior;
}

// Permite ao jogador explorar a mansao
void explorarSalas(Sala* atual) {
    char escolha;
    Pilha* trilha = NULL;

    while (atual != NULL) {
        printf("\nVoce esta na sala: %s\n", atual->nome);

        int temEsquerda = (atual->esquerda != NULL);
        int temDireita = (atual->direita != NULL);

        // Oferece opcoes de navegacao
        if (!temEsquerda && !temDireita) {
            printf("Esta sala nao possui mais caminhos.\n");
        }

        printf("Escolha um caminho:\n");
        if (temEsquerda) printf("  [e] Ir para a esquerda\n");
        if (temDireita)  printf("  [d] Ir para a direita\n");
        if (trilha != NULL) printf("  [a] Voltar para a sala anterior\n");
        printf("  [s] Sair da exploracao\n");
        printf("Opcao: ");
        scanf(" %c", &escolha);

        if (escolha == 'e') {
            if (temEsquerda) {
                trilha = empilhar(trilha, atual);
                atual = atual->esquerda;
            } else {
                printf("Nao ha caminho a esquerda.\n");
            }
        } else if (escolha == 'd') {
            if (temDireita) {
                trilha = empilhar(trilha, atual);
                atual = atual->direita;
            } else {
                printf("Nao ha caminho a direita.\n");
            }
        } else if (escolha == 'a') {
            Sala* anterior = NULL;
            trilha = desempilhar(trilha, &anterior);
            if (anterior != NULL) {
                atual = anterior;
            } else {
                printf("Voce ja esta na sala inicial. Nao ha como voltar.\n");
            }
        } else if (escolha == 's') {
            printf("Exploracao encerrada pelo jogador.\n");
            break;
        } else {
            printf("Opcao invalida. Tente novamente.\n");
        }
    }

    // Libera a pilha restante
    while (trilha != NULL) {
        Sala* temp;
        trilha = desempilhar(trilha, &temp);
    }
}

// Funcao principal que monta o mapa da mansao e inicia a exploracao
int main() {
    Sala* hall = criarSala("Hall de Entrada");
    Sala* salaEstar = criarSala("Sala de Estar");
    Sala* cozinha = criarSala("Cozinha");
    Sala* jardim = criarSala("Jardim");
    Sala* biblioteca = criarSala("Biblioteca");
    Sala* porao = criarSala("Porao");

    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = jardim;
    salaEstar->direita = biblioteca;
    cozinha->direita = porao;

    printf("Bem-vindo ao Detective Quest!\nExplore a mansao e descubra seus segredos...\n");
    explorarSalas(hall);

    free(hall);
    free(salaEstar);
    free(cozinha);
    free(jardim);
    free(biblioteca);
    free(porao);

    return 0;
}