#pragma once

#include "ll.h"

void append_node(Node** head, Node* node);
Node* concat_ll(Node* heada, Node* headb);
Node* quick_sort(Node* head);
void *parallel_quick_sort_wrapper(void *arg);