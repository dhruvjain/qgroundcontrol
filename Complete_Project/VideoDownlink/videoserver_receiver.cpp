#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{    
     int sockfd, newsockfd, portno,bytes=0,sock,fromlen;
     socklen_t clilen;
     char buf[1024];
     struct sockaddr_in serv_addr, cli_addr,from,server;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     listen(sockfd,5);

     clilen = sizeof(cli_addr);
     
     newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
     
     cout<<"connection made"<<newsockfd<<endl;     
     
     if (newsockfd < 0) 

          error("ERROR on accept");    
    
     
     sock=socket(AF_INET,SOCK_DGRAM,0);

     if(sock<0){
     error("Opening port");
     }
/*
    int length=sizeof(server);

    bzero(&server,length);
    server.sin_family = AF_INET;

    server.sin_addr.s_addr = INADDR_ANY;

    server.sin_port = htons(1234);

  if (-1 == bind(sock,(struct sockaddr *)&server,length))

{

      perror("error bind failed");

      close(sock);

      exit(1);
} 


  fromlen= sizeof(struct sockaddr_in);
  n=recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,(socklen_t*)&fromlen);
  printf("password received \n");
  write(1,buf,n);
  
  if(n<0){
           error("recvfrom");
          
        }
     
  if(!(buf[0]=='p' && buf[1]=='a' && buf[2]=='s' && buf[3]=='s' && buf[4]=='w' && buf[5]=='o' && buf[6]=='r' && buf[7]=='d'))
        exit(1);
       
*/
    cv::namedWindow("output",CV_WINDOW_AUTOSIZE);
     
    Mat  img = Mat::zeros(480,640,CV_8UC3);
    int  imgSize = img.total()*img.elemSize();  
    uchar sockData[imgSize];
   
 
     //Receive data here

     

   while(1){
   bzero(sockData,imgSize);
   img = Mat::zeros(480,640, CV_8UC3);  
   for (int i = 0; i < imgSize; i += bytes) {
   if ((bytes = recv(newsockfd, sockData +i, imgSize  - i, 0)) == -1) {
     error("can not receive");
    }
   
   }

   //n=sendto(sock,sockData,imgSize,0,(struct sockaddr *)&from,fromlen);
   // Assign pixel value to img

   int ptr=0;
  for (int i = 0;  i < img.rows; i++) {
  for (int j = 0; j < img.cols; j++) {                                     
   img.at<cv::Vec3b>(i,j) = cv::Vec3b(sockData[ptr+ 0],sockData[ptr+1],sockData[ptr+2]);
   ptr=ptr+3;
   }
  }
   
   
   cv::imshow("output",img);
   cvWaitKey(1);
}


}
