#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#define BUFFER_SIZE	8096000

typedef struct {
    char letters[100];
    char numbers[100];
} SeparatedStrings;

int main(int argc, char **argv) { 
	int sockfd, port; 
	char buffer[BUFFER_SIZE];
	struct sockaddr_in servaddr; 
	SeparatedStrings result;
	if (argc < 3) {
		printf("Some of the command line arguments missing");
		return -1;
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(errno); 
	}	
	printf("Socket successfully created..\n"); 
	servaddr.sin_family = AF_INET;
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
	port = atoi(argv[2]);
	servaddr.sin_port = htons(port); 
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(errno);
	} 
	printf("connected to the server..\n"); 

	while (1) {
        // Read input from the user
        memset(buffer, 32, BUFFER_SIZE);
        printf("Enter a string (or empty string to exit): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
		int length = BUFFER_SIZE;
		write(sockfd, &length, sizeof(length)); 
        // Send the message to the server
        if (send(sockfd, buffer, BUFFER_SIZE, 0) < 0) 
		{
            perror("Error sending message");
            return 1;
        }
        // Check if the user entered an empty string to exit
        if (strlen(buffer) == 0) {
            printf("Exiting...\n");
            break;
        }
        // Receive the response from the server
        memset(&result, 0, sizeof(SeparatedStrings));
        if (recv(sockfd, &result, sizeof(SeparatedStrings), 0) < 0) 
		{
            perror("Error receiving message");
            return 1;
        }

        printf("Server response: %s\n", result.letters);
		printf("Server response: %s\n", result.numbers);
    }

	close(sockfd);
	return 0;
	
} 