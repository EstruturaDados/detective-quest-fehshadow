#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_HASH 10
#define MAX_SUSPEITO 30
#define TOTAL_PISTAS 6
#define PISTAS_SORTEADAS 4

typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

typedef struct PistaNode {
    char pista[100];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

typedef struct ListaPistas {
    char pista[100];
    struct ListaPistas* prox;
} ListaPistas;

typedef struct Suspeito {
    char nome[MAX_SUSPEITO];
    ListaPistas* pistas;
    struct Suspeito* prox;
} Suspeito;

typedef struct Pilha {
    Sala* sala;
    struct Pilha* anterior;
} Pilha;

Suspeito* tabelaHash[TAM_HASH] = {NULL};

void paraMinusculas(char* texto) {
    for (int i = 0; texto[i]; i++) {
        if (texto[i] >= 'A' && texto[i] <= 'Z')
            texto[i] += 'a' - 'A';
    }
}

Sala* criarSala(const char* nome) {
    Sala* nova = (Sala*)malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    nova->pista[0] = '\0';
    nova->esquerda = nova->direita = NULL;
    return nova;
}

PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (pista[0] == '\0') return raiz;
    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*)malloc(sizeof(PistaNode));
        strcpy(novo->pista, pista);
        novo->esquerda = novo->direita = NULL;
        return novo;
    }
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    return raiz;
}

void exibirPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}

int hash(const char* pista) {
    return (int)(pista[0] % TAM_HASH);
}

void inserirNaHash(const char* pista, const char* suspeito) {
    int indice = hash(pista);
    Suspeito* atual = tabelaHash[indice];

    while (atual) {
        if (strcmp(atual->nome, suspeito) == 0) {
            ListaPistas* nova = (ListaPistas*)malloc(sizeof(ListaPistas));
            strcpy(nova->pista, pista);
            nova->prox = atual->pistas;
            atual->pistas = nova;
            return;
        }
        atual = atual->prox;
    }

    Suspeito* novoSuspeito = (Suspeito*)malloc(sizeof(Suspeito));
    strcpy(novoSuspeito->nome, suspeito);
    novoSuspeito->pistas = NULL;
    novoSuspeito->prox = tabelaHash[indice];
    tabelaHash[indice] = novoSuspeito;

    ListaPistas* nova = (ListaPistas*)malloc(sizeof(ListaPistas));
    strcpy(nova->pista, pista);
    nova->prox = NULL;
    novoSuspeito->pistas = nova;
}

int encontrarSuspeito(const char* pista, const char* suspeito) {
    int indice = hash(pista);
    Suspeito* atual = tabelaHash[indice];

    while (atual) {
        if (strcmp(atual->nome, suspeito) == 0) {
            ListaPistas* lp = atual->pistas;
            while (lp) {
                if (strcmp(lp->pista, pista) == 0)
                    return 1;
                lp = lp->prox;
            }
        }
        atual = atual->prox;
    }
    return 0;
}

void verificarSuspeitoFinal(PistaNode* raiz, const char* suspeito, int* contador) {
    if (raiz == NULL) return;
    verificarSuspeitoFinal(raiz->esquerda, suspeito, contador);
    if (encontrarSuspeito(raiz->pista, suspeito))
        (*contador)++;
    verificarSuspeitoFinal(raiz->direita, suspeito, contador);
}

Pilha* empilhar(Pilha* topo, Sala* sala) {
    Pilha* novo = (Pilha*)malloc(sizeof(Pilha));
    novo->sala = sala;
    novo->anterior = topo;
    return novo;
}

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

