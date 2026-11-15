
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "sqlite3.h"

sqlite3 *db;

/* Leitura de textos */
void ler_texto(const char *mensagem, char *destino, int tamanho) {
    printf("%s", mensagem);

    if (fgets(destino, tamanho, stdin) != NULL) {
        destino[strcspn(destino, "\n")] = '\0';

        if (strchr(destino, '\0') != NULL) {
            /* Mantém a entrada dentro do tamanho permitido */
        }
    }
}

/* Leitura de números */
int ler_numero(const char *mensagem) {
    char entrada[50];
    char *fim;
    long numero;

    while (1) {
        ler_texto(mensagem, entrada, sizeof(entrada));

        numero = strtol(entrada, &fim, 10);

        if (entrada[0] != '\0' && *fim == '\0' &&
            numero >= 0 && numero <= INT_MAX) {
            return (int)numero;
        }

        printf("Digite um numero valido.\n");
    }
}

/* Verifica se uma coluna existe em uma tabela */
int coluna_existe(const char *tabela, const char *coluna) {
    sqlite3_stmt *stmt;
    char sql[200];
    int existe = 0;

    snprintf(sql, sizeof(sql), "PRAGMA table_info(%s);", tabela);

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return 0;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *nome = sqlite3_column_text(stmt, 1);

        if (nome != NULL && strcmp((const char *)nome, coluna) == 0) {
            existe = 1;
            break;
        }
    }

    sqlite3_finalize(stmt);
    return existe;
}

/* Criação das tabelas */
int criar_tabelas() {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS tutores ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nome TEXT NOT NULL,"
        "telefone TEXT NOT NULL,"
        "email TEXT,"
        "endereco TEXT"
        ");"

        "CREATE TABLE IF NOT EXISTS animais ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nome TEXT NOT NULL,"
        "especie TEXT NOT NULL CHECK(especie IN ('Gato', 'Cachorro')),"
        "raca TEXT,"
        "sexo TEXT,"
        "data_nascimento TEXT,"
        "tutor_id INTEGER NOT NULL,"
        "FOREIGN KEY(tutor_id) REFERENCES tutores(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS agendamentos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "animal_id INTEGER NOT NULL,"
        "servico TEXT NOT NULL,"
        "data TEXT NOT NULL,"
        "horario TEXT NOT NULL,"
        "observacoes TEXT,"
        "status TEXT DEFAULT 'Pendente',"
        "FOREIGN KEY(animal_id) REFERENCES animais(id)"
        ");";

    char *erro = NULL;

    if (sqlite3_exec(db, sql, NULL, NULL, &erro) != SQLITE_OK) {
        printf("Erro ao criar tabelas: %s\n", erro);
        sqlite3_free(erro);
        return 0;
    }

    /* Compatibilidade com banco antigo sem a coluna endereco */
    if (!coluna_existe("tutores", "endereco")) {
        if (sqlite3_exec(db,
            "ALTER TABLE tutores ADD COLUMN endereco TEXT;",
            NULL, NULL, &erro) != SQLITE_OK) {
            printf("Erro ao atualizar tabela tutores: %s\n", erro);
            sqlite3_free(erro);
            return 0;
        }
    }

    return 1;
}

