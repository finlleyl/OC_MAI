#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tree.h"
#include "rabbit.h"

static struct nodeinfo*root=0;

int main(int argc,char**argv) {
    if(rabbit_connect("localhost",5672)) {
        printf("Error: RabbitMQ connect\n");
        return 1;
    }
    if(rabbit_create_queue("controller_resp")) {
        printf("Error: Can't create resp queue\n");
        return 1;
    }
    char line[1024];
    for(;;) {
        if(!fgets(line,sizeof(line),stdin)) break;
        if(strncmp(line,"create",6)==0) {
            char*cur=line+7;
            while(*cur==' ')cur++;
            char*i1=strtok(cur," \n");
            if(!i1) {printf("Error: [Custom error]\n");continue;}
            int id=atoi(i1);
            struct nodeinfo*n=tree_find(root,id);
            if(n) {printf("Error: Already exists\n");continue;}
            int pid=fork();
            if(pid<0) {printf("Error: [Custom error]\n");continue;}
            if(pid==0) {
                char qname[64];sprintf(qname,"node_%d",id);
                execl("./node","node",i1,qname,(char*)0);
                _exit(1);
            }
            waitpid(pid,NULL,WNOHANG);
            root=tree_insert(root,id,pid);
            printf("Ok: %d\n",pid);
        } else if(strncmp(line,"exec",4)==0) {
            line[strcspn(line, "\n")] = '\0';

            char*cur=line+5;
            while(*cur==' ') cur++;

            char parse_buf[1024];
            strcpy(parse_buf, cur);
            char *id_str = strtok(parse_buf," ");
            if(!id_str) {printf("Error:id: Not found\n");continue;}
            int id=atoi(id_str);

            struct nodeinfo*n=tree_find(root,id);
            if(!n) {printf("Error:%d: Not found\n",id);continue;}
            if(!n->available) {printf("Error:%d: Node is unavailable\n",id);continue;}

            char qname[64];
            sprintf(qname,"node_%d",id);

            char rest[512];
            sprintf(rest,"exec %s",cur);

            if(rabbit_send(qname,rest,strlen(rest))) {
                printf("Error:%d: Node is unavailable\n",id);
                continue;
            }

            char resp[1024];
            if(rabbit_recv("controller_resp",resp,sizeof(resp))) {
                printf("Error:%d: Node is unavailable\n",id);
                continue;
            }

            printf("%s\n",resp);
        } else if(strncmp(line,"ping",4)==0) {
            char*cur=line+5;
            while(*cur==' ')cur++;
            int id=atoi(cur);
            struct nodeinfo*n=tree_find(root,id);
            if(!n) {printf("Error: Not found\n");continue;}
            if(!n->available) {printf("Ok: 0\n");continue;}
            char qname[64];sprintf(qname,"node_%d",id);
            if(rabbit_send(qname,"ping",4)) {printf("Ok: 0\n");continue;}
            char resp[1024];
            if(rabbit_recv("controller_resp",resp,sizeof(resp))) {printf("Ok: 0\n");continue;}
            char*chk=strstr(resp,"Ok:");
            if(chk) {
                char*col=strrchr(resp,':');
                if(col && atoi(col+1)==1) {printf("Ok: 1\n");} else {printf("Ok: 0\n");}
            } else {
                printf("Ok: 0\n");
            }
        } else {
            printf("Error: [Custom error]\n");
        }
    }
    return 0;
}
