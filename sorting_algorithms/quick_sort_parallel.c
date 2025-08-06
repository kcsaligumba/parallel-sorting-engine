#include "quick_sort.h"
#include <pthread.h>

#define THRESHOLD 800000

void* quick_sort_thread_wrapper(void *arg){
    Node* head = (Node*)arg;
    
    //TODO
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