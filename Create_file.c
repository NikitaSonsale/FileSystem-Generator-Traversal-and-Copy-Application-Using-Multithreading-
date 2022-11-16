#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define FILE_SIZE 10485760

TCHAR PathName[200];


DWORD FindFile(LPCSTR lpPath, LPCSTR lpFileName);

int main()
{
  char buffer[10];
  DWORD dwWritten; 
  HANDLE hFile;  

  hFile = CreateFile("file.txt",
                    GENERIC_READ|GENERIC_WRITE,
                    FILE_SHARE_READ|FILE_SHARE_WRITE,
                    0,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    0);

  int i;
  for(i = 0; i < 10; i++)
  {
    buffer[i] = 'a';
  }

  int j = 0;
  while(j < (FILE_SIZE/10))
  {
    WriteFile(hFile,buffer,sizeof(buffer),&dwWritten,0);
    j = j + 1;
  }

  DWORD test;
  // test = SearchPath(NULL, 
  //                   "file.txt", 
  //                    NULL, 
  //                    200, 
  //                    PathName, 
  //                    NULL);

  test = FindFile(NULL, "file.txt");
  if(test == 0)
  {
    printf("file not found\n");
  }
  else
  {
    printf("The return value is: %d\n", test);
    printf("The path is %s\n", PathName);
  }

  CloseHandle(hFile);
  return 0;
}

DWORD FindFile(LPCSTR lpPath, LPCSTR lpFileName)
{
  DWORD test;
  test = SearchPath(lpPath, 
                    lpFileName, 
                     NULL, 
                     200, 
                     PathName, 
                     NULL);

  return test;
}