//Bibliotecas que executam funçoes,strings ,localidade e tempo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>

// define o maximo de clientes,projetos e itens

#define MAX_CLIENTES 100
#define MAX_PROJETOS 100
#define MAX_ESTOQUE 100

// tres estruturas ,clientes,projeto e estoque ,todas com as respectivas informaçoes que seram coletadas

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

//arrays  com variaveis  para cada funçao

Cliente clientes[MAX_CLIENTES];
Projeto projetos[MAX_PROJETOS];
Estoque estoque[MAX_ESTOQUE];

int totalClientes = 0;
int totalProjetos = 0;
int totalEstoque = 0;

// declara tosas as funçoes que vai ser utilizado no codigo

void cadastrarCliente();
void carregarClientes();
void cadastrarProjeto();
void excluirProjeto();
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
int idsDisponiveis[MAX_ESTOQUE];
 int totalIdsDisponiveis = 0;

//menu inicial onde o usuario pode escolher as funçoes que serao utilizadas

int main() {
    setlocale(LC_ALL, "portuguese");
    carregarClientes();
    int opcao;
    do {
        printf("\nMenu Principal:\n");
        printf("1. Cadastro\n");
        printf("2. Visualização\n");
        printf("3. Estoque\n");
        printf("4. Sair\n");
        printf("Escolha uma opção: ");
        opcao = lerInteiro();
        system("cls");
        switch (opcao) {
            case 1: menuCadastro(); break;
            case 2: menuVisualizacao(); break;
            case 3: menuEstoque(); break;
            case 4: printf("Saindo do programa...\n"); break;
            default: printf("Opção inválida!\n");
        }
    } while (opcao != 4);
    system("cls");
    return 0;
}

// carrega os dados do arquivo cliente prenchendo com as informaçoes cadastradas
void carregarClientes() {
    FILE *file = fopen("clientes.txt", "r");
    if (file == NULL) {
        return;
    }

    while (fscanf(file, "ID: %d\n", &clientes[totalClientes].id) != EOF) {
        fscanf(file, "Nome: %[^\n]\n", clientes[totalClientes].nome);
        fscanf(file, "Email: %[^\n]\n", clientes[totalClientes].email);
        fscanf(file, "Celular: %[^\n]\n\n", clientes[totalClientes].celular);
        totalClientes++;
    }

    fclose(file);
}

//le se o usuario informou um numero valido.

int lerInteiro() {
    int valor;
    while (scanf("%d", &valor) != 1) {
        printf("Erro: Por favor, insira um valor válido.\n");
        while (getchar() != '\n');
    }
    return valor;
}

//verifica se  os caracteres informados sao validos

int validarData(char *data) {
    if (strlen(data) != 8 || strspn(data, "0123456789") != 8) {
        printf("Erro: Formato de data inválido. Use ddmmaaaa.\n");
        return 0;
    }

    //converte cada caracter em inteiros para os valores de dia mes e ano

    int dia = (data[0] - '0') * 10 + (data[1] - '0');
    int mes = (data[2] - '0') * 10 + (data[3] - '0');
    int ano = (data[4] - '0') * 1000 + (data[5] - '0') * 100 + (data[6] - '0') * 10 + (data[7] - '0');

//verifica se dias , meses e anos estao validos ,se nao estiver retorna erro

    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano < 1900 || ano > 2100) {
        printf("Erro: Data inválida. Verifique os valores de dia, mês ou ano.\n");
        return 0;
    }

    // verifica a quantidade de dias nos meses e ajustando para anos bissextos e da erro se tentar colocar mais que o limite

    int diasNoMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0)) {
        diasNoMes[1] = 29;
    }

    if (dia > diasNoMes[mes - 1]) {
        printf("Erro: O mês %d possui no máximo %d dias.\n", mes, diasNoMes[mes - 1]);
        return 0;
    }

    //obtem a data atual do sistema  e incrementa um dia

    time_t t = time(NULL);
    struct tm tmAtual = *localtime(&t);

    int anoAtual = tmAtual.tm_year + 1900;
    int mesAtual = tmAtual.tm_mon + 1;
    int diaAtual = tmAtual.tm_mday;

    tmAtual.tm_mday++;
    mktime(&tmAtual);

    //verifica se a data anterior e a do mesmo dia ou anteriores

    if (ano < anoAtual || (ano == anoAtual && mes < mesAtual) || (ano == anoAtual && mes == mesAtual && dia <= diaAtual)) {
        printf("Erro: A data não pode ser anterior ou igual à data atual.\n");
        return 0;
    }

