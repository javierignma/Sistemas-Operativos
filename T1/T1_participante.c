#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#define READ_END 0
#define WRITE_END 1
#define BUFF_SIZE 7

char* puntaje_route = "./pipes/puntaje";
char* pagocobro_route = "./pipes/pagocobro";
char* suma_cartas_route = "./pipes/suma_cartas";
char* apuestas_route = "./pipes/apuestas";
char* carta_route = "./pipes/carta";
char* resultado_turno_route = "./pipes/resultado_turno";
char* solicitud_route = "./pipes/solicitud";

int get_from(char* pipe_route) {
    int pipe = open(pipe_route, O_RDONLY);
    char msg[BUFF_SIZE];
    read(pipe, msg, BUFF_SIZE);
    close(pipe_route);
    int dato_retorno;
    sscanf(msg, "%d", &dato_retorno);
    return dato_retorno;
}

void post_in(char* pipe_route, int valor) {
    int pipe = open(pipe_route, O_WRONLY);
    char msg[BUFF_SIZE];
    sprintf(msg, "%d", valor);
    write(pipe, msg, BUFF_SIZE);
    close(pipe_route);
}

int get_puntaje() {
    post_in(solicitud_route, 1);
    return get_from(puntaje_route);
}

int get_carta() {
    post_in(solicitud_route, 2);
    return get_from(carta_route);
}

int get_pagocobro(int suma_cartas, int puntaje, int apuestas) {
    post_in(solicitud_route, 3);
    post_in(suma_cartas_route, suma_cartas);
    post_in(puntaje_route, puntaje);;
    post_in(apuestas_route, apuestas);
    return get_from(pagocobro_route);
}

int main() {

    int monto = 10000;

    while (monto > 0) {
        int puntaje =  get_puntaje();
        printf("El puntaje de la mesa es de %d puntos, superalo!\n", puntaje);
        printf("Monto actual: $%d.\n", monto);
        int apuesta;
        int suma_cartas = 0;
        printf("Ingrese apuesta: ");
        scanf("%d", &apuesta);
        while(apuesta <= 0 || apuesta > monto) {
            printf("No puede apostar más de su monto o una cantidad nula o negativa!\n");
            printf("Ingrese nuevamente su apuesta: ");
            scanf("%d", &apuesta);
        }

        int solicitar_carta = 1;

        printf("La mesa comenzó a dar cartas.\n");
        while(solicitar_carta) {
            int carta = get_carta();
            suma_cartas += carta;
            printf("Ha recibido %d de la mesa. Hasta el momento sumas %d puntos.\n", carta, suma_cartas);
            printf("Ingrese 1 para pedir otra carta o 0 para terminar turno: ");
            scanf("%d", &solicitar_carta);
            while(solicitar_carta != 1 && solicitar_carta != 0) {
                printf("Solo puede ingresar 0 o 1, intente nuevamente: ");
                scanf("%d", &solicitar_carta);
            }
        }

        int pagocobro = get_pagocobro(suma_cartas, puntaje, apuesta);
        monto += pagocobro;
        if (pagocobro >= 0) {
            printf("Usted ha ganado $%d!\n", pagocobro);
            printf("Desea ocupar un puesto extra? (1 para sí y 0 para no): ");
            int puesto_extra;
            scanf("%d", &puesto_extra);
            while(puesto_extra > 1 || puesto_extra < 0) {
                printf("Intente nuevamente, ingrese solo 0 o 1: ");
                scanf("%d", &puesto_extra);
            }
            if(puesto_extra) {
                printf("Ocupando puesto extra...\n");
                fork();
            }
        }
        else {
            printf("Usted ha perdido $%d!\n", pagocobro);
        }

        if(monto <= 0) {
            printf("Se ha quedado sin dinero para apostar. FIN DEL JUEGO.\n");
            exit(0);
        }

    }

    return 0;

}
