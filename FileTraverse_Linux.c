#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define SUCCESS 1
#define FAILURE 0

typedef int status_t;

status_t find_file(char *path_name, char *file_name);

int main()
{
	status_t s;
	char *path_name = "/home/prashant/nikita"; 
	char *file_name = "test.txt";

	s = find_file(path_name, file_name);

	if(s == SUCCESS)
	{
		printf("File is Present in the given path : %s/%s\n", path_name, file_name);
	}
	else
	{
		printf("File is not present\n");
	}
    
    	return 0;
}

status_t find_file(char *path_name, char *file_name)
{
	struct dirent *de;  
	DIR *dr = opendir(path_name);

    	if (dr == NULL)  
    	{
        	printf("Could not open current directory" );
        	return FAILURE;
    	}

    	while ((de = readdir(dr)) != NULL)
    	{
        	if(strcmp(de->d_name, file_name) == 0)
        	{
                	return SUCCESS;
        	}
    	}

	closedir(dr);
    	return FAILURE;
}
