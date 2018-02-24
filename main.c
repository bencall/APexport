#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sqlite3.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdbool.h>

#include "album_list.h"
#include "picture_list.h"

/** SQL QUERIES **/
#define GET_ALBUMS_SQL "SELECT  name, \
                                uuid \
                                FROM RKAlbum WHERE isHidden = 0 AND isInTrash = 0 AND customSortAvailable = 1 ORDER BY name ASC"
#define GET_ALBUM_COUNT_SQL "SELECT COUNT(*) FROM RKAlbum WHERE isHidden = 0 AND isInTrash = 0 AND customSortAvailable = 1 ORDER BY name ASC"
#define GET_PICTURES_COUNT_SQL "SELECT COUNT(*) FROM RKMaster"
#define GET_ALL_PICTURES_SQL "SELECT  modelId, \
                                      uuid, \
                                      orientation, \
                                      name, \
                                      createDate, \
                                      fileModificationDate, \
                                      fileSize, \
                                      width, \
                                      height, \
                                      originalVersionName, \
                                      fileName, \
                                      isTrulyRaw, \
                                      imagePath, \
                                      imageDate,\
                                      fileCreationDate, \
                                      originalFileName, \
                                      imageTimeZoneOffsetSeconds \
                                      from RKMaster"

//#define GET_PICTURES_SQL(uuid) "SELECT v.filename, m.imagePath FROM RKCustomSortOrder o, RKVersion v, RKMaster m WHERE o.objectUuid = v.Uuid AND v.masterUuid = m.uuid AND o.containerUuid =" uuid "ORDER BY o.orderNumber ASC"


/** FUNCTION DECLARATIONS **/
void print_usage();
int copy_file(char *source_filename, char  *destination_filename);

struct picture_list *getPictureList(sqlite3 *sql3_photo_library);
int getPictureListCount(sqlite3 *sql3_photo_library);

struct album_list *getAlbumList(sqlite3 *sql3_photo_library);
int getAlbumListCount(sqlite3 *sql3_photo_library);

char *getMasterFolder(char *path_to_library);
char *getSQLite3Database(char *path_to_library);

char *retain(char *charToRetain);

/** GLOBAL Variables **/
int count_pictures = -1;
int count_album = -1;


/**
* MAIN Function - starting point
* Assert:
* 1) argc should be equal to 5. If not --> print_usage
**/
int main(int argc, char *argv[])
{
  char *library_path = NULL;                // Path to the General Apple Photos Library
  char *output_folder = NULL;               // Folder where to copy all pictures and XMP files
  char *master_folder_path = NULL;          // Path to the Master folder where all the pictures are stored
  char *sqlite3_db_path = NULL;             // Path to the sqlite3 file
  sqlite3 *sql3_photo_library = NULL;       // Sqlite3 object pointing to the Apple Photos database
  int rc;                                   // return argument

  printf("-------------------------\nWelcome to APexport 0.1a!\n-------------------------\n");

  // Check command line options
  while ((rc = getopt (argc, argv, "l:o:")) != -1)
  {
    switch (rc)
    {
      case 'l':
        library_path = optarg;
        break;
      case 'o':
        output_folder = optarg;
        break;
      case '?':
        if (optopt == 'l' | optopt == 'o'){
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
          return 1;
        }else{
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
          return 1;
        }
    }
  }

  // Check if we have both variables set
  if(!library_path | !output_folder)
  {
    print_usage();
  }

  // We get the path to the sqlite3 file and to the folder where the pics are stores
  master_folder_path = getMasterFolder(library_path);
  sqlite3_db_path = getSQLite3Database(library_path);

  // We open the sqlite db
  rc = sqlite3_open(sqlite3_db_path, &sql3_photo_library);
  if( rc ){
     fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(sql3_photo_library));
     return(1);
  }

  // GET THE ALBUM LIST
  struct album_list *albums = getAlbumListFromDB(sql3_photo_library);
  if(albums == NULL){
    return -1;
  } else {
    print_album_list(albums);
  }

  // GET THE PICTURE LIST
  printf("PICTURE LIST\n");
  struct picture_list *pictures = getPictureListFromDB(sql3_photo_library);
  if(pictures == NULL){
    return -1;
  } else {
//    printf("PRINT PICTURE LIST\n");
//    print_picture_list(pictures);
  }

  printf("EXPORT\n");
  export_picture_list_to_csv(pictures, "export.csv");

  // counts
  count_album = getAlbumListCountFromDB(sql3_photo_library);
  count_pictures = getPictureListCountFromDB(sql3_photo_library);
  printf("Pictures Count: %i / Albums Count: %i\n", count_pictures, count_album);

  // We close the DB
  sqlite3_close(sql3_photo_library);

  // deallocate
  free(master_folder_path);
  free(sqlite3_db_path);

}


/**
* CP FUNCTIONS
* Copies a file "from" to "to"
**/
int cp(const char *to, const char *from)
{
  printf("CP FUNCTION\n");
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}

/**
* getSQLite3Database FUNCTION
* Returns the path to the SQL file where all the info of the pictures are stored
**/
char *getSQLite3Database(char *path_to_library)
{
  char *temp = malloc(sizeof(char)*(10 + strlen(path_to_library)));
  sprintf(temp, "%s/database/photos.db", path_to_library);

  // Master Folder with all the pictures
  char *sqlite3_db_path = malloc((PATH_MAX + 1)*sizeof(char)); /* not sure about the "+ 1" */
  char *ret = realpath(temp, sqlite3_db_path);

  if (!ret){
    fprintf(stderr, "Problem with function realpath (Master folder): %s\n", strerror(errno));
    return NULL;
  }

  free(temp); // We can deallocate the temporary pointer
  return sqlite3_db_path;
}

/**
* GETMASTERFOLDER FUNCTION
* Returns the path to the master folder where all the pictures are stored
**/
char *getMasterFolder(char *path_to_library)
{
  char *temp = malloc(sizeof(char)*(10 + strlen(path_to_library)));
  sprintf(temp, "%s/Masters", path_to_library);

  // Master Folder with all the pictures
  char *master_folder_path = malloc((PATH_MAX + 1)*sizeof(char)); /* not sure about the "+ 1" */
  char *ret = realpath(temp, master_folder_path);

  if (!ret){
    fprintf(stderr, "Problem with function realpath (Master folder): %s\n", strerror(errno));
    return NULL;
  }

  free(temp); // We can deallocate the temporary pointer
  return master_folder_path;
}

/**
* Copies the char in memory and returns the new address
**/
char *retain(char *charToRetain)
{
  if(charToRetain == NULL){
    return NULL;
  }

  char *ret = malloc(strlen(charToRetain) * sizeof(char));
  if (ret==NULL){
    printf("problem malloc retain\n");
  }
  strcpy(ret, charToRetain);

  if (ret==NULL){
    printf("problem strcpy retain\n");
  }
  return ret;
}


/**
* PRINT_USAGE function
**/
void print_usage()
{
  printf("Usage: APexport -l [Photos library] -o [Output folder]\n\n[Photos library]  = path to your Apple Photos Library (.photoslibrary file)\n[Output folder]   = folder where APexport will export the pictures and the XMP files\n");
}
