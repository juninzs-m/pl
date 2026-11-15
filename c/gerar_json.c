/*
 * gerar_json.c
 * Instituto Animal Life - pagina inicial
 *
 * Programa em C que gera o arquivo data/dados.json (rode a partir da pasta raiz do site) (servicos e depoimentos)
 * usado pelo script.js para montar a pagina inicial.
 *
 * Compilar:  gcc gerar_json.c -o gerar_json
 * Executar:  ./gerar_json        (no Windows: gerar_json.exe)
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    const char *icone;
    const char *titulo;
    const char *descricao;
} Servico;

typedef struct {
    const char *iniciais;
    const char *nome;
    const char *pet;
    const char *texto;
} Depoimento;

static const Servico servicos[] = {
    {"🩺", "Consultas",    "Avaliação completa com nossos veterinários."},
    {"💉", "Vacinação",    "Calendário de vacinas para filhotes, adultos e idosos."},
    {"🔬", "Exames",       "Exames laboratoriais e de imagem."},
    {"🏥", "Cirurgias",    "Centro cirúrgico e acompanhamento no pós-operatório."},
    {"✂️", "Castração",    "Procedimento seguro, com orientação antes e depois."},
    {"🛁", "Banho e tosa", "Higiene e bem-estar para o seu pet."}
};

static const Depoimento depoimentos[] = {
    {"AC", "Ana Costa",      "Tutora da Luna",
     "A equipe foi atenciosa do começo ao fim. A Luna fez a cirurgia e eu recebia notícias a cada etapa."},
    {"RA", "Roberto Alves",  "Tutor do Bob",
     "Cheguei de madrugada com o Bob passando mal e fomos atendidos na hora. Muito agradecido."},
    {"FL", "Fernanda Lima",  "Tutora da Nala",
     "Marcar consulta é simples e a Nala nem estressa mais. Os veterinários têm paciência com gatos."}
};

int main(void) {
    const int qtdServicos = sizeof(servicos) / sizeof(servicos[0]);
    const int qtdDepoimentos = sizeof(depoimentos) / sizeof(depoimentos[0]);
    int i;

    FILE *arq = fopen("data/dados.json", "w");
    if (arq == NULL) {
        printf("Erro: nao foi possivel criar data/dados.json\n");
        return EXIT_FAILURE;
    }

    fprintf(arq, "{\n  \"servicos\": [\n");
    for (i = 0; i < qtdServicos; i++) {
        fprintf(arq,
                "    {\"icone\": \"%s\", \"titulo\": \"%s\", \"descricao\": \"%s\"}%s\n",
                servicos[i].icone, servicos[i].titulo, servicos[i].descricao,
                (i < qtdServicos - 1) ? "," : "");
    }

    fprintf(arq, "  ],\n  \"depoimentos\": [\n");
    for (i = 0; i < qtdDepoimentos; i++) {
        fprintf(arq,
                "    {\"iniciais\": \"%s\", \"nome\": \"%s\", \"pet\": \"%s\", \"texto\": \"%s\"}%s\n",
                depoimentos[i].iniciais, depoimentos[i].nome, depoimentos[i].pet,
                depoimentos[i].texto,
                (i < qtdDepoimentos - 1) ? "," : "");
    }
    fprintf(arq, "  ]\n}\n");

    fclose(arq);
    printf("data/dados.json gerado: %d servicos e %d depoimentos.\n", qtdServicos, qtdDepoimentos);
    return EXIT_SUCCESS;
}
