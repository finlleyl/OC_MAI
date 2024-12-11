#include "common.h"

NodeInfo* find_node(NodeInfo* head, int id) {
    NodeInfo* cur = head;
    while(cur) {
        if(cur->id == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_node(NodeInfo** head, int id, int parent_id, pid_t pid, const char* endpoint) {
    NodeInfo* n = (NodeInfo*)malloc(sizeof(NodeInfo));
    n->id = id;
    n->parent_id = parent_id;
    n->available = 1;
    n->pid = pid;
    strncpy(n->endpoint, endpoint, 255);
    n->endpoint[255]='\0';
    n->next = *head;
    *head = n;
}

int remove_node(NodeInfo** head, int id) {
    NodeInfo* cur = *head;
    NodeInfo* prev = NULL;
    while(cur) {
        if(cur->id == id) {
            if(prev) prev->next = cur->next; else *head = cur->next;
            free(cur);
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}
