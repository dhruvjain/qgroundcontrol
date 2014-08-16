#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>

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
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;
using namespace cv;

// Code runs on odroid to send frames to the server

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n,imgSize;                       
    struct sockaddr_in serv_addr;
    struct hostent *server;                     

    char buffer[256];     
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);

    
    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(portno);

   
    CvCapture* capture = 0;
    Mat frame, frameCopy, image;
    cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );
 
    int fromlen= sizeof(struct sockaddr_in);


    capture = cvCaptureFromCAM( 0 ); //0=default, -1=any camera, 1..99=your camera
    if(!capture) cout << "No camera detected" << endl;

    cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );
    

    if( capture )
    {
        cout << "In capture ..." << endl;
        for(;;)
        {
          IplImage* iplImg = cvQueryFrame( capture );
          frame = iplImg;

          if( frame.empty())
          break;
          if( iplImg->origin == IPL_ORIGIN_TL )
          frame.copyTo( frameCopy );
          else
          flip( frame, frameCopy, 0 );

          cvShowImage( "result", iplImg );
          
          frame = (frame.reshape(0,1)); // to make it continuous

          imgSize = frame.total()*frame.elemSize();
          
          //send
          n = sendto(sockfd, frame.data, imgSize,0, (const struct sockaddr *)&serv_addr, (socklen_t)fromlen);
        
          if(n < 0)
            cout<<"Failed to send."<<endl;
         // else
           // cout<<"Sent a frame of size "<<n<<endl;


         if( waitKey(1) >= 0 )
         break;

        }

        waitKey(0);
    }
    
    cvReleaseCapture( &capture );
    cvDestroyWindow( "result" );
    return 0;
}

