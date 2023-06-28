#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>

#define QUEUE_NO 5

typedef struct {
    char letters[100];
    char numbers[100];
} SeparatedStrings;

SeparatedStrings separateStrings(const char* input) {
    SeparatedStrings result;
    int lettersIndex = 0;
    int numbersIndex = 0;
    int length = strlen(input);

    for (int i = 0; i < length; i++) {
        if (isalpha(input[i])) {
            result.letters[lettersIndex++] = input[i];
        } else if (isdigit(input[i])) {
            result.numbers[numbersIndex++] = input[i];
        }
    }
    result.letters[lettersIndex] = '\0';
    result.numbers[numbersIndex] = '\0';
    return result;
}
int main(int argc, char **argv) {
	int sockfd, clientfd, port, length;
    int ret, nLeft;
    int idx = 0;

    SeparatedStrings result;
	struct sockaddr_in s_addr, c_addr;
	socklen_t socklen = (socklen_t)sizeof(struct sockaddr_in);
    //Step 1: Construct socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) 
    {
		printf("socket creation failed...\n");
		exit(errno);
	}
    printf("Socket successfully created..\n");

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	port = atoi(argv[1]);
	s_addr.sin_port = htons(port);
	s_addr.sin_addr.s_addr = INADDR_ANY;
    
    //Step 2: Bind address to socket
	if (bind(sockfd, (struct sockaddr*)&s_addr, sizeof(s_addr)) != 0){
		printf("socket creation failed...\n");
		exit(errno);
	}
    //Step 3: Listen request from client
	if (listen(sockfd, QUEUE_NO) != 0) {
		printf("socket listen failed...\n");
		exit(errno);
	}
    //Step 4: Communicate with client
    //accept request
    clientfd = accept(sockfd, (struct sockaddr*)&c_addr, &socklen);
    if (clientfd < 0) 
    {
        perror("Error accepting client connection");
        exit(EXIT_FAILURE);
    }
	printf("%s:%d connected\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
	while (1) 
    {
        read(clientfd, &length, sizeof(length));
        printf("received length: %d\n", length);
        char buffer[8096000];
        memset(buffer, 0, length);
        // if (recv(clientfd, buffer, BUFFER_SIZE - 1, 0) < 0) 
        // {
        //     perror("Error receiving");
        //     return 1;
        // }

        int bytes =0, i =0;
        int total = 0;
        for (i = 0; i < length; i += bytes) 
        {
    		if ((bytes = recv(clientfd, buffer +i, length - i, 0)) == -1)  
			{
        		break;
    		}
            total += bytes;
			printf("bytes received: %d %d\n", bytes, total);
		}

        // Check if client has disconnected
        if (strlen(buffer) == 0) {
            printf("Client disconnected.\n");
            break;
        }
        // printf("Received message from client: %s\n", buffer);
        
        result = separateStrings(buffer);
        if (send(clientfd, &result, sizeof(SeparatedStrings), 0) < 0) {
            perror("Error sending message");
            return 1;
        }
        memset(&result, 0, sizeof(SeparatedStrings));
	}
    // close(clientfd);

	close(sockfd);
	return 0;
}