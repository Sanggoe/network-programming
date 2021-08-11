# Chapter2. 소켓 프로그래밍 기초

## 2.1 소켓 개요

### 2.1.1 소켓 정의

* 소켓은 **TCP**나 **UDP**와 같은 **트랜스포트 계층**을 이용하는 **API**이다.
* 모든 Unix 운영체제에서 지원하며, Windows에서도 Winsock이라는 이름으로 소켓 API를 제공한다.
* 자바 플랫폼에서도 소켓을 이용하기 위한 클래스를 제공한다.

<br/>

#### 소켓 번호

* 유닉스에서 파일을 open하면, int 타입의 파일 디스크립터를 리턴한다.
* 정규 파일 뿐 아니라, 키보드, 모니터, 장치, 파이프, 소켓 등을 모두 파일로 취급한다.
* 이들은 각각 int 타입의 파일 디스크립터를 통해 액세스 하도록 하고 있다.
* 시스템(커널)은 파일에 관한 각종 정보를 담고 있는 구조체를 할당한다.
* 이 구조체를 가리키는 포인터들로 구성된 테이블을 파일 **디스크립터 테이블** 이라고 한다.
* 해당 인덱스 값이 바로 **파일 디스크립터**!
  * 시스템에서 default로 0(표준 입력 - 키보드), 1(표준 출력 - 모니터), 2(표준 에러 출력 - 모니터)을 할당
  * 따라서 기본적으로 파일 디스크립터는 3부터 할당된다.
* 소켓 개설 시 얻은 파일 디스크립터를 **소켓 디스크립터**라고도 하며, **소켓 번호**라고 지칭한다.

<br/>

#### 포트 번호

* **IP 주소**는 IP 데이터그램을 **목적지 호스트**까지 전달하는데 사용된다.
* 이 데이터를 **최종 전달**할 '프로세스(정확히는, **프로세스 내의 소켓**)'를 **구분**하기 위해 **포트번호**가 사용된다.
* 다시 말해, **특정 호스트**를 찾는데 **IP 주소**를, 호스트 내의 통신 접속점(**소켓**)을 구분하기 위해 **포트번호**를 쓴다.
* **응용 프로그램을 구분하기 위한 번호**라는 표현은, 프로그램당 하나의 소켓만 개설될 경우 맞는 표현이다.
* 포트번호는 TCP나 UDP 헤더에 실려 전송되며 16비트로 표현된다.
* TCP와 UDP의 포트번호는 독립적으로 운영되며 1~65535 의 값을 가질 수 있다.
* 따라서 포트번호 동시 사용은 가능하지만, 보통 같은 종류의 서비스를 동시에 제공할 때만 같은 번호를 쓴다.

<br/>

* 포트번호를 정리한 파일을 '서비스 파일'이라고 하는데, 유닉스의 `/etc/services` 에 이 내용이 들어있다.

```cmd
more services
# /etc/services:
# $Id: services,v 1.55 2013/04/14 ovasik Exp $
#
# Network services, Internet style
# IANA services version: last updated 2013-04-10
#
# Note that it is presently the policy of IANA to assign a single well-known
# port number for both TCP and UDP; hence, most entries here have two entries
# even if the protocol doesn't support UDP operations.
# Updated from RFC 1700, ``Assigned Numbers'' (October 1994).  Not all ports
# are included, only the more common ones.
#
# The latest IANA port assignments can be gotten from
#       http://www.iana.org/assignments/port-numbers
# The Well Known Ports are those from 0 through 1023.
# The Registered Ports are those from 1024 through 49151
# The Dynamic and/or Private Ports are those from 49152 through 65535
#
# Each line describes one service, and is of the form:
#
# service-name  port/protocol  [aliases ...]   [# comment]

tcpmux          1/tcp                           # TCP port service multiplexer
tcpmux          1/udp                           # TCP port service multiplexer
rje             5/tcp                           # Remote Job Entry
rje             5/udp                           # Remote Job Entry
echo            7/tcp
echo            7/udp
discard         9/tcp           sink null
discard         9/udp           sink null
systat          11/tcp          users
systat          11/udp          users
daytime         13/tcp
daytime         13/udp
qotd            17/tcp          quote
qotd            17/udp          quote
msp             18/tcp                          # message send protocol (historic)
msp             18/udp                          # message send protocol (historic)
chargen         19/tcp          ttytst source
chargen         19/udp          ttytst source
ftp-data        20/tcp
ftp-data        20/udp
# 21 is registered to ftp, but also used by fsp
ftp             21/tcp
ftp             21/udp          fsp fspd
ssh             22/tcp                          # The Secure Shell (SSH) Protocol
ssh             22/udp                          # The Secure Shell (SSH) Protocol
telnet          23/tcp
telnet          23/udp
# 24 - private mail system
lmtp            24/tcp                          # LMTP Mail Delivery
lmtp            24/udp                          # LMTP Mail Delivery
smtp            25/tcp          mail
smtp            25/udp          mail

```

<br/>

#### well-known port

* ftp(21), telnet(23), mail(25) 등 널리 사용되는 서비스라서 미리 지정된 포트를 **well-known port** 라고 한다.
* 보통 1023 이하로 배정되며, 따라서 사용자 임의 지정 포트는 1024번 이상을 사용해야 한다.

<br/>

#### 2.1.2 소켓 사용법

<br/>

#### 소켓 개설

* 소켓을 개설하여 통신에 이용하려면 다섯 가지 정보가 정해져야 한다.
  * 통신에 사용할 프로토콜(TCP / UDP)
  * 자신의 IP 주소 (프로그램이 수행되는 컴퓨터의 IP)
  * 자신의 Port 번호 (통신에 사용할 소켓을 구분하는 번호)
  * 상대방의 IP 주소 (통신하고자 하는 상대방 컴퓨터의 IP)
  * 상대방의 Port 번호 (목적지 컴퓨터 내에서 소켓을 구분하기 위한 번호)

<br/>

#### socket() 사용 문법 ★

```c
#include <sys/socket.h> 	// -> #include <netdb.h> 에 포함된다.
int socket(
    int domain,		// 프로토콜 체계
    int type,		// 서비스 타입
    int protocal);	// 소켓에서 사용할 프로토콜
```

* **domain** : 프로토콜 체계
  * TCP/IP 프로토콜을 사용하려면 **PF_INET** (Protocal Family Internet) 으로 지정
* **type** : 서비스타입
  * TCP를 이용하려면 : **SOCK_STREAM**
  * UDP를 이용하려면 : **SOCK_DGRAM**
  * Raw 소켓 생성시 : SOCK_RAW
* **protocal** : 이 소켓에서 사용할 프로토콜
  * 앞서 type에서 둘 중에 하나로 정한 경우 이미 TCP/UDP로 정해졌으므로 **0**을 쓰면 된다.
  * 추후에 배울 Raw 소켓 시, 상위 프로토콜을 지정하는데 사용된다.
* **socket()** 이 성공적으로 수행되면 **새로 만들어진 소켓번호**를 반환하고, Error 발생 시 **-1**을 반환한다.

<br/>

#### 파일 디스크립터를 출력하는 예제