/* Cadastro de tutor */
void cadastrar_tutor() {
    sqlite3_stmt *stmt;

    char nome[100];
    char telefone[30];
    char email[100];
    char endereco[200];

    ler_texto("Nome do tutor: ", nome, sizeof(nome));
    ler_texto("Telefone: ", telefone, sizeof(telefone));
    ler_texto("E-mail: ", email, sizeof(email));
    ler_texto("Endereco: ", endereco, sizeof(endereco));

    const char *sql =
        "INSERT INTO tutores (nome, telefone, email, endereco) "
        "VALUES (?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar cadastro.\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, telefone, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, endereco, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nTutor cadastrado com sucesso!\n");
        printf("ID do tutor: %d\n", (int)sqlite3_last_insert_rowid(db));
    } else {
        printf("Erro ao cadastrar tutor: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

/* Listagem de tutores */
void listar_tutores() {
    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT id, nome, telefone, email, endereco FROM tutores;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar tutores.\n");
        return;
    }

    printf("\n========== TUTORES CADASTRADOS ==========\n");

    int encontrou = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrou = 1;

        printf("\nID: %d\n", sqlite3_column_int(stmt, 0));
        printf("Nome: %s\n", sqlite3_column_text(stmt, 1));
        printf("Telefone: %s\n", sqlite3_column_text(stmt, 2));
        printf("E-mail: %s\n",
            sqlite3_column_text(stmt, 3) ?
            (const char *)sqlite3_column_text(stmt, 3) : "");
        printf("Endereco: %s\n",
            sqlite3_column_text(stmt, 4) ?
            (const char *)sqlite3_column_text(stmt, 4) : "");
    }

    if (!encontrou) {
        printf("Nenhum tutor cadastrado.\n");
    }

    sqlite3_finalize(stmt);
}

/* Cadastro de animal */
void cadastrar_animal() {
    sqlite3_stmt *stmt;

    char nome[100];
    char especie[20];
    char raca[80];
    char sexo[20];
    char nascimento[20];
    int tutor_id;

    listar_tutores();

    tutor_id = ler_numero("\nDigite o ID do tutor responsavel: ");

    ler_texto("Nome do animal: ", nome, sizeof(nome));

    printf("\nEspecie:\n");
    printf("1 - Cachorro\n");
    printf("2 - Gato\n");

    int opcao = ler_numero("Escolha: ");

    if (opcao == 1) {
        strcpy(especie, "Cachorro");
    } else if (opcao == 2) {
        strcpy(especie, "Gato");
    } else {
        printf("Opcao de especie invalida.\n");
        return;
    }

    ler_texto("Raca: ", raca, sizeof(raca));
    ler_texto("Sexo: ", sexo, sizeof(sexo));
    ler_texto("Data de nascimento (DD/MM/AAAA): ",
              nascimento, sizeof(nascimento));

    const char *sql =
        "INSERT INTO animais "
        "(nome, especie, raca, sexo, data_nascimento, tutor_id) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar cadastro do animal.\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, especie, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, raca, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, sexo, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, nascimento, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, tutor_id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nAnimal cadastrado com sucesso!\n");
        printf("ID do animal: %d\n", (int)sqlite3_last_insert_rowid(db));
    } else {
        printf("Erro ao cadastrar animal: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

/* Listagem de animais */
void listar_animais() {
    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT animais.id, animais.nome, animais.especie, "
        "animais.raca, animais.sexo, animais.data_nascimento, "
        "tutores.nome "
        "FROM animais "
        "INNER JOIN tutores ON animais.tutor_id = tutores.id;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar animais.\n");
        return;
    }

    printf("\n========== ANIMAIS CADASTRADOS ==========\n");

    int encontrou = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrou = 1;

        printf("\nID: %d\n", sqlite3_column_int(stmt, 0));
        printf("Nome: %s\n", sqlite3_column_text(stmt, 1));
        printf("Especie: %s\n", sqlite3_column_text(stmt, 2));
        printf("Raca: %s\n", sqlite3_column_text(stmt, 3));
        printf("Sexo: %s\n", sqlite3_column_text(stmt, 4));
        printf("Nascimento: %s\n", sqlite3_column_text(stmt, 5));
        printf("Tutor: %s\n", sqlite3_column_text(stmt, 6));
    }

    if (!encontrou) {
        printf("Nenhum animal cadastrado.\n");
    }

    sqlite3_finalize(stmt);
}

