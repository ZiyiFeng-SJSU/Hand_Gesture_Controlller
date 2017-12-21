#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wand/MagickWand.h> 
#include "/usr/include/postgresql/libpq-fe.h"
#include "/usr/local/include/opencv/cv.h"
#include "/usr/local/include/opencv/highgui.h"
#include "/usr/local/include/opencv/cvaux.h"
#include "/usr/local/include/opencv/cxcore.h"
#include "/usr/local/include/opencv2/imgproc.hpp"
#include "/usr/local/include/opencv2/imgproc/imgproc.hpp"
#include "/usr/local/include/opencv2/core.hpp"
#include "/usr/local/include/opencv2/core/core.hpp"

#include "stasm/stasm_lib.h"
#include "stasm/stasm_lib_ext.h"  // needed for stasm_search_auto_ext
#include "stasm/stasm_landmarks.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv/cv.hpp>

extern "C" {
#include "/usr/include/x86_64-linux-gnu/libavcodec/avcodec.h"
#include "/usr/include/x86_64-linux-gnu/libavformat/avformat.h"
#include "/usr/include/x86_64-linux-gnu/libswscale/swscale.h"
}





using namespace cv;
using namespace std;



#define SUCCESS 0
#define FAILURE 1
#define FALSE 0
#define TRUE 1





typedef struct get_data_node
{
  char *variable;
  char *value;
  struct get_data_node *next;
} get_data_node;





typedef struct post_data_node
{
  char *variable;  // variable is always NULL terminated
  char *value;     // value IS always NULL terminated.
  char *file_data; // binary file upload. NOT NULL Terminated! Length determined by file_length (below)
  uint64_t file_length;     // specifies the length (in bytes) of file 
  struct post_data_node *next;
} post_data_node;





typedef struct shape_file_node
{
  char face_id[256];
  struct shape_file_node *next;
} shape_file_node;





typedef struct face_landmark_node 
{
  int frame;
  int indice;
  float x;
  float y;
  struct face_landmark_node *next;
} face_landmark_node;





char * html_escape_string (char *);
char * encode_url_string (char *, unsigned int);
char * decode_url_string (char *);
char * decode_url_string_special (char *, unsigned int *);
get_data_node * retrieve_get_data (char *);
post_data_node * retrieve_post_data (char *);
int is_valid_unsigned_integer (char *);
get_data_node * add_get_data_to_list (get_data_node *, char *, char *); 
post_data_node * add_post_data_to_list (post_data_node *, char *, char *, char *, uint64_t);
unsigned int extract_unsigned_64bit_integer_from_argv (char *, uint64_t *);
unsigned int extract_whole_number_from_argv (char *, unsigned int *);
unsigned int extract_decimal_number_from_argv (char *, double *);
unsigned int extract_numbers_from_argv (char *, uint64_t *, unsigned int *);
uint64_t convert_ascii_base10_to_unsigned_64bit_integer (char *);
int      convert_ascii_base10_to_integer (char *);
char * convert_integer_to_ascii_base10 (int);
void send_http_header (void);
void display_html_page_error_occurred (char *);
void display_form (char *, int, int, int, face_landmark_node *, shape_file_node *);
char * db_insert_into_table_raw_face_metadata (PGconn *, int, int);
void db_insert_into_annotated_face_landmark_data (PGconn *, face_landmark_node *, char *, int);
face_landmark_node * retrieve_data_from_annotated_face_landmark_data (PGconn *, char *, int);
void db_update_raw_face_metadata_with_number_of_frames (PGconn *, char *, int);
void SaveFrame (AVFrame *, int, int, int, char *); 
void decode_and_extract_frames_from_motion_picture_file (PGconn *, char *, char *);
void retrieve_metadata (PGconn *, char *, int *, int *);
void convert_still_images_from_ppm_to_png_format (PGconn *, char *);
face_landmark_node * stasm (char *, int);
face_landmark_node * update_stasm (face_landmark_node *, int, int, int, int);
face_landmark_node * add_face_landmark_element (face_landmark_node *, int, int, float, float);
void draw_markers (face_landmark_node *, char *, int);
face_landmark_node * search_face_landmarks (face_landmark_node *, int, int);
shape_file_node * retrieve_all_annotated_shape_files (PGconn *);  
void free_linked_list_memory (face_landmark_node *, shape_file_node *, post_data_node *, get_data_node *);
face_landmark_node * initialize_hands (char *, int);




int main (int argc, char *argv[])
{
  PGconn   *db_connection;
  PGresult *db_result;
  post_data_node *post_data_list_head, *temp_post_data_element, *user_submitted_video_upload;
  get_data_node *get_data_list_head, *temp_get_data_element;
  char web_browser[1024], error_message[5000], client_IP_address[256], server_IP_address[256], filename[1280], form_data_variable[256], *face_id;
  int i, begin_frame, end_frame, current_frame, stasm_point_x[77], stasm_point_y[77], save_stasm_data;
  uint64_t file_upload_size, file_index;
  face_landmark_node *face_landmark_list_head, *face_landmark_element;
  shape_file_node *shape_file_list_head;
  FILE *fp;
 
  save_stasm_data = FALSE; 
  face_id = NULL;
  get_data_list_head  = NULL;
  post_data_list_head = NULL;
  face_landmark_list_head = NULL; 
  user_submitted_video_upload = NULL;
  shape_file_list_head = NULL;
  begin_frame = 0;
  end_frame = 0;
  current_frame = 0;
  for (i = 0; i < 77; i++)
  {
    stasm_point_x[i] = -1;
    stasm_point_y[i] = -1;
  }
  memset (client_IP_address, 0, sizeof(client_IP_address));
  strcpy (server_IP_address, "localhost");
  db_connection = PQconnectdb("host = 'localhost' dbname = 'hand_annotate' user = 'postgres' password = 'postgres'");
  if (PQstatus(db_connection) != CONNECTION_OK)
  {
    strcpy (error_message, "Connection to database failed: ");
    strcat (error_message, PQerrorMessage(db_connection));
    send_http_header ();
    display_html_page_error_occurred (error_message);
    PQfinish (db_connection);
    exit (FAILURE);
  }
  if (getenv("HTTP_USER_AGENT") != NULL)
  {
    if (strlen(getenv("HTTP_USER_AGENT")) < 1024)
    {
      strcpy (web_browser, getenv("HTTP_USER_AGENT"));
    }
  }
  if (getenv("QUERY_STRING") != NULL)
  {
    get_data_list_head = retrieve_get_data (getenv("QUERY_STRING"));
    temp_get_data_element = get_data_list_head;    
    while (temp_get_data_element != NULL)
    {
      if (strcmp(temp_get_data_element->variable, "face_id") == 0)
      {
        if (temp_get_data_element->value != NULL)
        {
          if (strlen(temp_get_data_element->value) < 255)
          {
            face_id = (char *) malloc (sizeof(char)*256);
            if (face_id != NULL)
            {
              strcpy (face_id, temp_get_data_element->value);
            }
          }
        }
      }
      if (strcmp(temp_get_data_element->variable, "current_frame") == 0)
      {
        if (temp_get_data_element->value != NULL)
        {
          current_frame = atoi (temp_get_data_element->value);
        }
      }
      temp_get_data_element = temp_get_data_element->next;
    }
  }
//  if (getenv("REMOTE_ADDR") != NULL)
//  {
//    if (strlen(getenv("REMOTE_ADDR")) < 256)
//    {
//      strcpy (client_IP_address, getenv("REMOTE_ADDR"));
//    }
//  }
  if (getenv("CONTENT_LENGTH") != NULL)
  {
    post_data_list_head = retrieve_post_data (getenv("CONTENT_LENGTH"));
    temp_post_data_element = post_data_list_head;    
    while (temp_post_data_element != NULL)
    {
      if (strcmp(temp_post_data_element->variable, "save_stasm_data") == 0)
      {
        if (temp_post_data_element->value != NULL)
        {
          save_stasm_data = TRUE;
        }
      }
      if (strcmp(temp_post_data_element->variable, "face_id") == 0)
      {
        if (temp_post_data_element->value != NULL)
        {
          face_id = temp_post_data_element->value;
        }
      }
      if (strcmp(temp_post_data_element->variable, "current_frame") == 0)
      {
        if (temp_post_data_element->value != NULL)
        {
          current_frame = atoi(temp_post_data_element->value);
        }
      }
      if (strcmp(temp_post_data_element->variable, "video") == 0)
      {
        if (temp_post_data_element->value != NULL)
        {
          user_submitted_video_upload = temp_post_data_element;
        }
      }
      for (i = 0; i < 77; i++)
      {
        sprintf (form_data_variable, "stasm_point%d_x", i);
        if (strcmp(temp_post_data_element->variable, form_data_variable) == 0)
        {
          if (temp_post_data_element->value != NULL)
          {
            stasm_point_x[i] = atoi(temp_post_data_element->value);
          }
        }
        sprintf (form_data_variable, "stasm_point%d_y", i);
        if (strcmp(temp_post_data_element->variable, form_data_variable) == 0)
        {
          if (temp_post_data_element->value != NULL)
          {
            stasm_point_y[i] = atoi(temp_post_data_element->value);
          }
        }
      }
      temp_post_data_element = temp_post_data_element->next;
    }
  }
  if (user_submitted_video_upload != NULL)
  {
    if (user_submitted_video_upload->file_length > 0) 
    {
//
// Save video user uploaded out to a file. First we must contact database to get a unique identification number to associate with the file upload.
      face_id = db_insert_into_table_raw_face_metadata (db_connection, 1, 0);
      if (face_id != NULL)
      {
        sprintf (filename, "./upload/stasm_training/%s", face_id);
        if (mkdir (filename, 0777) == 0) 
        {
          sprintf (filename, "./upload/stasm_training/%s/%s", face_id, face_id);
          fp = fopen (filename, "w+b");
          if (fp != NULL)
          {
            file_index = 0;
            while (file_index < user_submitted_video_upload->file_length)
            {
              fputc (user_submitted_video_upload->file_data[file_index], fp);
              file_index++;
            }
            fclose (fp);
//
// Now that file attachment was successfully uploaded/saved, associate that file attachment's unique id (face_id) with the user so they may see files uploaded whenever
// they log into the website.
            decode_and_extract_frames_from_motion_picture_file (db_connection, filename, face_id);
          }
        }
      }
    }
  }
  if (face_id != NULL)
  {
    retrieve_metadata (db_connection, face_id, &begin_frame, &end_frame);
    if (current_frame == 0)
    {
      current_frame = begin_frame;
    }
  }
  if (current_frame < begin_frame)
  {
    current_frame = end_frame;
  }
  if (current_frame > end_frame)
  {
    current_frame = begin_frame;
  }
  send_http_header();



  if (face_id != NULL)
  {
    face_landmark_list_head = retrieve_data_from_annotated_face_landmark_data (db_connection, face_id, current_frame);
  }
  if (face_landmark_list_head == NULL)
  {
    if (face_id != NULL)
    {
// comment this out for now since we are annotating hands, not faces.
//    face_landmark_list_head = stasm (face_id, current_frame);
      face_landmark_list_head = initialize_hands (face_id, current_frame);
    }
  }




  for (i = 0; i < 77; i++)
  {
    if ((stasm_point_x[i] > -1) && (stasm_point_y[i] > -1))
    {
      face_landmark_list_head = update_stasm (face_landmark_list_head, i, stasm_point_x[i], stasm_point_y[i], current_frame);
    }
  }
  if (save_stasm_data)
  {
    db_insert_into_annotated_face_landmark_data (db_connection, face_landmark_list_head, face_id, current_frame);
  }
  shape_file_list_head = retrieve_all_annotated_shape_files (db_connection);  
  draw_markers (face_landmark_list_head, face_id, current_frame);
  display_form (face_id, begin_frame, end_frame, current_frame, face_landmark_list_head, shape_file_list_head);
  free_linked_list_memory (face_landmark_list_head, shape_file_list_head, post_data_list_head, get_data_list_head);
  db_result = PQexec (db_connection, "CLOSE myportal");
  PQclear (db_result);
  PQfinish (db_connection);
  exit (SUCCESS);
}





