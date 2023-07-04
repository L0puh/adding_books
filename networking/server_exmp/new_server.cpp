#include <csignal>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT "3490"
#define BACKLOG 10


struct addrinfo init_addrinfo(struct addrinfo hints){
    size_t size_hints = sizeof(hints);
    memset(&hints, 0, size_hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    return hints;
}


bool print_error(int res){
    if (res == -1){
        fprintf(stderr, "server error: %s\n", strerror(errno));
        return true;
    }
    return false;
}
void *get_in_addr (struct sockaddr *sa){
    return &(((struct sockaddr_in*)sa)->sin_addr);  
}


int create_socket(struct addrinfo *servinfo, struct addrinfo *p, int sockfd){
    int res, yes = 1;
    for (p = servinfo; p != NULL; p = p->ai_next){

       sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        
        if (print_error(sockfd))
            continue;
        
        res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (print_error(res))
            exit(1); 

        res = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (print_error(res)){
            close(sockfd);
            exit(1);
        }
    
        break; 
    }
    
    if(p == NULL){
        fprintf(stderr, "failed to bind: %s\n", strerror(errno));
        exit(1);
    }

    freeaddrinfo(servinfo);
    return sockfd;
}

int main () {
    struct addrinfo hints, *servinfo, *p;
    hints = init_addrinfo(hints);
    
    int res = getaddrinfo(NULL, PORT, &hints, &servinfo);
    if (res != 0 ){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(res));
        return 1;
    }
    int sockfd;
    sockfd = create_socket(servinfo, p, sockfd); 
     
    res = listen(sockfd, BACKLOG);
    if (print_error(res))
        return 1;
    
    printf("wait for connection...\n");
    socklen_t sin_size;
    struct sockaddr_in their_addr;
    char s[INET_ADDRSTRLEN];
    while (true){
        sin_size = sizeof(their_addr);
        int sockfd_new = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (print_error(sockfd_new)){
            continue;
        }
        inet_ntop(their_addr.sin_family, 
                get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s));
        printf("connection from: %s\n", s);

        if(!fork()){
            close(sockfd);
            res = send(sockfd_new, "Hello, world", 13, 0);
            if (print_error(res))
                exit(1);
            close(sockfd_new);
            return 0;
        }
    }

return 0;
}







