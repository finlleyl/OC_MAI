#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "rabbit.h"

int main(int argc,char**argv) {
    if(argc<3)return 1;
    int id=atoi(argv[1]);
    const char* q=argv[2];
    if(rabbit_connect("localhost",5672))return 1;
    if(rabbit_create_queue(q))return 1;
    char buf[1024];
    while(1) {
        if(rabbit_recv(q,buf,sizeof(buf))) {
            sleep(1);
            continue;
        }
        if(strncmp(buf,"exec",4)==0) {
            char *s = buf + 5; 
            char *id_str = strtok(s," ");
            if(!id_str) continue;
            int exec_id = atoi(id_str);

            char *n_str = strtok(NULL," ");
            if(!n_str) continue;
            int n = atoi(n_str);

            long sum = 0;
            for (int i = 0; i < n; i++) {
                char *num_str = strtok(NULL," ");
                if(!num_str) break;
                sum += atol(num_str);
            }

            char resp[128];
            sprintf(resp,"Ok:%d: %ld",exec_id,sum);
            rabbit_send("controller_resp",resp,strlen(resp));
        } else if(strncmp(buf,"ping",4)==0) {
            char resp[32];
            sprintf(resp,"Ok:%d:1",id);
            rabbit_send("controller_resp",resp,strlen(resp));
        }
    }
    return 0;
}