char * encode_url_string (char *input_string, unsigned int byte_string_length)
{
  char *web_safe_output_string, *byte, temp_string[10];
  unsigned int output_string_length, i;
  
  web_safe_output_string = NULL;
  if (input_string != NULL)
  {
    output_string_length = 0;
    byte = input_string;
    i = 0;
    while (i < byte_string_length)
    {  
      if (((byte[i] >= 0) && (byte[i] <= 32))   || ((byte[i] >= 127) && (byte[i] <= 255)) || 
          (byte[i] == '$') || (byte[i] == '&')  || (byte[i] == '+') || (byte[i] == ',')   ||
          (byte[i] == '/') || (byte[i] == ':')  || (byte[i] == ';') || (byte[i] == '=')   ||
          (byte[i] == '?') || (byte[i] == '@')  || (byte[i] == '<') || (byte[i] == '>')   ||
          (byte[i] == '#') || (byte[i] == '%')  || (byte[i] == '{') || (byte[i] == '}')   ||
          (byte[i] == '|') || (byte[i] == '\\') || (byte[i] == '^') || (byte[i] == '~')   ||
          (byte[i] == '[') || (byte[i] == ']')  || (byte[i] == '`') || (byte[i] == '"')   ||
          (byte[i] == '\''))
      {
        output_string_length = output_string_length + 3;
      }
      else
      {
        output_string_length++;
      }
      i++;
    }
    if (output_string_length > 0)
    {
      web_safe_output_string = (char *) malloc(sizeof(char) * (output_string_length + 1));
      if (web_safe_output_string != NULL)
      {
        *web_safe_output_string = '\0';
        byte = input_string;
        i = 0;
        while (i < byte_string_length)
        {  
          if (((byte[i] >= 0) && (byte[i] <= 32))    || ((byte[i] >= 127) && (byte[i] <= 255)) || 
               (byte[i] == '$') || (byte[i] == '&')  || (byte[i] == '+') || (byte[i] == ',')   ||
               (byte[i] == '/') || (byte[i] == ':')  || (byte[i] == ';') || (byte[i] == '=')   ||
               (byte[i] == '?') || (byte[i] == '@')  || (byte[i] == '<') || (byte[i] == '>')   ||
               (byte[i] == '#') || (byte[i] == '%')  || (byte[i] == '{') || (byte[i] == '}')   ||
               (byte[i] == '|') || (byte[i] == '\\') || (byte[i] == '^') || (byte[i] == '~')   ||
               (byte[i] == '[') || (byte[i] == ']')  || (byte[i] == '`') || (byte[i] == '"')   ||
               (byte[i] == '\''))
          {
            sprintf (temp_string, "%%%02X", byte[i]);
          }
          else
          {
            temp_string[0] = byte[i];
            temp_string[1] = '\0';
          }
          strcat (web_safe_output_string, temp_string);
          i++;
        }
      }
    }
  }
  return web_safe_output_string;
}





char * html_escape_string (char *input_string)
{
  char *web_safe_output_string, *byte, temp_string[10];
  uint64_t output_string_length;
  
  web_safe_output_string = NULL;
  if (input_string != NULL)
  {
    output_string_length = 0;
    byte = input_string;
    while (*byte != '\0')
    {  
      if ((*byte == '"') || (*byte =='\'') || (*byte == '&') || (*byte == '<') || (*byte == '>'))
      {
        output_string_length = output_string_length + 5;
      }
      else
      {
        if ((*byte >= 127) && (*byte <= 255))
        {
          output_string_length = output_string_length + 6;
        }
        else
        {
          output_string_length++;
        }
      }
      byte++;
    }
    if (output_string_length > 0)
    {
      web_safe_output_string = (char *) malloc(sizeof(char) * (output_string_length + 1));
      if (web_safe_output_string != NULL)
      {
        *web_safe_output_string = '\0';
        byte = input_string;
        while (*byte != '\0')
        {  
          if ((*byte == '"') || (*byte =='\'') || (*byte == '&') || (*byte == '<') || (*byte == '>') || ((*byte >= 127) && (*byte <= 255)))
          {
            sprintf (temp_string, "&#%d;", *byte);
          }
          else
          {
            temp_string[0] = *byte;
            temp_string[1] = '\0';
          }
          strcat (web_safe_output_string, temp_string);
          byte++;
        }
      }
    }
  }
  return web_safe_output_string;
}




char * decode_url_string_special (char *url_encoded_string, unsigned int *output_string_length)
{
  char *output_string, *temp_output_string, *byte;
  unsigned int state;

  output_string = NULL;
  if (url_encoded_string != NULL)
  {
    state = 0;
    byte = url_encoded_string;
    *output_string_length = 0;
    while (*byte != '\0')
    {
      switch (state)
      {
        case 0:
          if (*byte == '%')
          {
            state = 1;
          }
          (*output_string_length)++;
        break;
	case 1:
          if (((*byte >= 'a') && (*byte <= 'f')) || ((*byte >= 'A') && (*byte <= 'F')) || ((*byte >= '0') && (*byte <= '9')))
          {
            state = 2;
          }
          else
          {
            state = 3;
          }
        break;
        case 2:
          if (((*byte >= 'a') && (*byte <= 'f')) || ((*byte >= 'A') && (*byte <= 'F')) || ((*byte >= '0') && (*byte <= '9')))
          {
            state = 0;
          }
          else
          {
            state = 3;
          }
        break;
        case 3:
        break;
      }
      byte++;
    }
    if ((state == 0) && (*output_string_length > 0))
    {
      output_string = (char *) malloc (sizeof(char) * (*output_string_length + 1));
      if (output_string != NULL)
      {
        byte = url_encoded_string;
        temp_output_string = output_string;
        while (*byte != '\0')
        {
          switch (state)
          {
            case 0:
              if (*byte == '+')
              {
                *temp_output_string = ' ';
                temp_output_string++;
              }
              else
              {
                if (*byte != '%')
                {
                  *temp_output_string = *byte;
                  temp_output_string++;
                }
                else
                {
                  state = 1;
                }
              }
            break;
            case 1:
              if ((*byte >= 'a') && (*byte <= 'f'))
              {
                *temp_output_string = *byte - 'a' + 10;
                state = 2;
              }
              else
              {
                if ((*byte >= 'A') && (*byte <= 'F'))
                {
                  *temp_output_string = *byte - 'A' + 10;
                  state = 2;
                }
                else
                {
                  if ((*byte >= '0') && (*byte <= '9'))
                  {
                    *temp_output_string = *byte - '0';
                    state = 2;
                  }
                  else
                  {
                    state = 3;
                  }
                }
              }
            break;
            case 2:
              if ((*byte >= 'a') && (*byte <= 'f'))
              {
                *temp_output_string = *temp_output_string * 16 + *byte - 'a' + 10;
                temp_output_string++;
                state = 0;
              }
              else
              {
                if ((*byte >= 'A') && (*byte <= 'F'))
                {
                  *temp_output_string = *temp_output_string * 16 + *byte - 'A' + 10;
                  temp_output_string++;
                  state = 0;
                }
                else
                {
                  if ((*byte >= '0') && (*byte <= '9'))
                  {
                    *temp_output_string = *temp_output_string * 16 + *byte - '0';
                    temp_output_string++;
                    state = 0;
                  }
                  else
                  {
                    state = 3;
                  }
                }
              }
            case 3:
            break;
          }
          byte++;
        }
      }
    }
  }
  return output_string;
}





