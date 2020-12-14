#define _XOPEN_SOURCE 700
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

struct timeval{
    long tv_sec;
    long  tv_usec;
};

int gettimeofday(struct timeval *restrict tp, void *restrict tzp);

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main(int argc, char** argv) {
    char buffer[BUFSIZ];
    enum CONSTEXPR { MAX_REQUEST_LEN = 1024};
    char request[MAX_REQUEST_LEN];
    char request_template[] = "GET / HTTP/1.1\r\nHost: %s\r\n\r\n";
    struct protoent *protoent;
    char *hostname = "example.com";
    in_addr_t in_addr;
    int request_len;
    int socket_file_descriptor;
    ssize_t nbytes_total, nbytes_last;
    struct hostent *hostent;
    struct sockaddr_in sockaddr_in;
    unsigned short server_port = 80;
    int noreq = 0;
    long fastestTime=99999;
    long slowestTime=-1;
    long sucess=0;
    long smallestRespSize=99999;
    long largestRespSize=-1;

    if(argc >= 2){

        hostname = argv[2];
        if(strcmp("--url", argv[1]) == 0){
            noreq=1;
        } else if(strcmp("--help", argv[1]) == 0) {
            printf("\n\n List of commands:\n./ashwinSocket --url routerproj.axs190172.workers.dev\n./ashwinSocket --profile routerproj.axs190172.workers.dev 7 //where 7 is the number of requests\n");
            return 0;
        }  else if(strcmp("--profile", argv[1]) == 0) {
            if(argc < 3) {
                printf("\n\n@@@Error. Enter --help for command details\n\n");
                return 0;
            }
            int g = atoi(argv[3]);
            noreq=g;
            // printf("\n \n hostname iss %s and noreq: %d\n", hostname, noreq);
        } else {
            printf("\n\n@@@Error. Enter --help for command details\n\n");
            return 0;
        }
    } else {
            printf("\n\n@@Error. Enter --help for command details\n\n");
            return 0;
    }
    long alltimes[noreq];
   for(int i=0;i<noreq; i++){
    //    printf("\n\nashwinnnn i=%d\n\n", i);
    request_len = snprintf(request, MAX_REQUEST_LEN, request_template, hostname);
    if (request_len >= MAX_REQUEST_LEN) {
        exit(EXIT_FAILURE);
    }

    protoent = getprotobyname("tcp");
    if (protoent == NULL) {
        perror("getprotobyname");
        exit(EXIT_FAILURE);
    }
    socket_file_descriptor=0;
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
    if (socket_file_descriptor == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    hostent = gethostbyname(hostname);
    if (hostent == NULL) {
        fprintf(stderr, "error: gethostbyname(\"%s\")\n", hostname);
        exit(EXIT_FAILURE);
    }
    in_addr = inet_addr(inet_ntoa(*(struct in_addr*)*(hostent->h_addr_list)));
    if (in_addr == (in_addr_t)-1) {
        fprintf(stderr, "error: inet_addr(\"%s\")\n", *(hostent->h_addr_list));
        exit(EXIT_FAILURE);
    }
    sockaddr_in.sin_addr.s_addr = in_addr;
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(server_port);

    /* Actually connect. */
    if (connect(socket_file_descriptor, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);
        nbytes_total = 0;
        bool suc=true;
        while (nbytes_total < request_len) {
            nbytes_last = write(socket_file_descriptor, request + nbytes_total, request_len - nbytes_total);
            if (nbytes_last == -1) {
                perror("write");
                suc=false;
                
            }
            sucess++;
            nbytes_total += nbytes_last;
        }
        long szofresp = sizeof(nbytes_total);
        if(smallestRespSize > szofresp){
            smallestRespSize = szofresp;
        }
        if(largestRespSize < szofresp){
            largestRespSize = szofresp;
        }
        // sleep(5);
            gettimeofday(&end, NULL);

            long seconds = (end.tv_sec - start.tv_sec);
    long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

        // printf("\n\nTime elpased is %d seconds and %d micros\n\n", seconds, micros);
        if(fastestTime > micros){
            fastestTime = micros;
        }
        if(slowestTime < micros){
            slowestTime=micros;
        }
        alltimes[i] = micros;
        //  printf("\n\n time for request: %ju\n\n",(end_secs-start_secs));

        /* Read the response. */
        if(!suc) continue;
        // fprintf(stderr, "debug: before first read\n");
        while ((nbytes_total = read(socket_file_descriptor, buffer, BUFSIZ)) > 0) {
            // fprintf(stderr, "debug: after a read\n");
            int sz = write(STDOUT_FILENO, buffer, nbytes_total);
            if(sz > 1) {
                break;
            }
            
        }
        // fprintf(stderr, "debug: after last read\n");
        close(socket_file_descriptor);
        if (nbytes_total == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

    }
    printf("\n");
    if(noreq > 1){
        long sum=0;
        long mean=0;
        for(int i=0; i<noreq; i++){
            sum+=alltimes[i];
        }
        mean=sum/(long)noreq;
        printf("\n\nHostname: %s", hostname);
        printf("\nThe number of requests: %d\n", noreq);
        printf("Fastest Time: %ld micros, Slowest Time: %ld micros\n",fastestTime,slowestTime);
        qsort(alltimes, 0, sizeof(int), cmpfunc);
        printf("Mean Time : %ld micros, Median: %ld\n",mean, alltimes[noreq/2]);
        long perc = (sucess*100)/noreq;
        printf("Percent Sucess: %ld\n", perc);
        printf("Size of Smallest Response: %ld bytes , Size of Largest Response %ld bytes\n\n", smallestRespSize, largestRespSize);
    }
    exit(EXIT_SUCCESS);
}
