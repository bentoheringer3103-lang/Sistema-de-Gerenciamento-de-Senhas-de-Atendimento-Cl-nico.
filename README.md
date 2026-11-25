# Sistema de Gerenciamento de Senhas de Atendimento Clínico

Projeto acadêmico desenvolvido em linguagem C para simular o cadastro de pacientes, a classificação didática de prioridades e o gerenciamento de uma fila de atendimento em uma clínica.

## 1. Objetivo

Organizar a fila de atendimento por três categorias — **Urgente**, **Preferencial** e **Normal** — respeitando a prioridade entre categorias e a ordem de chegada dentro de cada categoria (FIFO).

> **Aviso:** as regras de classificação deste projeto são exclusivamente didáticas e foram definidas conforme o enunciado da atividade. O sistema não realiza triagem médica e não substitui avaliação clínica ou a legislação aplicável.

## 2. Funcionalidades

- Cadastro de pacientes com nome, gênero e idade.
- Registro de gestação (quando aplicável), lactação, necessidade especial e condição de obesidade.
- Classificação automática em Normal, Preferencial ou Urgente.
- Geração de senhas independentes: `N001`, `P001` e `U001`.
- Consulta da próxima senha.
- Chamada do próximo paciente.
- Identificação de atendimento em andamento.
- Finalização do atendimento.
- Painel textual com senha chamada, próxima senha e quantidade de pessoas aguardando por categoria.
- Listagem da fila de espera.
- Validação de entradas e tratamento de opções inválidas.

## 3. Regras de classificação

A classificação segue a ordem de precedência abaixo:

1. **Urgente (U):** pacientes com idade superior a 80 anos.
2. **Preferencial (P):** gestantes, lactantes, pessoas com necessidade especial, pessoas com condição de obesidade, mulheres com 60 anos ou mais e homens com 65 anos ou mais.
3. **Normal (N):** pacientes que não se enquadram nas condições anteriores.

A fila é organizada por prioridade: Urgente, depois Preferencial e por último Normal. Dentro de cada categoria, vale a ordem de cadastro (FIFO).

## 4. Estrutura do projeto

```text
sistema-senhas-clinica/
├── main.c
└── README.md
```

## 5. Requisitos para executar

É necessário ter um compilador C instalado, como o GCC.

### Windows

Com GCC instalado (por exemplo, via MinGW), abra o terminal na pasta do projeto:

```bash
gcc -std=c11 -Wall -Wextra -pedantic main.c -o sistema_senhas.exe
sistema_senhas.exe
```

## 6. Funções implementadas

| Função | Responsabilidade |
|---|---|
| `lerInteiro()` | Lê e valida números inteiros dentro de uma faixa. |
| `lerTexto()` | Lê campos de texto obrigatórios. |
| `lerGenero()` | Valida o gênero informado. |
| `lerSimNao()` | Valida respostas de Sim ou Não. |
| `classificarPaciente()` | Define a categoria de prioridade do paciente. |
| `gerarSenha()` | Gera senha sequencial por categoria. |
| `nomeCategoria()` | Retorna o nome textual da categoria. |
| `cadastrarPaciente()` | Registra os dados e inclui o paciente na fila. |
| `proximoPaciente()` | Localiza o próximo paciente conforme prioridade e FIFO. |
| `consultarProximaSenha()` | Exibe quem deverá ser chamado em seguida. |
| `chamarProximoPaciente()` | Inicia o atendimento do próximo paciente. |
| `finalizarAtendimento()` | Finaliza o atendimento atual. |
| `mostrarPainel()` | Exibe o painel e os totais por categoria. |
| `listarFila()` | Lista os pacientes que ainda aguardam. |
| `mostrarMenu()` | Apresenta as opções do sistema. |
| `executarOpcao()` | Encaminha a opção escolhida para a função correspondente. |
| `main()` | Coordena o ciclo principal do programa. |

## 7. Exemplo de utilização

Exemplo ilustrativo de cadastro:

```text
Nome do paciente: Maria
Genero (F = feminino, M = masculino, O = outro): F
Idade: 35
A paciente esta gestante? (S/N): S
E lactante? (S/N): N
Possui necessidade especial? (S/N): N
Possui condicao de obesidade? (S/N): N

Paciente cadastrado com sucesso!
Senha: P001 | Categoria: Preferencial
```

Um paciente com 82 anos recebe uma senha urgente, como `U001`. Um paciente sem condição de prioridade recebe uma senha normal, como `N001`.

## 8. Observações técnicas

- O limite de armazenamento desta versão é de 500 pacientes por execução.
- Os dados ficam armazenados na memória e são perdidos quando o programa é encerrado.
- O sistema permite apenas um atendimento em andamento por vez.
- Senhas finalizadas não retornam à fila.
- A ordenação da fila é feita no momento da consulta/chamada, sem alterar a ordem de emissão dos registros.
