# include "contiki.h"
# include <stdio.h>
#include <time.h>

PROCESS(processo1, "exemplo");
AUTOSTART_PROCESSES(&processo1);

PROCESS_THREAD(processo1, ev, data) {
    time_t tempo_atual;
    static struct etimer timer;
    struct tm *info_tempo;

    PROCESS_BEGIN();
    etimer_set(&timer, 1 * CLOCK_SECOND);

    while(1) {
        time(&tempo_atual);
        info_tempo = localtime(&tempo_atual);

        printf("Vicente Migliorini\n");
        printf("Horário local: %02d:%02d\n\n", info_tempo->tm_hour, info_tempo->tm_min);

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
        etimer_reset(&timer);
    }

    PROCESS_END();
}