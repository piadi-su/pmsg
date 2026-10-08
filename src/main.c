#include <arpa/inet.h> 
#include <asm-generic/socket.h>
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
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

// usegae example

// PUSH list_name msg
// OK MESSAGE_SAVED
//
// POP list_name
// DATA msg

#define SERVER_IP "0.0.0.0"
#define PORT 5555
#define BUFFER_SIZE 4096
#define MAX_EVENTS 64


void set_nonblocking(int fd);

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
				int read_bytes = read(client_fd, data_buffer, sizeof(data_buffer));

				if(read_bytes <= 0){
					//fd error or closed connection
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
					close(client_fd);
					printf("[LOG][+] client disconnected \n");

				}
				else{
					write(client_fd, data_buffer, read_bytes);
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
