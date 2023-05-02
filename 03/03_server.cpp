#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void msg(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg)
{
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static void do_something(int connfd)
{
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0)
    {
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf));
}

int main()
{
    /*
        int socket(int domain, int type, int protocol)
        Domain: Protocol Family defined in <sys/socket.h>.    :: AF_INET for IPv4 (Internet Protocol)
        Type: Communication Semantics.    :: SOCK_STREAM for a connection-based protocol (TCP) as opposed to UDP (SOCK_DGRAM)
        Protocol: When a single protocol exists to support a
                    particular socket type within a given protocol
                    family  :: 0

        Returns: Success: file descriptor referring to newly created endpoint(socket), establishing communication
                    Error: -1, errno gets details as defined in <errno.h>
    */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        die("socket()");

    /*
    *   Needed for most server applications
        Sets the socket options
        Defined in <sys/socket.h>

        setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
        Socket:
        Level: Protocol level at which the option resides, SOL_SOCKET sets options at the socket level
        Option_name: Specifies a single option to set as defined in <sys/socket.h>. SO_REUSEADDR is used as
                        without this option, the server won’t able to bind to the same address if restarted.
                        Book challenges reader with:
                        "Exercise to reader: find out what exactly is SO_REUSEADDR and why it is needed."
                        Answer: It's an option that allows the reuse of local addresses and ports.
                        An issue arises when a process on a given IP address and port suddenly terminates
                        and then restarts again. When the process terminates, the corresponding socket
                        closes. When this happens, sockets enter TIME_WAIT, where rogue packets that haven't
                        reached their destination are given time to reach their destination. During this,
                        address/port combination bound to the socket is unavailable.
                        So when the process restarts, it will want to reuse that port/address but we need to
                        give it permission by enabling the SO_REUSEADDR option!
                        Also, SO_REUSEADDR allows wildcard addresses to bind to the same port.
    */
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    /*
        Bind
        Syntax that deals with IPv4 addresses

    */
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);     /* Converts unsigned short integer netshort:
                                        network byte order -> host byte order. (1234) -> (0.0.0.0:1234:) */
    addr.sin_addr.s_addr = ntohl(0); /* Converts the unsigned integer netlong:
                                        network byte order -> host byte order.
                                        wildcard address (0) -> (0.0.0.0) */
    /*
        int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
        Assigns the address specified by addr to the socket referred to
            by the file descriptor sockfd.
        Traditionally, this operation is called “assigning a name to a socket”.
        Sockfd:
        Sockaddr:
        Addrlen:
    */
    int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr)); // binds a name to a socket
    if (rv)
        die("bind()");

    /*
        Listen for connections on a socket
        int listen(int sockfd, int backlog);
        Marks sockfd as a passive socket, or a socket that will be used to accept incoming
            connection requests using accept().
        Sockfd: File descriptor pointing to socket
        Backlog: Max length to which queue of pending connections for sockfd may grow
            If queue is full and socket receives request, client may receive error or if
            client supports retransmission, may ignore the request and try again later
    */
    rv = listen(fd, SOMAXCONN);
    if (rv)
        die("listen()");

    /**
     *  Accept a connection on a socket
        int accept(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict addrlen);
        It extracts the first connection request on the queue of pending connections for the
            listening socket, sockfd, creates a new connected socket, and
            returns a new file descriptor referring to that socket
    */
    while (true)
    {
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if (connfd < 0)
            continue; // error

        do_something(connfd);
        close(connfd);
    }

    return 0;
}
