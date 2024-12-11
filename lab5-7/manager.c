#include "common.h"

int main() {
    void* ctx = zmq_ctx_new();
    NodeInfo* nodes = NULL;
    while(1) {
        char line[1024];
        if(!fgets(line,1024,stdin)) break;
        line[strcspn(line,"\n")]=0;
        if(strlen(line)==0) continue;
        char* cmd = strdup(line);
        char* token=strtok(cmd," ");
        if(!token) {free(cmd);continue;}
        if(strcmp(token,"create")==0) {
            char* sid=strtok(NULL," ");
            char* sparent=strtok(NULL," ");
            if(!sid) {
                printf("Error: [Custom error]\n");
                free(cmd);
                continue;
            }
            int nid=atoi(sid);
            if(find_node(nodes,nid)) {
                printf("Error: Already exists\n");
                free(cmd);
                continue;
            }
            int pid;
            int parent_id=-1;
            if(sparent) parent_id=atoi(sparent);
            if(parent_id!=-1 && !find_node(nodes,parent_id)) {
                printf("Error: Parent not found\n");
                free(cmd);
                continue;
            }
            if(parent_id!=-1) {
                NodeInfo* p=find_node(nodes,parent_id);
                if(!p || !p->available) {
                    printf("Error: Parent is unavailable\n");
                    free(cmd);
                    continue;
                }
            }
            pid=fork();
            if(pid==0) {
                char nid_str[32];sprintf(nid_str,"%d",nid);
                char pid_str[32];sprintf(pid_str,"%d",parent_id);
                execl("./node","node",nid_str,pid_str,(char*)NULL);
                exit(1);
            }
            char endpoint[256];
            sprintf(endpoint,"ipc:///tmp/node_%d.ipc",nid);
            add_node(&nodes,nid,parent_id,pid,endpoint);
            printf("Ok: %d\n",pid);
        } else if(strcmp(token,"exec")==0) {
            char* sid=strtok(NULL," ");
            if(!sid) {
                printf("Error:id: [Custom error]\n");
                free(cmd);
                continue;
            }
            int nid=atoi(sid);
            NodeInfo* n=find_node(nodes,nid);
            if(!n) {
                printf("Error:%d: Not found\n",nid);
                free(cmd);
                continue;
            }
            if(!n->available) {
                printf("Error:%d: Node is unavailable\n",nid);
                free(cmd);
                continue;
            }
            char* rest = strtok(NULL,"");
            if(!rest) {printf("Error:%d: [Custom error]\n",nid);free(cmd);continue;}
            void* sock=zmq_socket(ctx,ZMQ_REQ);
            zmq_setsockopt(sock,ZMQ_LINGER,0,0);
            int rc=zmq_connect(sock,n->endpoint);
            if(rc!=0) {
                n->available=0;
                printf("Error:%d: Node is unavailable\n",nid);
                zmq_close(sock);
                free(cmd);
                continue;
            }
            char sendbuf[1024];
            snprintf(sendbuf,1024,"exec %s",rest);
            zmq_send(sock,sendbuf,strlen(sendbuf),0);
            zmq_pollitem_t items[1];
            items[0].socket=sock;
            items[0].fd=0;
            items[0].events=ZMQ_POLLIN;
            rc=zmq_poll(items,1,200);
            if(rc>0 && (items[0].revents&ZMQ_POLLIN)) {
                char buf[1024];
                int sz=zmq_recv(sock,buf,1024,0);
                if(sz>0) {
                    buf[sz]=0;
                    printf("Ok:%d: %s\n",nid,buf);
                } else {
                    n->available=0;
                    printf("Error:%d: Node is unavailable\n",nid);
                }
            } else {
                n->available=0;
                printf("Error:%d: Node is unavailable\n",nid);
            }
            zmq_close(sock);
        } else if(strcmp(token,"ping")==0) {
            char* sid=strtok(NULL," ");
            if(!sid) {
                printf("Error: Not found\n");
                free(cmd);
                continue;
            }
            int nid=atoi(sid);
            NodeInfo* n=find_node(nodes,nid);
            if(!n) {
                printf("Error: Not found\n");
                free(cmd);
                continue;
            }
            if(!n->available) {
                printf("Ok: 0\n");
                free(cmd);
                continue;
            }
            void* sock=zmq_socket(ctx,ZMQ_REQ);
            zmq_setsockopt(sock,ZMQ_LINGER,0,0);
            int rc=zmq_connect(sock,n->endpoint);
            if(rc!=0) {
                n->available=0;
                printf("Ok: 0\n");
                zmq_close(sock);
                free(cmd);
                continue;
            }
            char pingbuf[]="ping";
            zmq_send(sock,pingbuf,strlen(pingbuf),0);
            zmq_pollitem_t items[1];
            items[0].socket=sock;
            items[0].fd=0;
            items[0].events=ZMQ_POLLIN;
            rc=zmq_poll(items,1,200);
            if(rc>0 && (items[0].revents&ZMQ_POLLIN)) {
                char buf[1024];
                int sz=zmq_recv(sock,buf,1024,0);
                if(sz>0) {
                    buf[sz]=0;
                    if(strcmp(buf,"1")==0) printf("Ok: 1\n"); else printf("Ok: 0\n");
                } else {
                    n->available=0;
                    printf("Ok: 0\n");
                }
            } else {
                n->available=0;
                printf("Ok: 0\n");
            }
            zmq_close(sock);
        } else {
            printf("Error: [Custom error]\n");
        }
        free(cmd);
    }
    while(nodes) {
        NodeInfo* tmp=nodes->next;
        kill(nodes->pid,SIGKILL);
        free(nodes);
        nodes=tmp;
    }
    zmq_ctx_term(ctx);
    return 0;
}
