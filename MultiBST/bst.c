#include "bst.h"
#include "pthredm.h"
#include <string.h>

/* define sentinel node */
node _SENTINEL = {0};
node* SENTINEL = &_SENTINEL;

/* limit number of nodes in stack to 20 */
counter _COUNTER = { 20, 0 };
counter* COUNTER = &_COUNTER;

pthread_cond_t treeInitCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t treeIsFull = PTHREAD_COND_INITIALIZER;
pthread_cond_t treeIsEmpty = PTHREAD_COND_INITIALIZER;
int treeIsInit = 0;

/* initialize tree, other thread are waiting for full init */
void* initialize(void* arg)
{
    LOCK(COUNTER->mutex);
    int numnodes = (int) arg;
    for (int i = 1; i<numnodes; ++i)
    {
        node* prev = NULL;
        node* root = TREE;
        node* el = TREE + i;
        while (root != SENTINEL)
        {
            prev = root;
            if (el->kv.value < root->kv.value)
                root = root->left;
            else
                root = root->right;
        }
        if (el->kv.value < prev->kv.value)
            prev->left = el;
        else
            prev->right = el;
        el->parent = prev;
    }
    treeIsInit = 1;
    UNLOCK(COUNTER->mutex);
    SEND_BROAD_SIGNAL(treeInitCond);
    return 0;
}

/* insert node */
void* add(void* arg)
{
    struct add_p* params  = (struct add_p*) arg;
    node* new_el;
    LOCK(COUNTER->mutex);
    if(!treeIsInit)
        WAIT(treeInitCond, COUNTER->mutex);

    while (COUNTER->allocated + 1 >= COUNTER->max_node_to_allocate)
    {   
        WAIT(treeIsFull, COUNTER->mutex);
        
    }    
    new_el = malloc(sizeof(struct node));
    new_el->left = SENTINEL;
    new_el->right = SENTINEL;
    new_el->kv.key = params->key;
    new_el->kv.value = (int) params->value;
    
    if (TREE == SENTINEL)
    {
        TREE = new_el;
    }
    else
    {
        node* prev = NULL;
        node* root = TREE;
        while (root != SENTINEL)
        {
            prev = root;
            if (new_el->kv.value < root->kv.value)
                root = root->left;
            else
                root = root->right;
        }
        
        if (new_el->kv.value < prev->kv.value)
            prev->left = new_el;
        else
            prev->right = new_el;
        new_el->parent = prev;
    }
    
    COUNTER->allocated++;
    UNLOCK(COUNTER->mutex);
    SEND_BROAD_SIGNAL(treeIsEmpty);
    return 0;
}


   

void* delete(void* arg)
{
    
    node* f;
    LOCK(COUNTER->mutex);
    if(!treeIsInit)
        WAIT(treeInitCond, COUNTER->mutex);
    UNLOCK(COUNTER->mutex);
    while (COUNTER->allocated == 0)
    {
        WAIT(treeIsEmpty, COUNTER->mutex);  
    }
    lookup_p l;
    int val;
    l.key = "q";
    l.value = (void**) &val;
   
    f = lookup(&l);
    if (f == NULL)
        return NULL;

    if (f->left == SENTINEL)
        replace(f, f->right);
    else if (f->right == SENTINEL)
        replace(f, f->left);
    else
    {
        node* tmp = min(f->right);
        if (tmp->parent != f)
        {
            replace(tmp, tmp->right);
            tmp->right = f->right;
            tmp->right->parent = tmp;
        }
        replace(f, tmp);
        tmp->left = f->left;
        tmp->left->parent = tmp;
    }
   
    COUNTER->allocated--;
    SEND_BROAD_SIGNAL(treeIsFull);
    UNLOCK(COUNTER->mutex);
    return NULL;
}


void* replace(node* who, node* with)
{
    if (who->parent == SENTINEL)
        TREE = with;
    else if(who == who->parent->left)
        who->parent->left = with;
    else
        who->parent->right = with;
    
    if (with != SENTINEL)
        with->parent = who->parent;
    
    return NULL;
}


void printInorder(node* root)
{ 
    if (root == SENTINEL) 
        return; 

    printInorder(root->left); 

    if(root)
        printf("%d ", root->kv.value);
       

    printInorder(root->right); 
} 

void* max(void* arg)
{
    node* root = (node*) arg;
    if (root->right == SENTINEL)
        return (void*) root;
    
    while (root->right != SENTINEL)
    {
        root = root->right;
    }
    return (void*) root;
}

void* min(void* arg)
{
    node* root = (node*) arg;
    if (root->left == SENTINEL)
        return (void*) root;
    
    while (root->left != SENTINEL)
    {
        root = root->left;
    }
    return (void*) root;
}


/* search for node by key. use stack structure  */
void* lookup(void* arg)
{
    int top = -1;
    node* stack[COUNTER->allocated];
    lookup_p* param = (lookup_p*) arg;
    FILE *fptr = fopen("example.txt", "wa");
    if (fptr == NULL) {
        printf("Error opening file!\n");
        exit(1); // Exit the program if an error occurred
    }
   
    LOCK(COUNTER->mutex);
    if(!treeIsInit)
        WAIT(treeInitCond, COUNTER->mutex);

    while (COUNTER->allocated == 0)
    {
        WAIT(treeIsEmpty, COUNTER->mutex);
    }
    
    stackPush(stack, &top, TREE);
    
    
    while (!stackIsEmpty(&top))
    {
        node* current = stackPop(stack, &top);
        
        fprintf(fptr, "%s\n", current->kv.key);
        fflush(fptr);
        if(!strcmp(current->kv.key, param->key))
        {
            param->value = (void**) &current->kv.value;
            printf("find %d\n", *param->value);
            UNLOCK(COUNTER->mutex);
            fclose(fptr);
            return current;
        }
        if (current->right != SENTINEL)
            stackPush(stack, &top, current->right);
        
        if (current->left != SENTINEL)
            stackPush(stack, &top, current->left);
    }
    UNLOCK(COUNTER->mutex);
    return NULL;
}

/* stack defenition */
int stackIsEmpty(int* top)
{
    return *top == -1;
}

void stackPush(node** stack, int* top, node* data) 
{
    if (*top > COUNTER->allocated)
    {
        printf("Stack overflow\n");
        return;
    }
        
    (*top)++;
    stack[*top] = data;
}

node* stackPop(node** stack, int* top) 
{
    if (stackIsEmpty(top)) 
    {
        printf("Stack is empty\n");
        return NULL;
    }
    node* data = stack[*top];
    (*top)--;
    return data;
}


node* stackPeek(node** stack, int* top) 
{
    if (stackIsEmpty(top)) 
    {
        printf("Stack is empty\n");
        return NULL;
    }
    return stack[*top];
}