/* Cadastro de agendamento */
void cadastrar_agendamento() {
    sqlite3_stmt *stmt;

    int animal_id;
    char servico[100];
    char data[20];
    char horario[20];
    char observacoes[300];

    listar_animais();

    animal_id = ler_numero("\nDigite o ID do animal: ");

    ler_texto("Servico desejado: ", servico, sizeof(servico));
    ler_texto("Data (DD/MM/AAAA): ", data, sizeof(data));
    ler_texto("Horario (HH:MM): ", horario, sizeof(horario));
    ler_texto("Observacoes: ", observacoes, sizeof(observacoes));

    const char *sql =
        "INSERT INTO agendamentos "
        "(animal_id, servico, data, horario, observacoes) "
        "VALUES (?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar agendamento.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, animal_id);
    sqlite3_bind_text(stmt, 2, servico, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, data, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, horario, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, observacoes, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nAgendamento realizado com sucesso!\n");
        printf("Numero do agendamento: %d\n",
               (int)sqlite3_last_insert_rowid(db));
    } else {
        printf("Erro ao realizar agendamento: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

/* Listagem de agendamentos */
void listar_agendamentos() {
    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT agendamentos.id, animais.nome, tutores.nome, "
        "agendamentos.servico, agendamentos.data, "
        "agendamentos.horario, agendamentos.observacoes, "
        "agendamentos.status "
        "FROM agendamentos "
        "INNER JOIN animais ON agendamentos.animal_id = animais.id "
        "INNER JOIN tutores ON animais.tutor_id = tutores.id "
        "ORDER BY agendamentos.id DESC;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar agendamentos: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("\n========== AGENDAMENTOS ==========\n");

    int encontrou = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrou = 1;

        printf("\nNumero: %d\n", sqlite3_column_int(stmt, 0));
        printf("Animal: %s\n", sqlite3_column_text(stmt, 1));
        printf("Tutor: %s\n", sqlite3_column_text(stmt, 2));
        printf("Servico: %s\n", sqlite3_column_text(stmt, 3));
        printf("Data: %s\n", sqlite3_column_text(stmt, 4));
        printf("Horario: %s\n", sqlite3_column_text(stmt, 5));

        printf("Observacoes: %s\n",
            sqlite3_column_text(stmt, 6) ?
            (const char *)sqlite3_column_text(stmt, 6) : "");

        printf("Status: %s\n", sqlite3_column_text(stmt, 7));
        printf("-----------------------------------\n");
    }

    if (!encontrou) {
        printf("Nenhum agendamento cadastrado.\n");
    }

    sqlite3_finalize(stmt);
}

/* Menu principal */
int main() {
    int opcao;

    if (sqlite3_open("agendamentos.db", &db) != SQLITE_OK) {
        printf("Erro ao abrir banco de dados: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);

    if (!criar_tabelas()) {
        sqlite3_close(db);
        return 1;
    }

    printf("====================================\n");
    printf("     INSTITUTO ANIMAL LIFE\n");
    printf("     SISTEMA DE AGENDAMENTO\n");
    printf("====================================\n");

    do {
        printf("\n============== MENU ==============\n");
        printf("1 - Cadastrar tutor\n");
        printf("2 - Listar tutores\n");
        printf("3 - Cadastrar animal\n");
        printf("4 - Listar animais\n");
        printf("5 - Realizar agendamento\n");
        printf("6 - Listar agendamentos\n");
        printf("0 - Sair\n");
        printf("==================================\n");

        opcao = ler_numero("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrar_tutor();
                break;

            case 2:
                listar_tutores();
                break;

            case 3:
                cadastrar_animal();
                break;

            case 4:
                listar_animais();
                break;

            case 5:
                cadastrar_agendamento();
                break;

            case 6:
                listar_agendamentos();
                break;

            case 0:
                printf("\nEncerrando o sistema...\n");
                break;

            default:
                printf("\nOpcao invalida. Tente novamente.\n");
        }

    } while (opcao != 0);

    sqlite3_close(db);

    return 0;
}