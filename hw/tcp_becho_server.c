#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFSIZE 30
void error_handling(char *message);

int main(int argc, char **argv) {
  int serv_sock;
  int clnt_sock;
  char message[BUFSIZE];
  int str_len, num = 0;

  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  int clnt_addr_size;

  if (argc != 2) {
    printf("Usage : %s <port> \n", argv[0]);
    exit(1);
  }

  serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  if (serv_sock == -1)
    error_handling("TCP 소켓 생성 오류");

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));

  if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
    error_handling("bind() error");

  if(listen(serv_sock, 5) == -1)
    error_handling("listen() error")

  sleep(5);

  clnt_addr_size =sizeof(clnt_addr)
  clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

  sleep(1);
  if(clnt_sock == -1)
    error_handling("accept() error");

  while (1) {
    str_len = read(clnt_sock, message, BUFSIZE);
  	printf("수신 번호 : %d \n", num++);
    write(clnt_sock, message, str_len);
  }
  close(serv_sock);
  close(clnt_sock);
  return 0;
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}
