#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <assert.h>

#define SUCCESS 1
#define FAILURE 0
#define FILE_SIZE 10485760
#define ONEMB 1048576

typedef struct node node_t;
typedef node_t list_t;
typedef int status_t;

char Buffer[ONEMB];
HANDLE hFile;

struct node
{
    char *data;
    struct node *next;
    struct node *prev;
};

void CreatingFile(char *FileName);
list_t *create_list(void);
status_t insert_end(list_t *p_list,char *new_data);
void show_list(list_t *p_list,const char* msg);
status_t destroy_list(list_t **pp_list);

void *xmalloc(size_t size_in_bytes);
node_t* get_list_node(char *new_data);
void generic_insert(node_t *p_beg,node_t *p_mid,node_t *p_end);

int main() 
{
    status_t s;
    int i;
    list_t *p_list = NULL;
    DWORD dwRead;
    
    CreatingFile("file.txt");
    p_list = create_list();
   
    for(i = 0; i < 10; i++)
    {
        ReadFile(hFile,Buffer,sizeof(Buffer),&dwRead,0);
        s = insert_end(p_list, Buffer);
        assert(s == SUCCESS); 
    }
    
    show_list(p_list,"AFTER INSERT");
    CloseHandle(hFile); 
    
    s = destroy_list(&p_list);
    assert(s == SUCCESS && p_list == NULL);
    
    return 0;
}

void CreatingFile(char *FileName)
{
    DWORD dwWritten;
    hFile = CreateFile(FileName,
                    GENERIC_READ|GENERIC_WRITE,
                    FILE_SHARE_READ|FILE_SHARE_WRITE,
                    0,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    0);

    int i;
    for(i = 0; i < 1048576; i++)
    {
        Buffer[i] = 'A' + (rand() % 26);
    }

    int j = 0;
    while(j < 10)
    {
        WriteFile(hFile,Buffer,sizeof(Buffer),&dwWritten,0);
        j = j + 1;
    }
}

list_t *create_list(void)
{
    node_t *p_new_node = NULL;
    p_new_node = get_list_node(NULL);
    p_new_node->next  = p_new_node;
    p_new_node->prev = p_new_node;
    return(p_new_node);
}

status_t insert_end(list_t *p_list, char* new_data)
{
    generic_insert(p_list->prev,get_list_node(new_data),p_list);
    return(SUCCESS);
}

void show_list(list_t *p_list,const char* msg)
{
    node_t *p_run = NULL;
	puts(msg);
	printf("[START]<->");
	for(p_run = p_list->next; p_run != p_list; p_run = p_run->next)
	{
		printf("[.........%s]<->", p_run->data);
	}
	puts("[END]");  
}

status_t destroy_list(list_t **pp_list)
{
	list_t *p_list = *pp_list;
	node_t *p_run, *p_run_next;
	for(p_run = p_list->next; p_run != p_list; p_run = p_run_next)
	{
		p_run_next = p_run->next;
		free(p_run);
	}
	free(p_list);
	*pp_list = NULL;
	return(SUCCESS);
}

node_t *get_list_node(char *new_data)
{
    node_t *p_node = NULL;
    p_node = (node_t*)xmalloc(sizeof(node_t));

    if(new_data != NULL)
    {
        p_node->data = (char*)xmalloc(strlen(new_data));
        strcpy(p_node->data,new_data);
    }
      
    p_node->next = NULL;
    p_node->prev = NULL;
    return(p_node);
}

void generic_insert(node_t *p_beg, node_t *p_mid, node_t *p_end)
{
    p_mid->next = p_end;
    p_mid->prev = p_beg;
    p_beg->next = p_mid;
    p_end->prev = p_mid;
}

void *xmalloc(size_t size_in_bytes)
{
    void *p = NULL;
    p = malloc(size_in_bytes);
    if(NULL == p)
    {
        puts("Unable to allocate memory ...");
        exit(-1);
    }
    return(p);
}






