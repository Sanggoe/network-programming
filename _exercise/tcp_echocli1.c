#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	int s, nbyte;
	struct sockaddr_in servaddr;
	char buf[MAXLINE+1];

	if (argc != 3) {
		printf("Usage : %s ip_address port\n", argv[0]);
		exit(0);
	}

	// socket 생성
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	// echo server socket address struct 작성
	bzero(&servaddr, sizeof(servaddr));

	// servaddr의 주소 지정
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	// request connect
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(1);
	}

	printf("입력 : ");
	if (fgets(buf, sizeof(buf), stdin) == NULL) {
		printf("Input Error\n");
		exit(0);
	}

	nbyte = strlen(buf);
	// send message to echo server
	if (write(s, buf, nbyte) < 0) {
		perror("write error\n");
		exit(0);
	}

	// receive echo server data
	printf("수신 : ");
	if ((nbyte = read(s, buf, MAXLINE)) < 0) {
		perror("read fail");
		exit(0);
	}
	buf[nbyte] = 0;
	printf("%s", buf);
	close(s);
	return 0;
}