```c
//--------------------------------------------------------------
// 파일명 : open_socket.c
// 기  능 : socket() 시스템 콜을 호출하고, 생성된 소켓번호를 출력
// 컴파일 : cc -o open_socket open_socket.c
// 사용법 : open_socket
//--------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>	// 소켓 시스템 콜에 필요한 상수 선언
#include <sys/stat.h>	// 파일의 상태에 대한 데이터 선언
#include <sys/socket.h>	// 소켓 시스템 콜 선언  -> #include <netdb.h> 에 포함된다.
#include <fcntl.h>	// open에 필요한 flag 선언 (O_RDONLY)
#include <unistd.h>

int main() {
    //파일 및 소켓번호
    int fd1, fd2, sd1, sd2;
    
    // 파일 열기
    fd1 = open("/etc/passwd", O_RDONLY, 0);
    printf("/etc/passwd's file descriptor = %d\n", fd1);
    
    // TCP 소켓 열기
    sd1 = socket(PF_INET, SOCK_STREAM, 0);
    printf("stream socket descriptor = %d\n", sd1);

    // UDP 소켓 열기
    sd2 = socket(PF_INET, SOCK_DGRAM, 0);
    printf("datagram socket descriptor = %d\n", sd2);
 
    // 또다른 파일 열기
    fd2 = open("/etc/hosts", O_RDONLY, 0);
    printf("/etc/hosts's file descriptor = %d\n", fd2);
    
    // 파일 및 소켓 닫기
    close(fd2);
    close(sd2);
    close(sd1);
    close(fd1);
    
    return 0;
}
```

* 결과

```
./open_socket

/etc/passwd's file descriptor = 3
stream socket descriptor = 4
datagram socket descriptor = 5
/etc/hosts's file descriptor = 6
```

* 파일 디스크립터는 **표준 입력(키보드) 0, 표준 출력(모니터) 1, 에러 출력(모니터) 2를 기본적으로 우선 할당**하기 때문에, close 해주지 않는 한 **3번 부터 할당**된다.
* 또한 한 프로세스에서 개설 가능한 소켓 최대 수는 64 또는 1024 등으로 제한된다.

<br/>

* 한 프로세스에서 개설 가능한 최대 소켓 수를 알려면 **getdtablesize()** 를 이용한다.

```c
#include <stdio.h>
#include <unistd.h>
printf("getdtablesize() = %d\n", getdtablesize());
```

<br/>

#### 헤더파일들

* 우리가 사용할 대부분의 헤더파일들은 다음 세 디렉토리에 나누어져 있다.

```
/usr/include
/usr/include/sys
/usr/include/netinet
```

* 함수 사용을 위해 필요한 헤더파일 및 컴파일 시 필요한 라이브러리들은 `man 함수이름` 명령어를 이용해 알 수 있다.
* **근데, 실습하다 알아보니.. <netdb.h> 하나 포함하면 웬만한거 많이 포함된다. socket.h나 in.h 등등**

<br/>

#### 소켓주소 구조체 ★

* 클라이언트 or 서버의 구체적인 주소 표현을 위해서는 **주소체계**(address family), **IP 주소**, **포트번호** 세 가지가 지정되어야 하며, 이 정보를 묶어 **소켓주소**(socket address)라고 부른다.
* **소켓주소**를 담을 구조체 **sockaddr**은 다음과 같고, 2byte의 **address family**와 14byte의 **주소(IP+port)**로 구성되어 있다.
* 추후 TCP, UDP 소켓통신 프로그램에서 보내고 받는 함수의 매개변수 형식으로 이 타입을 써서 casting 필요!

```c
struct sockaddr {
    u_short sa_family;	// address family
    char sa_data[14];	// 주소
};
```

<br/>

#### 개선한 소켓주소 구조체 sockaddr_in ★

* 하지만 **인터넷 프로그래밍**에서는 더 개선한 방법으로, **sockaddr** 구조체 대신 **sockaddr_in**을 쓴다.
* 인터넷 전용 소켓주소 구조체 **sockaddr_in**은 4byte의 **IP 주소**와, 2byte의 **포트번호**를 구분하여 액세스할 수 있다.
* **sockaddr_in**은 다시 32비트의 IP 주소를 저장하기 위해 **in_addr** 구조체를 사용한다.

```c
#include <netinet/in.h>
struct in_addr {
    u_long s_addr;		// 32비트의 binary IP 주소를 저장할 구조체
};

struct sockaddr_in {
    short	sin_family;			// 주소 체계
    u_short	sin_port;			// 16비트 포트번호
    struct	in_addr sin_addr;	// 32비트 IP 주소
    char	sin_zero[8];		// 전체 크기를 16바이트로 맞추기 위한 dummy
};
```

* 위 구조체들은 **in.h 헤더파일에 정의**되어 있다. (netdb.h에 포함)
* **sin_family** : 주소 체계
  * 인터넷 주소 체계를 사용하려면 **AF_INET**(address family)를 선택하면 된다.
  * **socket()으로 소켓을 개설할 때 프로토콜을 PF_INET으로 지정했으면 AF_INET만 사용 가능하다.**
  * **PF_INET**과 **AF_INET**의 값은 **2**로 같다.

<br/>

#### 소켓 사용 절차

* 간단하게 설명하면, 클라이언트와 서버가 각각 TCP 소켓을 개설하고 서로 연결한 다음 데이터를 송수신하고 소켓을 닫아 연결을 해제한다.

<br/>

#### 서버측

* 클라이언트 - 서버 통신 모델에서는 **서버 프로그램이 먼저 수행**되고 있어야 한다.

* **socket() call**을 통해 server는 먼저 **통신에 사용할 소켓을 개설**한다.
  * 이 때 socket()이 성공적으로 호출되면 소켓 번호가 반환된다.

* **bind()**
  * **bind() call**을 통해 통신에 사용할 **소켓번호**와 자신의 **소켓주소**를 연결시켜 둔다.
  * 서버에서 **bind()가 필요한 이유는, 소켓번호는 응용 프로그램 내에서만 알고있는 통신 창구번호이고, 소켓주소는 네트워크 시스템이 알고있는 주소이므로 이들의 관계를 묶어두어야 응용 프로세스와 네트워크 시스템 간에 데이터 전달이 가능하기 때문이다.** ★

* **listen()**
  * **listen() call**을 통해 수동 대기모드로 들어가며, 클라이언트로부터 오는 **연결 요청**(connect())을 처리할 수 있는 상태이다.

* **accept()**
* client측에서 연결 요청(connect())이 들어올 때 까지 **block 상태**를 유지한다.
  * 연결 요청이 들어와 성공하면, 해당 **클라이언트와 통신하는데 쓰기 위한 새로운 소켓**을 반환한다.

* send()  // or write()
  * 보내기
* recv()  // or read()
  * 받기
* close()
  * 소켓(파일) 닫기

<br/>

#### 클라이언트측

* **socket()**
  * client 역시 먼저 **socket() call**을 통해 **통신에 사용할 소켓**을 만든다.
  * 반환 받은 소켓이 해당 클라이언트가 **서버와 연결하기 위한 소켓 번호**이다.

* **connect()**
  * client는 접속할 상대 서버의 소켓 주소 구조체를 만들어 **connect()** 함수의 인자로 준다.
  * client는 **bind()**를 호출하지 않는데, 자신의 IP 주소나 포트번호를 지정해 둘 필요가 없기 때문이다.
  * 서버에 연결하여 서비스만 이용하면 되기 때문에 특정 포트번호를 사용할 이유가 없다.
  * 따라서 시스템이 자동으로 배정하는 포트번호를  사용한다.
  * server에서 연결 요청을 허용하기 전까지 **block**이 걸려있는 상태가 유지된다.

* recv()  // or read()
  * 받기
* send()  // or write()
  * 보내기
* close()
  * 소켓(파일) 닫기

<br/>

### 2.2 인터넷 주소 변환

* 바이트 순서변환
  * 숫자 표현 방식 차이에 의한 에러 방지를 위해서 바이트 순서 맞추는 절차 필요
* 인터넷 주소변환
  * 도메인 네임, 32비트 IP주소, 십진수(dotted decimal) 표시법 등 세 가지 방식 존재

