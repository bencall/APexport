
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct album_list{
  char *uuid;
  char *name;
  struct album_list *next;
  struct picture_list *pictures;
};


/**
* Adds an album to the list.
* If list is NULL, then it will create a new list
* RETURNS: 0 if ok, -1 if problem with malloc
**/
struct album_list * add_album(struct album_list *list, char *newUuid, char *newName);

/**
* Prints the full list
**/
void print_album_list(struct album_list *list);


/**
* returns the album with the given uuid
**/
struct album_list *find_album_for_uuid(struct album_list *list, char *uuid);

/**
* returns the album with the given name
**/
struct album_list *find_album_for_name(struct album_list *list, char *name);

/**
* Deallocs all the list
**/
void dealloc_album_list(struct album_list *list);
