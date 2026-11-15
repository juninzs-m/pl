#include <stdio.h>
#include "sqlite3.h"

int main() {
    sqlite3 *db;
    char *erro = NULL;

    if (sqlite3_open("agendamentos.db", &db) != SQLITE_OK) {
        printf("Erro ao abrir o banco: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    const char *sql =
        "PRAGMA foreign_keys = OFF;"
        "DELETE FROM agendamentos;"
        "DELETE FROM animais;"
        "DELETE FROM tutores;"
        "DELETE FROM sqlite_sequence "
        "WHERE name IN ('agendamentos', 'animais', 'tutores');"
        "PRAGMA foreign_keys = ON;";

    if (sqlite3_exec(db, sql, NULL, NULL, &erro) != SQLITE_OK) {
        printf("Erro ao limpar dados: %s\n", erro);
        sqlite3_free(erro);
        sqlite3_close(db);
        return 1;
    }

    printf("\nDados de teste apagados com sucesso!\n");
    printf("As informações foram apagagadas com sucesso!\n");

    sqlite3_close(db);
    return 0;
}