<br/>

#### 2.2.1 바이트 순서 ★

* **호스트 바이트 순서** : 컴퓨터가 **내부 메모리에 숫자를 저장하는 순서**로, **CPU** 종류에 따라 다르다!
  * 인텔 계열(80x86 계열)
    * 하위 바이트(little end)가 메모리에 먼저 저장된다고 해서 little-endian이라 한다.
  * 모토롤라 계열(MC68000 계열)
    * 상위 바이트(big end)가 메모리에 먼저 저장된다고 해서 big-endian이라 한다.
* **네트워크 바이트 순서** : **포트번호**나 **IP 주소** 같은 정보를 바이트 단위로 **네트워크로 전송하는 순서**
  * 네트워크 바이트 순서는 **high-order 바이트부터 전송하기로 정했다**. (big-endian 순서)
  * 즉, **인텔 계열의 CPU에서는 바꿔서 전달**해야 한다는 것!

<br/>

* 이런 문제를 해결하기 위해 바이트 순서를 바꿔주는 함수를 제공한다.

* Unsigned short integer 변환 (2byte)
  * **htons()** : host-to-network short 바이트 변환
  * **ntohs()** : network-to-host short 바이트 변환
* Unsigned long integer 변환 (4byte)
  * **htonl()** : host-to-network long 바이트 변환
  * **ntohl()** : network-to-host long 바이트 변환

<br/>

* 즉, 결론적으로 **intel** 계열에서는 **htons(), ntohs() 함수를 이용해 바이트 순서를 바꿔준다**. 하지만 모토롤라 계열에서는 호스트 바이트와 네트워크 바이트 순서가 같았으므로 htons(), ntohs() 함수는 **실제로는 아무 작업도 수행하지 않게 된다**.
* 또한 주의할 점은, 바이트 순서를 맞추는 것이 필요한 것은 **포트번호**와 같이 **어떤 숫자를 네트워크로 전송할 때**이다. 일반 데이터는 바이트 변환이 필요 없다. Queue 형식의 버퍼에 저장되었다가 전송되기 때문! 수신측에서도 수신된 데이터를 바이트 순서로 차례대로 저장하기 때문!

<br/>

#### 바이트 순서 확인 예

* 현재 사용중 PC의 **호스트 바이트 순서**가 **네트워크 바이트 순서**와 같은지 알아보는 프로그램
* UDP를 사용하는 echo 서비스의 포트번호를 알아내기 위해 **getservbyname()** 을 호출한다.

```c
#include <netdb.h>
pmyservent = getservbyname("echo", "udp");
```

* 해당 함수는 **서비스 이름**과 **프로토콜**을 인자로 호출한다.
* 서비스와 관련된 각종 정보를 포함하는 **servent 구조체의 포인터를 리턴**한다. ★

<br/>

#### servent 구조체 ★

```c
#include <netdb.h>
struct servent {
	char *s_name;		// 서비스 이름
    char **s_aliases;	// 별명 목록
    int  s_port;		// 포트번호
    char *s_proto;		// 사용하는 프로토콜
}
```

* servent 구조체는 <**netdb.h**> 에 저장되어 있다.

<br/>

#### 네트워크 바이트 순서와 호스트 바이트 순서가 일치하는지 확인하는 출력 예시

```c
//--------------------------------------------------------------------
// 파일명: byte_order.c
// 기  능: 호스트 바이트 순서 테스트 프로그램
// 컴파일: cc -o byte_order byte_order.c -lsocket
// 사용법: byte_order
//--------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>	// 위 두 헤더파일은 이것만 선언해도 포함 됨

int main(void) {
	struct servent *servent;
	servent = getservbyname("echo", "udp");
	if(servent==NULL) {
		printf("서비스 정보를 얻을 수 없음. \n\n");
		exit(0);	
	}
	
	printf("UDP 에코 포트번호 (네트워크 순서) : %d\n", servent->s_port);
	printf("UDP 에코 포트번호 (호스트 순서) : %d\n", ntohs(servent->s_port));
    // cpu가 인텔인 경우에는 반드시 ntohs()를 이용해 바이트 순서를 바꿔주어야 한다.
	
	return 0;
}
```

* 결과

```
./byte_order

UDP 에코 포트번호 (네트워크 순서) : 1792
UDP 에코 포트번호 (호스트 순서) : 7
```

* 네트워크 순서와 호스트 순서가 같으면 모토롤라 계열, 다르면 인텔 계열의 CPU를 사용한다.
* 우리는 인텔 계열이므로, **네트워크에서 온 정보를 받을 때는 항상 ntohs() 함수로 받아야 한다**.

<br/>

#### 2.2.2 IP 주소 변환

* 4byte(32비트)의 IP 주소를 편의에 따라 github.com과 같은 **domain name**, 또는 127.0.0.1 과 같은 **dotted decimal** 방식으로 한 바이트씩 나누어 표현하여 사용한다.
* 단, **dotted decimal**의 경우 숫자 변수가 아니라 **15개의 문자**로 구성된 **스트링 변수**가 사용된다!!
* 우리가 쓰는건 **도메인**과 **dotted decimal**, **프로토콜에서는 binary 주소**를 사용한다.

<br/>

* 소켓 프로그램에서는 이들 주소 표현법의 상호 보완을 위해 함수를 제공한다.
* **gethostbyname()** : domain to binary
  * 이름과 DNS주소를 통해서 받아온다.
* **gethostbyaddr()** : binary to domain
  * binary 주소를 통해서 받아온다.
* **inet_pton()** : presentation to numerical
  * 문자열로 표현된 dotted decimal을 binary 주소로 변환한다.
  * 단순 수학적 계산
* **inet_ntop()** :  numerical to presentation
  * binary 주소를 문자열 dotted decimal로 변환한다.
  * 단순 수학적 계산

<br/>

#### Dotted decimal과 Binary IP 주소간 변환 ★

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
const char *inet_ntop(int af, const void *src, char *dst, size_t cnt);
int *inet_pton(int af, const char *src, void *dst);
```

* **af **: 주소체계(address family)로, 앞서 본 것처럼 인터넷 주소체계를 사용하려면 **AF_INET**를 쓴다.
* **src** : 변환하려는 주소
  * ntop에서는 **binary address**
  * pton에서는 **DNS**주소
* **dst** : 변환된 결과를 담을 주소라고 생각하면 된다.
  * ntop에서는 **string을 담을 포인터**. 보통 **char형 배열**로 **buf**를 준다.
  * pton에서는 **binary address를 담을 소켓주소 구조체 in_addr의 포인터**
* **cnt** : string을 저장할 버퍼 buf의 크기를 함께 넣어준다.
  * 보통 **sizeof(buf)** 등으로 넣는다.
* numerical to presentation은 binary -> dotted decimal
* presentation to numerical은 dotted decimal -> binary

<br/>

#### Dotted decimal IP를 binary로 바꿔 출력, 다시 Dotted decimal 로 바꿔 출력하는 예제

```c
//--------------------------------------------------------------------
// 파일명: ascii_ip.c
// 기  능: ASCII (dotted decimal)로 포현된 주소를 4 바이트 IP 주소로 변환
// 컴파일: cc -o ascii_ip ascii_ip.c
// 사용법: ascii_ip 192.203.144.11
//--------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // in_addr 정의
#include <arpa/inet.h> // inet_pton(), inet_ntop() 정의
#include <netdb.h>

