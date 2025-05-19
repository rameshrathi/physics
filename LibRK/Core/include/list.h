//
//  list.h
//

#ifndef list_h
#define list_h

typedef struct Node {
    int data;
    struct Node *next;
}
Node;

Node *create_node(int data, Node *next);
Node *create_list(int *data, int count);

void free_node( Node *node );
void free_list( Node *root );
void print_list(Node *root );

Node * reverse_list( Node *root );
Node * reverse_max_nodes( Node *root, int max );

int list_size(Node *r);
void insert_node(Node *root, Node *element, int pos);
void delete_at(Node *root, int pos);

#endif

