# Chapter 5. 소켓 옵션

여기서는 네트워크 장비인 라우터가 멀티캐스트 기능을 지원해주는 환경에서..

서버의 역할을 대신 해주는 역할을 해주는 그룹채팅의 클라이언트!

라우터로 하여금 그룹채팅을 할 수 있게 하려면, 소켓의 옵션을 설정해서 그런 기능을 요청하게 된다.

멀티캐스트 외에도 소켓 옵션을 통해서 여러 기능들을 할 수 있다!

오늘은 멀티캐스트와 관련된 옵션 중심으로!



### 소켓 옵션 변경

```c
int setsockopt(int s, int level, int opt, const char *optval, int optlen);
int getsockopt(int s, int level, int opt, const char *optval, int *len);
```

* s : 소켓번호
* level : 프로토콜 레벨
  * SOL_SOCKET : 소켓의 일반적인 옵션 변경
  * IPPROTO_IP : IP 프로토콜에 관한 옵션 변경
  * IPPROTO_TCP : TCP에 관한 옵션 변경
* opt : 사용하고자 하는 옵션
* optval : 옵션 지정에 필요한 값의 포인터
* optlen : optval의 크기

<br/>

#### 옵션 레벨

* SOL_SOCKET : 소켓 레벨의 옵션을 변경
* IPPROTO_IP : IP 레벨의 옵션을 변경
* IPPROTO_TCP : TCP 레벨의 옵션을 변경

<br/>

#### SOL_SOCKET 레벨의 옵션

* SO_BROADCAST : 방송형 메시지 전송 허용
* SO_DEBUG : DEBUG 모드를 선택
* **SO_REUSEADDR** : 주소 재사용 선택
* **SO_LINGER**
  * 소켓을 닫을 때 미전송된 데이터가 있어도 지정된 시간만큼 기다렸다가 소켓을 닫음
* SO_KEEPALIVE : TCP의 keep-alive 동작 선택
* SO_OOBINLINE : OOB 데이터를 일반 데이터처럼 읽음 
* SO_RCVBUF : 수신버퍼의 크기 변경 
* SO_SNDBUF : 송신버퍼의 크기 변경

<br/>

#### IPPROTO_IP 레벨의 옵션

* IP_TTL – Time To Live 변경
* IP_MULTICAST_TTL – 멀티캐스트 데이터그램의 TTL 변경 
* IP_ADD_MEMBERSHIP – 멀티캐스트 그룹에 가입
* IP_DROP_MEMBERSHIP – 멀티캐스트 그룹에서 탈퇴
* IP_MULTICAST_LOOP – 멀티캐스트 데이터그램의 loopback 허용 여부
* IP_MULTICAST_IF – 멀티캐스트 데이터그램 전송용 인터페이스 지정

<br/>

#### IPPROTO_TCP 레벨의 옵션

* TCP_KEEPALIVE – keep-alive 확인 메시지 전송 시간 지정 
* TCP_MAXSEG – TCP의 MSSS(최대 메시지 크기) 지정 
* TCP_NODELAY – Nagle 알고리즘의 선택

<br/>

### 소켓 옵션 변경 예제

```c
int ttl, optlen;

getsockopt(s, IPPROTO_IP, IP_TTL, &ttl, &optlen);
printf(“TTL = %d\n”, ttl);

ttl=32;
setsockopt(s, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
printf(“TTL = %d\n”, ttl);
```

* setting 되어있는 TTL 값을 읽어오고, 새로운 값으로 세팅하는 예제

<br/>

## 5.2 소켓 옵션 종류

<br/>

### 소켓 옵션의 종류 - SO_KEEPALIVE

* SO_KEEPALIVE
  * TCP 연결이 정상적으로 지속되고 있는지를 주기적으로 확인하는 기능
  * 이 옵션이 셋트되어 있으면 TCP는 확인시간(예:2시간) 동안 데이터 송 수신이 없을 때 TCP 연결이 살아 있는지 물어보는 질문(keep alive prove)를 전송
* Keep Alive Prove의 세 가지 응답
  * 상대방이 ACK를 보낸다.
    * TCP 연결이 정상적으로 동작 
  * 상대방이 RST 에러를 보낸다.
    * 상대방 호스트가 꺼진 후 재부팅 된 상태 
    * TCP는 소켓을 닫으며 에러코드는 ECONNRESET 
  * 아무 응답이 없다. 
    * 질문을 몇 번 더 보내본 후 연결 종료
    * 버클리 유닉스의 경우 75초 간격으로 8번의 질문 후 종료(ETIMEDOUT) 
  * 중간 라우터의 응답 
    * ICMP(host unreachable error, network unreachable error) 
      * EHOSTENREACH, ENETURNREACH가 리턴

<br/>

### 소켓 옵션의 종류 - SO_LINGER