char * decode_url_string (char *url_encoded_string)
{
  char *output_string, *temp_output_string, *byte;
  uint64_t output_string_length;
  unsigned int state;

  output_string = NULL;
  if (url_encoded_string != NULL)
  {
    state = 0;
    byte = url_encoded_string;
    output_string_length = 0;
    while (*byte != '\0')
    {
      switch (state)
      {
        case 0:
          if (*byte == '%')
          {
            state = 1;
          }
          output_string_length++;
        break;
	case 1:
          if (((*byte >= 'a') && (*byte <= 'f')) || ((*byte >= 'A') && (*byte <= 'F')) || ((*byte >= '0') && (*byte <= '9')))
          {
            state = 2;
          }
          else
          {
            state = 3;
          }
        break;
        case 2:
          if (((*byte >= 'a') && (*byte <= 'f')) || ((*byte >= 'A') && (*byte <= 'F')) || ((*byte >= '0') && (*byte <= '9')))
          {
            state = 0;
          }
          else
          {
            state = 3;
          }
        break;
        case 3:
        break;
      }
      byte++;
    }
    if ((state == 0) && (output_string_length > 0))
    {
      output_string = (char *) malloc (sizeof(char) * (output_string_length + 1));
      if (output_string != NULL)
      {
        byte = url_encoded_string;
        temp_output_string = output_string;
        while (*byte != '\0')
        {
          switch (state)
          {
            case 0:
              if (*byte == '+')
              {
                *temp_output_string = ' ';
                temp_output_string++;
              }
              else
              {
                if (*byte != '%')
                {
                  *temp_output_string = *byte;
                  temp_output_string++;
                }
                else
                {
                  state = 1;
                }
              }
            break;
            case 1:
              if ((*byte >= 'a') && (*byte <= 'f'))
              {
                *temp_output_string = *byte - 'a' + 10;
                state = 2;
              }
              else
              {
                if ((*byte >= 'A') && (*byte <= 'F'))
                {
                  *temp_output_string = *byte - 'A' + 10;
                  state = 2;
                }
                else
                {
                  if ((*byte >= '0') && (*byte <= '9'))
                  {
                    *temp_output_string = *byte - '0';
                    state = 2;
                  }
                  else
                  {
                    state = 3;
                  }
                }
              }
            break;
            case 2:
              if ((*byte >= 'a') && (*byte <= 'f'))
              {
                *temp_output_string = *temp_output_string * 16 + *byte - 'a' + 10;
                temp_output_string++;
                state = 0;
              }
              else
              {
                if ((*byte >= 'A') && (*byte <= 'F'))
                {
                  *temp_output_string = *temp_output_string * 16 + *byte - 'A' + 10;
                  temp_output_string++;
                  state = 0;
                }
                else
                {
                  if ((*byte >= '0') && (*byte <= '9'))
                  {
                    *temp_output_string = *temp_output_string * 16 + *byte - '0';
                    temp_output_string++;
                    state = 0;
                  }
                  else
                  {
                    state = 3;
                  }
                }
              }
            case 3:
            break;
          }
          byte++;
        }
        *temp_output_string = '\0';
      }
    }
  }
  return output_string;
}





get_data_node * retrieve_get_data (char *raw_get_data)
{
  char *raw_variable, *variable, *raw_value, *value, byte;
  uint64_t      bytes_read, get_data_length, string_length_variable, string_length_value, begin, end, i, j, string_length;
  get_data_node *get_data_list_head;
  unsigned int state;
  int valid_get_data;

  get_data_list_head = NULL;
  get_data_length = strlen(raw_get_data);
  if (get_data_length > 0)
  {
//
// get data comes in as a continuous string of variable=value pairs like this:
// session_id=123&command=search&query=this+is+my+search
// 
// The next step is to parse this string for syntax errors. This is to prevent possible attacks from malicious users!
    valid_get_data = FALSE;
    state = 0;
    bytes_read = 0;
    while (bytes_read < get_data_length)
    {
      byte = raw_get_data[bytes_read];
      switch (state)
      {
        case 0:
          if (byte != '=')
          {
            state = 1;
          }
          else
          {
            state = 3;
          }
        break;
        case 1:
          if (byte == '=')
          {
            state = 2;
          }
        break;
        case 2:
          if (byte == '&')
          {
            state = 0;
          }
        break;
        case 3:
        break;
      }
      bytes_read++;
    }
    if (state == 2)
    {
      valid_get_data = TRUE;
    }
//
// Ok, now that the post data has been verified to be syntactically correct, parse once more to separate and store the variable/value pairs in
// a linked list data structure
    if (valid_get_data)
    {
      state = 0;
      bytes_read = 0;
      while (bytes_read < get_data_length)
      {
        byte = raw_get_data[bytes_read];
        switch (state)
        {
          case 0:
            begin = bytes_read;
            end = 0;
            state = 1;
          break;
          case 1:
            if (byte == '=')
            {
              end = bytes_read - 1;
              string_length = end - begin + 1;
              raw_variable = (char *) malloc (sizeof(char) * (string_length + 1));
              if (raw_variable != NULL)
              {
                i = 0;
                j = begin;
                while (i < string_length)
                {
                  raw_variable[i] = raw_get_data[j];
                  i++;
                  j++;
                }
                raw_variable[i] = '\0';
                variable = decode_url_string (raw_variable);
                free (raw_variable);
                raw_variable = NULL;
              }
              state = 2;
            }
          break;
          case 2:
            if (byte != '&')
            {
              begin = bytes_read;
              state = 3;
            }
            else
            {
              value = NULL;
              get_data_list_head = add_get_data_to_list (get_data_list_head, variable, value); 
              state = 0;
            }
          break;
          case 3:
            if (byte == '&')
            {
              end = bytes_read - 1;
              string_length = end - begin + 1;
              raw_value = (char *) malloc (sizeof(char) * string_length + 1);
              if (raw_value != NULL)
              {
                i = 0;
                j = begin;
                while (i < string_length)
                {
                  raw_value[i] = raw_get_data[j];
                  i++;
                  j++;
                }
                raw_value[i] = '\0';
                value = decode_url_string (raw_value);
                get_data_list_head = add_get_data_to_list (get_data_list_head, variable, value); 
                free (raw_value);
                raw_value = NULL;
                state = 0;
              }
            }   
          break;
        }  
        bytes_read++;
        if ((state == 3) && (bytes_read == get_data_length))
        {
          end = bytes_read - 1;
          string_length = end - begin + 1;
          raw_value = (char *) malloc (sizeof(char) * (string_length + 1));
          if (raw_value != NULL)
          {
            i = 0;
            j = begin;
            while (i < string_length)
            {
              raw_value[i] = raw_get_data[j];
              i++;
              j++;
            }
            raw_value[i] = '\0';
            value = decode_url_string (raw_value);
            get_data_list_head = add_get_data_to_list (get_data_list_head, variable, value); 
            free (raw_value);
            raw_value = NULL;
          }
        }
      }
    }
  }
  return get_data_list_head;
}






