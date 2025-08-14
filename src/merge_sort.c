#include "../include/merge_sort.h"


/*
    Split a linked list in half from the middle point
    NOTE: odd length lists should have the additional node in the first half list

    ARGS:
        Node* in    -> head of the linked list
        int* length -> pointer to a memory location where the length of the first half of the list can be placed
    RET:
        returns the head of the second half list while the input pointer still points to the first half
*/
Node* split_ll(Node* in) {

    /* check if list is empty or has only one element */
    if (in == NULL || in->next == NULL) {
        return in;
    }

    /*
     * Slow and Fast Pointer Technique: a slow pointer moves one step at a time, while a fast pointer moves
     * two steps at a time. As the fast pointer moves twice as fast as the slow pointer, when the fast pointer
     * reaches the end of the list, the slow pointer will be pointing to the last node of the first half.
     */

    /* initialize slow and fast pointers to head and a pointer that is always one node before slow */
    Node *slow = in; 
    Node *fast = in;
    Node *end_of_first;

    /* while fast pointer has not yet reached the end of the linked list */
    while (fast != NULL && fast->next != NULL) {
        /* set slow pointer to the next node, set fast pointer to the next next node, and increment length counter */
        end_of_first = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    /* update slow and end_of_first one more time if fast has not reached the actual end */
    if (fast != NULL) {
        end_of_first = slow;
        slow = slow->next;
    }

    /* set end_of_first's next to NULL to separate first and second half */
    end_of_first->next = NULL;

    return slow;
}

/*
    Merge two linked lists that are already sorted
    NOTE: this must be done iteratively, a recursive solution will take too much space

    ARGS:
        Node* heada -> one linked list
        Node* headb -> the other linked list
    RET:
        returns the head of the merged list
*/
Node* merge_ordered_ll(Node* heada, Node* headb) {

    /* if lists a and b are empty */
    if (heada == NULL && headb == NULL) {
        return NULL;
    }
    /* if list a is empty */
    if (heada == NULL) {
        return headb;
    }
    /* if list b is empty */
    if (headb == NULL) {
        return heada;
    }

    /* declare head of merged list and a curr pointer to traverse merged list */
    Node *head;
    Node *curr;

    /* determine the head of the merged list */
    /* if the value at heada is less than or equal to the value at headb */
    if (heada->val <= headb->val) {
        /* set head of merged list to heada and update curra to the next node */
        head = heada;
        heada = heada->next;
    }
    else {
        /* else, set head of merged list to headb and update headb to the next node */
        head = headb;
        headb = headb->next;
    }

    /* set curr to head */
    curr = head;

    /* traverse both lists */
    while (heada != NULL && headb != NULL) {
        /* if the value at curra is less than or equal to the value at currb */
        if (heada->val <= headb->val) {
            /* set curr to curra and update curra to the next node */
            curr->next = heada;
            heada = heada->next;
        }
        else {
            /* else, set curr to currb and update currb to the next node */
            curr->next = headb;
            headb = headb->next;
        }

        /* update curr to the next node in merged list */
        curr = curr->next;
    }

    /* if there are more nodes left in list a */
    if (heada != NULL) {
        /* append the rest of list a to merged list */
        curr->next = heada;
    }
    /* else if there are more nodes left in list b */
    if (headb != NULL) {
        /* append the rest of list b to merged list */
        curr->next = headb;
    }

    return head;
}

/*
    Recursively sort a linked list using merge sort

    ARGS:
        Node* head -> linked list head
    RET:
        returns the head of the sorted list
*/
Node* merge_sort(Node* head) {

    /* Base case: if the list is empty or has one element */
    if (head == NULL || head->next == NULL) {
        return head;
    }

    /* Recursive case: split the list into two halves, sort them, then return the merged result */
    Node* new_head = split_ll(head);
    
    head = merge_sort(head);
    new_head = merge_sort(new_head);

    return merge_ordered_ll(head, new_head);
}