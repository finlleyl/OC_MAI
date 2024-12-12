#include "tree.h"
struct nodeinfo* tree_insert(struct nodeinfo* r,int id,int pid) {
    if(!r) {
        struct nodeinfo* n=malloc(sizeof(*n));
        n->id=id; n->pid=pid; n->available=1; n->left=0; n->right=0;
        return n;
    }
    if(id<r->id) r->left=tree_insert(r->left,id,pid); else if(id>r->id) r->right=tree_insert(r->right,id,pid);
    return r;
}
struct nodeinfo* tree_find(struct nodeinfo* r,int id) {
    if(!r) return 0;
    if(id==r->id) return r;
    if(id<r->id) return tree_find(r->left,id);
    return tree_find(r->right,id);
}
