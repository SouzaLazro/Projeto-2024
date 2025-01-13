#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>

#define MAX_CLIENTES 100
#define MAX_PROJETOS 100
#define MAX_ESTOQUE 100

typedef struct {
    int id;
    char nome[50];
    char email[50];
    char celular[20];
} Cliente;

typedef struct {
    int id;
    char descricao[100];
    double valor;
    char dataEntrega[9];
    int clienteID;
} Projeto;

typedef struct {
    int id;
    char nome[50];
    int quantidade;
    double preco;
} Estoque;

Cliente clientes[MAX_CLIENTES];
Projeto projetos[MAX_PROJETOS];
Estoque estoque[MAX_ESTOQUE];
int totalClientes = 0;
int totalProjetos = 0;
int totalEstoque = 0;

void cadastrarCliente();
void cadastrarProjeto();
void visualizarClientes();
void visualizarProjetos();
void menuCadastro();
void menuVisualizacao();
void menuEstoque();
void menuPesquisaClientes();
void menuPesquisaProjetos();
void adicionarProduto();
void consultarEstoque();
void removerProduto();
void atualizarQuantidade();
void pesquisarProjetoPorID(int id);
void pesquisarProjetoPorDescricao(char *descricao);
int lerInteiro();
int validarData(char *data);

int main() {
    setlocale(LC_ALL, "portuguese");
    int opcao;
    do {
        printf("\nMenu Principal:\n");
        printf("1. Cadastro\n");
        printf("2. Visualização\n");
        printf("3. Estoque\n");
        printf("4. Sair\n");
        printf("Escolha uma opção: ");
        opcao = lerInteiro();

        switch (opcao) {
            case 1: menuCadastro(); break;
            case 2: menuVisualizacao(); break;
            case 3: menuEstoque(); break;
            case 4: printf("Saindo do programa...\n"); break;
            default: printf("Opção inválida!\n");
        }
    } while (opcao != 4);

    return 0;
}

int lerInteiro() {
    int valor;
    while (scanf("%d", &valor) != 1) {
        printf("Erro: Por favor, insira um valor válido.\n");
        while (getchar() != '\n');
    }
    return valor;
}

int validarData(char *data) {
    if (strlen(data) != 8 || strspn(data, "0123456789") != 8) {
        printf("Erro: Formato de data inválido. Use ddmmaaaa.\n");
        return 0;
    }

    int dia = (data[0] - '0') * 10 + (data[1] - '0');
    int mes = (data[2] - '0') * 10 + (data[3] - '0');
    int ano = (data[4] - '0') * 1000 + (data[5] - '0') * 100 + (data[6] - '0') * 10 + (data[7] - '0');

    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano < 1900 || ano > 2100) {
        printf("Erro: Data inválida. Verifique os valores de dia, mês ou ano.\n");
        return 0;
    }

    int diasNoMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0)) {
        diasNoMes[1] = 29;
    }

    if (dia > diasNoMes[mes - 1]) {
        printf("Erro: O mês %d possui no máximo %d dias.\n", mes, diasNoMes[mes - 1]);
        return 0;
    }

    time_t t = time(NULL);
    struct tm tmAtual = *localtime(&t);

    int anoAtual = tmAtual.tm_year + 1900;
    int mesAtual = tmAtual.tm_mon + 1;
    int diaAtual = tmAtual.tm_mday;

    tmAtual.tm_mday++;
    mktime(&tmAtual);

    if (ano < anoAtual || (ano == anoAtual && mes < mesAtual) || (ano == anoAtual && mes == mesAtual && dia <= diaAtual)) {
        printf("Erro: A data não pode ser anterior ou igual à data atual.\n");
        return 0;
    }

    tmAtual.tm_year += 7;
    mktime(&tmAtual);

    if (ano > tmAtual.tm_year + 1900 ||
        (ano == tmAtual.tm_year + 1900 && mes > tmAtual.tm_mon + 1) ||
        (ano == tmAtual.tm_year + 1900 && mes == tmAtual.tm_mon + 1 && dia > tmAtual.tm_mday)) {
        printf("Erro: A data não pode ser superior a 7 anos após a data atual.\n");
        return 0;
    }

    return 1;
}

