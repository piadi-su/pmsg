#include <arpa/inet.h> 
#include <asm-generic/socket.h>
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <strings.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 

// usegae example

// PUSH ciao_mondo Questo è il primo messaggio!
// OK MESSAGE_SAVED
//
// POP ciao_mondo
// DATA Questo è il primo messaggio!

#define SERVER_IP "0.0.0.0"
#define PORT 5555
#define BUFFER 4096

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

	listen_success = listen(srv_sockfd, 10);
	if (listen_success< 0){
		perror("Error: listen socket");
		return EXIT_FAILURE;
	}

	//converts ipv4/6 string to redable ip
	inet_pton(AF_INET, SERVER_IP, &address.sin_addr);
	

	//TODO add accept for asyinc connection with select for multi client





	


	

    return EXIT_SUCCESS;
}
