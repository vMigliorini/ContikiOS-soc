# include "contiki.h"
# include <stdio.h>

PROCESS(processo1, "processo1");
PROCESS(processo2, "processo2");
AUTOSTART_PROCESSES(&processo1, &processo2);
int valor = 0;

PROCESS_THREAD(processo1, ev, data) {

    PROCESS_BEGIN();

    while(1) {
        valor++;
        process_post(&processo2, PROCESS_EVENT_MSG, &valor);
        PROCESS_PAUSE();
    }

    PROCESS_END();
}

PROCESS_THREAD(processo2, ev, data) {
    PROCESS_BEGIN();

    while(1) {
        PROCESS_WAIT_EVENT();
        int valor_convertido = *(int *)data;
        if (ev == PROCESS_EVENT_MSG) {
            if (valor_convertido % 2 == 1){
                printf("valor: %d\n", valor_convertido);
            }
        }
    }

    PROCESS_END();
}