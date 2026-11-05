# include "contiki.h"
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include <time.h>

struct usuario_dados {
    char nome[50];
    long idade;
};

typedef enum {
    SENSOR_TEMPERATURA,
    SENSOR_SATURACAO,
    SENSOR_FREQUENCIA_CARDIACA
} TipoSensor;

typedef enum {
    ABAIXO_DO_LIMITE,
    ACIMA_DO_LIMITE,
    ZONA_DE_RISCO
} TipoAlteracao;

typedef struct {
    TipoSensor sensor;
    double valor;
    double limite_inferior;
    double limite_superior;
    TipoAlteracao alteracao;
} DadosAlerta;

struct usuario_dados usuario;

typedef struct {
    int fc_min, fc_max;
    int sat_normal, sat_baixa;
    double temp_febre, temp_hipo;
} Limites;

// Originalmente fiz com um .csv com dados reais
// essa tabela declarada é uma adaptação para o envio em um arquivo unico .c
//
// tabela[idade], idade de 0 a 120
// campos: frequencia_cardiaca_min, frequencia_cardiaca_max, saturacao_normal, saturacao_baixa, temp_febre, temp_hipo
static const Limites tabela[121] = { [0] = {100, 205, 95, 90, 38.0, 36.0}, [1] = {98, 140, 95, 90, 38.0, 35.0}, [2] = {98, 140, 95, 90, 38.0, 35.0}, [3] = {80, 120, 95, 90, 38.0, 35.0}, [4] = {80, 120, 95, 90, 38.0, 35.0}, [5] = {80, 120, 95, 90, 38.0, 35.0}, [6] = {75, 118, 95, 90, 38.0, 35.0}, [7] = {75, 118, 95, 90, 38.0, 35.0}, [8] = {75, 118, 95, 90, 38.0, 35.0}, [9] = {75, 118, 95, 90, 38.0, 35.0}, [10] = {75, 118, 95, 90, 38.0, 35.0}, [11] = {75, 118, 95, 90, 38.0, 35.0}, [12] = {60, 100, 95, 90, 38.0, 35.0}, [13] = {60, 100, 95, 90, 38.0, 35.0}, [14] = {60, 100, 95, 90, 38.0, 35.0}, [15] = {60, 100, 95, 90, 38.0, 35.0}, [16] = {60, 100, 95, 90, 38.0, 35.0}, [17] = {60, 100, 95, 90, 38.0, 35.0}, [18] = {60, 100, 95, 90, 38.0, 35.0}, [19] = {60, 100, 95, 90, 38.0, 35.0}, [20] = {60, 100, 95, 90, 38.0, 35.0}, [21] = {60, 100, 95, 90, 38.0, 35.0}, [22] = {60, 100, 95, 90, 38.0, 35.0}, [23] = {60, 100, 95, 90, 38.0, 35.0}, [24] = {60, 100, 95, 90, 38.0, 35.0}, [25] = {60, 100, 95, 90, 38.0, 35.0}, [26] = {60, 100, 95, 90, 38.0, 35.0}, [27] = {60, 100, 95, 90, 38.0, 35.0}, [28] = {60, 100, 95, 90, 38.0, 35.0}, [29] = {60, 100, 95, 90, 38.0, 35.0}, [30] = {60, 100, 95, 90, 38.0, 35.0}, [31] = {60, 100, 95, 90, 38.0, 35.0}, [32] = {60, 100, 95, 90, 38.0, 35.0}, [33] = {60, 100, 95, 90, 38.0, 35.0}, [34] = {60, 100, 95, 90, 38.0, 35.0}, [35] = {60, 100, 95, 90, 38.0, 35.0}, [36] = {60, 100, 95, 90, 38.0, 35.0}, [37] = {60, 100, 95, 90, 38.0, 35.0}, [38] = {60, 100, 95, 90, 38.0, 35.0}, [39] = {60, 100, 95, 90, 38.0, 35.0}, [40] = {60, 100, 95, 90, 38.0, 35.0}, [41] = {60, 100, 95, 90, 38.0, 35.0}, [42] = {60, 100, 95, 90, 38.0, 35.0}, [43] = {60, 100, 95, 90, 38.0, 35.0}, [44] = {60, 100, 95, 90, 38.0, 35.0}, [45] = {60, 100, 95, 90, 38.0, 35.0}, [46] = {60, 100, 95, 90, 38.0, 35.0}, [47] = {60, 100, 95, 90, 38.0, 35.0}, [48] = {60, 100, 95, 90, 38.0, 35.0}, [49] = {60, 100, 95, 90, 38.0, 35.0}, [50] = {60, 100, 95, 90, 38.0, 35.0}, [51] = {60, 100, 95, 90, 38.0, 35.0}, [52] = {60, 100, 95, 90, 38.0, 35.0}, [53] = {60, 100, 95, 90, 38.0, 35.0}, [54] = {60, 100, 95, 90, 38.0, 35.0}, [55] = {60, 100, 95, 90, 38.0, 35.0}, [56] = {60, 100, 95, 90, 38.0, 35.0}, [57] = {60, 100, 95, 90, 38.0, 35.0}, [58] = {60, 100, 95, 90, 38.0, 35.0}, [59] = {60, 100, 95, 90, 38.0, 35.0}, [60] = {60, 100, 95, 90, 38.0, 35.0}, [61] = {60, 100, 95, 90, 38.0, 35.0}, [62] = {60, 100, 95, 90, 38.0, 35.0}, [63] = {60, 100, 95, 90, 38.0, 35.0}, [64] = {60, 100, 95, 90, 38.0, 35.0}, [65] = {60, 100, 95, 90, 37.8, 35.0}, [66] = {60, 100, 95, 90, 37.8, 35.0}, [67] = {60, 100, 95, 90, 37.8, 35.0}, [68] = {60, 100, 95, 90, 37.8, 35.0}, [69] = {60, 100, 95, 90, 37.8, 35.0}, [70] = {60, 100, 94, 90, 37.8, 35.0}, [71] = {60, 100, 94, 90, 37.8, 35.0}, [72] = {60, 100, 94, 90, 37.8, 35.0}, [73] = {60, 100, 94, 90, 37.8, 35.0}, [74] = {60, 100, 94, 90, 37.8, 35.0}, [75] = {60, 100, 94, 90, 37.8, 35.0}, [76] = {60, 100, 94, 90, 37.8, 35.0}, [77] = {60, 100, 94, 90, 37.8, 35.0}, [78] = {60, 100, 94, 90, 37.8, 35.0}, [79] = {60, 100, 94, 90, 37.8, 35.0}, [80] = {60, 100, 94, 90, 37.8, 35.0}, [81] = {60, 100, 94, 90, 37.8, 35.0}, [82] = {60, 100, 94, 90, 37.8, 35.0}, [83] = {60, 100, 94, 90, 37.8, 35.0}, [84] = {60, 100, 94, 90, 37.8, 35.0}, [85] = {60, 100, 94, 90, 37.8, 35.0}, [86] = {60, 100, 94, 90, 37.8, 35.0}, [87] = {60, 100, 94, 90, 37.8, 35.0}, [88] = {60, 100, 94, 90, 37.8, 35.0}, [89] = {60, 100, 94, 90, 37.8, 35.0}, [90] = {60, 100, 94, 90, 37.8, 35.0}, [91] = {60, 100, 94, 90, 37.8, 35.0}, [92] = {60, 100, 94, 90, 37.8, 35.0}, [93] = {60, 100, 94, 90, 37.8, 35.0}, [94] = {60, 100, 94, 90, 37.8, 35.0}, [95] = {60, 100, 94, 90, 37.8, 35.0}, [96] = {60, 100, 94, 90, 37.8, 35.0}, [97] = {60, 100, 94, 90, 37.8, 35.0}, [98] = {60, 100, 94, 90, 37.8, 35.0}, [99] = {60, 100, 94, 90, 37.8, 35.0}, [100] = {60, 100, 94, 90, 37.8, 35.0}, [101] = {60, 100, 94, 90, 37.8, 35.0}, [102] = {60, 100, 94, 90, 37.8, 35.0}, [103] = {60, 100, 94, 90, 37.8, 35.0}, [104] = {60, 100, 94, 90, 37.8, 35.0}, [105] = {60, 100, 94, 90, 37.8, 35.0}, [106] = {60, 100, 94, 90, 37.8, 35.0}, [107] = {60, 100, 94, 90, 37.8, 35.0}, [108] = {60, 100, 94, 90, 37.8, 35.0}, [109] = {60, 100, 94, 90, 37.8, 35.0}, [110] = {60, 100, 94, 90, 37.8, 35.0}, [111] = {60, 100, 94, 90, 37.8, 35.0}, [112] = {60, 100, 94, 90, 37.8, 35.0}, [113] = {60, 100, 94, 90, 37.8, 35.0}, [114] = {60, 100, 94, 90, 37.8, 35.0}, [115] = {60, 100, 94, 90, 37.8, 35.0}, [116] = {60, 100, 94, 90, 37.8, 35.0}, [117] = {60, 100, 94, 90, 37.8, 35.0}, [118] = {60, 100, 94, 90, 37.8, 35.0}, [119] = {60, 100, 94, 90, 37.8, 35.0}, [120] = {60, 100, 94, 90, 37.8, 35.0} };

