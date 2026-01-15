#ifndef BST_H
#define BST_H

#include <pthread.h>

pthread_cond_t treeInitCond;
pthread_cond_t treeIsFull;
pthread_cond_t treeIsEmpty;
int treeIsInit;

struct KV
{
    int value;
    char* key;
};

struct node
{ 
    struct node* left;
    struct node* right;
    struct node* parent;
    struct KV kv;
};

typedef struct node node;
struct node* SENTINEL;
struct node* TREE;

struct add_p
{
    char *key; 
    void *value;
};


struct lookup_p
{
    char* key; 
    void** value;
};


struct Counter
{
    int max_node_to_allocate;
    int allocated;  
    pthread_mutex_t mutex;
};
struct Counter* COUNTER;
typedef struct Counter counter;


int stackIsEmpty(int* top);
void stackPush(node** stack, int* top, node* data);
node* stackPop(node** stack, int* top);
node* stackPeek(node** stack, int* top);


typedef struct add_p add_p;
typedef struct lookup_p lookup_p;

void printInorder(node* root); 
void* initialize(void* arg);
void* add(void* arg);
void* delete(void* arg);
void* lookup(void* arg);

void* max(void* arg);
void* min(void* arg);
void* replace(node* who, node* with);

#endif