void explorarSalas(Sala* atual, PistaNode** arvorePistas) {
    char escolha;
    Pilha* trilha = NULL;

    while (atual) {
        printf("\n📍 Sala atual: %s\n", atual->nome);
        if (strlen(atual->pista) > 0) {
            printf("🔎 Pista encontrada: \"%s\"\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
        } else {
            printf("🔍 Nenhuma pista nesta sala.\n");
        }

        printf("\nEscolha:\n");
        if (atual->esquerda) printf("  [e] Ir para a esquerda\n");
        if (atual->direita)  printf("  [d] Ir para a direita\n");
        if (trilha)          printf("  [a] Voltar\n");
        printf("  [s] Sair\nOpcao: ");
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
            if (anterior)
                atual = anterior;
            else
                printf("🔁 Voce ja esta na sala inicial.\n");
        } else if (escolha == 's') {
            printf("\n🔚 Exploracao encerrada.\n");
            break;
        } else {
            printf("Opcao invalida.\n");
        }
    }

    while (trilha) {
        Sala* temp;
        trilha = desempilhar(trilha, &temp);
    }
}

int main() {
    srand(time(NULL));

    const char* todasPistas[TOTAL_PISTAS] = {
        "Pegadas molhadas", "Luva rasgada",
        "Livro antigo", "Carta escondida",
        "Chave enferrujada", "Perfume raro"
    };

    const char* suspeitos[TOTAL_PISTAS] = {
        "bruno", "bruno",
        "bella", "bella",
        "alice", "alice"
    };

    Sala* hall = criarSala("Hall de Entrada");
    Sala* salaEstar = criarSala("Sala de Estar");
    Sala* cozinha = criarSala("Cozinha");
    Sala* jardim = criarSala("Jardim");
    Sala* biblioteca = criarSala("Biblioteca");
    Sala* porao = criarSala("Porao");
    Sala* quarto = criarSala("Quarto de hóspedes");

    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = jardim;
    salaEstar->direita = biblioteca;
    cozinha->direita = porao;
    porao->direita = quarto;

    Sala* salas[] = {salaEstar, jardim, biblioteca, porao, quarto, cozinha};

    for (int i = TOTAL_PISTAS - 1; i > 0; i--) {
    int j = rand() % (i + 1);

    // Troca pistas
    const char* tempP = todasPistas[i];
    todasPistas[i] = todasPistas[j];
    todasPistas[j] = tempP;

    // Troca suspeitos juntos
    const char* tempS = suspeitos[i];
    suspeitos[i] = suspeitos[j];
    suspeitos[j] = tempS;

    }

    // Distribuir 4 pistas sorteadas nas salas
    for (int i = 0; i < PISTAS_SORTEADAS; i++) {
        strcpy(salas[i]->pista, todasPistas[i]);
        inserirNaHash(todasPistas[i], suspeitos[i]);
    }

    printf("🔍 Bem-vindo ao Detective Quest: Ache o Suspeito!\nExplore a mansão e colete pistas...\n");

    printf("\n👤 Suspeitos na mansão:\n");
    printf(" - Bella está na Sala de Estar\n");
    printf(" - Alice está no Quarto de hóspedes\n");
    printf(" - Bruno está no Jardim\n");

    // Mostrar pistas sorteadas e seus suspeitos
    printf("\n🎲 Pistas sorteadas e seus suspeitos:\n");
    for (int i = 0; i < PISTAS_SORTEADAS; i++) {
        printf(" - \"%s\" aponta para %s\n", todasPistas[i], suspeitos[i]);
    }

    PistaNode* arvorePistas = NULL;
    explorarSalas(hall, &arvorePistas);

    printf("\n📜 Pistas coletadas:\n");
    if (arvorePistas)
        exibirPistas(arvorePistas);
    else
        printf("Nenhuma pista foi coletada.\n");

    char suspeito[MAX_SUSPEITO];
    printf("\n🕵️ Quem você acusa como culpado? Digite o nome: ");
    scanf(" %[^\n]", suspeito);
    paraMinusculas(suspeito);

    int contador = 0;
    verificarSuspeitoFinal(arvorePistas, suspeito, &contador);

    // ✅ A acusação só será considerada correta se o suspeito tiver pelo menos 2 das 4 pistas sorteadas
    if (contador >= 2)
        printf("\n✅ Acusacao correta! %s foi identificado com base nas pistas coletadas.\n", suspeito);
    else
        printf("\n❌ Acusacao falhou. Nao ha evidencias suficientes contra %s.\n", suspeito);

    return 0;
}