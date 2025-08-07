#include "quick_sort.h"


/*
    Append a node to the end of a linked list

    ARGS:
        Node** head -> pointer to the head of the list
        Node* node  -> node to append to the list]
*/
void append_node(Node** head, Node* node) {

    node->next = NULL;

    /* if the linked list is empty */
    if (*head == NULL) {
        /* update head to node */
        *head = node;
    }
    /* else, find last element and append node to the end */
    else {
        Node* tail = *head;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = node;
    }
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

    /* return headb if list a is empty */
    if (heada == NULL) {
        return headb;
    }
    /* return heada if list b is empty */
    if (headb == NULL) {
        return heada;
    }

    /* find last element of list a and append list b to the end */
    Node *curr = heada;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = headb;

    return heada;
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

    int pivot = head->val;
    Node *less = NULL, *equal = NULL, *greater = NULL;

    while (head != NULL) {
        /* update next */
        Node* next = head->next;

        if (head->val < pivot) {
            append_node(&less, head);
        }
        else if (head->val == pivot) {
            append_node(&equal, head);
        }
        else {
            append_node(&greater, head);
        }

        /* update head */
        head = next;
    }

    /* Recursive case: sort the less and greater lists using quick sort */
    less = quick_sort(less);
    greater = quick_sort(greater);

    /* Concatenate all lists */
    Node* sorted = concat_ll(less, equal);
    return concat_ll(sorted, greater);
}