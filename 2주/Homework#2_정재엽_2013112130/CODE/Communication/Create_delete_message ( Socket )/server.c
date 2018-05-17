/*
	int socket(int domain, int type, int protocol)
	domain : 어떤 환경에서 통신할지 (인터넷, 프로세스간)
	type : SOCK_STREAM(TCP/IP 통신), SOCK_DGRAM(UDP/IP 통신)
	protocol : 프로토콜 설정, 보통 0 사용 
	반환값 : 소켓 식별자, 실패시 -1

	서버 :  서버 소켓 열고 listen
	        클라이언트와 연결도 소켓 생성
	클라이언트 : connect -> 서버 : accept => 통신 연결 
	close() : 통신 종료 
*/#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define  BUFF_SIZE   1024

int   main( void)
{
   int   server_socket;
   int   client_socket;
   int   client_addr_size;

   struct sockaddr_in   server_addr;
   struct sockaddr_in   client_addr;

   char   buff_rcv[BUFF_SIZE+5];
   char   buff_snd[BUFF_SIZE+5];



   server_socket  = socket( PF_INET, SOCK_STREAM, 0);
   if( -1 == server_socket)
   {
      printf( "server socket 생성 실패\n");
      exit( 1);
   }

   memset( &server_addr, 0, sizeof( server_addr));
   server_addr.sin_family     = AF_INET;
   server_addr.sin_port       = htons( 4000);
   server_addr.sin_addr.s_addr= htonl( INADDR_ANY);

   if( -1 == bind( server_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) )
   {
      printf( "bind() 실행 에러\n");
      exit( 1);
   }

   while( 1)
   {
      if( -1 == listen(server_socket, 5))
      {
         printf( "대기상태 모드 설정 실패\n");
         exit( 1);
      }

      client_addr_size  = sizeof( client_addr);
      client_socket     = accept( server_socket, (struct sockaddr*)&client_addr,
                                          &client_addr_size);

      if ( -1 == client_socket)
      {
         printf( "클라이언트 연결 수락 실패\n");
         exit( 1);
      }

      recv  ( client_socket, buff_rcv, BUFF_SIZE, 0);
      printf( "receive: %s\n", buff_rcv);
      
      sprintf( buff_snd, "%d : %s", strlen( buff_rcv), buff_rcv);
      send( client_socket, buff_snd, strlen( buff_snd)+1, 0);       // +1: NULL까지 포함해서 전송
      close( client_socket);
   }
}