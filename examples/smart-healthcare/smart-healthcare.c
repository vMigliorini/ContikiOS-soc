/* Dupla: Vicente Migliorini e Gabriel Mota */
# include "contiki.h"
# include <stdio.h>
# include <stdlib.h>
# include <time.h>

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
    const char *classificacao;
} DadosAlerta;

// Valores de referencia
#define FC_BRADICARDIA   50     // x <= 50  = Bradicardia
#define FC_TAQUICARDIA   110    // x > 110  = Taquicardia
#define SAT_NORMAL       95     // x >= 95  = Normal
#define SAT_BAIXA        90     // x < 90   = Baixa
#define TEMP_FEBRE       37.0   // x > 37.0 = Febre
#define TEMP_HIPOTERMIA  35.5   // x < 35.5 = Hipotermia

//Eventos compartilhados entre os processos
static process_event_t EVENTO_INDICADORES_CRITICOS;

PROCESS(PTemperature, "Temperatura");
PROCESS(PBloodSaturation, "Saturacao Sanguinea");
PROCESS(PHeartRate, "Frequencia Cardiaca");
PROCESS(PAlterationAlert, "Alerta de criticidade");
PROCESS(PSmartWatch, "Smart Watch");

AUTOSTART_PROCESSES(&PSmartWatch);

PROCESS_THREAD(PSmartWatch, ev, data){

    PROCESS_BEGIN();

    srand(time(NULL));

    EVENTO_INDICADORES_CRITICOS = process_alloc_event();

    process_start(&PAlterationAlert, NULL);
    process_start(&PTemperature, NULL);
    process_start(&PBloodSaturation, NULL);
    process_start(&PHeartRate, NULL);

    PROCESS_END();
}

static void enviar_alerta(DadosAlerta *alerta, TipoSensor sensor, double valor, double limite_inferior, double limite_superior, TipoAlteracao alteracao, const char *classificacao) {
    alerta->sensor = sensor;
    alerta->valor = valor;
    alerta->limite_inferior = limite_inferior;
    alerta->limite_superior = limite_superior;
    alerta->alteracao = alteracao;
    alerta->classificacao = classificacao;

    process_post(&PAlterationAlert, EVENTO_INDICADORES_CRITICOS, alerta);
}

PROCESS_THREAD(PTemperature, ev, data){

    static struct etimer timer;
    static DadosAlerta alerta_temp;

    PROCESS_BEGIN();

    etimer_set(&timer, 3 * CLOCK_SECOND);

    while (1){
        double rand_temp = 34.0 + (rand() % 51) / 10.0;   // 34.0 a 39.0
        const char *classe = "Normal";

        if (rand_temp < TEMP_HIPOTERMIA) {
            classe = "Hipotermia";
            enviar_alerta(&alerta_temp, SENSOR_TEMPERATURA, rand_temp, TEMP_HIPOTERMIA, TEMP_HIPOTERMIA, ABAIXO_DO_LIMITE, classe);
        }
        else if (rand_temp > TEMP_FEBRE) {
            classe = "Febre";
            enviar_alerta(&alerta_temp, SENSOR_TEMPERATURA, rand_temp, TEMP_FEBRE, TEMP_FEBRE, ACIMA_DO_LIMITE, classe);
        }

        printf("Temperatura: %.1f - %s\n", rand_temp, classe);

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
        etimer_reset(&timer);
    }
    PROCESS_END();
}

PROCESS_THREAD(PBloodSaturation, ev, data){

    static struct etimer timer;
    static DadosAlerta alerta_sat;

    PROCESS_BEGIN();

    etimer_set(&timer, 3 * CLOCK_SECOND);

    while (1){
        int rand_sat = 85 + (rand() % 16);   // 85 a 100
        const char *classe = "Normal";

        if (rand_sat < SAT_BAIXA) {
            classe = "Baixa";
            enviar_alerta(&alerta_sat, SENSOR_SATURACAO, rand_sat, SAT_BAIXA, SAT_BAIXA, ABAIXO_DO_LIMITE, classe);
        }
        else if (rand_sat >= SAT_BAIXA && rand_sat < SAT_NORMAL) {
            classe = "Atencao";
            enviar_alerta(&alerta_sat, SENSOR_SATURACAO, rand_sat, SAT_BAIXA, SAT_NORMAL, ZONA_DE_RISCO, classe);
        }

        printf("Saturacao Sanguinea: %d - %s\n", rand_sat, classe);

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
        etimer_reset(&timer);
    }
    PROCESS_END();
}

PROCESS_THREAD(PHeartRate, ev, data){
    
    static struct etimer timer;
    static DadosAlerta alerta_freq;

    PROCESS_BEGIN();

    etimer_set(&timer, 3 * CLOCK_SECOND);

    while (1){
        int rand_rate = 40 + (rand() % 91);   // 40 a 130
        const char *classe = "Normal";

        if (rand_rate <= FC_BRADICARDIA) {
            classe = "Bradicardia";
            enviar_alerta(&alerta_freq, SENSOR_FREQUENCIA_CARDIACA, rand_rate, FC_BRADICARDIA, FC_BRADICARDIA, ABAIXO_DO_LIMITE, classe);
        }
        else if (rand_rate > FC_TAQUICARDIA) {
            classe = "Taquicardia";
            enviar_alerta(&alerta_freq, SENSOR_FREQUENCIA_CARDIACA, rand_rate, FC_TAQUICARDIA, FC_TAQUICARDIA, ACIMA_DO_LIMITE, classe);
        }

        printf("Frequencia cardiaca: %d - %s\n", rand_rate, classe);

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
            printf("Classificacao: %s\n", alerta->classificacao);

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