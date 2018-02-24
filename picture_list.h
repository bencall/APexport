
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct picture_list{
  int modelId;
  char *uuid;
  int orientation;
  char *name;
//TODO: change to timestamp
  char *createDate;
  char *fileModificationDate;
  int fileSize;
  int width;
  int height;
  char *originalVersionName;
  char *fileName;
  int isTrulyRaw;
  char *imagePath;
  char *imageDate;
  char *fileCreationDate;
  char *originalFileName;
  char *imageTimeZoneOffsetSeconds;

  struct picture_list *next;
};

/**
* Malloc structure and sets all ptr as NULL
*/
struct picture_list * init_picture_list();

/**
* Export structure to csv. Returns 0 if success
*/
int export_picture_list_to_csv(struct picture_list *list, char *filename);

/**
* Adds an album to the list.
* If list is NULL, then it will create a new list
* RETURNS: 0 if ok, -1 if problem with malloc
**/
struct picture_list * add_picture(struct picture_list *list, struct picture_list *newItem);

/**
* Prints the full list
**/
void print_picture_list(struct picture_list *list);

/**
* Deallocs all the list
**/
void dealloc_picture_list(struct picture_list *list);
