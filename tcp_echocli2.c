//-----------------------------------------------------------------------
// 파일명: get_hostent.c
// 동  작: 도메인 이름을 IP 주소로 변환
// 컴파일: cc -o get_hostent get_hostent.c
// 실  행: get_hostent www.skuniv.ac.kr
//-----------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// memcpy 함수 선언
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	struct hostent *hp;
	struct in_addr in;
	int i;
	char buf_ip[20];

	int s, nbyte;
	struct sockaddr_in servaddr;
        char buf[ MAXLINE + 1];	

	
	if(argc != 3) {
                printf("Usage: %s ip_address portnumber\n", argv[0]);
                exit(0);
        }
	hp = gethostbyname(argv[1]);
	if(hp==NULL) {
		printf("gethostbyname fail\n");
		exit(0);
	}

	for( i=0; hp->h_addr_list[i]; i++) {
		memcpy(&in.s_addr, hp->h_addr_list[i], sizeof(in.s_addr));
		inet_ntop(AF_INET, &in, buf_ip, sizeof(buf_ip));
	}

	// 소켓 생성
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
                perror("socket fail");
                exit(1);
        }
	
	bzero((char*)&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        inet_pton(AF_INET, buf_ip, &servaddr.sin_addr);
        servaddr.sin_port = htons(atoi(argv[2]));


	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
                perror("connect fail");
                exit(1);
        }

        printf("입력 : ");
        if (fgets(buf, sizeof(buf), stdin) == NULL)
                exit(0);

        nbyte = strlen(buf);

	if (write(s, buf, nbyte) < 0) {
                printf("write error\n");
                exit(0);
        }

	printf("수신 : ");
        if((nbyte = read(s, buf, MAXLINE)) < 0) {
                perror("read fail");
                exit(0);
        }
        buf[nbyte] = 0;
        printf("%s", buf);
        close(s);
        return 0;

}
