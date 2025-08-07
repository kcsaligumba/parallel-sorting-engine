#include "quick_sort.h"
#include <pthread.h>

#define THRESHOLD 800000

void* quick_sort_thread_wrapper(void *arg){
    Node* head = (Node*)arg;
    int length = get_ll_length(head);

    // if number of nodes less than threshold, use sequential version
    if (length < THRESHOLD) {
        head = quick_sort(head);
        pthread_exit((void*)head);
    }

    int pivot = head->val;
    Node *less = NULL, *equal = NULL, *greater = NULL, *next = NULL;

    while (head != NULL) {
        next = head->next;
        if (head->val < pivot) {
            append_node(&less, head);
        }
        else if (head->val == pivot) {
            append_node(&equal, head);
        }
        else {
            append_node(&greater, head);
        }
        head = next;
    }

    /* declare threads for sorting less and greater */
    pthread_t thread1, thread2;

    /* create first threads */
    pthread_create(&thread1, NULL, quick_sort_thread_wrapper, (void *)less);
    pthread_create(&thread2, NULL, quick_sort_thread_wrapper, (void *)greater);
    
    /* wait for threads to finish sorting */
    pthread_join(thread1, (void **)&less);
    pthread_join(thread2, (void **)&greater);

    /* concatenate sorted lists */
    head = concat_ll(less, equal);
    head = concat_ll(head, greater);

    /* terminate thread */
    pthread_exit((void *)head);
    return NULL;
}



int main() {
    Node* head = make_ll_from_file("input.txt");

    pthread_t thread;
    pthread_create(&thread, NULL, quick_sort_thread_wrapper, (void *)head);
    pthread_join(thread, (void **)&head);

    print_list(head,"quick_sort_parallel_output.txt");
    free_list(head);
    return 0;
}