* SO_LINGER 
  * close() 호출시 송신버퍼에 데이터가 남아 있어도 close()는 즉시 리턴 되고 TCP는 이 데이터를 모두 전송한 후 실제 연결을 종료
  * close() 함수가 리턴한 시점에서 모든 데이터가 전송된 것을 보장하지 는 않음
  * SO_LINGER 옵션은 close()를 호출한 후 상대방에서 정상적으로 종료 절차가 이루어 졌는지를 확인하기 위해 사용
    * close()는 지정한 linger 시간 동안 또는 정상 종료시까지 블록됨
    * 정상 종료전에 linger 시간이 타임아웃되면 ETIMEDOUT 에러 발생
  * 4-way handshake 
    * FIN, ACK, FIN, ACK의 순서로 동작 
    * 호스트 A가 B로 데이터 전송 후 close()를 호출한 시점에서 B가 모든 데이터를 읽었다고 확신할 수 없음

<br/>

### 소켓 옵션의 종류 - SO_RCVBUF, SO_SNDBUF

```
TCP, UDP는 송신버퍼와 수신버퍼를 가짐
– TCPdml 경우 write() 호출시 데이터를 송신 버퍼로 복사 – 데이터가 송신버퍼에 모두 복사되면 시스템이 데이터를 전송 – 전송 데이터는 유지하고 있다가 ACK를 수신 후 삭제 – 송신버퍼가 가득차면 write()는 블록됨 – 송신/수신버퍼의 크기를 사용자가 지정할 수 있음
• SO_SNDBUF – 송신 버퍼의 크기 확인 및 지정
• SO_RCVBUF – 수신 버퍼의 크기 확인 및 지정
• 송신/수신 버퍼의 크기 지정 방법
– 3-way handshake 후에는 버퍼 크기 변경이 불가
• 서버의 경우 listen() 호출 이전에 설정
• 클라이언트의 경우 connect() 호출 이전에 설정
```

<br/>

### 소켓 옵션의 종류 - 우리가 쓸 SO_REUSEADDR

* 동일한 소켓주소를 여러 프로세스 또는 한 프로세스 내의 여러 소켓 에서 중복 사용을 허용

  * 기본적으로 한 호스트 내에서는 같은 포트번호의 중복 사용이 불가

  ```C
  int set = 1;
  setsockopt(udp_내차1, SOL_SOCKET, SO_REUSEADDR, (void*)&set, sizeof(int));
  ```

* 소켓주소 재사용 옵션이 필요한 경우

  * TIME-WAIT 상태에서의 주소 재사용
    * ACTIVE CLOSE상태의 호스트는 TIME-WAIT 상태를 가지며 포트번호를 중복하여 사용할 수 없음
  * 자식 프로세스가 서버인 경우
    * 부모 프로세스가 종료된 후 다시 시작하면 포트번호 사용중 에러가 발생
    * 소켓 주소 재사용 옵션은 bind() 호출 이전에 설정해야 함
  * 멀티홈 서버의 경우
    * 호스트가 두개 이상의 IP주소를 가지며 같은 포트번호를 사용해야 하는 경우가 있음
  * 완전 중복 바인딩
    * 동일한 IP주소와 포트번호를 중복하여 bind()하는 것, UDP에서만 가능

<br/>

## 5.3 멀티캐스트 프로그래밍

* 앞선 구현의 경우, 클라이언트의 수 만큼 메시지 개수를 증가시켰다!!
* 하지만 라우터를 통해서 하게 되면 네트워크의 로드를 줄여줄 수 있다!!

<br/>

* 멀티캐스트
  * 하나의 데이터그램을 다수의 호스트로 동시에 전송
    * 하나의 데이터그램은 네트워크 내에서 복제되어 다수의 호스트로 전송
  * LAN 뿐만 아니라 인터넷에 연결되어 있는 모든 호스트에 가능
  * 호스트는 해당 멀티캐스트 그룹에 가입되어 있어야 함
* 멀티캐스트 그룹
  * 멀티캐스트 데이터그램을 수신하고자 하는 호스트들의 집합
  * 클래스 D의 IP주소를 그룹 주소로 사용
    * 224.0.0.0부터 239.255.255.255 사이의 값을 가짐
  * 해당 그룹 주소를 목적지로 하여 UDP 데이터그램을 전송
  * 중간 라우터가 MRP(Multicast Routing Protocol)을 지원해야 함



### 서버가 했던 멤버쉽 관리를 라우터가 해줘야한다!!

#### 소켓 옵션 지정

* 멀티캐스트와 관련된 소켓 옵션

  * IP_ADD_MEMBERSHIP : 그룹에 가입
  * IP_DROP_MEMBERSHIP : 그룹에서 탈퇴 
  * IP_MULTICAST_LOOP : 멀티캐스트 패킷의 loopback 허용 여부
  * IP_MULTICAST_TTL : 패킷의 TTL값 지정
  * IP_MULTICAST_IF : 패킷 전송을 위한 인터페이스 지정

* 멀티캐스트 그룹 가입/탈퇴

  * ip_mreq 구조체를 사용

  ```c
  struct ip_mreq {
  	struct in_addr imr_multiaddr;
  	struct in_addr imr_interface; // 자신의 인터페이스(IP주소)
  }
  ```

  