// verifica a data aual para sete anos no futuro

    tmAtual.tm_year += 7;
    mktime(&tmAtual);

    if (ano > tmAtual.tm_year + 1900 ||
        (ano == tmAtual.tm_year + 1900 && mes > tmAtual.tm_mon + 1) ||
        (ano == tmAtual.tm_year + 1900 && mes == tmAtual.tm_mon + 1 && dia > tmAtual.tm_mday)) {
        printf("Erro: A data não pode ser superior a 7 anos após a data atual.\n");
        return 0;
    }

    //se tudo for bem sucedido ira retornar 1

    return 1;
}

//salva os dados dos clientes no arquivo clientes.txt.

void salvarClientesOrdenados() {
    FILE *file = fopen("clientes.txt", "w");
    if (file == NULL) {
        printf("Erro: Não foi possível abrir o arquivo.\n");
        return;
    }

    for (int i = 0; i < totalClientes; i++) {
        fprintf(file, "ID: %d\n", clientes[i].id);
        fprintf(file, "Nome: %s\n", clientes[i].nome);
        fprintf(file, "Email: %s\n", clientes[i].email);
        fprintf(file, "Celular: %s\n\n", clientes[i].celular);
    }

    fclose(file);
}

//Esta função ordena os clientes por ID usando o algoritmo de ordenação por inserção

void ordenarClientesPorID() {
    for (int i = 1; i < totalClientes; i++) {
        Cliente temp = clientes[i];
        int j = i - 1;
        while (j >= 0 && clientes[j].id > temp.id) {
            clientes[j + 1] = clientes[j];
            j--;
        }
        clientes[j + 1] = temp;
    }
}

//Esta função cadastra um novo cliente no sistema.

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
             sleep(2);
    system("cls");
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
    ordenarClientesPorID();
    salvarClientesOrdenados();

    printf("Cliente cadastrado com sucesso e salvo no arquivo!\n");
    sleep(2);
    system("cls");
}


//remove um cliente do sistema com base no ID.

void excluirCliente() {
    if (totalClientes == 0) {
        printf("Erro: Não há clientes cadastrados.\n");
          sleep(2);
    system("cls");
        return;
    }

    int id;
    printf("Digite o ID do cliente a ser excluído: ");
    id = lerInteiro();

    int encontrado = 0;
    for (int i = 0; i < totalClientes; i++) {
        if (clientes[i].id == id) {
            encontrado = 1;
            for (int j = i; j < totalClientes - 1; j++) {
                clientes[j] = clientes[j + 1];
            }
            totalClientes--;
            break;
        }
    }

    if (!encontrado) {
        printf("Erro: Cliente com ID %d não encontrado.\n", id);
           sleep(2);
    system("cls");
        return;

    }

    // Atualiza o arquivo "clientes.txt"
    
    FILE *file = fopen("clientes_temp.txt", "w");
    if (file == NULL) {
        printf("Erro: Não foi possível abrir o arquivo.\n");
        return;
    }

    for (int i = 0; i < totalClientes; i++) {
        fprintf(file, "ID: %d\n", clientes[i].id);
        fprintf(file, "Nome: %s\n", clientes[i].nome);
        fprintf(file, "Email: %s\n", clientes[i].email);
        fprintf(file, "Celular: %s\n\n", clientes[i].celular);
    }

    fclose(file);
    remove("clientes.txt");
    rename("clientes_temp.txt", "clientes.txt");

    printf("Cliente excluído com sucesso!\n");
sleep(2);
system("cls");
}

