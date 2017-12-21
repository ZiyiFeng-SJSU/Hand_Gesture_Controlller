/*
	mytest.cpp
*/

//#ifndef INT64_C
//#define INT64_C(c) (c ## LL)
//#define UINT64_C(c) (c ## ULL)
//#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <wand/MagickWand.h> 
//#include "/usr/include/postgresql/libpq-fe.h"
//#include "/usr/local/include/opencv/cv.h"
//#include "/usr/local/include/opencv/highgui.h"
//#include "/usr/local/include/opencv/cvaux.h"
//#include "/usr/local/include/opencv/cxcore.h"
//#include "/usr/local/include/opencv2/imgproc.hpp"
//#include "/usr/local/include/opencv2/imgproc/imgproc.hpp"
//#include "/usr/local/include/opencv2/core.hpp"
//#include "/usr/local/include/opencv2/core/core.hpp"

#include "stasm_lib.h"
//#include "stasm_lib_ext.h"  // needed for stasm_search_auto_ext
//#include "stasm_landmarks.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv/cv.hpp>

//extern "C" {
//#include "/usr/include/x86_64-linux-gnu/libavcodec/avcodec.h"
//#include "/usr/include/x86_64-linux-gnu/libavformat/avformat.h"
//#include "/usr/include/x86_64-linux-gnu/libswscale/swscale.h"
//}





using namespace cv;
using namespace std;



#define SUCCESS 0
#define FAILURE 1
#define FALSE 0
#define TRUE 1





typedef struct face_landmark_node 
{
  int frame;
  int indice;
  float x;
  float y;
  struct face_landmark_node *next;
} face_landmark_node;





face_landmark_node * stasm (char *, int);
face_landmark_node * add_face_landmark_element (face_landmark_node *, int, int, float, float);
void draw_markers (face_landmark_node *, char *, int);
face_landmark_node * search_face_landmarks (face_landmark_node *, int, int);
void free_linked_list_memory (face_landmark_node *);




int main (int argc, char *argv[])
{
  char filename[1280], *face_id;
  int i, begin_frame, end_frame, current_frame, stasm_point_x[77], stasm_point_y[77], save_stasm_data;
  face_landmark_node *face_landmark_list_head, *face_landmark_element;
  FILE *fp;
 
  face_id = NULL;
  face_landmark_list_head = NULL; 
  begin_frame = 0;
  end_frame = 0;
  current_frame = 0;
  for (i = 0; i < 77; i++)
  {
    stasm_point_x[i] = -1;
    stasm_point_y[i] = -1;
  }
  face_landmark_list_head = stasm (face_id, current_frame);
  if (face_landmark_list_head != NULL)
  {
    printf ("face_landmark_list_head is NOT NULL!\n");
  }
  else
  {
    printf ("WARNING: face_landmark_list_head is NULL!\n");
  }


  face_landmark_element = face_landmark_list_head;
  while (face_landmark_element != NULL)
  {
    face_landmark_list_head = face_landmark_list_head->next;
    free (face_landmark_element);
    face_landmark_element = face_landmark_list_head;
  }
  exit (SUCCESS);
}




//borkborkbork
face_landmark_node * stasm (char *face_id, int current_frame)
{
  Mat img;
  char filename[1280], debug_filename[1280];
  int i, foundface;
  float landmarks[2 * stasm_NLANDMARKS]; // x,y coords (note the 2)
  static const char * const path = "This is a video dummy.";
  face_landmark_node *face_landmark_list_head;

  face_landmark_list_head = NULL;
//  sprintf (filename, "./ORIGINAL-38.1.png");
  img = imread (path, CV_LOAD_IMAGE_GRAYSCALE);
  if (!img.empty())
  {
    printf ("image width: %d, height: %d\n", img.cols, img.rows);
    if (stasm_search_single(&foundface, landmarks, (const char*)img.data, img.cols, img.rows, path, "../data"))
    {
      if (foundface)
      {
        printf ("You found a hand...\n");
        for (i = 0; i < stasm_NLANDMARKS; i++)
        {
          face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, i, landmarks[i*2], landmarks[i*2+1]);
        }
      }
    }
    else
    {
      printf("stasm_search_single() returned false!\n");
    }
  }
  return face_landmark_list_head;
}