// Evento compartilhado entre os processos para alertar possíveis criticidades
static process_event_t EVENTO_INDICADORES_CRITICOS;

PROCESS(PTemperature, "Temperatura");
PROCESS(PBloodSaturation, "Saturação Sanguínea");
PROCESS(PHeartRate, "Frequência Cardíaca");
PROCESS(PAlterationAlert, "Alerta de criticidade");
PROCESS(PSmartWatch, "Smart Watch");

AUTOSTART_PROCESSES(&PSmartWatch);

PROCESS_THREAD(PSmartWatch, ev, data){

    char linha[50], *fim;
    bool cadastro_efetuado = false;

    PROCESS_BEGIN();

    srand(time(NULL));
    usuario.idade = -1;
    usuario.nome[0] = '\0';

    while (!cadastro_efetuado){
        if (usuario.nome[0] == '\0'){
            printf("Insira seu nome: ");
            if (!fgets(linha, sizeof(linha), stdin)) {
                printf("Erro ao inserir nome. Tente novamente!\n");
                continue;
            }
            linha[strcspn(linha, "\n")] = '\0';
            strncpy(usuario.nome, linha, sizeof(usuario.nome) - 1);
        }

        if (usuario.idade == -1){
            printf("Insira sua Idade: ");
            if (!fgets(linha, sizeof(linha), stdin)) {
                printf("Erro ao inserir idade. Tente novamente!\n");
                continue;
            }
            usuario.idade = strtol(linha, &fim, 10);
            if (fim == linha || (*fim != '\n' && *fim != '\0') || usuario.idade < 0 || usuario.idade > 120) {
                printf("Idade Inválida. Tente novamente!\n");
                usuario.idade = -1;
                continue;
            }
            cadastro_efetuado = true;
        }
    }
    printf("\n");

    EVENTO_INDICADORES_CRITICOS = process_alloc_event();

    process_start(&PAlterationAlert, NULL);
    process_start(&PTemperature, &usuario);
    process_start(&PBloodSaturation, &usuario);
    process_start(&PHeartRate, &usuario);

    PROCESS_END();
}

