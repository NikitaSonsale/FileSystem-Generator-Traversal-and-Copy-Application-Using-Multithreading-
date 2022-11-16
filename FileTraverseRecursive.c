#include <stdio.h>
#include <Windows.h>
#include <string.h>

#define SUCCESS 1
#define FAILURE 0

typedef int status_t;

status_t FindFile(char *PathName, char *FileName);

int main(void)
{
    status_t s;
    char *PathName = "C:\\Users\\Prashant\\Desktop\\Nikita";
    char *FileName = "abc.txt";

    s = FindFile(PathName, FileName);

    if(s == FAILURE)
    {
        puts("\n NOT A SINGLE FILE WITH THIS NAME \n");
    }

    return 0;
}

status_t FindFile(char *PathName, char *FileName)
{
    if(PathName == NULL || FileName == NULL)
    {
        puts("Parameters for function FindFile are invalid");
        //printf("%s", strerror(errno));
        return FAILURE;
    }
    
    char FilePath[MAX_PATH];
    char NewPath[MAX_PATH];
    static int count = 0;
 
    HANDLE hFind;
    WIN32_FIND_DATA FindFileData;             

    strcpy(NewPath,PathName);                

    strcpy(FilePath, PathName);
    strcat(FilePath, "//*");                  

    char *Separator = "\\";       

    if((hFind = FindFirstFile(FilePath, &FindFileData)) != INVALID_HANDLE_VALUE)
    {
        do{
            if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(strcmp(".",FindFileData.cFileName) == 0)
                {
                    continue;
                }
                else if(strcmp("..",FindFileData.cFileName) == 0)
                {
                    continue;
                }
                else
                {      
                    strcat(NewPath, Separator);                 
                    strcat(NewPath, FindFileData.cFileName);   
                    FindFile(NewPath, FileName);
                    strcpy(NewPath, PathName);
                }
            }

            else
            {
                if(strcmp(FindFileData.cFileName, FileName) == 0)
                {
                    printf("FILE PATH : %s\\%s\n", PathName, FileName);
                    count++;
                }
            }
        }while(FindNextFile(hFind, &FindFileData));
    }
    
     
    if(count == 0)
    {
        return FAILURE;
    }

    FindClose(hFind);
    return (SUCCESS);
}