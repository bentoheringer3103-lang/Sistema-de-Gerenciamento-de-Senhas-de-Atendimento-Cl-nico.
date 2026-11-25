#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PACIENTES 500
#define TAM_SENHA 16
#define TAM_NOME 80

typedef enum {
    NORMAL = 0,
    PREFERENCIAL = 1,
    URGENTE = 2
} Categoria;

typedef struct {
    char nome[TAM_NOME];
    char genero; /* M = masculino, F = feminino, O = outro */
    int idade;
    int gestante;
    int lactante;
    int necessidadeEspecial;
    int obesidade;
    char senha[TAM_SENHA];
    Categoria categoria;
    int status; /* 0 = aguardando, 1 = em atendimento, 2 = finalizado */
    unsigned long ordem;
} Paciente;

typedef struct {
    Paciente itens[MAX_PACIENTES];
    int quantidade;
} Fila;

static Fila fila;
static unsigned long proximaOrdem = 1;
static int contadorNormal = 0;
static int contadorPreferencial = 0;
static int contadorUrgente = 0;
static int indiceEmAtendimento = -1;

void limparEntrada(void);
int lerInteiro(const char *mensagem, int minimo, int maximo);
char lerGenero(void);
void lerTexto(const char *mensagem, char *destino, size_t tamanho);
int lerSimNao(const char *mensagem);
Categoria classificarPaciente(const Paciente *p);
void gerarSenha(Categoria categoria, char *destino, size_t tamanho);
const char *nomeCategoria(Categoria categoria);
void cadastrarPaciente(void);
int proximoPaciente(void);
void consultarProximaSenha(void);
void chamarProximoPaciente(void);
void finalizarAtendimento(void);
void mostrarPainel(void);
void listarFila(void);
void mostrarMenu(void);
void executarOpcao(int opcao);

void limparEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int lerInteiro(const char *mensagem, int minimo, int maximo) {
    char linha[100];
    int valor;
    char extra;

    while (1) {
        printf("%s", mensagem);
        if (!fgets(linha, sizeof(linha), stdin)) {
            printf("\nEntrada encerrada.\n");
            exit(EXIT_FAILURE);
        }
        if (sscanf(linha, " %d %c", &valor, &extra) == 1 &&
            valor >= minimo && valor <= maximo) {
            return valor;
        }
        printf("Entrada invalida. Informe um numero entre %d e %d.\n",
               minimo, maximo);
    }
}

void lerTexto(const char *mensagem, char *destino, size_t tamanho) {
    size_t len;
    while (1) {
        printf("%s", mensagem);
        if (!fgets(destino, (int)tamanho, stdin)) {
            printf("\nEntrada encerrada.\n");
            exit(EXIT_FAILURE);
        }
        len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        } else {
            limparEntrada();
        }
        if (strlen(destino) > 0) return;
        printf("O campo nao pode ficar vazio.\n");
    }
}

char lerGenero(void) {
    char linha[30];
    char genero;
    while (1) {
        printf("Genero (F = feminino, M = masculino, O = outro): ");
        if (!fgets(linha, sizeof(linha), stdin)) exit(EXIT_FAILURE);
        genero = (char)toupper((unsigned char)linha[0]);
        if ((genero == 'F' || genero == 'M' || genero == 'O') &&
            (linha[1] == '\n' || linha[1] == '\0')) {
            return genero;
        }
        printf("Opcao invalida. Digite F, M ou O.\n");
    }
}

int lerSimNao(const char *mensagem) {
    char linha[30];
    char resposta;
    while (1) {
        printf("%s (S/N): ", mensagem);
        if (!fgets(linha, sizeof(linha), stdin)) exit(EXIT_FAILURE);
        resposta = (char)toupper((unsigned char)linha[0]);
        if ((resposta == 'S' || resposta == 'N') &&
            (linha[1] == '\n' || linha[1] == '\0')) {
            return resposta == 'S';
        }
        printf("Resposta invalida. Digite S ou N.\n");
    }
}

