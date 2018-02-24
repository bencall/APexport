#include "picture_list.h"

/**
* Adds a picture to the list
* If list is NULL, then it will create a new list
**/
struct picture_list * add_picture(struct picture_list *list, struct picture_list *newItem)
{
  // Create new item to add to the list and sets the fields
  if(newItem == NULL){
    return list;
  }

  // Malloc the char*
  // char *uuid = malloc(sizeof(char) * strlen(newUuid));
  // strcpy(uuid, newUuid);
  // char *name = malloc(sizeof(char) * strlen(newName));
  // strcpy(name, newName);

  // new->uuid = uuid;
  // new->name = name;

  // The next field will point to the start of the previous list
  newItem->next = list;
  return newItem;
}

/**
* Prints the full list
**/
void print_picture_list(struct picture_list *list)
{
  struct picture_list *cursor = list;

  while (cursor != NULL) {
    printf("-------------------------------------------------\n");
    // Print fields
    printf("modelID:                %i\n", cursor->modelId);
    printf("UUID:                   %s\n", cursor->uuid);
    printf("Orientation:            %i\n", cursor->orientation);
    printf("Name:                   %s\n", cursor->name);
    printf("Creation Date:          %s\n", cursor->createDate);
    printf("File Modification Date: %s\n", cursor->fileModificationDate);
//    printf("File size:              %i\n", cursor->fileSize);
    printf("Width:                  %i\n", cursor->width);
    printf("Height:                 %i\n", cursor->height);
    printf("Original name:          %s\n", cursor->originalVersionName);
    printf("Filename:               %s\n", cursor->fileName);
    printf("Raw?:                   %i\n", cursor->isTrulyRaw);
    printf("Path:                   %s\n", cursor->imagePath);
    printf("Date:                   %s\n", cursor->imageDate);
    printf("File Creation Date:     %s\n", cursor->fileCreationDate);
    printf("Original filename:      %s\n", cursor->originalFileName);
    printf("Timezone Offset:        %s\n", cursor->imageTimeZoneOffsetSeconds);

    cursor = cursor->next;
  }
}

int export_picture_list_to_csv(struct picture_list *list, char *filename)
{
  // Open file
  FILE *fp = fopen(filename, "w");
  if (fp==NULL){
    return -1;
  }

  // Print column headers
  int ret =  fputs( "modelID, UUID, Orientation, Name, Creation Date, File Modification Date, Width, Height, Original name, Filename, Raw?, Path, Date, File Creation Date, Original filename, Timezone Offset\n", fp );
  if (ret<0){
    return -1;
  }

  struct picture_list *cursor = list;

  while (cursor != NULL) {
    ret =  fprintf(fp, "%i, %s, %i, %s, %s, %s, %i, %i, %s, %s, %i, %s, %s, %s, %s, %s\n", cursor->modelId, cursor->uuid, cursor->orientation, cursor->name, cursor->createDate, cursor->fileModificationDate, cursor->width, cursor->height, cursor->originalVersionName, cursor->fileName, cursor->isTrulyRaw, cursor->imagePath, cursor->imageDate, cursor->fileCreationDate, cursor->originalFileName, cursor->imageTimeZoneOffsetSeconds);
    if (ret<0){
      return -1;
    }
    cursor = cursor->next;
  }

  fclose(fp);
  return 0;
}

struct picture_list * init_picture_list()
{
  struct picture_list *newPicture = malloc(sizeof(struct picture_list));
  newPicture->uuid                        = NULL;
  newPicture->name                        = NULL;
  newPicture->createDate                  = NULL;
  newPicture->fileModificationDate        = NULL;
  newPicture->originalVersionName         = NULL;
  newPicture->fileName                    = NULL;
  newPicture->imagePath                   = NULL;
  newPicture->imageDate                   = NULL;

  return newPicture;
}

/**
* Deallocs the whole list
**/
void dealloc_picture_list(struct picture_list *list)
{
  // struct picture_list *cursor = list;
  //
  // while (cursor != NULL) {
  //   list = cursor;
  //   cursor = cursor->next;
  //   free(list->uuid);
  //   free(list->name);
  //   free(list);
  // }

  return;
}