static void enviar_alerta(DadosAlerta *alerta, TipoSensor sensor, double valor, double limite_inferior, double limite_superior, TipoAlteracao alteracao) {
    alerta->sensor = sensor;
    alerta->valor = valor;
    alerta->limite_inferior = limite_inferior;
    alerta->limite_superior = limite_superior;
    alerta->alteracao = alteracao;

    process_post(&PAlterationAlert, EVENTO_INDICADORES_CRITICOS, alerta);
}

PROCESS_THREAD(PTemperature, ev, data){

    static struct usuario_dados *dados;
    static struct etimer timer;
    static DadosAlerta alerta_temp;

    PROCESS_BEGIN();

    dados = (struct usuario_dados *) data;

    etimer_set(&timer, 3 * CLOCK_SECOND);

    while (1){
        double rand_temp = 35.0 + (rand() % 36) / 10.0;
        
        printf("Temperatura: %f\n", rand_temp);

        if (rand_temp < tabela[dados->idade].temp_hipo) {
            enviar_alerta(&alerta_temp, SENSOR_TEMPERATURA, rand_temp, tabela[dados->idade].temp_hipo, tabela[dados->idade].temp_hipo, ABAIXO_DO_LIMITE);

        }
        else if (rand_temp > tabela[dados->idade].temp_febre) {
            enviar_alerta(&alerta_temp, SENSOR_TEMPERATURA, rand_temp, tabela[dados->idade].temp_febre, tabela[dados->idade].temp_febre, ACIMA_DO_LIMITE);
        }

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
        etimer_reset(&timer);
    }
    PROCESS_END();
}

