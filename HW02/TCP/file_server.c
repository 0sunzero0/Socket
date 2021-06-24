#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 2048  // recv buffer

void error_handling(char *message);

int main(int argc, char **argv)
{
    //서버와 클라이언트의 소켓 디스크립터
    int serv_sock;
    int clnt_sock;

    //ip address와 port number 등의 자료를 저장할 구조체
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;

    FILE* fp = NULL;
    char file_type; // 파일 모드 flag : 0 : binary | ASCII
    char file_name[200];

    char message[BUFSIZE];  //Receiver쪽의 버퍼

    int str_len;

    int clnt_addr_size;


    if(argc!=2){
      printf("Usage : %s <port>\n", argv[0]);
      exit(1);
    }

    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
      error_handling("socket() error");

    // ip address와 port number 할당을 위한 구조체 초기화
    memset(&serv_addr, 0, sizeof(serv_addr));

    // Address Family = IPv4 , Ip Address 32비트 빅엔디안 형으로 저장, 포트넘버 int값으로 변환 후, 16비트 빅엔디안 형으로 저장
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
      error_handling("bind() error");

    // Waiting Queue Size 5.
    if(listen(serv_sock, 5)==-1)
      error_handling("listen() error");

    while(1) {
      // 대기 큐에서, 첫 번째로 대기 중에 있는 연결요청을 참조하여, 클라이언트와의 연결 구성 및 에러체크!
      clnt_addr_size=sizeof(clnt_addr);
      clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);

      if(clnt_sock == -1)
	     error_handling("accept() error");

      while(1) {
        // TCP의 경우, 바이트 스트림이기 때문에, 버퍼가 다 차지 않았다면,
        // 이름과 내용 정보를 같이 한꺼번에 전송할 수 있기 때문에,
        // 파일 이름 길이를 받아와서, 바운더리를 체크해준다. 그 후, file_name에 구분하여 저장함!
        printf("now we start\n");

        recv(clnt_sock, &str_len, sizeof(int), 0);

        memset(file_name, 0, sizeof(file_name));
        recv(clnt_sock, file_name, str_len, 0);

         printf("received file name : %s\n", file_name);

        fp = fopen(file_name, "wb");

        int fp_block_sz = 0;

        while(1){
          //		printf("start to write in file\n");
		     memset(message, 0, sizeof(message));
		     str_len=recv(clnt_sock, message, BUFSIZE, 0);

         if(str_len==0) // 클라이언트로 부터, 강제 close 되었거나, 파일 전송이 끝났다면 break!!
			      break;
		     fwrite(message, sizeof(char), str_len, fp);
	      }
        // 파일 전송이 끝났다면, file close 시키고 break
        fclose(fp);
	      break;
      }
    }
    // client_sd를 close시키고 다시 iterative하게 동작!!
    close(clnt_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
