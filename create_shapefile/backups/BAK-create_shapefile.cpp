#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "/usr/include/postgresql/libpq-fe.h"





typedef struct face_landmark_node 
{
  int frame;
  int indice;
  int x;
  int y;
  struct face_landmark_node *next;
} face_landmark_node;





face_landmark_node * add_face_landmark_element (face_landmark_node *, int, float, float);






face_landmark_node * add_face_landmark_element (face_landmark_node *face_landmark_list_head, int frame, int indice, int pixel_location_x, int pixel_location_y)
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





face_landmark_node * retrieve_data_from_annotated_face_landmark_data (PGconn *db_connection, char *face_id)
{
  PGresult *db_result;
  char db_statement[5000];
  face_landmark_node *face_landmark_list_head;
  int i, x, y, frame, indice;

  face_landmark_list_head = NULL;
  sprintf (db_statement, "SELECT frame, indice, x, y FROM annotated_face_landmark_data WHERE face_id = '%s' ORDER BY frame, indice", face_id);
  db_result = PQexec(db_connection, db_statement);
  if (PQresultStatus(db_result) == PGRES_TUPLES_OK)
  {
    for (i = 0; i < PQntuples(db_result); i++)
    {
      frame = atoi(PQgetvalue(db_result, i, 0));
      indice = atoi(PQgetvalue(db_result, i, 1));
      x = atoi(PQgetvalue(db_result, i, 2));
      y = atoi(PQgetvalue(db_result, i, 3));
      face_landmark_list_head = add_face_landmark_element (face_landmark_list_head, frame, indice, x, y);
    }
    PQclear(db_result);
  }
  return face_landmark_list_head;
}





int main (int argc, char *argv[]) 
{
  PGconn *db_connection;
  PGresult *db_result;
  face_landmark_node *face_landmark_list_head, *face_landmark_element;
  char face_id[256];
  int i, frame;

  face_landmark_list_head = NULL;
  face_id[0] = '\0';
  if (argc != 2)
  {
    printf ("Please enter a face_id number on command line.\n");
    return (0);
  }
  db_connection = PQconnectdb("host = 'localhost' dbname = 'hand_annotate' user = 'postgres' password = 'postgres'");
  if (PQstatus(db_connection) != CONNECTION_OK)
  {
    fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(db_connection));
    PQfinish (db_connection);
    return (-1);
  }
  i = strlen(argv[1]);
  if (i < 255)
  {
    strcpy (face_id, argv[1]);
  }
  face_landmark_list_head = retrieve_data_from_annotated_face_landmark_data (db_connection, face_id);
  if (face_landmark_list_head != NULL)
  {
    printf ("shape %s.shape\n", face_id);
    printf ("\n");
    printf ("Directories /home/gesture/hand_shape_files/%s\n", face_id);
    printf ("\n");
    frame = face_landmark_list_head->frame;
    printf ("0000 %s_%d\n", face_id, frame);
    printf ("{ 77 2\n");
  }
  face_landmark_element = face_landmark_list_head;
  while (face_landmark_element != NULL)
  {
    if (frame != face_landmark_element->frame)
    {
      printf ("}\n");
      frame = face_landmark_element->frame;
      printf ("0000 %s_%d\n", face_id, frame);
      printf ("{ 77 2\n");
    }
    printf ("%d %d\n", face_landmark_element->x, face_landmark_element->y); 
    face_landmark_element = face_landmark_element->next;
  }
  if (face_landmark_list_head != NULL)
  {
    printf ("}\n");
  }
//
// Free all memory
  while (face_landmark_list_head != NULL)
  {
    face_landmark_element = face_landmark_list_head;
    face_landmark_list_head = face_landmark_list_head->next;
    free (face_landmark_element);
    face_landmark_element = NULL;
  }
//
// Disconnect from database server
  db_result = PQexec (db_connection, "CLOSE myportal");
  PQclear (db_result);
  PQfinish (db_connection);
  return 0;
}
