#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	int s, nbyte;
	struct sockaddr_in servaddr;
	char buf[MAXLINE+1];

	if(argc != 2) {
		printf("Usage: %s ip_address\n", argv[0]);
		exit(0);
	}

	// socket 생성
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}

	// init server socket address struct
	bzero(&servaddr, sizeof(servaddr));

	// servaddr 주소 지정
	servaddr.sin_family = AF_INET; 
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_port = htons(13);

	// request connect
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(1);
	}
	
	// receive dytime data and print it
	if ((nbyte = read(s, buf, MAXLINE)) < 0) {
		perror("read fail");
		exit(1);
	}

	buf[nbyte] = 0;
	printf("%s\n", buf);
	close(s);
	return 0;
}

