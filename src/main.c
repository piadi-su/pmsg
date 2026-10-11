#include <arpa/inet.h> 
#include <asm-generic/socket.h>
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <strings.h> 
#include <sys/select.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/epoll.h> 
#include <fcntl.h>
#include <errno.h> 
#include <signal.h> 
#include <string.h> 

// my inc
#include "types.h"

// usegae example

// PUSH list_name msg
// OK MESSAGE_SAVED
//
// POP list_name
// DATA msg



int main(void){

	int srv_sockfd;
	int bind_success;
	int listen_success;
	int connect_success;
	
	struct sockaddr_in address;
	memset(&address, 0 , sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;


	//epoll stuf
	int epoll_fd;
	struct epoll_event ev, events[MAX_EVENTS];


	srv_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (srv_sockfd < 0){
		perror("Error: creating the socket");
		return EXIT_FAILURE;
	}

	int opt = 1;
	setsockopt(srv_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	bind_success = bind(srv_sockfd, (struct sockaddr*)&address, sizeof(address));
	if (bind_success < 0){
		perror("Error: binding the socket");
		return EXIT_FAILURE;
	}

	listen_success = listen(srv_sockfd, SOMAXCONN);
	if (listen_success < 0){
		perror("Error: listen socket");
		return EXIT_FAILURE;
	}

	set_nonblocking(srv_sockfd);



	epoll_fd = epoll_create1(0);
	if(epoll_fd < 0){
		perror("Error: starting epoll");
	}

	ev.events = EPOLLIN;
	ev.data.fd = srv_sockfd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, srv_sockfd, &ev);


	printf("[LOG][+] listening for connection on %s:%d\n",SERVER_IP,PORT);

	while(1){

		//fd monior rfds = realy file descriptors
		int rfds= epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

		for(int i = 0 ; i < rfds; i++){
			//if the ready fd is the srv one there is a new incomming connection
			if(events[i].data.fd == srv_sockfd){

				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);

				//accept a new client 
				int client_fd = accept(srv_sockfd, (struct sockaddr*)&client_addr ,&client_len);
				if(client_fd < 0){
					perror("Error: can't create client_fd file descriptor");
					continue;
				}

				//setup new fd and add to the list
				set_nonblocking(client_fd);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = client_fd;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
				printf("[LOG][+] new client connection\n");
			}
			else{
				
				int client_fd = events[i].data.fd;

				//check if epoll sing error/disconnection
				if (events[i].events & (EPOLLHUP | EPOLLERR)) {
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
					close(client_fd);
					printf("[LOG][+] client disconnected \n");
				
				}

				char data_buffer[BUFFER_SIZE];
				memset(data_buffer, 0, sizeof(data_buffer));
				int read_bytes = read(client_fd, data_buffer, sizeof(data_buffer));

				if(read_bytes <= 0){
					//fd error or closed connection
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
					close(client_fd);
					printf("[LOG][+] client disconnected \n");

				}
				else{
					
					//rm possibile new line chars from the input buffer
					data_buffer[strcspn(data_buffer, "\r\n")] = 0;

					char method[16];
					char room[64];
					/*
					 putting buffer size so it doesent overflow
					 the max size of the buffer it's
					 BUFFER_SIZE - 16 - 64
					 */
					char message[BUFFER_SIZE];

					// const char *msg_saved = "OK message saved\n";

					int pargs_count = sscanf(data_buffer,"%15s %63s %4095[^\n]",method, room, message);

					if (pargs_count >= 1 && strcmp(method, "PUSH") == 0){
	
						if(pargs_count == 3){
							printf("[DEBUG][PARSED] method: %s, to room: %s, message: %s \n", method, room, message);
							// : queue_push(room, message);
							write(client_fd, "OK message saved\r\n",18);
						}
						else{
							write(client_fd, "ERR usage: PUSH <room> <message>\r\n",34);
						}

					}

					else if (pargs_count >= 1 && strcmp(method, "POP") == 0){
	
						if(pargs_count == 2){
							printf("[DEBUG][PARSED] method: %s, to room: %s\n", method, room);
							// : char *msg = queue_pop(room);
							write(client_fd, "DATA <message>\r\n",16);
						}
						else{
							write(client_fd, "ERR usage: POP <room> \r\n",24);
						}
					}
					
					else{
						write(client_fd, "ERR UNKNOWN COMMAND\r\n", 21);
					}
	
				}



			}




		}

	}





	


	
	close(epoll_fd);
	close(srv_sockfd);

    return EXIT_SUCCESS;
}

//for makeing accept, read, write not blocking 
void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