int main(int argc, char *argv[]) {
	struct in_addr inaddr;	// 32비트 IP주소 구조체
	char buf[20];

	if(argc < 2) {
		printf("사용법 : %s IP 주소(dotted decimal) \n", argv[0]);
		exit(0);
	}
	printf("*입력한 dotted decimal IP 주소: %s\n", argv[1]);

	inet_pton(AF_INET, argv[1], &inaddr.s_addr);
	printf(" inet_pton(%s) = 0x%X\n", argv[1], inaddr.s_addr);
	inet_ntop(AF_INET, &inaddr.s_addr, buf, sizeof(buf));
	printf(" inet_ntop(0x%X) = %s\n", inaddr.s_addr, buf);

	return 0;
}
```

* 결과

```
ascii_ip 192.203.144.11

* 입력한 dotted decimal IP 주소: 192.203.144.11
 inet_pton(192.203.144.11) = 0xB90CBC0
 inet_ntop(0xB90CBC0) = 192.203.144.11
```

* 매개변수로 문자열 형태의 dotted decimal 주소를 준다.
* inet_pton()을 이용해 4byte의 binary IP 주소로 바꾸어 출력한다.
* inet_ntop()를 이용해 binary IP를 다시 dotted decimal 주소로 바꾸어 출력한다.

<br/>

#### 도메인과 Binary IP 주소간 변환 ★

* 앞서 binary와 dotted decimal 간의 변환은 단순히 **수학적 계산**이었다면, domain 변환은 다르다.
* **DNS** (Domain name Service) 서버의 도움을 받아야 한다.
* <**netdb.h**> 헤더파일에 정의되어 있다.

```c
#include <netdb.h>
struct hostent *gethostbyname(const char *hname);
struct hostent *gethostbyaddr(const char *in_addr, int len, int family);
```

* **gethostbyname** : domain -> binary
  * 매개변수로 **domain**을 주고, 호스트의 정보를 가지고 있는 **hostent 구조체의 포인터**를 반환한다.
* **gethostbyaddr** : binary -> domain
  * 매개변수로 **in_addr**의 포인터(32비트 **binary 주소**), 주소의 **길이**, 주소 타입(**AF_INET**)을 주고, **hostent 구조체 포인터**를 반환한다.
  * 최근 보안 문제로 대부분의 DNS 서버가 Reverse DNS 기능을 제공하지 않고 있다.

<br/>

#### hostent (호스트 정보) 구조체 정의 ★

```c
struct hostent {
    char*	h_name;			// 호스트 이름
    char**	h_aliases;		// 호스트 별명들
    int 	h_addrtype;		// 호스트 주소의 종류 (AF_INET = 2 등 주소체계)
    int 	h_length;		// 주소의 크기 (바이트 단위, IPv4에서는 4byte)
    char**	h_addr_list;	// IP 주소 리스트
};
#define	h_addr h_addr_list[0] // 첫 번째 (대표) 주소
```

<br/>

#### Domain to Binary, Binary to Domain

* 도메인 네임을 인자로 줘서 해당 호스트 hostent 구조체 정보를 구해, 구조체 내용을 출력해보는 예제
* hostent 구조체 속의 binary 주소를, inet_ntop 함수를 이용해 dotted decimal로 바꾸어 출력하는 것이다.

```c
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
#include <netinet/in.h> // in_addr 정의
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> // gethostbyname() 정의
#include <errno.h>

int main(int argc, char *argv[]) {
	struct hostent *hp;
	struct in_addr in;
	int i;
	char buf[20];

	if(argc<2) { // 인자가 작으면
		printf("Usage: %s hostname\n", argv[0]);
		exit(1);
	}
	hp = gethostbyname(argv[1]);
	if(hp==NULL) {
		printf("gethostbyname fail\n");
		exit(0);
	}
	printf("호스트 이름		: %s\n", hp->h_name);
	printf("호스트 주소타입 번호	: %d\n", hp->h_addrtype);
	printf("호스트 주소의 길이	: %d\n", hp->h_length);
	for(i=0; hp->h_addr_list[i]; i++) {
		memcpy(&in.s_addr, hp->h_addr_list[i], sizeof(in.s_addr));
		inet_ntop(AF_INET, &in, buf, sizeof(buf)); // &in이나 &in.s_addr이나 상관없다.
		printf("IP 주소(%d 번째) : %s ", i+1, buf); // 구조체 멤버가 하나 밖에 없기 때문
	}
	for( i=0; hp->h_aliases[i]; i++) {
		printf("호스트 별명(%d 번째) : %s ", i+1, hp->h_aliases[i]);
	}
	puts("");
	return 0;
}
```

* 결과

```
get_hostent css.skuniv.ac.kr

호스트 이름            : skuniv.ac.kr
호스트 주소타입 번호    : 2
호스트 주소의 길이      : 4
IP 주소(1 번째) : 117.17.142.162
호스트 별명 : css
```

* 목적지 호스트의 도메인 네임을 인자로 받아, gethostbyname()을 이용해 목적지 호스트의 hostent 구조체를 얻는다.
* hostent 내의 호스트 이름, 별명, 주소체계, dotted decimal 인터넷 주소 등을 출력한다.

<br/>

#### Binary to Domain

* binary 주소로부터 도메인 네임을 얻기 위해서는 **gethostbyaddr()** 를 사용하면 된다.
* 즉, 사용자가 보기에는 **Dotted decimal** 주소를 **Domain**으로 변경해주는 것 같아 보이지만, 사실 수학적 계산을 통해 dotted decimal 주소를 **binary** 주소로 변환한 후 그 **binary 주소를 통해 domain을 얻어오는** 함수가 **gethostbyaddr()** 이다.
* 현재 사용중인 자신의 컴퓨터의 도메인 이름을 얻으려면 **uname()**이나 **gethostname()**을 써도 된다.

<br/>

#### Dotted decimal 주소를 인자로 주어 도메인 네임을 출력하는 예제

```c
//-----------------------------------------------------------------------
// 파일명: get_host_byaddr.c
// 동  작: hostent 구조체 내용 출력 프로그램
// 컴파일: cc -o get_host_byaddr get_host_byaddr.c
// 실  행: get_host_byaddr 192.203.144.27
//-----------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // in_addr 정의
#include <arpa/inet.h> // inet_ntop, inet_pton
#include <netdb.h> // gethostbyaddr() 정의

int main(int argc, char *argv[]) {
	struct hostent *myhost;
	struct in_addr in;

	if(argc<2) { // 인자가 작으면
		printf("사용법: %s ip_address \n", argv[0]);
		exit(0);
	}
	
	inet_pton(AF_INET, argv[1], &in.s_addr); // dotted decimal -> 32bit 주소
	myhost = gethostbyaddr((char*)&(in.s_addr), sizeof(in.s_addr), AF_INET);
						  // typecasting. 안해도 상관은 없는듯 하다.
    
	if (myhost == NULL) {
		printf("Error at gethostbyaddr()\n");
		exit(0);
	}
	printf("호스트 이름: %s\n", myhost->h_name);
	return 0;
}
```

* 결과

```
./get_host_byaddr 127.0.0.1

호스트 이름: localhost
```

<br/>

### 2.3 TCP 클라이언트 프로그램

#### 2.3.1 TCP 클라이언트 프로그램 작성 절차

* 클라이언트는 먼저 **socket()**으로 소켓을 개설하고 **connect()**를 호출하여 서버에게 **연결을 요청**한다.
* 연결이 이루어지면 **send()와 recv()**를 사용하여 데이터를 송수신하고, 작업이 종료되면 **close()**로 소켓을 닫는다.

<br/>

#### socket(), 소켓 개설

* socket()으로 소켓을 개설할 때 연결형(**TCP**) 또는 비연결형(**UDP**) 소켓을 선택해야 한다.

* **연결형**은 서비스 type인자를 **SOCK_STREAM**으로, **비연결형**은 **SOCK_DGRAM**으로 한다.

* 소켓 통신 프로그램에서는 사용할 트랜스포트 프로토콜, 자신의 IP 주소와 포트번호, 상대 IP 주소와 포트번호 등 다섯 개의 정보가 지정되어야 한다.

* 소켓 "**만들 때**"는 트랜스포트 **프로토콜**만 지정한다.

<br/>

#### socket() 함수 사용 문법 - 이전 내용 복습

```c
int socket(
    int domain,		// 프로토콜 체계
    int type,		// 서비스 타입 -> SOCK_STREAM(TCP) 또는 SOCK_DGRAM(UDP)
    int protocal);	// 소켓에서 사용할 프로토콜 -> 위에서 이미 결정 되었으므로 0!