//para cadastrar um novo projeto associado a um cliente.

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
            sleep(2);
    system("cls");
        return;
    }

    projetos[totalProjetos].id = id;
    projetos[totalProjetos].clienteID = clienteID;
    totalProjetos++;
    printf("Projeto cadastrado com sucesso!\n");
    sleep(2);
    system("cls");
}


//um menu para o usuário escolher entre várias opções relacionadas ao cadastro de clientes e projetos.

void menuCadastro() {
    int opcao;
    do {
        printf("\nMenu de Cadastro:\n");
        printf("1. Cadastrar Cliente\n");
        printf("2. Cadastrar Projeto\n");
        printf("3. Excluir Cliente\n");
        printf("4. Excluir Projeto\n");
        printf("5. Voltar\n");
        printf("Escolha uma opção: ");
        opcao = lerInteiro();
system("cls");

        switch (opcao) {
            case 1: cadastrarCliente(); break;
            case 2: cadastrarProjeto(); break;
            case 3: excluirCliente(); break;
            case 4: excluirProjeto(); break; //
            case 5: break;
            default: printf("Opção inválida!\n");
        }
    } while (opcao != 5);
}

//verifica se existe projetos cadastrados e pede o id do projeto a ser excluido e exclui se existir

void excluirProjeto() {
    if (totalProjetos == 0) {
        printf("Erro: Não há projetos cadastrados.\n");
            sleep(2);
    system("cls");
        return;
    }

    int id;
    printf("Digite o ID do projeto a ser excluído: ");
    id = lerInteiro();

    int encontrado = 0;
    for (int i = 0; i < totalProjetos; i++) {
        if (projetos[i].id == id) {
            encontrado = 1;
            for (int j = i; j < totalProjetos - 1; j++) {
                projetos[j] = projetos[j + 1];
            }
            totalProjetos--;
            printf("Projeto excluído com sucesso!\n");
                sleep(2);
    system("cls");
            break;
        }
    }

    if (!encontrado) {
        printf("Erro: Projeto com ID %d não encontrado.\n", id);
            sleep(2);
    system("cls");
    }
}

//um menu para o usuário escolher entre várias opções de visualização de clientes e projetos.

