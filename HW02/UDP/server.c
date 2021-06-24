/*      UDP server.c    */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSIZE 1024

void error_handling(char *message);

int main(int argc, char **argv){
    int serv_sock;
    char message[BUFSIZE];
    int str_len, num=0;
    FILE *stream;
    char filename[20];
    int filenamesize;
    char NAME[]="";

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    int clnt_addr_size;

    if(argc!=2){
        printf("Usage: %s <port>\n",argv[0]);
        exit(1);
    }

    serv_sock=socket(PF_INET,SOCK_DGRAM,0);
    if(serv_sock==-1)
            error_handling("UDP socket generation error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
        error_handling("bind() error");

    while(1){
        printf("\n\nRECEIVING FILENAME\n");

        while(1){
            clnt_addr_size=sizeof(clnt_addr);

            str_len=recvfrom(serv_sock, &filenamesize, sizeof(int), 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

            str_len=recvfrom(serv_sock, filename, sizeof(filename), 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
            if(filenamesize==str_len){
                sendto(serv_sock,NAME,strlen(NAME),0,(struct sockaddr*)&clnt_addr,sizeof(clnt_addr));
                filename[filenamesize]='\0';
                printf("filename: %s\n",filename);
                printf("filenamesize: %d\n",filenamesize);
                break;
            }

        }
        stream = fopen(filename,"wb");
        printf("Transfer start\n");
        while(1){
            str_len=recvfrom(serv_sock,message,BUFSIZE,0,(struct sockaddr*)&clnt_addr,&clnt_addr_size);

            if(!strcmp(message,"end_of_file")) break;
            fwrite(message,1,str_len,stream);
            memset(message,0,sizeof(message));

        }
        fclose(stream);
        printf("End of Transfer\n");
        memset(&filenamesize,0,sizeof(int));
        memset(filename,0,sizeof(filenamesize));
    }
    close(serv_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}