```

<br/>

#### connect(), 서버에 연결요청

* 클라이언트가 connect()를 호출해 서버에게 연결요청을 하며, 이 때 **3-way handshake**가 시작된다.
  * 3-way handshake : 연결형 서비스에서 서버와 클라이언트간 연결을 확인하기 위해 세 번 명령어를 주고 받는 것.
    * 먼저 클라이언트가 서버에게 연결요청을 하고 (SYN)
    * 서버는 연결요청 (SYN)을 받고, 클라이언트에게 요청을 수락한다 (+ ACK)
    * 클라이언트는 서버의 응답을 (SYN + ACK) 잘 받았다고 (ACK) 다시 확인을 보낸다

<br/>

#### connect() 사용 문법 ★

```c
int connect (
	int s,							// 서버와 연결시킬 소켓번호
	const struct sockaddr *addr,	// 상대방 서버의 소켓주소 구조체 typecasting을 꼭 해줘야함 ★
	int addrlen);					// 구조체 *addr의 크기
```

* **addr**은 연결할 서버의 IP 주소 및 2byte의 포트번호를 포함하는 소켓주소 구조체 **sockaddr_in**을 저장
* 3 way handshake가 성공하여 서버 연결시, connect()는 0을 반환한다. 실패하면 -1을 반환
* 호출 중 문제가 생기면, 반드시 close()로 해당 소켓을 닫고 새로운 socket을 만든 후 사용하는게 안전

<br/>

#### sockaddr - 이전 내용 복습

```c
struct sockaddr {
    u_short sa_family;	// address family
    char sa_data[14];	// 주소
};
```

<br/>

#### in_addr, sockaddr_in - 이전 내용 복습

```c
// 위에꺼 복습
struct in_addr {
    u_long s_addr;		// 32비트의 IP 주소를 저장할 구조체
};

struct sockaddr_in {
    short	sin_family;			// 주소 체계
    u_short	sin_port;			// 16비트 포트번호
    struct	in_addr sin_addr;	// 32비트 IP 주소
    char	sin_zero[8];		// 전체 크기를 16바이트로 맞추기 위한 dummy
};
```

<br/>

#### send(), recv() 데이터 송수신 ★

* 클라이언트와 서버가 연결 되면 소켓의 송수신 함수를 이용해 데이터를 주고받는다.


```c
int send(int s, char* buf, int length, int flags);
int write(int s, void* buf, int length);
```

* TCP type의 STREAM형 소켓을 통하여 데이터를 송신하는 함수이다.
* **s** : 송신할 **소켓번호**
* **buf** : 송신할 데이터 **버퍼**
* **length** : 전송할 데이터 크기 
* **flags** : 보통 **0**
* 함수 호출에 성공하면 실제 **전송된 데이터 크기를 바이트 단위**로 반환한다.

<br/>

```c
int recv(int s, char* buf, int length, int flags);
int read(int s, void* buf, int length);
```

* TCP type의 STREAM형 소켓을 통하여 데이터를 수신하는 함수이다.
* **s** : 수신할 **소켓번호**
* **buf** : 수신할 데이터 **버퍼**
* **length** : 읽을 데이터 크기
* **flags** : 보통 **0**
* 함수 호출에 성공하면 실제 **읽은 데이터 크기를 바이트 단위**로 반환한다.

<br/>

#### TCP에서

* write()나 send()를 실행하면 데이터는 먼저 TCP 계층에 있는 **송신버퍼**(send buffer)로 들어간다.
* 만약 버퍼가 비어있지 않으면 프로세스는 블록 상태로 가며 프로그램이 write() 상태에서 기다린다.
* 송신버퍼에 들어있던 데이터가 전송되고 write한 데이터가 송신버퍼로 모두 이동하면 write()가 리턴된다.
* 즉, **write()문이 리턴 되었다는 것은 데이터가 TCP 계층의 송신버퍼로 들어갔다는 의미**이다.
* **데이터가 목적지에 전달 되었다는 의미가 아님**을 주의하자.

<br/>

#### close(), 소켓 닫기

* 소켓 사용을 마칠 때 반드시 close()로 소켓을 닫아야한다.
* 클라이언트나 서버 둘 중 누구나 먼저 호출 가능하다.
* 기본적으로 전송중인 데이터가 모두 전달된 후에 TCP 연결이 종료된다.
* 옵션을 주어 버퍼를 버리고 강제 종료도 가능하다.

<br/>

#### 2.3.2 TCP 클라이언트 예제 프로그램

<br/>

#### daytime 클라이언트 프로그램 예제

* 서버에서 제공하는 daytime 서비스를 이용한 클라이언트 프로그램 예제 코드

```c
//---------------------------------------------
// 파일명 : mydaytime.c
// 기  능 : daytime 서비스를 요청하는 TCP(연결형) 클라이언트
// 컴파일 : cc -o mydaytime mydaytime.c
// 사용법 : mydaytime 203.252.65.3
//---------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	int s, nbyte;
	struct sockaddr_in servaddr; // 서버 소켓 주소 구조체
	char buf[ MAXLINE + 1 ];

	if(argc != 2) {
		printf("Usage: %s ip_address\n", argv[0]);
		exit(0);
	}

	// 소켓 생성  // 프로토콜체계 인터넷, 서비스 TCP
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}
	// 서버의 소켓주소 구조체 servaddr을 '\0'으로 초기화
	bzero((char*)&servaddr, sizeof(servaddr));
	// servaddr의 주소 지정
	servaddr.sin_family = AF_INET; // 주소체계 인터넷
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr); // dotted decimal to binary
	servaddr.sin_port = htons(13); // well-known port 13

	// 연결 요청  // 소켓 디스크립터, 서버 소켓주소 구조체(Type casting)
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail"); // 연결 실패시 -1 반환
		exit(1);
	}
	// 서버가 보내오는 daytime 데이터의 수신 및 화면 출력
	 if((nbyte = read(s, buf, MAXLINE)) < 0) {
		perror("read fail");
		exit(1);
	 }
	 buf[nbyte] = 0; // 문자열의 끝
	 printf("%s", buf);
	 close(s);
	 return 0;
}
```

* 결과

```c
./mydaytime 129.6.15.28

