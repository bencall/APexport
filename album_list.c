#include "album_list.h"

/**
* Adds an album to the list
* If list is NULL, then it will create a new list
**/
struct album_list * add_album(struct album_list *list, char *newUuid, char *newName)
{
  // Create new item to add to the list and sets the fields
  struct album_list *new = malloc(sizeof(struct album_list));
  if(new == NULL){
    return NULL;
  }

  // Malloc the char*
  char *uuid = malloc(sizeof(char) * strlen(newUuid));
  strcpy(uuid, newUuid);
  char *name = malloc(sizeof(char) * strlen(newName));
  strcpy(name, newName);

  new->uuid = uuid;
  new->name = name;

  // The next field will point to the start of the previous list
  new->next = list;
  return new;
}

/**
* Prints the full list
**/
void print_album_list(struct album_list *list)
{
  struct album_list *cursor = list;

  while (cursor != NULL) {
    printf("UUID:%s   |   NAME:%s\n", cursor->uuid, cursor->name);
    cursor = cursor->next;
  }
}


/**
* returns the album with the given uuid
**/
struct album_list *find_album_for_uuid(struct album_list *list, char *uuid)
{
  struct album_list *cursor = list;

  while (cursor != NULL) {
    if (strcmp(cursor->uuid, uuid) == 0){
      return cursor;
    } else {
      cursor = cursor->next;
    }
  }

  return NULL;
}

/**
* returns the album with the given name
**/
struct album_list *find_album_for_name(struct album_list *list, char *name)
{
  struct album_list *cursor = list;

  while (cursor != NULL) {
    if (strcmp(cursor->name, name) == 0){
      return cursor;
    } else {
      cursor = cursor->next;
    }
  }

  return NULL;
}

/**
* Deallocs all the list
**/
void dealloc_album_list(struct album_list *list){
  struct album_list *cursor = list;

  while (cursor != NULL) {
    list = cursor;
    cursor = cursor->next;
    free(list->uuid);
    free(list->name);
    free(list);
  }

  return;
}
