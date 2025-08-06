#include "radix_sort.h"

int main() {
    Node* head = make_ll_from_file("input.txt");

    head = radix_sort(head);

    print_list(head,"radix_sort_serial_output.txt");
    free_list(head);
    return 0;
}
