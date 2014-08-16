#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

using namespace std;
using namespace cv;
// port should be 1234
int main(int argc, char *argv[]){

   int sock,length,n,bytes=0;
   struct sockaddr_in server,from;
   struct hostent *hp;
   char buffer[256];
  
   if(argc !=3){
  
    printf("Usage server port \n");
    exit(1);
  
  }

   sock=socket(AF_INET,SOCK_DGRAM,0);
 
   if(sock<0){
        perror("socket");
   }
   server.sin_family=AF_INET;

   hp=gethostbyname(argv[1]);

  if(hp==0){

   perror("unknown host");
  }

  bcopy((char *)hp->h_addr,(char *)&server.sin_addr,hp->h_length);

  server.sin_port=htons(atoi(argv[2]));
  length=sizeof(struct sockaddr_in);


  printf("Please enter the password to get the video \n");

  bzero(buffer,256);
  fgets(buffer,255,stdin);

  n=sendto(sock,buffer,strlen(buffer),0,(const sockaddr*)&server,length);
  if(n<0)
  {
  perror("send to");

}
  cv::namedWindow("output",CV_WINDOW_AUTOSIZE);
     
  Mat  img = Mat::zeros(480,640,CV_8UC3);
  int  imgSize = img.total()*img.elemSize();  
  uchar sockData[imgSize];
   
 
  //Receive data here

   
 while(1){
 
   bzero(sockData,imgSize);
   img = Mat::zeros(480,640, CV_8UC3);  
   for (int i = 0; i < imgSize; i += bytes) {
   if ((bytes = recvfrom(sock, sockData +i, imgSize  - i, 0,(struct sockaddr *)&from,(socklen_t*)&length)) == -1) {
     perror("can not receive");
    }
   
   }

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
