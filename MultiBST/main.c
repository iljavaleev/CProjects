#include <pthread.h>
#include <string.h>
#include "pthredm.h"
#include "bst.h"


int main(int argc, char** argv)
{
    FILE *filePointer;
    filePointer = fopen(argv[1], "r");
    if (filePointer == NULL)
        ERROR("error open file");
    
    char buffer[255];
    if (fgets(buffer, sizeof(buffer), filePointer) == NULL) 
        ERROR("error read");

    pthread_mutex_init(&COUNTER->mutex, NULL);

    char pairs[COUNTER->max_node_to_allocate][20];
    char* token;
    token = strtok(buffer, " ");
    strcpy(pairs[0], token);
    COUNTER->allocated = 1;
    while (token != NULL)
    {
        token = strtok(NULL, " ");
        if (token)
        {
            strcpy(pairs[COUNTER->allocated], token);
            COUNTER->allocated++;
        }  
    }
    if (COUNTER->allocated >= COUNTER->max_node_to_allocate)
        ERROR("error: max all nodes");

    TREE = calloc(COUNTER->max_node_to_allocate, sizeof(node));

    
    for (int j=0; j < COUNTER->allocated; ++j)
    {
        struct KV kv;
        kv.key = strtok(pairs[j], "=");
        kv.value = atoi(strtok(NULL, "="));
        TREE[j].left = SENTINEL;
        TREE[j].parent = SENTINEL;
        TREE[j].right = SENTINEL;
        TREE[j].kv = kv;
    }

    int count = COUNTER->allocated;

    void* numnode = (void*) count;
    pthread_t init_thread, add_thread, lookup_thread, delete_thread; 
    CREATE(init_thread, initialize, numnode);
    
    add_p p;
    p.key = "M";
    p.value = (void *) 98;
    CREATE(add_thread, add, &p);

    
    // lookup_p l;
    // int val;
    // l.key = "q";
    // l.value = (void**) &val;
    // CREATE(lookup_thread, lookup, &l);
    
    char* key = "q";
    CREATE(delete_thread, delete, key);

    
    // void* lookup_res_ptr;
    // JOIN_WITH_RES(lookup_thread, &lookup_res_ptr);
    // if (lookup_res_ptr != NULL)
    // {   
    //     printf("%d\n", ((node*) lookup_res_ptr)->kv.value);
    // } 

    JOIN(delete_thread);
    JOIN(init_thread);
    JOIN(add_thread);
    printInorder(TREE);
    pthread_mutex_destroy(&COUNTER->mutex); 
    exit(EXIT_SUCCESS);
}