59133 20-10-11 15:17:56 22 0 0 642.5 UTC(NIST) *
```

* bzero()는 0으로 set하기 위해 사용하였다. 대신에 memset() 도 사용 가능하다.
* 단, strcpy() 등의 C string은 안된다. 기본적으로 문자열 끝에 자동으로 NULL 문자를 추가하기 때문에 통신 프로그램에서는 C string으로 다루면 안된다.
* 보안 문제로 인해 많은 서버들이 daytime 서비스를 제공하지 않는다.
* 따라서.. daytime 서비스를 제공하는 미국의 한 사이트 ip를 매개변수로 주었다.

<br/>

#### TCP 에코 클라이언트

* 에코는 well-known 포트 7번을 통해 제공되며, 클라이언트가 보낸 문자열을 다시 클라이언트로 전송한다.
* 앞선 daytime 코드와 거의 유사하게 작성된다.

<br/>

* dotted decimal 주소를 인자로 주어 해당 서버에 문자열을 보낸 후, 다시 클라이언트로 보낸 문자열을 반환 받는 echo 서비스 예제

```c
//-------------------------------------------------------
// 파일명 : tcp_echocli.c
// 기  능 : echo 서비스를 요청하는 TCP(연결형) 클라이언트
// 컴파일 : cc -o tcp_echocli tcp_echocli.c
// 사용법 : tcpi_echocli 203.252.65.3
//-------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	int s, nbyte;
	struct sockaddr_in servaddr; // 서버 소켓 주소 구조체
	char buf[ MAXLINE + 1 ];

	if(argc != 2) {
		printf("Usage: %s ip_address\n", argv[0]);
		exit(0);
	}

	// 소켓 생성  // 프로토콜체계 인터넷, 서비스 TCP
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}
	// 에코 서버의 소켓주소 구조체 작성
	bzero((char*)&servaddr, sizeof(servaddr));
	// servaddr의 주소 지정
	servaddr.sin_family = AF_INET; // 주소체계 인터넷
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr); // dotted decimal to binary
	servaddr.sin_port = htons(7); // well-known port 7

	// 연결 요청  // 소켓 디스크립터, 서버 소켓주소 구조체(Type casting)
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail"); // 연결 실패시 -1 반환
		exit(1);
	}

	printf("입력 : ");
	if (fgets(buf, sizeof(buf), stdin) == NULL) // 키보드로부터 입력 받기
		exit(0);

	nbyte = strlen(buf); // 입력받은 문자열 길이
	// 에코 서버로 메시지 송신
	if (write(s, buf, nbyte) < 0) { // 소켓
		printf("write error\n");
		exit(0);
	}

	// 수신된 에코 데이터 화면출력
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
```

* 결과

```
./tcp_echo 127.0.0.1

입력 : 서버가 다들 echo서비스를 안해줍니다.
수신 : 서버가 다들 echo서비스를 안해줍니다.
```

* echo 서비스는 다들 안해주더라. 어떤걸 해봐도.. 그래서 실습 해보진 못했다.

<br/>



<br/>

<br/>

<br/>

<br/>

<br/>

71P 부터 6주차 시간~

* Iterative : 들어온 클라이언트 순서대로 서비스를 제공하는 방식. TCP, 연결형 서버이다.
* concurent : 요청해 온 클라이언트를 일정 시간을 할당하며 연속적으로 (사용자가 동시로 느끼게) 제공한다.

<br/>

### 2.4 TCP 서버 프로그램

#### 2.4.1 TCP 서버 프로그램 작성 절차

<br/>

#### socket(), 소켓의 생성

* **서버**도 클라이언트 처럼 **통신을 위해 소켓**을 만들어야 한다.

* 클라이언트와 마찬가지로 연결형(**TCP**) 소켓을 위해서는 type 인자로 **SOCK_STREAM**을 지정해야 한다.

  * ```c
    socket(PF_INET, SOCK_STREAM, 0);
    ```

<br/>

#### bind()

* **socket()**으로 생성된 소켓은 해당 프로그램의 **유일한 소켓 번호**를 배정 받는다.
* 하지만 이 번호는 **응용 프로그램만 알고 있는 번호**이므로 **외부와 통신**하려면 이 **소켓번호와 소켓 주소**(IP+port)를 **연결해 두어야 한다**. 이 때 bind()가 사용된다.
* 서버에서 반드시 bind()가 필요한 이유는, 임의의 클라이언트가 서버 프로그램 특정 소켓으로 접속하려면 서버는 자신의 소켓번호와 클라이언트가 알고있는 **자신의 IP 주소 및 포트번호**(= 서버 프로그램의 소켓 주소)를 미리 연결시켜 두는 것이 필요하기 때문이다.

<br/>

#### ★ **플라스틱 프로젝트 서버 호스팅 한 것을 생각해보자** ★

* 서버 호스팅 할 때 docker에서 port binding 해줬던 것!!
* -p 명령어로 8080:8080 해주는 것과 같다. 
* 그럼 클라이언트(웹 브라우저)에서 접속할 때는 호스팅한 서버 IP와 그에 해당하는 port를 함께 써주어 접속할 수 있는 것이다.
* 예를 들어 20.41.104.177:8080 (IP:PORT) 로 접속했던 것 처럼!

<br/>

#### bind() 사용 문법 ★

```c
int bind(
    int s,					// 소켓 번호
    struct sockaddr *addr,	// 서버 자신의 소켓 주소 구조체 포인터
    int len);				// *addr 구조체의 크기
```

* 성공하면 **0**, 실패하면 **-1**을 반환한다.
* 인자 **s**는 바인드 시킬 **소켓 번호**(서버의 소켓 번호)이고, **addr**은 서버의 자신의 **소켓 주소** 구조체를 가리킨다.

<br/>

#### **IP 주소 203.252.65.3, 포트번호 3008 인 소켓 구조체와 bind() 하는 예**

```c
#define SERV_IP_ADDR "203.252.65.3"
#define SERV_PORT 3008

// 소켓 생성
s = socket(PF_INET, SOCK_STREAM, 0);
struct sockaddr_in servaddr;

// 소켓주소 구조체 내용
servaddr.sin_family = AF_INET;
inet_pton(AF_INET, SERV_IP_ADDR, &servaddr.sin_addr);
//servaddr.sin_addr.s_addr = inet_addr(SERV_IP_ADDR); 도 가능
servaddr.sin_port = htons(SERV_PORT);

// 소켓번호와 소켓 주소를 bind
bind(s, (struct sockaddr *)&servaddr, sizeof(servaddr));
```

* 위의 경우, IP를 직접 지정하여 주었는데, 서버가 자신의 IP 주소를 자동으로 가져다 쓰려면 `INADDR_ANY`를 사용할 수 있다.

```c
servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
```

* ANY의 의미는, 서버가 두 개 이상의 IP 주소를 가지고 있을 때 임의의 서버 IP 주소를 목적지로 하여 들어오는 데이터그램을 모두 받아 들이겠다는 의미를 나타낸다.
* 또한 특정 IP 주소 대신 INADDR_ANY를 사용함으로써 프로그램의 **이식성**이 좋아진다.

<br/>

#### listen()

* 클라이언트의 TCP socket은 **서버 연결 요청**을 위해 사용되고, 서버에서 TCP socket은 **연결 요청을 받기 위해** 쓰인다.
  * 보내는 클라이언트 소켓은 **능동적** 소켓이라 하며, 받는 서버 소켓은 **수동적** 소켓이라 한다.
  * socket() 에 의해 생성되는 소켓은 **기본적으로 능동적 소켓**이다.
  * 따라서 이를 **수동적 소켓으로 바꿔주기 위해 listen()**을 사용한다.

<br/>

#### listen() 사용 문법 ★

```c
int listen(
	int s,			// 소켓번호
	int backlog);	// 연결을 기다리는 클라이언트의 최대 수
