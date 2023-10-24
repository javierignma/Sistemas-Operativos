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

void inicializar() {
    unlink(puntaje_route);
    unlink(pagocobro_route);
    unlink(suma_cartas_route);
    unlink(apuestas_route);
    unlink(carta_route);
    unlink(resultado_turno_route);
    unlink(solicitud_route);
    
    remove(puntaje_route);
    remove(pagocobro_route);
    remove(suma_cartas_route);
    remove(apuestas_route);
    remove(carta_route);
    remove(resultado_turno_route);
    remove(solicitud_route);

    mkfifo(puntaje_route, 0666);
    mkfifo(pagocobro_route, 0666);
    mkfifo(suma_cartas_route, 0666);
    mkfifo(apuestas_route, 0666);
    mkfifo(carta_route, 0666);
    mkfifo(resultado_turno_route, 0666);
    mkfifo(solicitud_route, 0666);
}

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

void post_puntaje (){
    int puntaje = (rand() % 21) + 1;
    post_in(puntaje_route, puntaje);
    printf("Se envió un puntaje = %d\n", puntaje);
}

void post_carta (){
    int carta = (rand() % 10) + 1;
    post_in(carta_route, carta);
    printf("Se envió una carta = %d\n", carta);
}

void post_pagocobro() {
    int suma_cartas = get_from(suma_cartas_route);
    int puntaje = get_from(puntaje_route);
    int apuestas = get_from(apuestas_route);

    int pagocobro;

    if(suma_cartas > puntaje && suma_cartas <= 21) {
        pagocobro = apuestas;
    }
    else {
        pagocobro = -apuestas;
    }

    post_in(pagocobro_route ,pagocobro);
    printf("Se envio un cobro = $%d\n", pagocobro);
}

int main() {
    printf("INICIO\n");

    inicializar();

    while (1) {
        int solicitud = get_from(solicitud_route);
        switch (solicitud)
        {
        case 1:
            post_puntaje();
            break;
        
        case 2:
            post_carta();
            break;
        
        case 3:
            post_pagocobro();
            break;
        }
    }

    return 0;

}
