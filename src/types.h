#ifndef TYPES_H
#define TYPES_H


#define SERVER_IP "0.0.0.0"
#define PORT 5555
#define BUFFER_SIZE 8192
#define MAX_EVENTS 64

//list of message in a room
typedef struct Message {
    char *payload;          
    struct Message *next;   
} Message;

//list of all the rooms 
typedef struct Queue {
    char name[64];          
    Message *head;          
    Message *tail;          
	struct Queue *next;     
} Queue;



//func
void set_nonblocking(int fd);


Queue* find_room(const char *room);
Queue* create_room(const char* room_name);
int queue_push(const char *room_name, const char *message_text);

char* queue_pop(const char* room_name);

#endif