Categoria classificarPaciente(const Paciente *p) {
    if (p->idade > 80) {
        return URGENTE;
    }

    if (p->gestante || p->lactante || p->necessidadeEspecial || p->obesidade) {
        return PREFERENCIAL;
    }

    if ((p->genero == 'F' && p->idade >= 60) ||
        (p->genero == 'M' && p->idade >= 65)) {
        return PREFERENCIAL;
    }

    return NORMAL;
}

void gerarSenha(Categoria categoria, char *destino, size_t tamanho) {
    int numero;
    char prefixo;

    switch (categoria) {
        case URGENTE:
            numero = ++contadorUrgente;
            prefixo = 'U';
            break;
        case PREFERENCIAL:
            numero = ++contadorPreferencial;
            prefixo = 'P';
            break;
        default:
            numero = ++contadorNormal;
            prefixo = 'N';
            break;
    }
    snprintf(destino, tamanho, "%c%03d", prefixo, numero);
}

const char *nomeCategoria(Categoria categoria) {
    switch (categoria) {
        case URGENTE: return "Urgente";
        case PREFERENCIAL: return "Preferencial";
        default: return "Normal";
    }
}

void cadastrarPaciente(void) {
    Paciente p;

    if (fila.quantidade >= MAX_PACIENTES) {
        printf("\nLimite de %d pacientes atingido.\n", MAX_PACIENTES);
        return;
    }

    printf("\n=== CADASTRO DE PACIENTE ===\n");
    lerTexto("Nome do paciente: ", p.nome, sizeof(p.nome));
    p.genero = lerGenero();
    p.idade = lerInteiro("Idade: ", 0, 120);

    p.gestante = 0;
    if (p.genero == 'F') {
        p.gestante = lerSimNao("A paciente esta gestante?");
    }
    p.lactante = lerSimNao("E lactante?");
    p.necessidadeEspecial = lerSimNao("Possui necessidade especial?");
    p.obesidade = lerSimNao("Possui condicao de obesidade?");

    p.categoria = classificarPaciente(&p);
    gerarSenha(p.categoria, p.senha, sizeof(p.senha));
    p.status = 0;
    p.ordem = proximaOrdem++;

    fila.itens[fila.quantidade++] = p;

    printf("\nPaciente cadastrado com sucesso!\n");
    printf("Senha: %s | Categoria: %s\n", p.senha, nomeCategoria(p.categoria));
}

int proximoPaciente(void) {
    int melhor = -1;
    int i;

    for (i = 0; i < fila.quantidade; i++) {
        if (fila.itens[i].status != 0) continue;

        if (melhor == -1 ||
            fila.itens[i].categoria > fila.itens[melhor].categoria ||
            (fila.itens[i].categoria == fila.itens[melhor].categoria &&
             fila.itens[i].ordem < fila.itens[melhor].ordem)) {
            melhor = i;
        }
    }
    return melhor;
}

void consultarProximaSenha(void) {
    int indice = proximoPaciente();

    printf("\n=== PROXIMA SENHA ===\n");
    if (indice == -1) {
        printf("Nao ha pacientes aguardando atendimento.\n");
    } else {
        printf("Senha: %s | Categoria: %s | Paciente: %s\n",
               fila.itens[indice].senha,
               nomeCategoria(fila.itens[indice].categoria),
               fila.itens[indice].nome);
    }
}

void chamarProximoPaciente(void) {
    int indice;

    if (indiceEmAtendimento != -1) {
        printf("\nFinalize o atendimento atual (%s) antes de chamar outro.\n",
               fila.itens[indiceEmAtendimento].senha);
        return;
    }

    indice = proximoPaciente();
    if (indice == -1) {
        printf("\nNao ha pacientes aguardando atendimento.\n");
        return;
    }

    fila.itens[indice].status = 1;
    indiceEmAtendimento = indice;

    printf("\n*** SENHA CHAMADA: %s ***\n", fila.itens[indice].senha);
    printf("Paciente: %s\nCategoria: %s\n",
           fila.itens[indice].nome,
           nomeCategoria(fila.itens[indice].categoria));
}