PROCESS_THREAD(PBloodSaturation, ev, data){

    static struct usuario_dados *dados;
    static struct etimer timer;
    static DadosAlerta alerta_sat;

    PROCESS_BEGIN();

    dados = (struct usuario_dados *) data;

    etimer_set(&timer, 3 * CLOCK_SECOND);

    while (1){
        
        int rand_sat = 93 + (rand() % 8);

        printf("Saturação Sanguínea: %d\n", rand_sat);
        
        if (rand_sat < tabela[dados->idade].sat_baixa) {
            enviar_alerta(&alerta_sat, SENSOR_SATURACAO, rand_sat, tabela[dados->idade].sat_baixa, tabela[dados->idade].sat_baixa, ABAIXO_DO_LIMITE);
        }
        else if (rand_sat >= tabela[dados->idade].sat_baixa && rand_sat < tabela[dados->idade].sat_normal) {
            enviar_alerta(&alerta_sat, SENSOR_SATURACAO, rand_sat, tabela[dados->idade].sat_baixa, tabela[dados->idade].sat_normal, ZONA_DE_RISCO);
        }

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
        etimer_reset(&timer);
    }
    PROCESS_END();
}

PROCESS_THREAD(PHeartRate, ev, data){
    
    static struct usuario_dados *dados;
    static struct etimer timer;
    static DadosAlerta alerta_freq;

    PROCESS_BEGIN();

    dados = (struct usuario_dados *) data;

    etimer_set(&timer, 3 * CLOCK_SECOND);

    while (1){
        int rand_rate = tabela[dados->idade].fc_min - 5 + (rand() % (tabela[dados->idade].fc_max - tabela[dados->idade].fc_min + 11));

        printf("Frequência cardíaca: %d\n", rand_rate);
        
        if (rand_rate <= tabela[dados->idade].fc_min) {
            enviar_alerta(&alerta_freq, SENSOR_FREQUENCIA_CARDIACA, rand_rate, tabela[dados->idade].fc_min, tabela[dados->idade].fc_min, ABAIXO_DO_LIMITE);
        }
        else if (rand_rate > tabela[dados->idade].fc_max) {
            enviar_alerta(&alerta_freq, SENSOR_FREQUENCIA_CARDIACA, rand_rate, tabela[dados->idade].fc_max, tabela[dados->idade].fc_max, ACIMA_DO_LIMITE);
        }
        
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
        etimer_reset(&timer);
    }
    PROCESS_END();
}

PROCESS_THREAD(PAlterationAlert, ev, data) {

    static DadosAlerta *alerta;
    static const char *nome_sensor;

    PROCESS_BEGIN();

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(ev == EVENTO_INDICADORES_CRITICOS);
        if (data != NULL) {
            alerta = (DadosAlerta *) data;

            switch (alerta->sensor) {
                case SENSOR_TEMPERATURA:
                    nome_sensor = "Temperatura";
                    break;

                case SENSOR_SATURACAO:
                    nome_sensor = "Saturacao sanguinea";
                    break;

                case SENSOR_FREQUENCIA_CARDIACA:
                    nome_sensor = "Frequencia cardiaca";
                    break;
            }
            printf("\n");
            printf("====================================\n");
            printf("       ALERTA DE SAUDE CRITICO      \n");
            printf("====================================\n");

            printf("Indicador: %s\n", nome_sensor);
            printf("Valor medido: %.2f\n", alerta->valor);

            switch (alerta->alteracao) {

                case ABAIXO_DO_LIMITE:

                    printf("Condicao: ABAIXO DO LIMITE\n");
                    printf(
                        "Limite minimo: %.2f\n",
                        alerta->limite_inferior
                    );
                    break;

                case ACIMA_DO_LIMITE:

                    printf("Condicao: ACIMA DO LIMITE\n");
                    printf(
                        "Limite maximo: %.2f\n",
                        alerta->limite_superior
                    );
                    break;

                case ZONA_DE_RISCO:

                    printf("Condicao: ZONA DE RISCO\n");
                    printf(
                        "Intervalo: %.2f ate %.2f\n",
                        alerta->limite_inferior,
                        alerta->limite_superior
                    );
                    break;
            }

            printf("====================================\n");
            printf("\n");
        }
    }
    PROCESS_END();
}