```

* 클라이언트는 **listen**을 통해 **수동적 소켓**으로 바꾸어 놓은 **서버 소켓 번호**를 목적지로 **connect()**를 호출한다.
* 3-way handshake가 시작되고, 커널(시스템)은 이를 마친 후 **설정된 연결**을 **받아들이는 과정**이 필요하다.
* 이 때 쓰이는 것이 다음에 나올 accept()인데, 한 번에 **하나의 연결**만 가져간다.
* 여러 요청이 동시에 들어올 경우 커널은 설정된 연결들을 accept 큐에 넣어두고 호출될 때마다 하나씩 꺼낸다.
* listen의 인자 **backlog**는 이 **큐**에 **대기**시킬 수 있는 **연결의 최대 수**를 의미한다.
* (iterative 형식은 순차적 지원 형식이므로 대기 가능 클라이언트의 최대 수를 넣어 제한한다)

<br/>

* 리눅스 커널 버전에 따라 다르지만, 2.2 이후로는 큐에 대기하는 연결은 handshake가 완료된 연결들이다.
* 여러 유닉스 버전에 따라 다르기 때문에, 이 backlog에 의존해서는 안된다.
* 예를 들어 아래의 경우를 보자.
* 2개의 connect() 요청 후 accept() 처리 하지 않으면 다음에 요청되는 connect() 요청들은 무시된다.
* Connection Refuesed나 Connection time out 등이 발생하는 것이다.

```c
listen(s, 2);
```

* 소켓을 단지 수동 대기모드(수동적 소켓)로 바꿔주는 것이므로, 성공시 **0**, 실패시 **-1**을 즉시 리턴한다.

<br/>

#### accept()

* 서버가 listen() 호출 후에, 실제로 client의 연결 요청을 받아들이기 위해 accept()를 호출한다.
* 이 때, 서버는 연결 요청이 들어오기 전까지 block 상태가 된다.

<br/>

#### accept() 사용 문법 ★

```c
int accept(
    int s,					// 소켓 번호
    struct sockaddr *addr,	// 연결요청을 한 클라이언트의 소켓주소 구조체
    int *addrlen);			// *addr 구조체 크기의 포인터
```

* accept() 수행이 성공하면 접속된 클라이언트와 사용될 새로운 소켓이 만들어지고, 이 소켓번호가 반환된다.
* 실패 시에는 -1이 반환된다.
* 즉, 다시 정리하면 **연결된 클라이언트와 소통할 새 소켓번호**가 리턴 되고, **accept()** 함수의 인자로 준 구조체와 크기 포인터에 **클라이언트 소켓주소 구조체**와 그 **구조체의 길이**가 저장된다.

<br/>

#### TCP 3-way handshake 연결 동작 ★★★

* 클라이언트 측에서 **connect()**를 호출하면, 커널은 **SYN(X)**를 서버측으로 전송한다.
* 서버측 커널은 이를 수신하고 **ACK(X+1)과 SYN(Y)**로 응답한다.
* 응답에 대해 잘 받았다고 클라이언트측 커널은 다시 **ACK(Y+1)**을 보냄으로써 3-way handshake가 완성된다.
* 하지만 만약 서버측이 **listen()**을 호출하지 않았다면 서버측은 **RST(Reset)**를 응답한다.
* 그러면 클라이언트 측은 **Connect Refused** 에러를 리턴한다.
* 즉, 서버측 커널이 **3-way handshake**를 하려면 반드시 **listen()**이 호출되어 있어야 한다. 
* 이제 서버측 커널은 accept 큐에 이 연결을 전달하고, **accept()**를 호출하여 큐에서 하나의 연결을 꺼내가면 그 때 클라이언트의 **connect()** 함수가 리턴하고 데이터 송수신이 가능해진다.

<br/>

#### 2.4.2 TCP 에코 서버 프로그램

* **accept()**는 새로운 소켓(**accp_sock**)을 생성하며 리턴한다. 또한, 접속된 클라이언트의 소켓주소 정보를 두 번째 인자 (**cliaddr**)를 통해 알려준다.

```c
accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen)
```

<br/>

#### 에코 서비스를 수행하는 서버 코드 예제

```c
//-----------------------------------------------
// 파일명 : tcp_echoserv.c
// 기  능 : 에코 서비스를 수행하는 서버
// 컴파일 : cc -o tcp_echoserv tcp_echoserv.c
// 사용법 : ./tcp_echoserv.c 3008
//-----------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr, cliaddr;	// 소켓 두개
	int listen_sock, accp_sock,		// 소켓 번호
		addrlen=sizeof(cliaddr),	// 소켓주소 구조체 길이
		nbyte,
		count=0;
	char buf[ MAXLINE + 1 ];

	if(argc != 2) {
		printf("usage: %s port\n", argv[0]);
		exit(0);
	}
	// 소켓 생성
	if((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	// servaddr을 '\0'으로 초기화
	bzero((char *)&servaddr, sizeof(servaddr));
	// servaddr 세팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));	// 문자열 포트번호

	// bind() 호출
	if(bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
		exit(0);
	}
	// 소켓을 수동 대기모드로 세팅
	listen(listen_sock, 5);
	// 여기까지 하면 준비 완료!
	
	// iterative 에코 서비스 수행
	while(1) {
		puts("서버가 요청을 기다림");
		// 연결 요청을 기다림. 여기서 block 된다!
		accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
		if(accp_sock < 0) {
			perror("accept fail");
			exit(0);
		}
		printf("%d 번째 클라이언트가 연결됨..\n", ++count);
		nbyte = read(accp_sock, buf, MAXLINE);
		buf[nbyte] = 0;
		printf("%s", buf);
		write(accp_sock, buf, nbyte);
		close(accp_sock);
	}
	close(listen_sock);
	return 0;
}
```

* 결과

```
./tcp_echoserv 3008

server waiting..
server waiting..
server waiting..
server waiting..

```

<br/>

#### 수정된 tcp_echocli1 예제

* 에코 서버의 ip 주소와 포트를 함께 인자로 주어 연결하는 예제 코드

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
        int s, nbyte;
        struct sockaddr_in servaddr;
        char buf[MAXLINE + 1];

        if (argc != 3) { // 이 부분 달라짐 !!!
                printf("Usage: %s ip_address port_num\n", argv[0]);
                exit(0);
        }

        // 소켓 생성
        if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
                perror("socket fail");
                exit(1);
        }

        // 에코 서버 소켓주소 구조체 초기화
        bzero(&servaddr, sizeof(servaddr));

        // servaddr 주소 지정
        servaddr.sin_family = AF_INET;
        //inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
        servaddr.sin_addr.s_addr = inet_addr(argv[1]);
        servaddr.sin_port = htons(atoi(argv[2])); // 이 부분 달라짐 !!!

        // connect 요청
        if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
                perror("connect fail");
                exit(1);
        }

        // 보낼 데이터 입력
        printf("입력 : ");
        if (fgets(buf, sizeof(buf), stdin) == NULL)
                exit(0);
        nbyte = strlen(buf);

        // 에코 서버로 메시지 송신
        if (write(s, buf, nbyte) < 0) {
                perror("write fail");
                exit(0);
        }

        // 수신한 에코 데이터 출력
        if ((nbyte = read(s, buf, MAXLINE)) < 0) {
                perror("read fail");
                exit(0);
        }
        buf[nbyte] = 0;
        printf("수신 : %s", buf);
        close(s);

        return 0;
}
```

* 결과

```c
./tcp_echocli1 127.0.0.1 3008

입력 : input test
수신 : input test
```

<br/>

#### 수정된 tcp_echocli2 예제

* 에코 서버의 도메인 주소와 포트를 함께 인자로 주어 연결하는 예제 코드

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
        int s, nbyte;
        struct sockaddr_in servaddr;
        struct in_addr in;
        struct hostent *hp;
        char buf[MAXLINE + 1];

        if (argc != 3) {
                printf("Usage : %s domain_name port_num\n", argv[0]);
                exit(0);
        }

        // 소켓 생성
        if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
                perror("socket fail");
                exit(0);
        }

        // 도메인을 바이너리로 변환
        hp = gethostbyname(argv[1]);
        memcpy(&in, hp->h_addr_list[0], sizeof(in));

        // 서버 소켓주소 초기화
        bzero(&servaddr, sizeof(servaddr));

        // servaddr 셋팅
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = in.s_addr;
        servaddr.sin_port = htons(atoi(argv[2]));

        // connect() 연결
        if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
                perror("connect fail");
                exit(1);
        }

        // 보낼 데이터 입력
        printf("입력 : ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
                exit(0);
        }
        nbyte = strlen(buf);

        // 데이터 송신 write()
        if (write(s, buf, nbyte) < 0) {
                perror("write fail");
                exit(0);
        }

        // 데이터 수신 read() 및 출력
        if ((nbyte = read(s, buf, MAXLINE)) < 0) {
                perror("read fail");
                exit(0);
        }
        buf[nbyte] = 0;
        printf("수신 : %s", buf);

        // 소켓 close
        close(s);

        return 0;
}
```

<br/>



<br/>

### 2.5 UDP 프로그램

#### 2.5.1 UDP 프로그램 작성 절차

<br/>

* UDP를 이용하는 비연결형 프로그램은 TCP보다 간단하다.
* UDP 소켓 개설 시, type 인자로 **SOCK_DGRAM**을 지정하면 된다.
* 비연결형이므로 연결 설정을 위한 **connect()** 시스템 콜을 사용할 **필요 없다**.
* UDP 소켓을 이용해 데이터를 송수신할 때는 각 데이터그램마다 목적지 **IP 주소**와 **포트번호**(소켓주소)를 항상 함수 인자로 주어야한다.

<br/>

#### sendto()와 recvfrom() 함수 사용법 ★

```c
int sendto(int s, char* buf, int length, int flags,
          sockaddr* to, int tolen)
