/*      UDP client.c    */
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
    int sock;
    char message[BUFSIZE];
    int str_len, addr_size;
    struct sockaddr_in serv_addr;
    struct sockaddr_in from_addr;
    FILE *stream;
    char filename[20];
    int filenamesize;
    char END[]="end_of_file";

    if(argc!=4){
        printf("Usage: %s <IP> <port> <file_name>\n",argv[0]);
        exit(1);
    }

    sock=socket(PF_INET, SOCK_DGRAM, 0);
    if(sock==-1)
        error_handling("UDP socket generation error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    /*  file    */
    strcpy(filename,argv[3]);
    filenamesize=strlen(filename);
    printf("filename: %s\n",filename);
    printf("filenamesize: %d\n",filenamesize);
    stream=fopen(filename,"rb");
    while(1){
        sendto(sock,&filenamesize,sizeof(int),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        sendto(sock,filename,strlen(filename),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        if((str_len=recvfrom(sock,message,BUFSIZE,0,(struct sockaddr*)&from_addr,&addr_size))==0){
            break;
        }
    }

    printf("Start Sending Files\n");
    while(feof(stream)==0){
        str_len=fread(message,1,sizeof(message),stream);
        sendto(sock,message,str_len,0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        memset(message,0,sizeof(message));
    }
    sendto(sock,END,strlen(END),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    printf("Finished Sending\n");
    fclose(stream);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}
