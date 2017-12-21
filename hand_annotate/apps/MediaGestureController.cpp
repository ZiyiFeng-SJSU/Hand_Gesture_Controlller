/**
	====== Media Gesture Controller ======
	2017-12-11 
	Senior Project, San Jose State University
	- Lance Barrett
	- Khang Doan
	- Zoe Zeng
	- John Strube

	This application attempts to function as a hand gesture bluetooth controller for a media device.

	[NOT IN USE] Timer code snippets retrieved from: 
		https://stackoverflow.com/questions/2962785/c-using-clock-to-measure-time-in-multi-threaded-programs/2962914#2962914	
			// struct timespec start, finish;
			// double elapsed;
			// clock_gettime(CLOCK_MONOTONIC, &start);
			// clock_gettime(CLOCK_MONOTONIC, &finish);
			// elapsed = (finish.tv_sec - start.tv_sec);
			// elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;    
*/

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "stasm_lib.h"

//libaries for bluetooth
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

//pipe to talk to python prog
#define FIFO "/tmp/gesturepipe"
#define THUMB 10
#define WRIST 0
#define INDEX 25
#define PINKIE 63
#define SIGNAL_SEC_WINDOW 1

using namespace cv;
using namespace std;

static const char * const path = "This is a video dummy.";
static const char * const FilePath = "../demo4.MOV";
// /*DEBUG*/static const String hand_cascade_name = "1256617233-1-haarcascade_hand.xml";
// /*DEBUG*/CascadeClassifier hand_cascade;

void detectAndDisplay( Mat frame, int i );
int processLandmarks(float points[], bool &playing);
bool tooSmall();

float landmarks[2*stasm_NLANDMARKS];
int main(int argc, char* argv[]) {
	cv::Mat_<unsigned char> frame; 
	cv::Mat_<unsigned char> img;
	
	time_t start, end; 			// Measure time for intervals
	int interval	= -1;		// Keep track of time interval
	int dets 		= 0;		// Count # of detections within interval
   	bool playing	= false;
   	int sumWidth	= 0;
   	int minY, maxY  = 0;
   	int sumHeight   = 0;
   	int command     = 0;
   	double widthToHeight = 0.0;
   	Point2f stasm_point;


	// //***********Setting up pipes*************
	FILE *fp;
    umask(0);
    int fifo = mknod(FIFO, S_IFIFO|0666, 0); //Create named pipe
    if(fifo == -1 && errno != EEXIST) 	//If pipe already exists ignore error and use existing pipe
    {
        printf("FIFO could not be created\n");
        perror("Error");
        return -1;
    }
    printf("Pipe Created\n");

    fp = fopen(FIFO, "w");
    setbuf(fp, NULL);
    if(fp == NULL)
    {
        printf("FIFO could not be opened");
        perror("Error");
        return -1;
    }
    printf("Pipe Opened\n");
	//**************************

    VideoCapture cap(0);	//pass 0 to grab live feed
    if (!cap.isOpened()) {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }

    double dW = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    double dH = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE);

	/* Live Video Feed Loop */
	while (1) {
		if (interval < 0) start = time(0);

		cap >> frame;  if(!frame.data) break;
		cv::cvtColor(frame, img, cv::COLOR_BGR2GRAY);
		int foundhand=0;

		if (stasm_search_single(
				&foundhand, 
				landmarks, 
				(const char*)img.data, 
				img.cols, 
				img.rows, 
				path, 
				"../data")) {


			if (foundhand) {
				if(!tooSmall()) {
					dets++;
					foundhand = 0; // reset found flag			

					// draw the landmarks on the image as white dots (image is monochrome)
					stasm_force_points_into_image(landmarks, img.cols, img.rows);

			        for (int i = 0; i < stasm_NLANDMARKS; i++) {
			        	if (minY > landmarks[i*2+1]) minY = landmarks[i*2+1];
			        	if (maxY < landmarks[i*2+1]) maxY = landmarks[i*2+1];

		                stasm_point = Point2f (landmarks[i*2], landmarks[i*2+1]);
		                circle (img, stasm_point, 5, Scalar(255,255,255), CV_FILLED, CV_AA, 0);
	                }
	                // cout << "xdelta: " << abs(landmarks[INDEX*2] - landmarks[PINKIE*2]) << endl;
	                // cout << "(" << landmarks[PINKIE * 2] << "," << landmarks[PINKIE*2+1] << ")"
	                // 	 << "(" << landmarks[INDEX * 2] << "," << landmarks[INDEX*2+1] << ")" << endl;
	                sumWidth += abs(landmarks[INDEX*2] - landmarks[PINKIE*2]);		
	                sumHeight += maxY - minY;			 
				}
			} 
    	} 
    	
    	end = time(0);
    	interval = difftime(end, start);
    	widthToHeight = (double) sumWidth / sumHeight;

    	if (interval > SIGNAL_SEC_WINDOW) {
    		// evaluate detection
    		if (dets > 7) {

		        for (int i = 0; i < stasm_NLANDMARKS; i++) {
		        	// if (minY > landmarks[i*2+1]) minY = landmarks[i*2+1];
		        	// if (maxY < landmarks[i*2+1]) maxY = landmarks[i*2+1];

	                stasm_point = Point2f (landmarks[i*2], landmarks[i*2+1]);
	                circle (img, stasm_point, 5, Scalar(0,0,255), CV_FILLED, CV_AA, 0);
                }
    			//0-fastForward	1-Next | 2-Pause | 3-Play | 4-Previous | 5-Rewind | 6-Stop
    			if (widthToHeight > 0.25) {
    				// Play Pause
					if (playing) {
						playing = false;
						command = 2;
					} else {
						playing = true;
						command = 3;
					}
				} else if (widthToHeight > 0.10 && !playing) {
					command = 1; // Next
				}
    		// } else if (dets > 3 && widthToHeight > 0.10 && widthToHeight < 0.28) {
    		// 	command = 1;
    		} else command = -1;

    		if (command > 0) {
    			switch (command) {
    				case 2:
    					cout << "Command: Pause" << endl; break;
    				case 3:
						cout << "Command: Play" << endl; break;
					case 1:
						cout << "Command: Next" << endl; break;
					default:
						cout << "Not Defined." << endl;
    			}
				//sending commands through pipes 
				fprintf(fp,"%c", command + 48);    			
    		}
    		
		
    		// cout << (playing ? "<<Playing>>  " : "<<Paused>>  ") 
    		// 	<< "Detections this interval: " << dets 
    		// 	<< "  Avg Width To Height Ratio: " << (sumHeight > 0 ? (double) sumWidth / sumHeight : 0) << endl;
    		interval = -1;
    		sumWidth = 0;
    		sumHeight = 0;
    		dets = 0;
    	}

		imshow("MyVideo", img);
		if (waitKey(30) == 1048603) {
			cout << "ESC KEY PRESSED BY THE USER." << endl;
			break;
		}		
	}

    return 0;
}

bool tooSmall() {
	return (landmarks[THUMB*2]-landmarks[WRIST*2]<90);
	// /*DEBUG*/ return false;
}
