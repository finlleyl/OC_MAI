#ifndef TREE_H
#define TREE_H
#include <stdlib.h>
struct nodeinfo {
    int id;
    int pid;
    int available;
    struct nodeinfo *left;
    struct nodeinfo *right;
};
struct nodeinfo* tree_insert(struct nodeinfo*,int,int);
struct nodeinfo* tree_find(struct nodeinfo*,int);
#endif
