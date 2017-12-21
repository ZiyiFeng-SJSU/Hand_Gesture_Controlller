#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// #include "opencv/highgui.h"
#include "stasm_lib.h"

//libaries for bluetooth
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
//pipe to talk to python prog
#define FIFO "/tmp/gesturepipe"


using namespace cv;
using namespace std;
static const char * const path = "This is a video dummy.";
static const char * const FilePath = "../demo4.MOV";
// /*DEBUG*/static const String hand_cascade_name = "1256617233-1-haarcascade_hand.xml";
// /*DEBUG*/CascadeClassifier hand_cascade;

void detectAndDisplay( Mat frame, int i );
int processLandmarks(float points[]);

float landmarks[2*stasm_NLANDMARKS];
int main(int argc, char* argv[])
{
	// //***********Setting up pipes*************
	// FILE *fp;
 //    umask(0);
 //    int fifo = mknod(FIFO, S_IFIFO|0666, 0); //Create named pipe
 //    if(fifo == -1 && errno != EEXIST) 	//If pipe already exists ignore error and use existing pipe
 //    {
 //        printf("FIFO could not be created\n");
 //        perror("Error");
 //        return -1;
 //    }
 //    printf("Pipe Created\n");

 //    fp = fopen(FIFO, "w");
 //    setbuf(fp, NULL);
 //    if(fp == NULL)
 //    {
 //        printf("FIFO could not be opened");
 //        perror("Error");
 //        return -1;
 //    }
 //    printf("Pipe Opened\n");
	// //**************************

    Point2f stasm_point;
    int thumb = 10;
	int wrist = 0;

    //-- 1. Load the cascade
  //   if( !hand_cascade.load( hand_cascade_name ) ) { 
		// printf("--(!)Error loading face cascade\n"); return -1; 
  //   }

    VideoCapture cap(0);	//pass 0 to grab live feed
    if (!cap.isOpened()) {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }

    double dW = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    double dH = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	// cout << "Frame size: " << dW << " x " << dH << endl;
	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE);

	while (1) {
		cv::Mat_<unsigned char> frame; cv::Mat_<unsigned char> img;

		cap >> frame;  if(!frame.data) break;
		int foundhand=0;
		
		
		
		/* TeST*/
		 cv::cvtColor(frame, img, cv::COLOR_BGR2GRAY);

		if (stasm_search_single(
				&foundhand, 
				landmarks, 
				(const char*)img.data, 
				img.cols, 
				img.rows, 
				path, 
				"../data")) {


			if (!foundhand) {
				cout << "not found\n";
			} else {
				
				 if((landmarks[thumb*2]-landmarks[wrist*2])>90){
					// cout << "FOUND!";
					foundhand = 0; // reset found flag
					//sending commands through pipes
					//0-fastForward	1-Next | 2-Pause | 3-Play | 4-Previous | 5-Rewind | 6-Stop
					 int commands = processLandmarks(landmarks);
					 // fprintf(fp,"%c", commands + 48);
					 // sleep(500);
				}

			}

			
			
	        // draw the landmarks on the image as white dots (image is monochrome)
			
	        if((landmarks[thumb*2]-landmarks[wrist*2])>90){
	        stasm_force_points_into_image(landmarks, img.cols, img.rows);
	        cout << "xdelta: " << landmarks[63*2] - landmarks[10*2] << endl;
	        for (int i = 0; i < stasm_NLANDMARKS; i++)
                {

                        stasm_point = Point2f (landmarks[i*2], landmarks[i*2+1]);
                        circle (img, stasm_point, 5, Scalar(255,255,255), CV_FILLED, CV_AA, 0);
        
                }
            }

    	} 
    	
		imshow("MyVideo", img);
		if (waitKey(30) == 1048603) {
			cout << "ESC KEY PRESSED BY THE USER." << endl;
			break;
		}		
		// cout << waitKey(30) << endl;
}
    
    return 0;
}
int processLandmarks(float points[])
{
	int thumb = 10;
	int index = 25;
	//n*2 = Xn; n*2+1 = Yn

	//if thumbs.x > index.x
	if(points[thumb*2]<points[index*2]) return 2;// send next
	else return 1;//send pause
	return 0;
}