void cadastrarCliente() {
    if (totalClientes >= MAX_CLIENTES) {
        printf("Erro: Limite de clientes atingido.\n");
        return;
    }

    printf("\nCadastro de Cliente\n");
    int id;
    printf("ID: ");
    id = lerInteiro();

    for (int i = 0; i < totalClientes; i++) {
        if (clientes[i].id == id) {
            printf("Erro: ID já existe.\n");
            return;
        }
    }

    clientes[totalClientes].id = id;
    printf("Nome: ");
    scanf(" %49[^\n]", clientes[totalClientes].nome);
    printf("Email: ");
    scanf(" %49[^\n]", clientes[totalClientes].email);
    printf("Celular: ");
    while (1) {
        scanf(" %19[^\n]", clientes[totalClientes].celular);
        if (strlen(clientes[totalClientes].celular) == 11 &&
            strspn(clientes[totalClientes].celular, "0123456789") == 11) {
            break;
        } else {
            printf("Erro: O celular deve ter exatamente 11 dígitos numéricos. Tente novamente: ");
        }
    }
    totalClientes++;
    printf("Cliente cadastrado com sucesso!\n");
}

void cadastrarProjeto() {
    if (totalProjetos >= MAX_PROJETOS) {
        printf("Erro: Limite de projetos atingido.\n");
        return;
    }

    printf("\nCadastro de Projeto\n");
    int id, clienteID;
    printf("ID do Projeto: ");
    id = lerInteiro();

    for (int i = 0; i < totalProjetos; i++) {
        if (projetos[i].id == id) {
            printf("Erro: ID do projeto já existe.\n");
            return;
        }
    }

    printf("Descrição: ");
    scanf(" %99[^\n]", projetos[totalProjetos].descricao);
    printf("Valor: ");
    scanf("%lf", &projetos[totalProjetos].valor);

    char dataEntrega[9];
    printf("Data de Entrega (ddmmaaaa): ");
    while (1) {
        scanf(" %8[^\n]", dataEntrega);
        if (validarData(dataEntrega)) {
            break;
        } else {
            printf("Por favor, insira uma data válida (ddmmaaaa): ");
        }
    }

    strcpy(projetos[totalProjetos].dataEntrega, dataEntrega);

    printf("ID do Cliente Associado: ");
    clienteID = lerInteiro();

    int clienteIndex = -1;
    for (int i = 0; i < totalClientes; i++) {
        if (clientes[i].id == clienteID) {
            clienteIndex = i;
            break;
        }
    }

    if (clienteIndex == -1) {
        printf("Erro: Cliente não encontrado.\n");
        return;
    }

    projetos[totalProjetos].id = id;
    projetos[totalProjetos].clienteID = clienteID;
    totalProjetos++;
    printf("Projeto cadastrado com sucesso!\n");
}

void menuCadastro() {
    int opcao;
    do {
        printf("\nMenu de Cadastro:\n");
        printf("1. Cadastrar Cliente\n");
        printf("2. Cadastrar Projeto\n");
        printf("3. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: cadastrarCliente(); break;
            case 2: cadastrarProjeto(); break;
            case 3: break;
            default: printf("Opcao invalida!\n");
        }
    } while (opcao != 3);
}

void menuVisualizacao() {
    int opcao;
    do {
        printf("\nMenu de Visualizacao:\n");
        printf("1. Visualizar Clientes\n");
        printf("2. Visualizar Projetos\n");
        printf("3. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: visualizarClientes(); break;
            case 2: visualizarProjetos(); break;
            case 3: break;
            default: printf("Opcao invalida!\n");
        }
    } while (opcao != 3);
}

void visualizarClientes() {
    int opcaoPesquisa;
    do {
        printf("\nMenu de Visualizacao de Clientes:\n");
        printf("1. Exibir Lista Completa\n2. Pesquisar Cliente\n3. Voltar\nEscolha uma opcao: ");
        scanf("%d", &opcaoPesquisa);

        if (opcaoPesquisa == 1) {
            printf("\nLista de Clientes:\n");
            printf("%-5s %-20s %-30s %-15s\n", "ID", "Nome", "Email", "Celular");
            for (int i = 0; i < totalClientes; i++) {
                printf("%-5d %-20s %-30s %-15s\n",
                       clientes[i].id,
                       clientes[i].nome,
                       clientes[i].email,
                       clientes[i].celular);
            }
        } else if (opcaoPesquisa == 2) {
            menuPesquisaClientes();
        } else if (opcaoPesquisa == 3) {
            return;
        } else {
            printf("Opcao invalida!\n");
        }
    } while (1);
}