```

* **s** : **소켓번호**
* **buf** : 전송할 데이터 담긴 **버퍼**
* **length** : 전송할 buf **버퍼의 크기** (바이트단위)
* **flags** : 보통 **0**
* **to** : **목적지의 소켓주소** 구조체
* **tolen** : **to 주소의 크기**

<br/>

```c
int recvfrom(int s, char* buf, int length, int flags,
            sockaddr* from, int* tolen)
```

* **s** : **소켓번호**
* **buf** : 수신할 데이터를 담을 **버퍼**
* **length** : buf **버퍼의 크기** (바이트단위)
* **flags** : 보통 **0**
* **from** : **발신자의 소켓주소** 구조체
* **fromlen** : **from 주소의 길이** (**포인터**) ★

<br/>

#### 2.5.2 UDP 에코 프로그램

<br/>

#### UDP를 이용하는 echo client 프로그램

```c
socket(PF_INET, SOCK_DGRAM, 0);
...
sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, addrlen);
recvfrom(s, buf, MAXLINE, 0, (struct sockaddr *)&servaddr, &addrlen);
```

* **UDP**를 이용하는 에코 클라이언트 프로그램에서는 **소켓 개설**과 **서버로 메시지 보내고 받는 부분**이 달라진다.
* recvfrom의 마지막 인자 **addrlen**은 '**포인터**' 라는 사실을 주의하자.

<br/>

```c
unsigned long inet_addr(const char *string);
```

* **inet_pton()** 함수처럼, **dotted decimal to binary** 간의 변환을 해주는 함수이다.
* char형 포인터 **string을 매개변수**로 주면 **32bit binary 주소 형태**로 변환하여 반환한다.

<br/>

* **소스 코드는 다음과 같다.**

```c
//---------------------------------------------
// 파일명 : udp_echocli.c
// 기  능 : echo 서비스를 요청하는 UDP(비연결형) 클라이언트
// 컴파일 : cc -o udp_echocli udp_echocli.c
// 사용법 : udp_echocli 117.17.142.162 4008
//---------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int s, nbyte, addrlen = sizeof(servaddr);
	char buf[ MAXLINE + 1 ];

	if(argc != 3) {
		printf("Usage: %s ip_address port_numberr\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}

	// 에코 서버의 소켓주소 구조체 작성
	bzero((char*)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));

	// 키보드 입력 받음
	printf("입력 : ");
	if (fgets(buf, sizeof(buf), stdin) == NULL) {
		printf("fgets fail");
		exit(0);
	}

	// 에코 서버로 메시지 송신
	if (sendto(s, buf, strlen(buf), 0,
		(struct sockaddr *)&servaddr, addrlen) < 0) {
		perror("sendto fail");
		exit(0);
	}

	// 수신된 에코 메시지
	if((nbyte = recvfrom(s, buf, MAXLINE, 0,
		(struct sockaddr *)&servaddr, &addrlen)) < 0) {
		perror("recvfrom fail");
		exit(0);
	}
	buf[nbyte] = 0;
	printf("%s", buf);
	close(s);
	return 0;
}
```

* 결과

```
./udp_echocli 127.0.0.1 4008

입력 : test
수신 : test
```

<br/>

<br/>

#### UDP를 이용하는 echo server 프로그램

```c
//-----------------------------------------------
// 파일명 : udp_echoserv.c
// 기  능 : 에코 서비스를 수행하는 서버
// 컴파일 : cc -o udp_echoserv udp_echoserv.c
// 사용법 : ./udp_echoserv.c 4008
//-----------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr, cliaddr;	// 소켓 두개
	int s, nbyte, addrlen=sizeof(struct sockaddr);
	char buf[ MAXLINE + 1 ];

	if(argc != 2) {
		printf("usage: %s port\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	// servaddr을 '\0'으로 초기화
	bzero((char *)&servaddr, addrlen);
	bzero((char*)&cliaddr, addrlen);

	// servaddr 세팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));	// 문자열 포트번호

	// bind() 호출
	if(bind(s, (struct sockaddr *)&servaddr, addrlen) < 0) {
		perror("bind fail");
		exit(0);
	}
	// 여기까지 하면 준비 완료!
	
	// iterative 에코 서비스 수행
	while(1) {
		puts("Server : waiting request.");
		nbyte = recvfrom(s, buf, MAXLINE, 0,
			(struct sockaddr*)&cliaddr, &addrlen);
		if(nbyte<0) {
			perror("recvfrom fail");
			exit(1);	
		}
		buf[nbyte] = 0;
		printf("%d byte recv: %s\n", nbyte, buf);
		if (sendto(s, buf, nbyte, 0,
			(struct sockaddr*)&cliaddr, addrlen) < 0) {
			perror("sendto fail");
			exit(1);
		}
		puts("sendto complete");
	}
	return 0;
}
```

* 결과

```
./udp_echoserv 4008

server waiting..
5 byte recv: test

sendto complete
server waiting..
6 byte recv: test2

sendto complete
server waiting..
6 byte recv: test1
```

<br/>

<br/>

#### 포트 번호 배정

* 앞서 했던건 서버의 포트번호를 지정해야 했다.
* 소켓에는 ___, 상대의 포트 번호, IP / 나의 포트번호, IP 정보가 들어있다고 했다.
* 클라이언트의 포트번호에 대해서는 언급한적도, 설정한 적도 없었다. 그 이유에 대해 알아보자~

<br/>

* 클라이언트의 포트번호는 **시스템**에서 임의 번호를 자동적으로 배정해주는 번호를 사용한다.
* **기억할 것**은, **서버의 포트번호는 내가 설정**해주지만, **클라이언트의 경우 시스템에서 자동으로 배정**해준다.
* TCP 소켓의 경우는 connect() 호출이 성공한 후에, UDP 소켓의 경우는 sendto() 함수가 성공한 후에 배정한다.
  * getsockname() 을 통해 자신의 호스트에 있는 소켓 정보를 알아낼 수 있다.

<br/>

<br/>

