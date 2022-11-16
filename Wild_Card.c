#include <stdio.h>
#include <Windows.h>
#include <string.h>

#define SUCCESS         1
#define FAILURE         0

typedef int status_t;

status_t FindFileWithWildCard(char *PathName, char *WildCard);

int main(void)
{
    status_t s;
    char *PathName = "C:\\Users\\Prashant\\Desktop\\Nikita";
    char *WildCard = "\\*.c";

    s = FindFileWithWildCard(PathName, WildCard);

    if(s == FAILURE)
    {
        printf("\nNot a single file with given wild card\n");
    }

    return 0;
}

status_t FindFileWithWildCard(char *PathName, char *WildCard)
{
    //char FilePath[MAX_PATH];
    char *FilePath = (char*)malloc(strlen(WildCard) + strlen(PathName));
    
    if(FilePath == NULL)
    {
        printf("Fail to Allocate memory\n");
        return(FAILURE);
    }

    HANDLE hFind;
    WIN32_FIND_DATA FindFileData;

    strcpy(FilePath, PathName);          
    strcat(FilePath, WildCard);               
 
    while(1)
    {
        if((hFind = FindFirstFile(FilePath, &FindFileData)) != INVALID_HANDLE_VALUE)
        {
          do{
              printf("%s\n", FindFileData.cFileName);
          }while(FindNextFile(hFind, &FindFileData));
          break;
        }
        else
        {
          free(FilePath);
          FilePath = NULL;
          return FAILURE;
        }    
    }

    FindClose(hFind);
    
    free(FilePath);
    FilePath = NULL;
    return SUCCESS;
}