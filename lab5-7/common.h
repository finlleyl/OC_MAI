#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>

typedef struct NodeInfo {
    int id;
    int parent_id;
    int available;
    char endpoint[256];
    pid_t pid;
    struct NodeInfo *next;
} NodeInfo;

NodeInfo* find_node(NodeInfo* head, int id);
void add_node(NodeInfo** head, int id, int parent_id, pid_t pid, const char* endpoint);
int remove_node(NodeInfo** head, int id);
#endif
