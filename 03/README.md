## What

2 simple (incomplete and broken) programs demonstrating some necessary syscalls.
03_server.cpp functions as a server that receives connections from clients,
reads a solitary message, and produces a single response. 03_client acts as a client
that links to the server, composes a single message, and retrieves a single reply.

## How to run:

`g++ -Wall -Wextra -O2 -g 03_server.cpp -o server`
`g++ -Wall -Wextra -O2 -g 03_client.cpp -o client`

## Expected output:

> $ ./server
>
> client says: hello

> $ ./client
>
> server says: world