void finalizarAtendimento(void) {
    if (indiceEmAtendimento == -1) {
        printf("\nNao existe atendimento em andamento.\n");
        return;
    }

    fila.itens[indiceEmAtendimento].status = 2;
    printf("\nAtendimento da senha %s finalizado.\n",
           fila.itens[indiceEmAtendimento].senha);
    indiceEmAtendimento = -1;
}

void mostrarPainel(void) {
    int i, aguardandoNormal = 0, aguardandoPreferencial = 0, aguardandoUrgente = 0;
    int indice = proximoPaciente();

    printf("\n================ PAINEL DE ATENDIMENTO ================\n");
    if (indiceEmAtendimento == -1) {
        printf("Senha em atendimento: Nenhuma\n");
    } else {
        printf("Senha em atendimento: %s - %s\n",
               fila.itens[indiceEmAtendimento].senha,
               fila.itens[indiceEmAtendimento].nome);
    }

    if (indice == -1) {
        printf("Proxima senha: Nenhuma\n");
    } else {
        printf("Proxima senha: %s (%s)\n",
               fila.itens[indice].senha,
               nomeCategoria(fila.itens[indice].categoria));
    }

    for (i = 0; i < fila.quantidade; i++) {
        if (fila.itens[i].status != 0) continue;
        switch (fila.itens[i].categoria) {
            case URGENTE: aguardandoUrgente++; break;
            case PREFERENCIAL: aguardandoPreferencial++; break;
            default: aguardandoNormal++; break;
        }
    }

    printf("\nPacientes aguardando:\n");
    printf("Urgentes: %d\nPreferenciais: %d\nNormais: %d\n",
           aguardandoUrgente, aguardandoPreferencial, aguardandoNormal);

    printf("\nProximas senhas (ordem de chamada):\n");
    {
        int exibidos = 0;
        Categoria prioridade;
        for (prioridade = URGENTE; prioridade >= NORMAL; prioridade--) {
            for (i = 0; i < fila.quantidade; i++) {
                if (fila.itens[i].status == 0 &&
                    fila.itens[i].categoria == prioridade) {
                    printf("%s - %s (%s)\n",
                           fila.itens[i].senha, fila.itens[i].nome,
                           nomeCategoria(fila.itens[i].categoria));
                    exibidos++;
                }
            }
        }
        if (exibidos == 0) printf("Nenhuma senha aguardando.\n");
    }
    printf("=======================================================\n");
}

void listarFila(void) {
    int i, exibidos = 0;
    printf("\n=== FILA DE ESPERA ===\n");
    for (i = 0; i < fila.quantidade; i++) {
        if (fila.itens[i].status == 0) {
            printf("%s | %-15s | %-13s\n",
                   fila.itens[i].senha, fila.itens[i].nome,
                   nomeCategoria(fila.itens[i].categoria));
            exibidos++;
        }
    }
    if (!exibidos) printf("Fila vazia.\n");
}

void mostrarMenu(void) {
    printf("\n========== SISTEMA DE SENHAS - CLINICA ==========\n");
    printf("1 - Cadastrar paciente e gerar senha\n");
    printf("2 - Consultar proxima senha\n");
    printf("3 - Chamar proximo paciente\n");
    printf("4 - Finalizar atendimento atual\n");
    printf("5 - Exibir painel de chamada\n");
    printf("6 - Listar fila de espera\n");
    printf("0 - Encerrar sistema\n");
    printf("=================================================\n");
}

void executarOpcao(int opcao) {
    switch (opcao) {
        case 1: cadastrarPaciente(); break;
        case 2: consultarProximaSenha(); break;
        case 3: chamarProximoPaciente(); break;
        case 4: finalizarAtendimento(); break;
        case 5: mostrarPainel(); break;
        case 6: listarFila(); break;
        case 0: printf("\nSistema encerrado. Obrigado!\n"); break;
        default: printf("\nOpcao invalida. Escolha uma opcao do menu.\n");
    }
}

int main(void) {
    int opcao;

    fila.quantidade = 0;
    do {
        mostrarMenu();
        opcao = lerInteiro("Escolha uma opcao: ", 0, 6);
        executarOpcao(opcao);
    } while (opcao != 0);

    return 0;
}
