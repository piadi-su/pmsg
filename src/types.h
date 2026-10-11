#ifndef TYPES_H
#define TYPES_H


#define SERVER_IP "0.0.0.0"
#define PORT 5555
#define BUFFER_SIZE 4096
#define MAX_EVENTS 64

//list of message in a room
typedef struct Message {
    char *payload;          // Il testo del messaggio
    struct Message *next;   // Puntatore al messaggio successivo nella stessa camera
} Message;

//list of all the rooms 
typedef struct Queue {
    char name[64];          // Il nome identificativo della camera (es. "notifiche")
    Message *head;          // Primo messaggio (da dove si fa POP)
    Message *tail;          // Ultimo messaggio (dove si fa PUSH)
    struct Queue *next;     // Puntatore alla prossima camera creata nel sistema
} Queue;



//func
void set_nonblocking(int fd);

#endif
