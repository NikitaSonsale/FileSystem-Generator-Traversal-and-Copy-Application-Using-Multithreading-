#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>

#define SUCCESS 1
#define FAILURE 0
#define FILE_SIZE 10485760
#define ONEMB 1048576
#define MEMORY_ALLOCATION_FAILURE 0
#define FILE_OPENING_FAILURE 1
#define RETURN_COUNT 0
#define RETURN_NODE 1
#define FILE_OPENING_FAILURE 1

typedef struct node node_t;
typedef node_t list_t;
typedef int status_t;
typedef struct thread_data thread_data_t;

struct node
{
    char *data;
    struct node *next;
    struct node *prev;
    int count;
    int pos;
};

struct thread_data
{
    struct node *p_list;
    int fd;
    int start_address;
    size_t size;

};

pthread_mutex_t mt = PTHREAD_MUTEX_INITIALIZER;

void *thread_function(void *);
void *thread_function_write(void *);
status_t create_file(char *file_name);
list_t *create_list();
status_t insert_end(list_t *p_list, char *new_data);
void show_list(list_t *p_list, const char *msg);
status_t destroy_list(list_t **pp_list);
void swap_node(node_t *node1, node_t *node2);

node_t *get_list_node(char *new_data);
void generic_insert(node_t *p_beg, node_t *p_mid, node_t *p_end);
void *xmalloc(size_t size_in_bytes);
status_t error_check(void *ptr, const char *msg, int flag);
status_t sorting(list_t *p_list);


int main()
{
    int i;
    status_t s;
    int ret;
    list_t *p_list = NULL;
    void *p_ret;
    node_t *p_run;
    thread_data_t *p_thread_data = NULL;
    p_thread_data = (thread_data_t *)malloc(10 * sizeof(thread_data_t));

    if (p_thread_data == NULL)
    {
        return (-1);
    }

    s = create_file("pqr.txt");
    if (s == FAILURE)
    {
        return (-1);
    }
    p_list = create_list();
    if (p_list == NULL)
    {
        return (-1);
    }
    p_list->count = 0;

    pthread_t thread_ids[10];
    int fd = open("pqr.txt", O_RDONLY);
    int k = 0;
    int j = 0;
    for (i = 0; i < 10; ++i)
    {
        (p_thread_data + j)->p_list = p_list;
        (p_thread_data + j)->fd = fd;
        (p_thread_data + j)->start_address = k;
        (p_thread_data + j)->size = ONEMB;

        ret = pthread_create(&thread_ids[i], NULL, thread_function, (void *)(p_thread_data + j));
        assert(ret == 0);
        k = k + ONEMB;
        j = j + 1;
    }

    for (i = 0; i < 10; ++i)
    {
        ret = pthread_join(thread_ids[i], &p_ret);
        assert(ret == 0);
        // printf("thread exited with %s\n", (char *)p_ret);
    }

    close(fd);

    show_list(p_list, "AFTER INSERT");
    s = sorting(p_list);
    show_list(p_list, "AFTER SORTING");

    printf("no of nodes :%d\n", p_list->count);

    fd = open("new_file.txt", O_WRONLY|O_CREAT, S_IRWXU);
    for (i = 0; i < 10; ++i)
    {
        p_thread_data[i].fd = fd;
        ret = pthread_create(&thread_ids[i], NULL, thread_function_write, (void *)&p_thread_data[i]);
    }

    for (i = 0; i < 10; ++i)
    {
        ret = pthread_join(thread_ids[i], &p_ret);
        assert(ret == 0);
        // printf("thread exited with %s\n", (char *)p_ret);
    }

    s = destroy_list(&p_list);
    assert(s == SUCCESS && p_list == NULL);

    free(p_thread_data);
    p_thread_data = NULL;
    close(fd);
    return 0;
}

status_t create_file(char *file_name)
{
    FILE *fp = NULL;
    int i, ret;

    char *buffer = (char *)malloc(sizeof(char) * ONEMB);
    if (error_check(buffer, "buffer in create_file()", MEMORY_ALLOCATION_FAILURE) == SUCCESS)
    {
        return FAILURE;
    }

    fp = fopen(file_name, "w");
    if (error_check(fp, "writing in create_file()", FILE_OPENING_FAILURE) == SUCCESS)
    {
        free(buffer);
        buffer = NULL;
        return (FAILURE);
    }

    char k = 'A';
    for (int j = 0; j < FILE_SIZE; j = j + ONEMB)
    {
        for (i = 0; i < ONEMB; i++)
        {
            buffer[i] = k;
        }
        ret = fwrite(buffer, sizeof(char), ONEMB, fp);
        if (ret != ONEMB)
        {
            free(buffer);
            buffer = NULL;
            fclose(fp);
            fp = NULL;
            printf("unable to write data in file");
            return (FAILURE);
        }
        k = k + 1;
    }

    free(buffer);
    buffer = NULL;
    fclose(fp);
    fp = NULL;
    return SUCCESS;
}

