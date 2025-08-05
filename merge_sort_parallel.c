#include "merge_sort.h"
#include <pthread.h>

#define THRESHOLD 800000

void* merge_sort_thread_wrapper(void *arg){
    Node* head = (Node*)arg;
    int length = get_ll_length(head);
    Node* new_head = split_ll(head);

    // if number of nodes less than threshold, use sequential version
    if (length < THRESHOLD) {
        head = merge_sort(head);
        new_head = merge_sort(new_head);
        head = merge_ordered_ll(head, new_head);
        pthread_exit((void*)head);
    }

    /* declare threads for sorting first and second half */
    pthread_t thread1, thread2;

    /* create first threads */
    pthread_create(&thread1, NULL, merge_sort_thread_wrapper, (void *)head);
    pthread_create(&thread2, NULL, merge_sort_thread_wrapper, (void *)new_head);
    
    /* wait for threads to finish sorting */
    pthread_join(thread1, (void **)&head);
    pthread_join(thread2, (void **)&new_head);

    /* merge lists */
    head = merge_ordered_ll(head, new_head);

    /* terminate thread */
    pthread_exit((void *)head);
    return NULL;
}



int main() {
    Node* head = make_ll_from_file("input.txt");

    pthread_t thread;
    pthread_create(&thread, NULL, merge_sort_thread_wrapper, (void *)head);
    pthread_join(thread, (void **)&head);

    print_list(head,"parallel_output.txt");
    free_list(head);
    return 0;
}