post_data_node * retrieve_post_data (char *post_data_length_string)
{
  char *raw_variable, *variable, *raw_value, *value, *raw_post_data, byte, *boundary_string, *file_data;
  uint64_t      bytes_read, post_data_length, string_length_variable, string_length_value, begin, end, i, j, string_length, boundary_string_length, file_length;
  post_data_node *post_data_list_head;
  unsigned int state, match, done, is_style2_post_data_format, num_digits, num_dashes;
  int valid_post_data;

  post_data_list_head = NULL;
  is_style2_post_data_format = FALSE;
//
// First check to make sure input string is a valid unsigned integer and not some random garbage. 
// The reason for doing this is to prevent possible malicious attacks.
  if (is_valid_unsigned_integer (post_data_length_string))
  {
    post_data_length = convert_ascii_base10_to_unsigned_64bit_integer (post_data_length_string);
    if (post_data_length > 0)
    {
      raw_post_data = (char *) malloc(sizeof(char) * (post_data_length + 1));
      if (raw_post_data != NULL)
      {
        bytes_read = 0;
        while (bytes_read < post_data_length)
        {
          raw_post_data[bytes_read] = getchar();
          bytes_read++;
        }
        raw_post_data[bytes_read] = '\0';
//
// post data comes in as a continuous string of variable=value pairs like this (style #1):
// 
// session_id=123&command=search&query=this+is+my+search
// 
// 
//
// Alternately, if a file is being uploaded, we'll receive something in a format that looks like this (style #2):
//
// -----------------------------27462048810346577881349256922\r\n
// Content-Disposition: form-data; name="video"; filename="rendered_model_preview_thumbnail.png"\r\n
// Content-Type: image/png\r\n
// \r\n
// PNG\r\n\n\rIHDRddsRGBpHYstIME"btEXtCommentCreated with GIMPWIDATx_H_QAxUdXM"]EwAI"xxd(wKiwqL79gsuvyg=waaaaaaaaV+;JIKGvO%$))'OLMM/&$IW\I(<"bUUPv3$ES{{;H$vq7ad%Or8J`xxgDt5x\X-//y&0JUUUe6e0J$._LDUUU]f@QC_#Dt-9slwsDT^^0JVj5V}{&Hl0$>iO>|k,#e8&00$;%Eu`or(wF,L&4M[YYyltsXZZZ0_~?~o===_|z.;q^.)).Q @D~]ZZG&K;|"cdr \nQEE?^V=J7u_r_$Y&t-Y;;;-~H70P \H/HQ86UUtxsgG?\r)Z[[<~<x'p8v(vcS]LNNeH$N:t6\r\r\r\rx-d[rRTT9rd}}=H<??fZ2,PkS; ^wyW#y`>::\nkR|!02>}Z***#e~2cnnN4(J,^w!>22yF|~za%{$:00+-^w!faut/N'd2)j~+!'aaaaaaaaarWSoIENDB`\r\n
// -----------------------------27462048810346577881349256922\r\n
// Content-Disposition: form-data; name="submit"\r\n
// \r\n
// submit\r\n
// -----------------------------27462048810346577881349256922\r\n
// Content-Disposition: form-data; name="session_id"\r\n
// \r\n8FCADC223625049991E2A13149EDAF9AE4536A81\r\n
// -----------------------------27462048810346577881349256922--\r\n
//
// The next step is to parse this string to determine if data is in format style #2:
        state = 0;
        bytes_read = 0;
        num_dashes = 0;
        num_digits = 0;
        done = FALSE;
        while ((bytes_read < post_data_length) && (!done))
        {
          byte = raw_post_data[bytes_read];
          switch (state)
          {
            case 0:
              if (byte == '-')
              {
                num_dashes++;
                state = 0;
              }
              else
              {
                if ((byte >= '0') && (byte <= '9'))
                {
                  num_digits++;
                  state = 1;
                }
                else
                {
                  done = TRUE;
                }
              }
            break;
            case 1:
              if ((byte >= '0') && (byte <= '9'))
              {
                num_digits++;
                state = 1;
              }
              else
              {
                if (byte == '\r')
                {
                  state = 2;
                }
                else
                {
                  done = TRUE;
                }
              }
            break;
            case 2:
              if (byte == '\n')
              {
                if ((num_dashes > 5) && (num_digits > 5))
                {
                  is_style2_post_data_format = TRUE;
                }
              }
              done = TRUE;
            break;
          }
          if (!done)
          {
            bytes_read++;
          }
        }
//
// Knowing the data doesn't appear to be in style #2 post data format, go ahead and parse as style #1 format.
        if (!is_style2_post_data_format)
        {
          state = 0;
          bytes_read = 0;
          while (bytes_read < post_data_length) 
          {
            byte = raw_post_data[bytes_read];
            switch (state)
            {
              case 0:
                begin = bytes_read;
                end = 0;
                state = 1;
              break;
              case 1:
                if (byte == '=')
                {
                  end = bytes_read - 1;
                  string_length = end - begin + 1;
                  raw_variable = (char *) malloc (sizeof(char) * (string_length + 1));
                  if (raw_variable != NULL)
                  {
                    i = 0;
                    j = begin;
                    while (i < string_length)
                    {
                      raw_variable[i] = raw_post_data[j];
                      i++;
                      j++;
                    }
                    raw_variable[i] = '\0';
                    variable = decode_url_string (raw_variable);
                    free (raw_variable);
                    raw_variable = NULL;
                  }
                  state = 2;
                }
              break;
              case 2:
                if (byte != '&')
                {
                  begin = bytes_read;
                  state = 3;
                }
                else
                {
                  value = NULL;
                  post_data_list_head = add_post_data_to_list (post_data_list_head, variable, value, NULL, 0); 
                  state = 0;
                }
              break;
              case 3:
                if (byte == '&')
                {
                  end = bytes_read - 1;
                  string_length = end - begin + 1;
                  raw_value = (char *) malloc (sizeof(char) * (string_length + 1));
                  if (raw_value != NULL)
                  {
                    i = 0;
                    j = begin;
                    while (i < string_length)
                    {
                      raw_value[i] = raw_post_data[j];
                      i++;
                      j++;
                    }
                    raw_value[i] = '\0';
                    value = decode_url_string (raw_value);
                    post_data_list_head = add_post_data_to_list (post_data_list_head, variable, value, NULL, 0); 
                    free (raw_value);
                    raw_value = NULL;
                    state = 0;
                  }
                }   
              break;
            }  
            bytes_read++;
            if ((state == 3) && (bytes_read == post_data_length))
            {
              end = bytes_read - 1;
              string_length = end - begin + 1;
              raw_value = (char *) malloc (sizeof(char) * (string_length + 1));
              if (raw_value != NULL)
              {
                i = 0;
                j = begin;
                while (i < string_length)
                {
                  raw_value[i] = raw_post_data[j];
                  i++;
                  j++;
                }
                raw_value[i] = '\0';
                value = decode_url_string (raw_value);
                post_data_list_head = add_post_data_to_list (post_data_list_head, variable, value, NULL, 0); 
                free (raw_value);
                raw_value = NULL;
              }
            }
          }
        }
//
// Parse the data as style #2 post data format.
        if (is_style2_post_data_format)
        {
          done = FALSE;
          state = 0;
          bytes_read = 0;
          while ((bytes_read < post_data_length) && (!done))
          {
            byte = raw_post_data[bytes_read];
            switch (state)
            {
              case 0:
                begin = 0;
                end = 0;
                if (byte == '-')
                {
                  begin = bytes_read;
                  state = 1;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 1:
                if (byte == '-')
                {
                  state = 1;
                }
                else
                {
                  if ((byte >= '0') && (byte <= '9'))
                  {
                    state = 2;
                  }
                  else
                  {
                    done = TRUE;
                  }
                }
              break;
              case 2:
                if ((byte >= '0') && (byte <= '9'))
                {
                  state = 2;
                }
                else
                {
                  if (byte == '\r')
                  {
                    end = bytes_read - 1;
                    state = 3;
                  }
                  else
                  {
                    done = TRUE;
                  }
                }
              break;
              case 3:
                if (byte == '\n')
                {
                  state = 4;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 4:
                boundary_string_length = end - begin + 1;
                boundary_string = (char *) malloc (sizeof(char) * (boundary_string_length + 1));
                if (boundary_string != NULL)
                {
                  i = 0;
                  j = begin;
                  while (i < boundary_string_length)
                  {
                    boundary_string[i] = raw_post_data[j];
                    i++;
                    j++;
                  }
                  boundary_string[i] = '\0';
                  if (byte == 'C')
                  {
                    state = 5;
                  }
                  else
                  {
                    done = TRUE;
                  }
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 5:
                if (byte == 'o') // Co
                {
                  state = 6;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 6:
                if (byte == 'n') // Con
                {
                  state = 7;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 7:
                if (byte == 't') // Cont
                {
                  state = 8;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 8:
                if (byte == 'e') // Conte
                {
                  state = 9;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 9:
                if (byte == 'n') // Conten
                {
                  state = 10;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 10:
                if (byte == 't') // Content
                {
                  state = 11;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 11:
                if (byte == '-') // Content-
                {
                  state = 12;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 12:
                if (byte == 'D') // Content-D
                {
                  state = 13;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 13:
                if (byte == 'i') // Content-Di
                {
                  state = 14;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 14:
                if (byte == 's') // Content-Dis
                {
                  state = 15;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 15:
                if (byte == 'p') // Content-Disp
                {
                  state = 16;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 16:
                if (byte == 'o') // Content-Dispo
                {
                  state = 17;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 17:
                if (byte == 's') // Content-Dispos
                {
                  state = 18;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 18:
                if (byte == 'i') // Content-Disposi
                {
                  state = 19;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 19:
                if (byte == 't') // Content-Disposit
                {
                  state = 20;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 20:
                if (byte == 'i') // Content-Dispositi
                {
                  state = 21;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 21:
                if (byte == 'o') // Content-Dispositio
                {
                  state = 22;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 22:
                if (byte == 'n') // Content-Disposition
                {
                  state = 23;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 23:
                if (byte == ':') // Content-Disposition:
                {
                  state = 24;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 24:
                if (byte == ' ') // Content-Disposition:<space> 
                {
                  state = 25;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 25:
                if (byte == 'f') // Content-Disposition: f
                {
                  state = 26;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 26:
                if (byte == 'o') // Content-Disposition: fo
                {
                  state = 27;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 27:
                if (byte == 'r') // Content-Disposition: for
                {
                  state = 28;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 28:
                if (byte == 'm') // Content-Disposition: form
                {
                  state = 29;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 29:
                if (byte == '-') // Content-Disposition: form-
                {
                  state = 30;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 30:
                if (byte == 'd') // Content-Disposition: form-d
                {
                  state = 31;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 31:
                if (byte == 'a') // Content-Disposition: form-da
                {
                  state = 32;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 32:
                if (byte == 't') // Content-Disposition: form-dat
                {
                  state = 33;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 33:
                if (byte == 'a') // Content-Disposition: form-data
                {
                  state = 34;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 34:
                if (byte == ';') // Content-Disposition: form-data;
                {
                  state = 35;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 35:
                if (byte == ' ') // Content-Disposition: form-data;<space>
                {
                  state = 36;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 36:
                if (byte == 'n') // Content-Disposition: form-data; n
                {
                  state = 37;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 37:
                if (byte == 'a') // Content-Disposition: form-data; na 
                {
                  state = 38;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 38:
                if (byte == 'm') // Content-Disposition: form-data; nam
                {
                  state = 39;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 39:
                if (byte == 'e') // Content-Disposition: form-data; name
                {
                  state = 40;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 40:
                if (byte == '=') // Content-Disposition: form-data; name=
                {
                  state = 41;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 41:
                if (byte == '"') // Content-Disposition: form-data; name="
                {
                  state = 42;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 42:
                if (byte != '"') // Content-Disposition: form-data; name="
                {
                  begin = bytes_read;
                  state = 43;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 43:
                if (byte != '"') // Content-Disposition: form-data; name=".......
                {
                  state = 43;
                }
                else
                {
                  end = bytes_read - 1;
                  string_length = end - begin + 1;
                  variable = (char *) malloc (sizeof(char) * (string_length + 1)); // 1 extra byte reserved for null terminator
                  if (variable != NULL)
                  {
                    i = 0;
                    j = begin;
                    while (i < string_length)
                    {
                      variable[i] = raw_post_data[j];
                      i++;
                      j++;
                    }
                    variable[i] = '\0';
                    state = 44;
                  }
                  else
                  {
                    done = TRUE;
                  }
                }
              break;
              case 44:
                if (byte == ';')  // Content-Disposition: form-data; name=".......";
                {
                  state = 45;
                }
                else
                {
                  if (byte == '\r')  // Content-Disposition: form-data; name="......."\r
                  {
                    state = 80;
                  }
                  else
                  {
                    done = TRUE;
                  }
                }
              break;
              case 45:
                if (byte == ' ')  // Content-Disposition: form-data; name=".......";<space>
                {
                  state = 46;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 46:
                if (byte == 'f')  // Content-Disposition: form-data; name="......."; f
                {
                  state = 47;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 47:
                if (byte == 'i')  // Content-Disposition: form-data; name="......."; fi
                {
                  state = 48;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 48:
                if (byte == 'l')  // Content-Disposition: form-data; name="......."; fil
                {
                  state = 49;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 49:
                if (byte == 'e')  // Content-Disposition: form-data; name="......."; file
                {
                  state = 50;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 50:
                if (byte == 'n')  // Content-Disposition: form-data; name="......."; filen
                {
                  state = 51;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 51:
                if (byte == 'a')  // Content-Disposition: form-data; name="......."; filena
                {
                  state = 52;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 52:
                if (byte == 'm')  // Content-Disposition: form-data; name="......."; filenam
                {
                  state = 53;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 53:
                if (byte == 'e')  // Content-Disposition: form-data; name="......."; filename
                {
                  state = 54;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 54:
                if (byte == '=')  // Content-Disposition: form-data; name="......."; filename=
                {
                  state = 55;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 55:
                if (byte == '"')  // Content-Disposition: form-data; name="......."; filename="
                {
                  state = 56;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 56:
                begin = bytes_read;
                if (byte != '"')  // Content-Disposition: form-data; name="......."; filename=".......
                {
                  state = 57;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 57:
                if (byte != '"')  // Content-Disposition: form-data; name="......."; filename=".......
                {
                  state = 57;
                }
                else              // Content-Disposition: form-data; name="......."; filename="......."
                {
                  end = bytes_read - 1;
                  string_length = end - begin + 1;
                  value = (char *) malloc (sizeof(char) * (string_length + 1)); // 1 extra byte reserved for null terminator
                  if (value != NULL)
                  {
                    i = 0;
                    j = begin;
                    while (i < string_length)
                    {
                      value[i] = raw_post_data[j];
                      i++;
                      j++;
                    }
                    value[i] = '\0';
                    state = 58;
                  }
                  else
                  {
                    done = TRUE;
                  }
                }
              break;
              case 58:
                if (byte == '\r')  // Content-Disposition: form-data; name="......."; filename="......."\r
                {
                  state = 59;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 59:
                if (byte == '\n')  // Content-Disposition: form-data; name="......."; filename="......."\r\n
                {
                  state = 60;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 60:
                if (byte == 'C')  // C
                {
                  state = 61;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 61:
                if (byte == 'o')  // Co
                {
                  state = 62;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 62:
                if (byte == 'n')  // Con
                {
                  state = 63;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 63:
                if (byte == 't')  // Cont
                {
                  state = 64;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 64:
                if (byte == 'e')  // Conte
                {
                  state = 65;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 65:
                if (byte == 'n')  // Conten
                {
                  state = 66;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 66:
                if (byte == 't')  // Content
                {
                  state = 67;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 67:
                if (byte == '-')  // Content-
                {
                  state = 68;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 68:
                if (byte == 'T')  // Content-T
                {
                  state = 69;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 69:
                if (byte == 'y')  // Content-Ty
                {
                  state = 70;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 70:
                if (byte == 'p')  // Content-Typ
                {
                  state = 71;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 71:
                if (byte == 'e')  // Content-Type
                {
                  state = 72;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 72:
                if (byte == ':')  // Content-Type:
                {
                  state = 73;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 73:
                if (byte == ' ')  // Content-Type:<space>
                {
                  state = 74;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 74:
                if (byte != '\r')  // Content-Type: ........
                {
                  state = 74;
                }
                else
                {
                  state = 75;      // Content-Type: ........\r
                }
              break;
              case 75:
                if (byte == '\n')  // Content-Type: ........\r\n
                {
                  state = 76;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 76:
                if (byte == '\r')  // \r
                {
                  state = 77;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 77:
                if (byte == '\n')  // \r\n
                {
                  state = 78;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 78: // beginning of binary data for file upload begins here.
                begin = bytes_read;
                state = 79;
              break;
              case 79:
                i = 0;
                j = bytes_read;
                match = TRUE;
                while ((i < boundary_string_length) && (j < post_data_length) && match)
                {
                  if (boundary_string[i] == raw_post_data[j])
                  {
                    i++;
                    j++;
                  }
                  else
                  {
                    match = FALSE;
                  }
                }
                if ((i == boundary_string_length) && match)
                {
                  end = bytes_read - 2; // subtract 2 bytes because \r\n always follows data stream.
                  if (end > begin)
                  {
                    file_length = end - begin;
                    file_data = (char *) malloc (sizeof(char) * (file_length));
                    if (file_data != NULL)
                    {
                      i = 0;
                      j = begin;
                      while (i < file_length)
                      {
                        file_data[i] = raw_post_data[j];
                        i++;
                        j++;
                      }
                    post_data_list_head = add_post_data_to_list (post_data_list_head, variable, value, file_data, file_length); 
                    }
                  }
                  bytes_read = bytes_read + boundary_string_length - 1;
                  state = 87;
                }
                else
                {
                  state = 79;
                }
              break;
              case 80:
                if (byte == '\n')  // Content-Disposition: form-data; name="......."\r\n
                {
                  state = 81;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 81:
                if (byte == '\r')  // \r
                {
                  state = 82;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 82:
                if (byte == '\n')  // \r\n
                {
                  state = 83;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 83:
                begin = bytes_read;
                state = 84;
              break;
              case 84:
                if (byte != '\r')  // .......
                {
                  state = 84;
                }
                else
                {
                  state = 85;
                }
              break;
              case 85:
                if (byte == '\n')  // .......\r\n
                {
                  state = 86;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 86:
                i = 0;
                j = bytes_read;
                match = TRUE;
                while ((i < boundary_string_length) && (j < post_data_length) && match)
                {
                  if (boundary_string[i] == raw_post_data[j])
                  {
                    i++;
                    j++;
                  }
                  else
                  {
                    match = FALSE;
                  }
                }
                if ((i == boundary_string_length) && match)
                {
                  end = bytes_read - 3;
                  string_length = end - begin + 1;
                  value = (char *) malloc (sizeof(char) * (string_length + 1)); // 1 extra byte reserved for null terminator
                  if (value != NULL)
                  {
                    i = 0;
                    j = begin;
                    while (i < string_length)
                    {
                      value[i] = raw_post_data[j];
                      i++;
                      j++;
                    }
                    value[i] = '\0';
                    post_data_list_head = add_post_data_to_list (post_data_list_head, variable, value, NULL, 0); 
                    bytes_read = bytes_read + boundary_string_length - 1;
                    state = 87;
                  }
                  else
                  {
                    done = TRUE;
                  }
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 87:
                if (byte == '\r')
                {
                  state = 88;
                }
                else
                {
                  if ((bytes_read + 3) < post_data_length)
                  {
                    if ((raw_post_data[bytes_read] == '-') && (raw_post_data[bytes_read + 1] == '-') && (raw_post_data[bytes_read + 2] == '\r') && (raw_post_data[bytes_read + 3] == '\n'))
                    {
                      done = TRUE;
                    }
                  }
                  else
                  {
                    done = TRUE;
                  }
                }
              break;
              case 88:
                if (byte == '\n')
                {
                  state = 89;
                }
                else
                {
                  done = TRUE;
                }
              break;
              case 89:
                if (byte == 'C')
                {
                  state = 5;
                }
                else
                {
                  done = TRUE;
                }
              break;
            }
            bytes_read++;
          }
        }
        free (boundary_string);
        free (raw_post_data);
        boundary_string = NULL;
        raw_post_data = NULL;
      }
    }
  }
  return post_data_list_head;
}




int is_valid_unsigned_integer (char *number_string)
{
  unsigned int state, i;
  int is_valid_number;

  is_valid_number = FALSE;
  state = 0;
  i = 0;
  while ((number_string[i] != '\0') && (state == 0))
  {
    if ((number_string[i] >= '0') && (number_string[i] <= '9'))
    {
      state = 0;
      i++;
    }
    else
    {
      state = 1;
    }
  }
  if (state == 0)
  {
    is_valid_number = TRUE;
  }
  return is_valid_number;
}






char * convert_unsigned_char_to_ascii_hex (char *source_char_ptr, unsigned int string_length)
{
  char byte, *hex_string_ptr;
  unsigned int byte_counter, offset, num_hex_digits, hex_digit;

  byte_counter = 0;
  offset = 0;
  hex_string_ptr = (char *) malloc(sizeof(char *) * (string_length * 2 + 1));
  while (byte_counter < string_length)
  {
    byte = source_char_ptr[byte_counter];
    num_hex_digits = 2;
    while (num_hex_digits > 0)
    {
      --num_hex_digits;
      hex_digit = byte % 16;
      if (hex_digit < 10)
      {
        hex_digit = hex_digit + '0';
      }
      else
      {
        hex_digit = hex_digit - 10 + 'A';
      }
      hex_string_ptr[offset + num_hex_digits] = hex_digit;
      byte = byte / 16;
    } 
    offset = offset + 2;
    byte_counter++;
  }
  hex_string_ptr[offset] = '\0';
  return hex_string_ptr;
}






int convert_ascii_base10_to_integer (char *number_string)
{
  int i, number, sign;

  i = 0;
  number = 0;
  sign = 1;
  if (number_string[i] == '-')
  {
    sign = -1;
    i++;
  }
  while (number_string[i] != '\0')
  {
    number = number * 10 + number_string[i] - '0';
    i++;
  }
  number = number * sign;
  return number;
}





float convert_ascii_string_db_double_array_results_to_float (char *database_results, int desired_element1, int desired_element2)
{
  char *byte, number_string[500];
  int state, i, j, k, begin, end, element_counter1, element_counter2;
  float single_precision_float_result;

  single_precision_float_result = 0.0;
  begin = -1;
  end   = -1;
  element_counter1 = -1;
  element_counter2 = -1;
  state = 0;
  i = 0;
  byte = database_results;
  while ((byte[i] != '\0') && (state < 6))
  {
    switch (state)
    {
      case 0: 
        if (byte[i] == '{')
        {
          state = 1;
        }
        else
        {
          state = 7;
        }
      break;
      case 1: 
        if (byte[i] == '{')
        {
          state = 2;
          element_counter1++;
          element_counter2 = -1;
        }
        else
        {
          state = 7;
        }
      break;
      case 2: 
        if ((byte[i] == '-') || ((byte[i] >= '0') && (byte[i] <= '9')))
        {
          state = 3;
          element_counter2++;
          begin = i;
          end = -1;
        }
        else
        {
          state = 7;
        }
      break;
      case 3:
        if (byte[i] == ',')
        {
          state = 2;
          if (begin != -1)
          {
            end = i - 1;
            if ((desired_element1 == element_counter1) && (desired_element2 == element_counter2))
            {
              state = 6;
              j = begin;
              k = 0;
              while (j <= end)
              {
                number_string[k] = byte[j];
                j++;
                k++; 
              }
              number_string[k] = '\0';
              single_precision_float_result = (float) atof(number_string);
            }
          }
        }
        else
        {
          if ((byte[i] >= '0') && (byte[i] <= '9'))
          {
            state = 3;
          }
          else
          {
            if (byte[i] == '.')
            {
              state = 4;
            }
            else
            {
              if (byte[i] == '}')
              {
                state = 5;
                if (begin != -1)
                {
                  end = i - 1;
                  if ((desired_element1 == element_counter1) && (desired_element2 == element_counter2))
                  {
                    state = 6;
                    j = begin;
                    k = 0;
                    while (j <= end)
                    {
                      number_string[k] = byte[j];
                      j++;
                      k++; 
                    }
                    number_string[k] = '\0';
                    single_precision_float_result = (float) atof(number_string);
                  }
                }
              }
              else
              {
                state = 7;
              }
            }
          }
        }
      break;
      case 4:
        if (byte[i] == ',')
        {
          state = 2;
          if (begin != -1)
          {
            end = i - 1;
            if ((desired_element1 == element_counter1) && (desired_element2 == element_counter2))
            {
              state = 6;
              j = begin;
              k = 0;
              while (j <= end)
              {
                number_string[k] = byte[j];
                j++;
                k++; 
              }
              number_string[k] = '\0';
              single_precision_float_result = (float) atof(number_string);
            }
          }
        }
        else
        {
          if ((byte[i] >= '0') && (byte[i] <= '9'))
          {
            state = 4;
          }
          else
          {
            if (byte[i] == '}')
            {
              state = 5;
              if (begin != -1)
              {
                end = i - 1;
                if ((desired_element1 == element_counter1) && (desired_element2 == element_counter2))
                {
                  state = 6;
                  j = begin;
                  k = 0;
                  while (j <= end)
                  {
                    number_string[k] = byte[j];
                    j++;
                    k++; 
                  }
                  number_string[k] = '\0';
                  single_precision_float_result = (float) atof(number_string);
                }
              }
            }
            else
            {
              state = 7;
            }
          }
        }
      break;
      case 5:
        if (byte[i] == ',')
        {
          state = 1;
        }
        else
        {
          if (byte[i] == '}')
          {
            state = 6;
          }
          else
          {
            state = 7;
          }
        }
      break;
      case 6:
      break;
      case 7:
      break;
      default:
      break;
    }
    i++;
  }
  return single_precision_float_result;
}





uint64_t convert_ascii_base10_to_unsigned_64bit_integer (char *number_string)
{
  unsigned int i;
  uint64_t number;

  i = 0;
  number = 0;
  if (number_string[i] == '-')
  {
    i++;
  }
  while (number_string[i] != '\0')
  {
    number = number * 10 + number_string[i] - '0';
    i++;
  }
  return number;
}





char * convert_integer_to_ascii_base10 (int number)
{
  unsigned short int num_digits, sign_byte;
  int temp_number;
  char *number_string, sign;
  
  num_digits = 0;
  if (number == 0)
  {
    number_string = (char *) malloc(sizeof(char) * 2);
    number_string[0] = '0';
    number_string[1] = '\0';
  }
  else
  {
    sign_byte = 0;
    if (number < 0)
    {
      sign_byte = 1;
    }
    temp_number = abs(number);
    while (temp_number > 0)
    {
      temp_number = temp_number / 10;
      num_digits++;
    }
    number_string = (char *) malloc(sizeof(char) * (num_digits + sign_byte + 1));
    number_string[num_digits + sign_byte] = '\0';
    temp_number = abs(number);
    while (num_digits > 0)
    {
      --num_digits;
      number_string[num_digits + sign_byte] = temp_number % 10 + '0';
      temp_number = temp_number / 10;
    }
    if (number < 0)
    {
      number_string[0] = '-';
    }
  }
  return number_string;
}





void send_http_header (void)
{
  printf ("Content-type: text/html\n\n");
}  





void display_html_page_error_occurred (char *error_message)
{
  printf ("<html>");
  printf ("<body>");
  if (error_message != NULL)
  {
    printf ("<p>%s</p>\n", error_message);
  }
  printf ("</body>");
  printf ("</html>");
}





post_data_node * add_post_data_to_list (post_data_node *post_data_list_head, char *variable, char *value, char *file_data, uint64_t file_length)
{
  post_data_node *new_post_data_element, *temp_post_data_element;

  new_post_data_element = (post_data_node *) malloc(sizeof(post_data_node));
  if (new_post_data_element != NULL)
  {
    new_post_data_element->variable    = variable;
    new_post_data_element->value       = value;
    new_post_data_element->file_data   = file_data;
    new_post_data_element->file_length = file_length;
    new_post_data_element->next        = NULL;
  }
  if (post_data_list_head == NULL) {
    post_data_list_head = new_post_data_element;
  }
  else
  {
    temp_post_data_element = post_data_list_head;
    while (temp_post_data_element->next != NULL) 
    {
      temp_post_data_element = temp_post_data_element->next;
    }
    temp_post_data_element->next = new_post_data_element;
  }
  return post_data_list_head;
}





get_data_node * add_get_data_to_list (get_data_node *get_data_list_head, char *variable, char *value) 
{
  get_data_node *new_get_data_element, *temp_get_data_element;

  new_get_data_element = (get_data_node *) malloc(sizeof(get_data_node));
  if (new_get_data_element != NULL)
  {
    new_get_data_element->variable = variable;
    new_get_data_element->value    = value;
    new_get_data_element->next     = NULL;
  }
  if (get_data_list_head == NULL) {
    get_data_list_head = new_get_data_element;
  }
  else
  {
    temp_get_data_element = get_data_list_head;
    while (temp_get_data_element->next != NULL) 
    {
      temp_get_data_element = temp_get_data_element->next;
    }
    temp_get_data_element->next = new_get_data_element;
  }
  return get_data_list_head;
}





char * db_insert_into_table_raw_face_metadata (PGconn *db_connection, int begin_frame, int end_frame)
{
  char database_statement_ptr[5000], *face_id, *number_string_ptr;
  int face_id_string_length;
  PGresult *db_result;

  face_id = NULL;
  strcpy (database_statement_ptr, "INSERT INTO raw_face_metadata (start_frame, end_frame) VALUES ('");
  number_string_ptr = convert_integer_to_ascii_base10 (begin_frame);
  if (number_string_ptr != NULL)
  {
    strcat (database_statement_ptr, number_string_ptr);
    free (number_string_ptr);
    number_string_ptr = NULL;
  }
  strcat (database_statement_ptr, "','");
  number_string_ptr = convert_integer_to_ascii_base10 (end_frame);
  if (number_string_ptr != NULL)
  {
    strcat (database_statement_ptr, number_string_ptr);
    free (number_string_ptr);
    number_string_ptr = NULL;
  }
  strcat (database_statement_ptr, "') RETURNING face_id");
  db_result = PQexec (db_connection, database_statement_ptr);
  if (PQresultStatus(db_result) == PGRES_TUPLES_OK)
  {
    if (PQntuples(db_result) == 1)
    {
      face_id_string_length = strlen (PQgetvalue(db_result, 0, 0));
      face_id = (char *) malloc(sizeof(char) * (face_id_string_length + 1));
      if (face_id != NULL)
      { 
        strcpy (face_id, PQgetvalue(db_result, 0, 0));
      }
    }
  } 
  PQclear(db_result);
  return (face_id);
}    






void convert_still_images_from_ppm_to_png_format (PGconn *db_connection, char *face_id)
{ 
  MagickBooleanType status; 
  MagickWand *magick_wand; 
  int original_width, original_height, begin_frame, end_frame, current_frame;
  char input_filename[1280], output_filename[1280];

  begin_frame = 0;
  end_frame = 0;
  retrieve_metadata (db_connection, face_id, &begin_frame, &end_frame);
  if ((begin_frame > 0) && (end_frame > 0))
  {
    MagickWandGenesis(); 
    magick_wand = NewMagickWand(); 
    current_frame = begin_frame;
    while (current_frame <= end_frame)
    {
      sprintf (input_filename, "./upload/stasm_training/%s/ORIGINAL-%s.%d.ppm", face_id, face_id, current_frame);
      sprintf (output_filename, "./upload/stasm_training/%s/ORIGINAL-%s.%d.png", face_id, face_id, current_frame);
      status= MagickReadImage (magick_wand, input_filename); 
      if (status != MagickFalse) 
      {
        status = MagickWriteImages (magick_wand, output_filename, MagickTrue); 
        if (status != MagickFalse) 
        {
          unlink (input_filename);     
        }
      }
      ClearMagickWand (magick_wand);
      current_frame++;
    }
    magick_wand = DestroyMagickWand (magick_wand); 
    MagickWandTerminus(); 
  }
}








void SaveFrame (AVFrame *pFrame, int width, int height, int iFrame, char *face_id) 
{
  FILE *pFile;
  char szFilename[2000];
  int  y;

  sprintf (szFilename, "./upload/stasm_training/%s/ORIGINAL-%s.%d.ppm", face_id, face_id, iFrame);
  pFile = fopen (szFilename, "wb");
  if (pFile == NULL)
  {
    return;
  }
//
// Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
//  
// Write pixel data
  for (y=0; y < height; y++)
  {
    fwrite (pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);
  }
  fclose(pFile);
}





void decode_and_extract_frames_from_motion_picture_file (PGconn *db_connection, char *input_filename, char *face_id)
{
  AVFormatContext   *pFormatCtx = NULL;
  int               i, videoStream;
  AVCodecContext    *pCodecCtx = NULL;
  AVCodec           *pCodec = NULL;
  AVFrame           *pFrame = NULL; 
  AVFrame           *pFrameRGB = NULL;
  AVPacket          packet;
  int               frameFinished;
  int               numBytes;
  uint8_t           *buffer = NULL;
  AVDictionary      *optionsDict = NULL;
  struct SwsContext *sws_ctx = NULL;

  av_register_all();
  if (avformat_open_input (&pFormatCtx, input_filename, NULL, NULL) != 0)
  {
    return;
  }
  if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
  {
    return; // Couldn't find stream information
  }
//  av_dump_format just writes metadata information about the video file characteristics but nothing else. Just mute this.
  av_dump_format (pFormatCtx, 0, input_filename, 0);
  videoStream=-1;
  for (i = 0; i < pFormatCtx->nb_streams; i++)
  {
    if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) 
    {
      videoStream=i;
      break;
    }
  }
  if (videoStream == -1)
  {
    return;
  }
  pCodecCtx = pFormatCtx->streams[videoStream]->codec;
  pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
  if (pCodec == NULL) 
  {
//    fprintf (stderr, "Unsupported codec!\n");
    return;
  }
  if (avcodec_open2(pCodecCtx, pCodec, &optionsDict) < 0)
  {
    return;
  }
  pFrame = avcodec_alloc_frame();
  pFrameRGB = avcodec_alloc_frame();
  if (pFrameRGB == NULL)
  {
    return;
  }
  numBytes = avpicture_get_size (PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
  buffer = (uint8_t *) av_malloc (numBytes * sizeof(uint8_t));
  sws_ctx = sws_getContext (pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
  avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height); 
  i = 0;
  while (av_read_frame (pFormatCtx, &packet) >= 0) 
  {
    if (packet.stream_index == videoStream) 
    {
      avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
      if (frameFinished) 
      {
        sws_scale (sws_ctx, (uint8_t const * const *)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
	SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, ++i, face_id);
      }
    }
    av_free_packet(&packet);
  }
  av_free(buffer);
  av_free(pFrameRGB);
  av_free(pFrame);
  avcodec_close(pCodecCtx);
  avformat_close_input(&pFormatCtx);
  db_update_raw_face_metadata_with_number_of_frames (db_connection, face_id, i);
}





void db_update_raw_face_metadata_with_number_of_frames (PGconn *db_connection, char *face_id, int end_frame)
{
  char db_statement_ptr[5000], *number_string_ptr;
  int face_id_string_length;
  PGresult *db_result;

  if (face_id != NULL)
  {
    strcpy (db_statement_ptr, "UPDATE raw_face_metadata SET end_frame = '");
    number_string_ptr = convert_integer_to_ascii_base10 (end_frame);
    if (number_string_ptr != NULL)
    {
      strcat (db_statement_ptr, number_string_ptr);
      free (number_string_ptr);
      number_string_ptr = NULL;
    }
    strcat (db_statement_ptr, "' WHERE face_id = '");
    strcat (db_statement_ptr, face_id);
    strcat (db_statement_ptr, "'");
    db_result = PQexec (db_connection, db_statement_ptr);
    if (PQresultStatus(db_result) != PGRES_COMMAND_OK)
    {
      printf("%s\n", PQerrorMessage(db_connection));
    }
    PQclear(db_result);
  }
}





void db_insert_into_annotated_face_landmark_data (PGconn *db_connection, face_landmark_node *face_landmark_list_head, char *face_id, int current_frame)
{
  PGresult *db_result;
  face_landmark_node *face_landmark_element;
  char db_statement_ptr[5000];
  int x, y, do_update;

  if (face_id != NULL)
  {
    face_landmark_element = face_landmark_list_head;
    while (face_landmark_element != NULL)
    {
      if (face_landmark_element->frame == current_frame)
      {
        do_update = FALSE;
        x = face_landmark_element->x;
        y = face_landmark_element->y;
        sprintf (db_statement_ptr, "INSERT INTO annotated_face_landmark_data VALUES ('%s','%d','%d','%d','%d')", face_id, face_landmark_element->frame, face_landmark_element->indice, x, y);
        db_result = PQexec (db_connection, db_statement_ptr);
        if (PQresultStatus(db_result) != PGRES_COMMAND_OK)
        {
          do_update = TRUE;
        }
        PQclear(db_result);
        if (do_update)
        {
          sprintf (db_statement_ptr, "UPDATE annotated_face_landmark_data SET x = '%d', y = '%d' WHERE (face_id = '%s') AND (frame = '%d') AND (indice = '%d')", x, y, face_id, face_landmark_element->frame, face_landmark_element->indice);
          db_result = PQexec (db_connection, db_statement_ptr);
          if (PQresultStatus(db_result) != PGRES_COMMAND_OK)
          {
            printf("%s\n", PQerrorMessage(db_connection));
          }
          PQclear(db_result);
        }
      }
      face_landmark_element = face_landmark_element->next;
    }
  }
}





void retrieve_metadata (PGconn *db_connection, char *face_id, int *begin_frame, int *end_frame)
{
  PGresult *db_result;
  char db_statement[5000];

  sprintf (db_statement, "SELECT start_frame, end_frame FROM raw_face_metadata WHERE face_id = '%s'", face_id);
  db_result = PQexec(db_connection, db_statement);
  if (PQresultStatus(db_result) == PGRES_TUPLES_OK)
  {
    if (PQntuples(db_result) == 1)
    {
      *begin_frame = atoi(PQgetvalue(db_result, 0, 0));
      *end_frame = atoi(PQgetvalue(db_result, 0, 1));
    }
    PQclear(db_result);
  }
}





void display_form (char *face_id, int begin_frame, int end_frame, int current_frame, face_landmark_node *face_landmark_list_head, shape_file_node *shape_file_list_head)
{
  shape_file_node *shape_file_element;
  face_landmark_node *face_landmark_element;
  int previous_frame, next_frame, indice, x, y;

  previous_frame = current_frame;
  next_frame = current_frame;
  if (previous_frame == begin_frame)
  {
    previous_frame = end_frame;
  }
  else
  {
    previous_frame--;
  }
  if (next_frame == end_frame)
  {
    next_frame = begin_frame;
  }
  else
  {
    next_frame++;
  }
  printf ("<!doctype html>\n");
  printf ("<html xmlns=\"https://www.w3.org/1999/xhtml\" dir=\"ltr\" lang=\"en-US\">\n");
  printf ("  <head>\n");
  printf ("    <title>Hand Annotate</title>\n");
  printf ("    <link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"http://localhost/css/rob_style.css\"/>\n");
  printf ("    <script type=\"text/javascript\" src=\"http://localhost/jquery/jquery-1.11.1.js\"></script>\n");
  printf ("    <script type=\"text/javascript\" src=\"http://localhost/jquery/jquery-ui-1.11.0/jquery-ui.min.js\"></script>\n");
  if (face_landmark_list_head != NULL)
  {
    printf ("    <script type=\"text/javascript\">\n");
    printf ("<!--\n");
  }
  indice = 0;
  face_landmark_element = face_landmark_list_head;
  while (face_landmark_element != NULL)
  {
    printf ("$(document).ready(function()\n");
    printf ("{\n");
    printf ("  $('#dg%d').draggable (\n", indice);
    printf ("  {\n");
    printf ("    containment: '#container',\n");
    printf ("    scroll: false,\n");
    printf ("    opacity: 0.35,\n");
    printf ("    stack: $('#dg%d'),\n", indice);
    printf ("    drag: function(event)\n");
    printf ("    {\n");
    printf ("      jQuery('.stasm_info').html('STASM point %d at (' + $(this).position().left.toFixed() + ',' + $(this).position().top.toFixed() + ')');\n", indice);
    printf ("    },\n");
    printf ("    stop: function(event , ui)\n");
    printf ("    {\n");
    printf ("      jQuery('.stasm_info').html('STASM point %d at (' + $(this).position().left.toFixed() + ',' + $(this).position().top.toFixed() + ')');\n", indice);
    printf ("      $('#stasm_point%d_x').val($(this).position().left.toFixed());\n", indice);
    printf ("      $('#stasm_point%d_y').val($(this).position().top.toFixed());\n", indice);
    printf ("    }\n");                         
    printf ("  });\n");
    printf ("  function updateCoordinate (newCoordinate)\n");
    printf ("  {\n");
    printf ("    jQuery('.stasm_info').text(newCoordinate);\n");
    printf ("  }\n");
    printf ("});\n");
    indice++;
    face_landmark_element = face_landmark_element->next;
  }
  if (face_landmark_list_head != NULL)
  {
    printf ("-->\n");
    printf ("    </script>\n");
  }
  printf ("  </head>\n");
  printf ("  <body>\n");
  if (face_id != NULL)
  {
    printf ("    <div id=\"container\" style=\"width: 1920px; height: 1080px; border: 1px solid black; position: relative;\">\n");
    printf ("      <img src=\"http://localhost/images/%s/%s.%d.png\">\n", face_id, face_id, current_frame);
    indice = 0;
    face_landmark_element = face_landmark_list_head;
    while (face_landmark_element != NULL)
    {
      x = face_landmark_element->x;
      y = face_landmark_element->y;
      printf ("      <img src=\"http://localhost/img/zebra_crosshair.png\" alt=\"STASM %d\" width=\"7\" height=\"7\" id=\"dg%d\" style=\"position: absolute; left: %dpx; top: %dpx;\"/>\n", indice, indice, x, y);
      indice++;
      face_landmark_element = face_landmark_element->next;
    }
    printf ("    </div>\n");
  }
  if (face_landmark_list_head != NULL)
  {
    printf ("    <table>\n");
    printf ("      <tr>\n");
    printf ("        <td><a class=\"button\" href=\"http://localhost/cgi-bin/hand_annotate.cgi?face_id=%s&current_frame=%d\">Previous frame</a>&nbsp;<a class=\"button\" href=\"http://localhost/cgi-bin/hand_annotate.cgi?face_id=%s&current_frame=%d\">Next frame</a></td>\n", face_id, previous_frame, face_id, next_frame);
    printf ("      </tr>\n");
    printf ("    </table>\n");
  }
  printf ("    <div class=\"stasm_info\"></div>\n");
  if (face_landmark_list_head != NULL)
  {
    printf ("    <form name=\"save_stasm_data\" action=\"http://localhost/cgi-bin/hand_annotate.cgi\" enctype=\"multipart/form-data\" method=\"post\">\n");
    printf ("      <input name=\"face_id\" type=\"hidden\" value=\"%s\">\n", face_id);
    printf ("      <input name=\"current_frame\" type=\"hidden\" value=\"%d\">\n", current_frame);
  }
  indice = 0;
  face_landmark_element = face_landmark_list_head;
  while (face_landmark_element != NULL)
  {
    x = face_landmark_element->x;
    y = face_landmark_element->y;
    printf ("      <input id=\"stasm_point%d_x\" name=\"stasm_point%d_x\" type=\"hidden\" value=\"%d\">\n", indice, indice, x);
    printf ("      <input id=\"stasm_point%d_y\" name=\"stasm_point%d_y\" type=\"hidden\" value=\"%d\">\n", indice, indice, y);
    indice++;
    face_landmark_element = face_landmark_element->next;
  }
  if (face_landmark_list_head != NULL)
  {
    printf ("      <p><input name=\"save_stasm_data\" type=\"submit\" value=\"Save STASM data on frame %d for video %s\"></p>\n", current_frame, face_id);
    printf ("    </form>\n");
  }
  printf ("    <form action=\"http://localhost/cgi-bin/hand_annotate.cgi\" enctype=\"multipart/form-data\" method=\"post\">\n");
  if (shape_file_list_head != NULL)
  {
    printf ("      <p>Edit STASM training <select name=\"face_id\">");
    shape_file_element = shape_file_list_head;
    while (shape_file_element != NULL)
    {
      printf ("<option value=\"%s\">Video %s</option>", shape_file_element->face_id, shape_file_element->face_id);
      shape_file_element = shape_file_element->next;
    }
    printf ("</select> OR upload a new video from file: <input type=\"file\" name=\"video\"></p>\n");
  }
  else
  {
    printf ("      <p>Upload a video from file: <input type=\"file\" name=\"video\"></p>\n");
  }
  printf ("      <p><input name=\"submit\" type=\"submit\" value=\"submit\"></p>\n");
  printf ("    </form>\n");
  printf ("  </body>\n");
  printf ("</html>\n");
}





face_landmark_node * stasm (char *face_id, int current_frame)
{
  Mat img;
  char filename[1280], debug_filename[1280];
  int i, foundface;
  float landmarks[2 * stasm_NLANDMARKS]; // x,y coords (note the 2)
  static const char * const path = "bogus_path";
  face_landmark_node *face_landmark_list_head;

  face_landmark_list_head = NULL;
  sprintf (filename, "./upload/stasm_training/%s/ORIGINAL-%s.%d.ppm", face_id, face_id, current_frame);

  img = imread (filename, CV_LOAD_IMAGE_GRAYSCALE);
  if (!img.empty())
  {
    if (stasm_search_single(&foundface, landmarks, (const char*)img.data, img.cols, img.rows, path, "./data"))
    {
      if (foundface)
      {
        for (i = 0; i < stasm_NLANDMARKS; i++)
        {
          face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, i, landmarks[i*2], landmarks[i*2+1]);
        }
      }
    }
  }
  return face_landmark_list_head;
}





face_landmark_node * update_stasm (face_landmark_node *face_landmark_list_head, int stasm_point, int stasm_point_x, int stasm_point_y, int current_frame)
{
  face_landmark_node *face_landmark_element;
  int found;

  if ((stasm_point > -1) && (stasm_point_x > -1) && (stasm_point_y > -1))
  {
    found = FALSE;
    face_landmark_element = face_landmark_list_head;
    while ((face_landmark_element != NULL) && (!found))
    {
      if ((face_landmark_element->indice == stasm_point) && (face_landmark_element->frame == current_frame))
      {
        found = TRUE;
      }
      if (!found)
      {
        face_landmark_element = face_landmark_element->next;
      }
    }
    if (found)
    {
      face_landmark_element->x = stasm_point_x;
      face_landmark_element->y = stasm_point_y;
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





face_landmark_node * retrieve_data_from_annotated_face_landmark_data (PGconn *db_connection, char *face_id, int frame)
{
  PGresult *db_result;
  char db_statement[5000], float_number_string[1000], int_number_string[1000];
  unsigned int i, number_string_length, block_element, current_block_structure_element;
  face_landmark_node *face_landmark_list_head;
  float x, y;
  int indice;

  face_landmark_list_head = NULL;
  strcpy (db_statement, "SELECT indice, x, y FROM annotated_face_landmark_data WHERE face_id = '");
  strcat (db_statement, face_id);
  strcat (db_statement, "'");
  strcat (db_statement, " AND frame = '");
  int_number_string[0] = '\0';
  sprintf (int_number_string, "%d", frame);
  strcat (db_statement, int_number_string);
  strcat (db_statement, "'");
  db_result = PQexec(db_connection, db_statement);
  if (PQresultStatus(db_result) == PGRES_TUPLES_OK)
  {
    for (i = 0; i < PQntuples(db_result); i++)
    {
      indice = atoi(PQgetvalue(db_result, i, 0));
      x = atof(PQgetvalue(db_result, i, 1));
      y = atof(PQgetvalue(db_result, i, 2));
      face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, frame, indice, x, y);
    }
    PQclear(db_result);
  }
  return face_landmark_list_head;
}



shape_file_node * retrieve_all_annotated_shape_files (PGconn *db_connection)  
{
  PGresult *db_result;
  shape_file_node *shape_file_list_head, *new_shape_file_element;
  char db_statement[5000];
  int i;

  shape_file_list_head = NULL;
  strcpy (db_statement, "SELECT DISTINCT(face_id) FROM annotated_face_landmark_data ORDER BY face_id DESC");
  db_result = PQexec(db_connection, db_statement);
  if (PQresultStatus(db_result) == PGRES_TUPLES_OK)
  {
    for (i = 0; i < PQntuples(db_result); i++)
    {
      if (strlen(PQgetvalue(db_result, i, 0)) < 255)
      {
        new_shape_file_element = (shape_file_node *) malloc(sizeof(shape_file_node));
        if (new_shape_file_element != NULL)
        {
          strcpy (new_shape_file_element->face_id, PQgetvalue(db_result, i, 0));
          new_shape_file_element->next = shape_file_list_head;
          shape_file_list_head = new_shape_file_element;
        }
      }
    }
    PQclear(db_result);
  }
  return shape_file_list_head;
}





void free_linked_list_memory (face_landmark_node *face_landmark_list_head, shape_file_node *shape_file_list_head, post_data_node *post_data_list_head, get_data_node *get_data_list_head)
{
  face_landmark_node *face_landmark_element;
  shape_file_node *shape_file_element;
  post_data_node *post_data_element;
  get_data_node *get_data_element;

  face_landmark_element = face_landmark_list_head;
  while (face_landmark_element != NULL)
  {
    face_landmark_list_head = face_landmark_list_head->next;
    free (face_landmark_element);
    face_landmark_element = face_landmark_list_head;
  }
  shape_file_element = shape_file_list_head;
  while (shape_file_element != NULL)
  {
    shape_file_list_head = shape_file_list_head->next;
    free (shape_file_element);
    shape_file_element = shape_file_list_head;
  }
  post_data_element = post_data_list_head;
  while (post_data_element != NULL)
  {
    post_data_list_head = post_data_list_head->next;
    free (post_data_element);
    post_data_element = post_data_list_head;
  }
  get_data_element = get_data_list_head;
  while (get_data_element != NULL)
  {
    get_data_list_head = get_data_list_head->next;
    free (get_data_element);
    get_data_element = get_data_list_head;
  }
}






face_landmark_node * initialize_hands (char *face_id, int current_frame)
{
  face_landmark_node *face_landmark_list_head;

  face_landmark_list_head = NULL;
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 0, 180, 636);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 1, 194, 598);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 2, 200, 570);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 3, 180, 540);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 4, 155, 507);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 5, 124, 476);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 6, 102, 429);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 7, 79, 394);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 8, 69, 361);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 9, 53, 330);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 10, 57, 306);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 11, 83, 309);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 12, 113, 328);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 13, 130, 354);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 14, 150, 382);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 15, 174, 404);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 16, 196, 384);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 17, 213, 362);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 18, 226, 333);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 19, 222, 299);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 20, 213, 265);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 21, 205, 223);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 22, 194, 182);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 23, 192, 150);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 24, 193, 114);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 25, 213, 91);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 26, 236, 110);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 27, 241, 140);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 28, 247, 169);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 29, 260, 207);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 30, 271, 247);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 31, 289, 289);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 32, 295, 249);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 33, 293, 217);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 34, 293, 179);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 35, 295, 147);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 36, 300, 116);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 37, 309, 79);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 38, 335, 65);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 39, 351, 88);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 40, 350, 118);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 41, 350, 148);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 42, 350, 181);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 43, 350, 215);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 44, 351, 248);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 45, 357, 291);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 46, 372, 252);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 47, 382, 219);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 48, 392, 188);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 49, 400, 156);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 50, 409, 126);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 51, 433, 108);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 52, 447, 134);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 53, 443, 162);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 54, 437, 196);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 55, 431, 230);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 56, 425, 272);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 57, 420, 304);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 58, 419, 337);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 59, 439, 307);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 60, 452, 280);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 61, 466, 257);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 62, 482, 221);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 63, 510, 213);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 64, 515, 239);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 65, 507, 269);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 66, 494, 296);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 67, 481, 337);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 68, 467, 371);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 69, 457, 408);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 70, 447, 450);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 71, 437, 488);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 72, 424, 519);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 73, 413, 554);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 74, 405, 589);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 75, 393, 618);
  face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, current_frame, 76, 382, 647);
  return face_landmark_list_head;
}
