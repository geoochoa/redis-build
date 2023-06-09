#include <stdint.h> // For integer types
#include <stdlib.h> // Access to various macros
#include <string.h>
#include <stdio.h>      // Std Inp/Output
#include <errno.h>      // For knowing what went wrong & killing program
#include <unistd.h>     // Access to constants, types & misc functions
#include <arpa/inet.h>  // Access to  in_port_t, in_addr_t & in_addr
#include <sys/socket.h> // Access to  socklen_t (unsigned opaque integral type of length of at least 32 bits) & sockaddr
#include <netinet/ip.h> // Access to  sockaddr_in

static void die(const char *msg)
{
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int main()
{

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        die("socket()");

        struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        die("connect");
    }

    char msg[] = "hello";
    write(fd, msg, strlen(msg));

    char rbuf[64] = {};
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if (n < 0)
    {
        die("read");
    }
    printf("server says: %s\n", rbuf);
    close(fd);
    return 0;
}