void menuVisualizacao() {
    int opcao;
    do {
        printf("\nMenu de Visualizacao:\n");
        printf("1. Visualizar Clientes\n");
        printf("2. Visualizar Projetos\n");
        printf("3. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
    system("cls");

        switch (opcao) {
            case 1: visualizarClientes(); break;
            case 2: visualizarProjetos(); break;
            case 3: break;
            default: printf("Opcao invalida!\n");
        }
    } while (opcao != 3);
}

//um menu para visualizar clientes, oferecendo opções para exibir a lista completa ou pesquisar um cliente específico.



void visualizarClientes() {
    int opcaoPesquisa;
    do {
        printf("\nMenu de Visualizacao de Clientes:\n");
        printf("1. Exibir Lista Completa\n2. Pesquisar Cliente\n3. Voltar\nEscolha uma opcao: ");
        scanf("%d", &opcaoPesquisa);
    system("cls");

        if (opcaoPesquisa == 1) {
            FILE *file = fopen("clientes.txt", "r");
            if (file == NULL) {
                printf("Erro: Não foi possível abrir o arquivo.\n");
                return;
            }
            Cliente cliente;
            printf("\nLista de Clientes:\n");
            printf("%-5s %-20s %-30s %-15s\n", "ID", "Nome", "Email", "Celular");
            while (fscanf(file, "ID: %d\nNome: %49[^\n]\nEmail: %49[^\n]\nCelular: %19[^\n]\n\n",
                          &cliente.id, cliente.nome, cliente.email, cliente.celular) != EOF) {
                printf("%-5d %-20s %-30s %-15s\n",
                       cliente.id,
                       cliente.nome,
                       cliente.email,
                       cliente.celular);
            }
            fclose(file);
        } else if (opcaoPesquisa == 2) {
            menuPesquisaClientes();
        } else if (opcaoPesquisa == 3) {
            return;
        } else {
            printf("Opcao invalida!\n");
        }
    } while (1);
    system("cls");
}

//menu de opções para o usuário pesquisar clientes por diferentes critérios. Dependendo da escolha do usuário, a função busca as informações no arquivo clientes.txt.

void menuPesquisaClientes() {
    int opcaoPesquisa;
    do {
        printf("\nMenu de Pesquisa de Clientes:\n");
        printf("1. Pesquisar por ID\n2. Pesquisar por Nome\n3. Pesquisar por Email\n4. Pesquisar por Celular\n5. Voltar\nEscolha uma opcao: ");
        scanf("%d", &opcaoPesquisa);
    system("cls");

        FILE *file = fopen("clientes.txt", "r");
        if (file == NULL) {
            printf("Erro: Não foi possível abrir o arquivo.\n");
            return;
        }

        Cliente cliente;
        int encontrado = 0;

        if (opcaoPesquisa == 1) {
            int id;
            printf("Digite o ID do Cliente: ");
            id = lerInteiro();
            while (fscanf(file, "ID: %d\nNome: %49[^\n]\nEmail: %49[^\n]\nCelular: %19[^\n]\n\n",
                          &cliente.id, cliente.nome, cliente.email, cliente.celular) != EOF) {
                if (cliente.id == id) {
                    printf("%-5d %-20s %-30s %-15s\n", cliente.id, cliente.nome, cliente.email, cliente.celular);
                    encontrado = 1;

                    break;
                }
            }
            if (!encontrado) {
                printf("Cliente nao encontrado.\n");
                 sleep(2);
    system("cls");
            }
        } else if (opcaoPesquisa == 2) {
            char nome[50];
            printf("Digite o nome do Cliente: ");
            scanf(" %49[^\n]", nome);

            while (fscanf(file, "ID: %d\nNome: %49[^\n]\nEmail: %49[^\n]\nCelular: %19[^\n]\n\n",
                          &cliente.id, cliente.nome, cliente.email, cliente.celular) != EOF) {
                if (strstr(cliente.nome, nome) != NULL) {
                    printf("%-5d %-20s %-30s %-15s\n", cliente.id, cliente.nome, cliente.email, cliente.celular);
                    encontrado = 1;
                }
            }
            if (!encontrado) {
                printf("Cliente nao encontrado.\n");
                 sleep(2);
    system("cls");
            }
        } else if (opcaoPesquisa == 3) {
            char email[50];
            printf("Digite o email do Cliente: ");
            scanf(" %49[^\n]", email);
            while (fscanf(file, "ID: %d\nNome: %49[^\n]\nEmail: %49[^\n]\nCelular: %19[^\n]\n\n",
                          &cliente.id, cliente.nome, cliente.email, cliente.celular) != EOF) {
                if (strstr(cliente.email, email) != NULL) {
                    printf("%-5d %-20s %-30s %-15s\n", cliente.id, cliente.nome, cliente.email, cliente.celular);
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
            while (fscanf(file, "ID: %d\nNome: %49[^\n]\nEmail: %49[^\n]\nCelular: %19[^\n]\n\n",
                          &cliente.id, cliente.nome, cliente.email, cliente.celular) != EOF) {
                if (strstr(cliente.celular, celular) != NULL) {
                    printf("%-5d %-20s %-30s %-15s\n", cliente.id, cliente.nome, cliente.email, cliente.celular);
                    encontrado = 1;
                }
            }
            if (!encontrado) {
                printf("Cliente nao encontrado.\n");
            }
        } else if (opcaoPesquisa == 5) {
            fclose(file);
            break;
        } else {
            printf("Opcao invalida!\n");
        }

        fclose(file);

    } while (1);
    system("cls");
}

//um menu para o usuário visualizar projetos e permite que o usuário exiba a lista completa de projetos ou pesquise um projeto específico.



void visualizarProjetos() {
    int opcaoPesquisa;
    do {
        printf("\nMenu de Visualizacao de Projetos:\n");
        printf("1. Exibir Lista Completa\n2. Pesquisar Projeto\n3. Voltar\nEscolha uma opcao: ");
        scanf("%d", &opcaoPesquisa);
    system("cls");

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
                     sleep(2);
    system("cls");
        }
    } while (1);

}

//um submenu para o usuário pesquisar projetos por ID ou Descrição.

void menuPesquisaProjetos() {
    int opcaoPesquisa;
    do {
        printf("\nMenu de Pesquisa de Projetos:\n");
        printf("1. Pesquisar por ID\n2. Pesquisar por Descricao\n3. Voltar\nEscolha uma opcao: ");
        scanf("%d", &opcaoPesquisa);
    system("cls");

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
                     sleep(2);
    system("cls");
        }
    } while (1);
}

//pesquisa um projeto pelo ID e exibe suas informações.

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
                 sleep(2);
    system("cls");
    }
}

