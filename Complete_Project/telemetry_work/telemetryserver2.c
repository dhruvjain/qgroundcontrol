#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdlib.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <fcntl.h>

#include <termios.h>

#include <sys/ioctl.h>

int iport=1234; // communiction port between mavproxy and server
int port=8080;

#define MAVLINK_REMOTE_UDP_PORT 14550 
#define MAVLINK_LOCAL_UDP_PORT 14551  
typedef struct
{
  int sock;
  struct sockaddr_in address;
  int addr_len;
} connection_t;

char* data[10];
int no_of_uavs;

void * process(void * ptr,void* ptr2)
{
       

  struct sockaddr_in gcAddr,from,fromAddr;

       
  int len;
  connection_t * conn;
  long addr = 0;
  int sock,n;
  int num=*(int *)ptr2;
  char buf[1024];
  
  if (!ptr) pthread_exit(0); 

  conn = (connection_t *)ptr;
  sock=conn->sock;
  from=conn->address;
  
  int fromlen=sizeof(struct sockaddr_in);
        
  gcAddr.sin_family = AF_INET;

  gcAddr.sin_addr.s_addr = (conn->address).sin_addr.s_addr;

  gcAddr.sin_port = htons(MAVLINK_REMOTE_UDP_PORT);
        
   
  for (;;) {   

        int rc = sendto(sock, data[num], sizeof(data[num]),0,(const struct sockaddr *)&gcAddr, sizeof(gcAddr));
        if (rc == -1) {

        perror("Error writing UDP port ");

        }

  }

  /* close socket and clean up */
  close(conn->sock);
  free(conn);
  pthread_exit(0);
}
void one_uav_to_server(void* ptr,void* ptr2){

       connection_t* conn;
       conn=(connection_t *)ptr;
       int num=*(int *)ptr2;

       struct sockaddr_in locAddr,fromAddr,address;
       address=conn->address;
       int udp_socket=conn->sock;
       char* rcv_buff;

       rcv_buff=(char*)malloc(1024*sizeof(char));
       char printbuf[INET_ADDRSTRLEN];
       char printbuff[INET_ADDRSTRLEN];

       int fromlen=sizeof(struct sockaddr_in);
       data[num]=(char*)malloc(1024*sizeof(char));
       inet_ntop(address.sin_family, &address.sin_addr.s_addr, printbuff, sizeof(printbuff));
       printf("%s\n",printbuff);

       for (;;) {

        
        int recSize = recvfrom(udp_socket, rcv_buff, sizeof(rcv_buff),0,(struct sockaddr *)&fromAddr, &fromlen);  // receiving from UAV
        

        //for(int i=0;i<sizeof(rcv_buff);i++){
        // data[num][i]=rcv_buff[i];

        //}
        if (recSize == -1) {
  
        perror("Error recvfrom ");

        }
        inet_ntop(fromAddr.sin_family, &fromAddr.sin_addr.s_addr, printbuf, sizeof(printbuf));
        printf("%s\n",printbuf);
        if(!strcmp(printbuf,printbuff))
           data[num]=rcv_buff;
       
    }

}

void uavs_to_server(int udp_socket){
       connection_t * connection2;
       struct sockaddr_in locAddr,fromAddr;
       pthread_t mainthread_uav[10];
     
       printf("%d \n",udp_socket);
       int i=0;
       char* rcv_buff;
       rcv_buff=(char*)malloc(1024*sizeof(char));
       char* buffer;
       buffer=(char*)malloc(1024*sizeof(char));
       int p1=1234;
       int fromlen=sizeof(struct sockaddr_in);

       memset(&locAddr, 0, sizeof(locAddr));

       locAddr.sin_family = AF_INET;
                                            
       locAddr.sin_addr.s_addr = INADDR_ANY;


       locAddr.sin_port = htons(iport); // port at which uavs send data   .


       if (udp_socket == -1) {

        perror("Failed to open udp socket ");

        pthread_exit(0);

       }

       if (-1 == bind(udp_socket,(struct sockaddr *)&locAddr, sizeof(struct sockaddr)))

        {

        perror("error bind failed");

        close(udp_socket);

        pthread_exit(0);
        } 




       while(1){

         connection2 = (connection_t *)malloc(sizeof(connection_t));

         int recSize = recvfrom(udp_socket, rcv_buff, sizeof(rcv_buff),0,(struct sockaddr *)&fromAddr, &fromlen);
         printf("Received buffer %s",rcv_buff);
         connection2->address=fromAddr;
         connection2->addr_len=fromlen;
         connection2->sock=udp_socket;
       
         if(recSize<0)
         {
         perror("error in receive");
         }
         sprintf(buffer, "%d", p1);
         
         no_of_uavs=1+i; 
         
         pthread_create(&mainthread_uav[i], 0, one_uav_to_server,(void *) connection2,(void *) i);
         i++;
          
  }

}


int main()
{
  int sock = -1,n;
  struct sockaddr_in address,from;
  int port;
  connection_t * connection;
  pthread_t thread;
  char buf[1024];
  pthread_t mainthread;
  char uavnum[4];
  int fromlen=sizeof(struct sockaddr_in);


  /* check for command line arguments */
  
  /* create socket */
  sock=socket(AF_INET,SOCK_DGRAM,0);
  printf("%d \n",sock);
  if (sock <= 0)
  {
   
    return -3;
  }

  /* bind socket to port to receive from client */
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);   
  if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
  {
    
    return -4;
  }

  
  printf(" ready and listening\n");
  int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);  
  pthread_create(&mainthread, 0, uavs_to_server, udp_socket);

  while (1)
  {
                
    /* accept incoming connections */
    connection = (connection_t *)malloc(sizeof(connection_t));
    connection->sock=sock;
    
    n=recvfrom(sock,buf,1024,0,(struct sockadd*)&from, &fromlen);

    if (connection->sock <= 0)
    {
      free(connection);
    }
    if(!(buf[0]=='p' && buf[1]=='a' && buf[2]=='s' && buf[3]=='s' && buf[4]=='w' && buf[5]=='o' && buf[6]=='r' && buf[7]=='d'))
       continue;

    printf("Correct password received \n");
    connection->address=from;
    connection->addr_len=fromlen;
    bzero(uavnum,4);
    sprintf(uavnum, "%d", no_of_uavs);

    n=sendto(sock,uavnum,sizeof(uavnum),0,(struct sockaddr *)&from,fromlen);

    n=recvfrom(sock,buf,1024,0,(struct sockadd*)&from,&fromlen);

    

    uintmax_t id = strtoumax(buf, NULL, sizeof(buf));

    
    /* start a new thread but do not wait for it */
    pthread_create(&thread, 0, process, (void *) connection,(void *) id);
    pthread_detach(thread);
    
  }
  
  return 0;
}