void *thread_function(void *args)
{
    char *Buffer = NULL;
    status_t s;
    thread_data_t *p_thread_data = (thread_data_t *)args;

    Buffer = (char *)malloc(sizeof(char) * ONEMB);
    if (error_check(Buffer, "Buffer in thread_function()", MEMORY_ALLOCATION_FAILURE) == SUCCESS)
    {
        pthread_exit("FAILURE");
    }

    if (pread(p_thread_data->fd, Buffer, p_thread_data->size, p_thread_data->start_address) != 0)
    {

        pthread_mutex_lock(&mt);

        p_thread_data->p_list->pos = p_thread_data->start_address / ONEMB;
        printf("\n pos = %d\n", p_thread_data->p_list->pos);

        s = insert_end(p_thread_data->p_list, Buffer);
        if (s == FAILURE)
        {
            pthread_mutex_unlock(&mt);
            free(Buffer);
            Buffer = NULL;
            puts("unable to insert data at end in file");
            pthread_exit("FAILURE");
        }
        pthread_mutex_unlock(&mt);
    }

    free(Buffer);
    Buffer = NULL;
    pthread_exit("SUCCESS");
}

void *thread_function_write(void *args)
{
    status_t s;
    node_t *p_run;
    thread_data_t *p_thread_data = (thread_data_t *)args;

    pthread_mutex_lock(&mt);
    for (p_run = p_thread_data->p_list->next; p_run != p_thread_data->p_list; p_run = p_run->next)
    {
        if (p_run->pos * ONEMB == p_thread_data->start_address)
        {
            pwrite(p_thread_data->fd, p_run->data, p_thread_data->size, p_thread_data->start_address);
        }
    }
    pthread_mutex_unlock(&mt);

    pthread_exit("SUCCESS");
}

list_t *create_list(void)
{
    node_t *p_new_node = NULL;

    p_new_node = get_list_node(NULL);
    if (p_new_node == NULL)
    {
        return p_new_node;
    }
    p_new_node->next = p_new_node;
    p_new_node->prev = p_new_node;
    return (p_new_node);
}

status_t insert_end(list_t *p_list, char *new_data)
{
    node_t *new_node = get_list_node(new_data);
    new_node->pos = p_list->pos;
    generic_insert(p_list->prev, new_node, p_list);
    p_list->count++;
    return (SUCCESS);
}

void show_list(list_t *p_list, const char *msg)
{
    node_t *p_run;
    if (msg)
        puts(msg);
    if (p_list->next == p_list)
    {
        puts("List Empty");
    }

    puts("[BEG]<->");
    for (p_run = p_list->next; p_run != p_list; p_run = p_run->next)
    {
        printf("[.....%s]<->\n", p_run->data);
    }
    puts("[END]");
}

status_t destroy_list(list_t **pp_list)
{
    list_t *p_list = *pp_list;
    node_t *p_run, *p_run_next;
    for (p_run = p_list->next; p_run != p_list; p_run = p_run_next)
    {
        p_run_next = p_run->next;
        free(p_run);
    }
    free(p_list);
    *pp_list = NULL;
    return (SUCCESS);
}

node_t *get_list_node(char *new_data)
{
    node_t *p_node = NULL;
    p_node = (node_t *)xmalloc(sizeof(node_t));
    if (error_check(p_node, "p_node in get_list_node()", MEMORY_ALLOCATION_FAILURE) == SUCCESS)
    {
        return (p_node);
    }
    if (new_data != NULL)
    {
        p_node->data = (char *)xmalloc(strlen(new_data));
        if (error_check(p_node->data, "new data in get_list_node()", MEMORY_ALLOCATION_FAILURE) == SUCCESS)
        {
            return NULL;
        }
        strcpy(p_node->data, new_data);
    }

    p_node->next = NULL;
    p_node->prev = NULL;
    return (p_node);
}

void generic_insert(node_t *p_beg, node_t *p_mid, node_t *p_end)
{
    p_mid->next = p_end;
    p_mid->prev = p_beg;
    p_end->prev = p_mid;
    p_beg->next = p_mid;
}

void *xmalloc(size_t size_in_bytes)
{
    void *p = NULL;
    p = malloc(size_in_bytes);
    if (error_check(p, "p in xmalloc()", MEMORY_ALLOCATION_FAILURE) == SUCCESS)
    {
        return p;
    }
    return (p);
}

status_t error_check(void *ptr, const char *msg, int flag)
{
    if (ptr == NULL)
    {
        if (msg)
        {
            if (flag == MEMORY_ALLOCATION_FAILURE)
                printf("unable to allocate memory to %s\n", msg);
            else if (flag == FILE_OPENING_FAILURE)
                printf("Unable to open a file for %s\n", msg);
        }

        return (SUCCESS);
    }
    return (FAILURE);
}

status_t sorting(list_t *p_list)
{
	int i, j;
    node_t *p_run;
   
    for (i = 0; i < 10 - 1; ++i)
    {
        for (p_run = p_list->next, j = 0; j < 10 - i - 1; ++j, p_run = p_run->next)
        {
			if(p_run != p_list)
			{	
                if (p_run->pos > p_run->next->pos)
     			{
	                swap_node(p_run, p_run->next);
					p_run = p_run->prev;
            	}
			}
        }
    }

    return (SUCCESS);
}

void swap_node(node_t *node1, node_t *node2)
{
    /* char temp[ONEMB];
     int temp_pos;

     strcpy(temp, node1->data);
     temp_pos = node1->pos;

     strcpy(node1->data, node2->data);
     node1->pos = node2->pos;

     strcpy(node2->data, temp);
     node2->pos = temp_pos;*/

    node_t *temp;
    temp = node1->next;

    node1->next = node2->next;
    node2->next = temp;

    node1->next->prev = node1;
    node2->next->prev = node2;

    temp = node1->prev;

    node1->prev = node2->prev;
    node2->prev = temp;

    node1->prev->next = node1;
    node2->prev->next = node2;

}