void menuPesquisaClientes() {
    int opcaoPesquisa;
    do {
        printf("\nMenu de Pesquisa de Clientes:\n");
        printf("1. Pesquisar por ID\n2. Pesquisar por Nome\n3. Pesquisar por Email\n4. Pesquisar por Celular\n5. Voltar\nEscolha uma opcao: ");
        scanf("%d", &opcaoPesquisa);

        if (opcaoPesquisa == 1) {
            int id;
            printf("Digite o ID do Cliente: ");
            id = lerInteiro();
            int encontrado = 0;
            for (int i = 0; i < totalClientes; i++) {
                if (clientes[i].id == id) {
                    printf("%-5d %-20s %-30s %-15s\n",
                           clientes[i].id,
                           clientes[i].nome,
                           clientes[i].email,
                           clientes[i].celular);
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) {
                printf("Cliente nao encontrado.\n");
            }
        } else if (opcaoPesquisa == 2) {
            char nome[50];
            printf("Digite o nome do Cliente: ");
            scanf(" %49[^\n]", nome);
            int encontrado = 0;
            for (int i = 0; i < totalClientes; i++) {
                if (strstr(clientes[i].nome, nome) != NULL) {
                    printf("%-5d %-20s %-30s %-15s\n",
                           clientes[i].id,
                           clientes[i].nome,
                           clientes[i].email,
                           clientes[i].celular);
                    encontrado = 1;
                }
            }
            if (!encontrado) {
                printf("Cliente nao encontrado.\n");
            }
        } else if (opcaoPesquisa == 3) {
            char email[50];
            printf("Digite o email do Cliente: ");
            scanf(" %49[^\n]", email);
            int encontrado = 0;
            for (int i = 0; i < totalClientes; i++) {
                if (strstr(clientes[i].email, email) != NULL) {
                    printf("%-5d %-20s %-30s %-15s\n",
                           clientes[i].id,
                           clientes[i].nome,
                           clientes[i].email,
                           clientes[i].celular);
                    encontrado = 1;
                }
            }
            if (!encontrado) {
                printf("Cliente nao encontrado.\n");
            }
        } else if (opcaoPesquisa == 4) {
            char celular[20];
            printf("Digite o celular do Cliente: ");
            scanf(" %19[^\n]", celular);
            int encontrado = 0;
            for (int i = 0; i < totalClientes; i++) {
                if (strstr(clientes[i].celular, celular) != NULL) {
                    printf("%-5d %-20s %-30s %-15s\n",
                           clientes[i].id,
                           clientes[i].nome,
                           clientes[i].email,
                           clientes[i].celular);
                    encontrado = 1;
                }
            }
            if (!encontrado) {
                printf("Cliente nao encontrado.\n");
            }
        } else if (opcaoPesquisa == 5) {
            break;
        } else {
            printf("Opcao invalida!\n");
        }
    } while (1);
}

void visualizarProjetos() {
    int opcaoPesquisa;
    do {
        printf("\nMenu de Visualizacao de Projetos:\n");
        printf("1. Exibir Lista Completa\n2. Pesquisar Projeto\n3. Voltar\nEscolha uma opcao: ");
        scanf("%d", &opcaoPesquisa);

        if (opcaoPesquisa == 1) {
            printf("\nLista de Projetos:\n");
            printf("%-5s %-20s %-10s %-15s %-20s\n", "ID", "Descricao", "Valor", "Data de Entrega", "Cliente");
            for (int i = 0; i < totalProjetos; i++) {
                int clienteID = projetos[i].clienteID;
                printf("%-5d %-20s R$%-10.2f %-15s %-20s\n",
                       projetos[i].id,
                       projetos[i].descricao,
                       projetos[i].valor,
                       projetos[i].dataEntrega,
                       clientes[clienteID - 1].nome);
            }
        } else if (opcaoPesquisa == 2) {
            menuPesquisaProjetos();
        } else if (opcaoPesquisa == 3) {
            return;
        } else {
            printf("Opcao invalida!\n");
        }
    } while (1);
}

void menuPesquisaProjetos() {
    int opcaoPesquisa;
    do {
        printf("\nMenu de Pesquisa de Projetos:\n");
        printf("1. Pesquisar por ID\n2. Pesquisar por Descricao\n3. Voltar\nEscolha uma opcao: ");
        scanf("%d", &opcaoPesquisa);

        if (opcaoPesquisa == 1) {
            int id;
            printf("Digite o ID do Projeto: ");
            id = lerInteiro();
            pesquisarProjetoPorID(id);
        } else if (opcaoPesquisa == 2) {
            char descricao[100];
            printf("Digite a descricao do Projeto: ");
            scanf(" %99[^\n]", descricao);
            pesquisarProjetoPorDescricao(descricao);
        } else if (opcaoPesquisa == 3) {
            break;
        } else {
            printf("Opcao invalida!\n");
        }
    } while (1);
}

void pesquisarProjetoPorID(int id) {
    int encontrado = 0;
    for (int i = 0; i < totalProjetos; i++) {
        if (projetos[i].id == id) {
            printf("%-5d %-20s R$%-10.2f %-15s\n",
                   projetos[i].id,
                   projetos[i].descricao,
                   projetos[i].valor,
                   projetos[i].dataEntrega);
            encontrado = 1;
            break;
        }
    }
    if (!encontrado) {
        printf("Projeto nao encontrado.\n");
    }
}

void pesquisarProjetoPorDescricao(char* descricao) {
    int encontrado = 0;
    for (int i = 0; i < totalProjetos; i++) {
        if (strstr(projetos[i].descricao, descricao) != NULL) {
            printf("%-5d %-20s R$%-10.2f %-15s\n",
                   projetos[i].id,
                   projetos[i].descricao,
                   projetos[i].valor,
                   projetos[i].dataEntrega);
            encontrado = 1;
        }
    }
    if (!encontrado) {
        printf("Projeto nao encontrado.\n");
    }
}

void menuEstoque() {
    int opcao;
    do {
        printf("\nMenu de Estoque:\n");
        printf("1. Adicionar Produto\n");
        printf("2. Consultar Estoque\n");
        printf("3. Remover Produto\n");
        printf("4. Atualizar Quantidade\n");
        printf("5. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: adicionarProduto(); break;
            case 2: consultarEstoque(); break;
            case 3: removerProduto(); break;
            case 4: atualizarQuantidade(); break;
            case 5: break;
            default: printf("Opcao invalida!\n");
        }
    } while (opcao != 5);
}

void adicionarProduto() {
    if (totalEstoque >= MAX_ESTOQUE) {
        printf("Erro: Limite de estoque atingido.\n");
        return;
    }

    printf("\nCadastro de Produto\n");
    estoque[totalEstoque].id = totalEstoque + 1;
    printf("Nome do Produto: ");
    scanf(" %49[^\n]", estoque[totalEstoque].nome);
    printf("Quantidade: ");
    scanf("%d", &estoque[totalEstoque].quantidade);
    printf("Preco unitario: ");
    scanf("%lf", &estoque[totalEstoque].preco);
    printf("ID: %d\n", estoque[totalEstoque].id);
    printf("Produto cadastrado com sucesso! \n");

    totalEstoque++;
}

void consultarEstoque() {
    printf("\nLista de Produtos no Estoque:\n");
    for (int i = 0; i < totalEstoque; i++) {
        printf("ID: %-3d Nome: %-20s Quantidade: %-3d Preco: R$%.2f\n",
               estoque[i].id,
               estoque[i].nome,
               estoque[i].quantidade,
               estoque[i].preco);
    }
}

void removerProduto() {
    int id;
    printf("Digite o ID do produto a ser removido: ");
    id = lerInteiro();

    int index = -1;
    for (int i = 0; i < totalEstoque; i++) {
        if (estoque[i].id == id) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        for (int i = index; i < totalEstoque - 1; i++) {
            estoque[i] = estoque[i + 1];
        }
        totalEstoque--;
        printf("Produto removido com sucesso!\n");
    } else {
        printf("Produto nao encontrado.\n");
    }
}

void atualizarQuantidade() {
    int id, novaQuantidade;
    printf("Digite o ID do produto para atualizar a quantidade: ");
    id = lerInteiro();

    int encontrado = 0;
    for (int i = 0; i < totalEstoque; i++) {
        if (estoque[i].id == id) {
            printf("Digite a nova quantidade para o produto %s: ", estoque[i].nome);
            scanf("%d", &novaQuantidade);
            estoque[i].quantidade = novaQuantidade;
            printf("Quantidade atualizada com sucesso!\n");
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Produto nao encontrado.\n");
    }
}