face_landmark_node * add_face_landmark_element (face_landmark_node *face_landmark_list_head, int frame, int indice, float pixel_location_x, float pixel_location_y)
{
  face_landmark_node *new_face_landmark_element, *face_landmark_element, *previous_face_landmark_element;

  new_face_landmark_element = (face_landmark_node *) malloc (sizeof (face_landmark_node));
  if (new_face_landmark_element != NULL)
  {
    new_face_landmark_element->frame = frame;
    new_face_landmark_element->indice = indice;
    new_face_landmark_element->x = pixel_location_x;
    new_face_landmark_element->y = pixel_location_y;
    new_face_landmark_element->next = NULL;
    if (face_landmark_list_head != NULL)
    {
      face_landmark_element = face_landmark_list_head;
      while (face_landmark_element->next != NULL) 
      {
        face_landmark_element = face_landmark_element->next;
      }
      face_landmark_element->next = new_face_landmark_element;
    }
    else
    {
      face_landmark_list_head = new_face_landmark_element;
    }
  }
  return face_landmark_list_head;
}





void draw_markers (face_landmark_node *face_landmark_list_head, char *face_id, int frame)
{
  char output_filename[1280], filename[1280];
  IplImage *img;
  CvPoint point_P1;
  CvPoint point_P2;
  face_landmark_node *face_landmark_element, *face_landmark_element2;
  int radius = 1;
  int i;

  sprintf (filename, "./upload/stasm_training/%s/ORIGINAL-%s.%d.ppm", face_id, face_id, frame);
  img = cvLoadImage (filename);
  if (!img)
  {
//    printf( "No image data for file %s\n", filename);
  }
  else
  {
    for (i = 0; i < 76; i++)
    {
      face_landmark_element = search_face_landmarks (face_landmark_list_head, frame, i);
      face_landmark_element2 = search_face_landmarks (face_landmark_list_head, frame, i+1);
      if ((face_landmark_element != NULL) && (face_landmark_element2 != NULL))
      {
        point_P1 = cvPoint (face_landmark_element->x, face_landmark_element->y);
        point_P2 = cvPoint (face_landmark_element2->x, face_landmark_element2->y);
        cvLine (img, point_P1, point_P2, CV_RGB (0, 0, 255), 1, 8, 0);
      }
    }
//
// NOTE: I purposefully use a second switch statement here rather than embed the drawing statements (below) in the the switch statement above. The reason
// is I want to ensure that all lines are drawn first before compositing OVER the lines with dots. It makes for a tidy look as the last element drawn
// will appear above other layers.
    for (i = 0; i < 77; i++)
    {
      face_landmark_element = search_face_landmarks (face_landmark_list_head, frame, i);
      if (face_landmark_element != NULL)
      {
        point_P1 = cvPoint (face_landmark_element->x, face_landmark_element->y);
        cvCircle (img, point_P1, radius, CV_RGB (255, 255, 0), -1, 8, 0); 
      }
    }
    sprintf (output_filename, "./upload/stasm_training/%s/%s.%d.png", face_id, face_id, frame);
    cvSaveImage (output_filename, img);
    cvReleaseImage (&img);
  }
}





face_landmark_node * search_face_landmarks (face_landmark_node *face_landmark_list_head, int frame, int indice)
{
  face_landmark_node *face_landmark_element;
  int found;

  found = FALSE;
  face_landmark_element = face_landmark_list_head;
  while ((face_landmark_element != NULL) && (!found))
  {
    if (face_landmark_element->frame == frame)
    {
      if (face_landmark_element->indice == indice)
      {
        found = TRUE;
      }
    }
    if (!found)
    {
      face_landmark_element = face_landmark_element->next;
    }
  }
  return face_landmark_element;
}




