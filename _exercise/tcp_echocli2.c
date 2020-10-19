#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	int s, nbyte;
	struct sockaddr_in servaddr;
	struct hostent *hp;
	struct in_addr inaddr;
	char buf[MAXLINE+1];

	if (argc != 3) {
		printf("Usage : %s hostname port\n", argv[0]);
		exit(0);
	}

	// socket 생성
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	hp = gethostbyname(argv[1]);
	if (hp == NULL) {
		printf("gethostbyname fail\n");
		exit(0);
	}
	
	// echo server socket address struct 작성
	bzero(&servaddr, sizeof(servaddr));

	// servaddr의 주소 지정
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	memcpy(&servaddr.sin_addr, hp->h_addr_list[0], sizeof(servaddr.sin_addr));

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
