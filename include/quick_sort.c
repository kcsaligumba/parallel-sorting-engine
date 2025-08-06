#include "quick_sort.h"

/*
    Append a node to the end of a linked list

    ARGS:
        Node** head -> pointer to the head of the list
        Node* node  -> node to append to the list]
*/
void append_node(Node** head, Node* node) {
    //TODO
}

/*
    Quick sort a linked list

    ARGS:
        Node* heada -> one linked list
        Node* headb -> the other linked list
    RET:
        returns the head of the concatenated list (a followed by b)
*/
Node* concat_ll(Node* heada, Node* headb) {
    //TODO
}

/*
    Recursively sort a linked list using quick sort

    ARGS:
        Node* head -> linked list head
    RET:
        returns the head of the sorted list
*/
Node* quick_sort(Node* head) {

    /* Base case: if the list is empty or has one element */
    if (head == NULL || head->next == NULL) {
        return head;
    }

    //TODO
}