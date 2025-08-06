#include "quick_sort.h"

/*
    Quick sort a linked list

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

    
}