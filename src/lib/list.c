//
//  list.c
//

#include <stdio.h>
#include <stdlib.h>
#include "include/types.h"
#include "include/list.h"

Node *create_node(int data, Node *next)
{
    Node *p = (Node*) malloc(sizeof(Node));
    p->data = data;
    p->next = next;
    return p;
}

Node *create_list(int *data, int count)
{
    Node *root = create_node(*data++, NULL);
    Node *t = root;
    while (--count) {
        t->next = create_node(*data++, NULL);
        t = t->next;
    }
    return root;
}

// insert element at given position otherwise at last
void insert_node(Node *root, Node *element, int pos)
{
    while (root->next && --pos)
        root = root -> next;
    if (root->next) {
        element->next = root->next;
    } else {
        element->next = NULL;
    }
    root->next = element;
}

void free_list(Node *root)
{
    if (root->next)
        free_list(root->next);
    free_node(root);
}

void free_node(Node *node)
{
    node->next = NULL;
    free(node);
}

void print_list(Node *root)
{
    while (root) {
        printf("%d  ", root->data);
        root = root->next;
    }
    printf("\n");
}

// 1 -> 2 -> 3 -> 4  => 1 <- 2 <- 3 -> 4 =>
Node * reverse_list( Node *root )
{
    if (!root || !root->next )
        return root;

    Node *prev = NULL;
    Node *curr = root;
    Node *next = NULL;
    while (curr) {
        next = curr->next;  // store next
        curr->next = prev; // Reverse current's next
        
        prev = curr; // move curr to prev
        curr = next; // move next to curr
    }
    // Return the prev node
    return prev;
}

// (1 -> 2 -> 3 -> 4)  => (1 <- 2 <- 3 -> 4)
Node * reverse_max_nodes( Node *root, int max )
{
    if (!root || !root->next )
        return root;

    Node *prev = NULL;
    Node *curr = root;
    Node *next = NULL;
    while (curr && max--) {
        next = curr->next;  // store next
        curr->next = prev; // Reverse current's next
        prev = curr; // move curr to prev
        curr = next; // move next to curr
    }
    if (curr) {
        root -> next = curr;
    }
    // Return the prev node
    return prev;
}

int list_size(Node *r)
{
    int len = 0;
    while (r)
    {
        r = r -> next;
        len++;
    }
    return len;
}

// Considering 0 index as starting index
void delete_at(Node *root, int pos)
{
    Node *prev = NULL;
    while (root->next && pos--)
    {
        prev = root;
        root = root->next;
    }
    if (pos > 0) {
        // returns if less elements
        return;
    }
    if (root->next) {
        prev->next = root->next;
    } else {
        prev->next = NULL;
    }
    root->next = NULL;
    free(root);
}
