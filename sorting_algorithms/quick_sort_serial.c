#include "quick_sort.h"

int main() {
    Node* head = make_ll_from_file("input.txt");

    head = quick_sort(head);

    print_list(head,"quick_sort_serial_output.txt");
    free_list(head);
    return 0;
}