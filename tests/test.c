#include "merge_sort.h"
#include "quick_sort.h"

void print_list_test(Node *in)
{
    Node * curr = in;
    while(curr!=NULL)
    {
        printf("%d -> ",curr->val);
        curr = curr->next;
    }
    printf("NULL\n");
    return;
}

void test0()
{
    printf("Test split_ll\n");
    Node *list = make_ll_from_file("test_files/test1.txt");
    printf("Original list:\n");
    print_list_test(list);
    Node *split = split_ll(list);
    printf("First half:\n");
    print_list_test(list);
    printf("Second half:\n");
    print_list_test(split);

    free_list(list);
    free_list(split);
    return;
}

void test1()
{
    printf("Test merge_ordered_ll\n");
    Node *list1 = make_ll_from_file("test_files/test1.txt");
    Node *list2 = make_ll_from_file("test_files/test2.txt");
    printf("First list:\n");
    print_list_test(list1);
    printf("Second list:\n");
    print_list_test(list2);
    
    Node *list_merged = merge_ordered_ll(list1, list2);
    printf("Merged list:\n");
    print_list_test(list_merged);

    free_list(list_merged);
    return;
}

void test2()
{
    printf("Test merge_sort 1\n");
    Node *list = make_ll_from_file("test_files/test3.txt");
    printf("Original list:\n");
    print_list_test(list);

    Node *sorted_list = merge_sort(list);
    printf("Sorted list:\n");
    print_list_test(sorted_list);

    free_list(sorted_list);
    return;
}

void test3()
{
    printf("Test quick_sort\n");
    Node *list = make_ll_from_file("test_files/test3.txt");
    printf("Original list:\n");
    print_list_test(list);

    Node *sorted_list = quick_sort(list);
    printf("Sorted list:\n");
    print_list_test(sorted_list);

    free_list(sorted_list);
    return;
}

int main(int argc, char *argv[]) {
    if (argc == 1){
        printf("Enter Test Number, 0 for split_ll, 1 for merge_ordered_ll, 2 for merge_sort, 3 for quick_sort\n");
    }
    else if(atoi(argv[1]) == 0) {
        test0();
    }
    else if(atoi(argv[1]) == 1) {
        test1();
    }
    else if(atoi(argv[1]) == 2) {
        test2();
    }
    else if(atoi(argv[1]) == 3) {
        test3();
    }
    else {
        printf("Enter Test Number, 0 for split_ll, 1 for merge_ordered_ll, 2 for merge_sort\n");
    }
    return 0;
}