//pesquisa projetos pela descrição e exibe suas informações.

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
    system("cls");
    }
}

//adiciona um ID disponível ao array de IDs disponíveis.



void adicionarIDDisponivel(int id) {
    if (totalIdsDisponiveis < MAX_ESTOQUE) {
        idsDisponiveis[totalIdsDisponiveis++] = id;
    }
}

//ordena o estoque por ID usando o algoritmo de ordenação por inserção

void ordenarEstoquePorID() {
    for (int i = 1; i < totalEstoque; i++) {
        Estoque temp = estoque[i];
        int j = i - 1;
        while (j >= 0 && estoque[j].id > temp.id) {
            estoque[j + 1] = estoque[j];
            j--;
        }
        estoque[j + 1] = temp;
    }
}

//salva os dados dos produtos no arquivo estoque.txt

void salvarEstoqueOrdenado() {
    FILE *file = fopen("estoque.txt", "w");
    if (file == NULL) {
        printf("Erro: Não foi possível abrir o arquivo.\n");
        return;
    }

    for (int i = 0; i < totalEstoque; i++) {
        fprintf(file, "ID: %d\n", estoque[i].id);
        fprintf(file, "Nome: %s\n", estoque[i].nome);
        fprintf(file, "Quantidade: %d\n", estoque[i].quantidade);
        fprintf(file, "Preço: %.2f\n\n", estoque[i].preco);
    }

    fclose(file);
}

//adiciona um novo produto ao estoque.

void adicionarProduto() {
    if (totalEstoque >= MAX_ESTOQUE) {
        printf("Erro: Limite de estoque atingido.\n");
        sleep(2);
        system("cls");
        return;
    }

    int id;
    if (totalIdsDisponiveis > 0) {
        id = idsDisponiveis[--totalIdsDisponiveis];
    } else {
        id = totalEstoque + 1;
    }

    printf("\nCadastro de Produto\n");
    estoque[totalEstoque].id = id;
    printf("Nome do Produto: ");
    scanf(" %49[^\n]", estoque[totalEstoque].nome);
    printf("Quantidade: ");
    scanf("%d", &estoque[totalEstoque].quantidade);
    printf("Preco unitario: ");
    scanf("%lf", &estoque[totalEstoque].preco);
    printf("ID: %d\n", estoque[totalEstoque].id);
    printf("Produto cadastrado com sucesso! \n");
    sleep(2);
    system("cls");

    totalEstoque++;
    ordenarEstoquePorID();
    salvarEstoqueOrdenado();
}

//exibe um menu para o gerenciamento do estoque.

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

//exibe a lista completa de produtos no estoque.

void consultarEstoque() {
    printf("\nConsulta de Estoque:\n");
    for (int i = 0; i < totalEstoque; i++) {
        printf("ID: %d\n", estoque[i].id);
        printf("Nome: %s\n", estoque[i].nome);
        printf("Quantidade: %d\n", estoque[i].quantidade);
        printf("Preço: %.2f\n\n", estoque[i].preco);
    }
}

//remove um produto do estoque com base no ID.

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

//atualiza a quantidade de um produto no estoque com base no ID.

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
