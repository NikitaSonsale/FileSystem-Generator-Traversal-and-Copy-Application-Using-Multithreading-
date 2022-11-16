#include <stdio.h>
#include <Windows.h>
#include <string.h>

#define SUCCESS         1
#define FAILURE         0
#define IT_IS_DIRECTORY 2

typedef int status_t;

status_t FindFile(char *PathName, char *FileName);

int main(void)
{
    status_t s;
    char *PathName = "C:\\Users\\Prashant\\Desktop";
    char *FileName = "Nikita";

    s = FindFile(PathName, FileName);

    if(s == SUCCESS)
    {
        printf("\nFILE PATH IS : %s\\%s\n",PathName, FileName);
    }

    else if(s == IT_IS_DIRECTORY)
    {
        printf("\nIT IS A DIRECTORY\n");
    }

    else
    {
        printf("\nFILE IS NOT PRESENT AT GIVEN PATH\n");
    }

    return 0;
}

status_t FindFile(char *PathName, char *FileName)
{
    //char FilePath[MAX_PATH];
    char *FilePath = (char*)malloc(3 * sizeof(char) + strlen(PathName));
    
    if(FilePath == NULL)
    {
        printf("Fail to Allocate memory\n");
        return(FAILURE);
    }

    HANDLE hFind;
    WIN32_FIND_DATA FindFileData;

    strcpy(FilePath, PathName);          
    strcat(FilePath, "//*");                

    if((hFind = FindFirstFile(FilePath, &FindFileData)) != INVALID_HANDLE_VALUE)
    {
        do{
            if(strcmp(FindFileData.cFileName, FileName) == 0)
            {
                if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    free(FilePath);
                    FilePath = NULL;
                    return (IT_IS_DIRECTORY);
                }
                
                free(FilePath);
                FilePath = NULL;
                return SUCCESS;
            }
        }while(FindNextFile(hFind, &FindFileData));
    }    
    
    FindClose(hFind);
    
    free(FilePath);
    FilePath = NULL;
    return FAILURE;
}