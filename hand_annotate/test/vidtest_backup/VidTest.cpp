#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// #include "opencv/highgui.h"
#include "stasm_lib.h"

using namespace cv;
using namespace std;
static const char * const path = "This is a video dummy.";
// /*DEBUG*/static const String hand_cascade_name = "1256617233-1-haarcascade_hand.xml";
// /*DEBUG*/CascadeClassifier hand_cascade;

void detectAndDisplay( Mat frame, int i );

int main(int argc, char* argv[])
{


    //-- 1. Load the cascade
  //   if( !hand_cascade.load( hand_cascade_name ) ) { 
		// printf("--(!)Error loading face cascade\n"); return -1; 
  //   }

    VideoCapture cap("IMG_3549.MOV");
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
		int foundhand;
		
		// if (!bSuccess) {
		// 	cout << "Cannot read a frame from the video stream." << endl;
		// 	break;
		// }
		
		/* TeST*/
		cv::cvtColor(frame, img, cv::COLOR_BGR2GRAY);
		float landmarks[2*stasm_NLANDMARKS];

		// if (stasm_search_single(
		// 		&foundhand, 
		// 		landmarks, 
		// 		(const char*)img.data, 
		// 		img.cols, 
		// 		img.rows, 
		// 		path, 
		// 		"/home/lanceb247/progs/hand_annotate/data")) {

			if (!foundhand) {
				cout << "not found";
			} else 
				cout << "FOUND!";

			
			if (foundhand) {
	        	// stasm_force_points_into_image(landmarks, img.cols, img.rows);
	        	for (int i = 0; i < stasm_NLANDMARKS; i++) {
	        		cout << "STASM output:" << endl;
	        		cout << landmarks[i*2+1] << ',' << landmarks[i*2] << endl;
	        	}
	        }

	        //draw the landmarks on the image as white dots (image is monochrome)
	        stasm_force_points_into_image(landmarks, img.cols, img.rows);
	        for (int i = 0; i < stasm_NLANDMARKS; i++){
	            img(cvRound(landmarks[i*2+1]), cvRound(landmarks[i*2])) = 255;	        

            	img(cvRound(landmarks[i*2+1]), cvRound(landmarks[i*2])) = 255;
		}

    
  
		//-----------
		imshow("MyVideo", img);
		if (waitKey(30) == 1048603) {
			cout << "ESC KEY PRESSED BY THE USER." << endl;
			break;
		}		
		// cout << waitKey(30) << endl;
	}
    
    return 0;
}


/* @function detectAndDisplay */
// void detectAndDisplay( Mat frame, int i )
// {
//     std::vector<Rect> hands;
//     Mat frame_gray;
//     cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
//     //equalizeHist( frame_gray, frame_gray );

// //cascade.detectMultiScale (Img, objects, 1.1f, 4, CASCADE_FIND_BIGGEST_OBJECT, Size (20, 20)); 

//     //-- Detect hand
// //    hand_cascade.detectMultiScale( frame_gray, hands, 1.2, 3, 0, cvSize(30, 30) ); //, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );
//     hand_cascade.detectMultiScale( 
//     	frame_gray, 					//const MAT& image
//     	,hands							//vector<Rect>& objects
//     	,1.001 							//double scaleFactor = 1.1
//     	,10 							//int minNeighbors=3
//     	,CASCADE_FIND_BIGGEST_OBJECT	//int flags = 0
//     	,cvSize(50, 50)					//Size minSize=Size()
//     	,null							//Size maxSize=Size()
//     	 ); 
//     	 //, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

//     for ( size_t p = 0; p < hands.size(); p++ ) {
//         rectangle( frame, cvPoint(hands[p].x, hands[p].y), cvPoint(hands[p].x + hands[p].width, hands[p].y + hands[p].height), Scalar(0,0,255));    

//     }
    
//     //-- Show what you got
//     imshow( winNa[i], frame );
// }
