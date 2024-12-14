//
// Created by root on 24-12-7.
//
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <poll.h>
#include <pthread.h>
#include <sys/epoll.h>

#define BUFFER_LENGTH 128

struct conn_item {
    int fd;
    char buffer[BUFFER_LENGTH];
    int idx;
};

struct conn_item connlist[1024] = {0};

using namespace std;

void *client_thread(void *arg) {
    int clientfd = *(int *)arg;
    while (true) {
        char buffer[128] = {0};
        int count = recv(clientfd, buffer, 128, 0);
        if (count == 0) {
            break;
        }
        send(clientfd, buffer, count, 0);
        printf("clientfd: %d\n, count: %d\n, buffer: %s\n", clientfd, count, buffer);
    }
    close(clientfd);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(sockaddr_in));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(2048);

    if (-1 == bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) {
        perror("bind");
        return -1;
    }
    listen(sockfd, 10);
#if 0

    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    int clientfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len);
    printf("accept\n");

#if 0
    char buffer[128] = {0};
    int count = recv(clientfd, buffer, 128, 0);
    send(clientfd, buffer, count, 0);
    printf("sockfd: %d, clientfd: %d\n, count: %d\n, buffer: %s\n", sockfd, clientfd, count, buffer);

#else
    while (true) {
        char buffer[128] = {0};
        int count = recv(clientfd, buffer, 128, 0);
        if (count == 0) {
            break;
        }
        send(clientfd, buffer, count, 0);
        printf("sockfd: %d, clientfd: %d\n, count: %d\n, buffer: %s\n", sockfd, clientfd, count, buffer);
    }

#endif

#elif
    while (true) {
        struct sockaddr_in clientaddr;
        socklen_t len = sizeof(clientaddr);
        int clientfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len);

        pthread_t thid;
        pthread_create(&thid, NULL, client_thread, &clientfd);
    }

#elif 0
    // int nready = select(maxfd, rset, wset, eset, timeout);

    fd_set rfds, rset;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    int maxfd = sockfd;

    printf("loop\n");

    while (true) {
        rset = rfds;
        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int clientfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len);
            printf("sockfd: %d\n", clientfd);

            FD_SET(clientfd, &rfds);
            maxfd = clientfd;
        }

        int i = 0;
        for (i = sockfd + 1; i <= maxfd; i++) {
            if (FD_ISSET(i, &rset)) {
                char buffer[128] = {0};
                int count = recv(i, buffer, 128, 0);
                if (count == 0) {
                    printf("disconnect\n");
                    // close(i);
                    FD_CLR(i, &rfds);
                    close(i);
                    break;
                }

                send(i, buffer, count, 0);
                printf("clientfd: %d, count: %d, buffer: %s\n", i, count, buffer);
            }
        }
    }

#elif 0
    struct pollfd fds[1024] = {0};

    fds[sockfd].fd = sockfd;
    fds[sockfd].events = POLLIN;

    int maxfd = sockfd;

    while (true) {
        int nready = poll(fds, maxfd + 1, -1);
        if (fds[sockfd].revents & POLLIN) {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);

            int clientfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len);

            printf("sockfd: %d\n", clientfd);
            fds[clientfd].fd = clientfd;
            fds[clientfd].events = POLLIN;

            maxfd = clientfd;
        }

        int i = 0;
        for (i = sockfd + 1; i <= maxfd; i++) {
            if (fds[i].revents & POLLIN) {
                char buffer[128] = {0};
                int count = recv(i, buffer, 128, 0);
                if (count == 0) {
                    printf("disconnect\n");

                    fds[i].fd = -1;
                    fds[i].events = 0;

                    close(i);
                    continue;
                }

                send(i, buffer, count, 0);
                printf("clientfd: %d, count: %d, buffer: %s\n", i, count, buffer);
            }
        }
    }

#else
    int epfd = epoll_create(1);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    struct epoll_event events[1024] = {0};
    while (true) {
        int nready = epoll_wait(epfd, events, 1024, -1);

        int i = 0;
        for (i = 0; i < nready; i++) {
            int connfd = events[i].data.fd;
            if (sockfd == connfd) {
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);

                int clientfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len);

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = clientfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);

                connlist[clientfd].fd = clientfd;
                memset(connlist[clientfd].buffer, 0, BUFFER_LENGTH);
                connlist[clientfd].idx = 0;

                printf("clientfd: %d\n", clientfd);
            }
            else if (events[i].events & EPOLLIN) {
                char *buffer = connlist[connfd].buffer;
                int idx = connlist[connfd].idx;
                int count = recv(i, buffer + idx, BUFFER_LENGTH - idx, 0);
                if (count == 0) {
                    printf("disconnect\n");

                    epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);
                    close(events[i].data.fd);
                    continue;
                }
                connlist[connfd].idx += count;

                send(connfd, buffer, count, 0);
                printf("clientfd: %d, count: %d, buffer: %s\n", connfd, count, buffer);
            }
        }
    }

#endif

    getchar();
    // close(clientfd);
    return 0;
}
