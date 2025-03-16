1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines when a command's output is fully received from the server by checking for an End-of-File (EOF) character that the server sends after completing command execution. The client handles partial reads by using a fixed-size buffer (RDSH_COMM_BUFF_SZ, which is 64KB) for receiving data or processing each chunk as it arrives, without waiting for the entire message.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

TCP's stream-oriented nature requires application protocols to implement their own message framing mechanisms since TCP doesn't preserve message boundaries. In order for a networked shell proticol define and detect the beginning and end of a command sent over a TCP connection, The protocol uses an ASCII EOF character (0x04) as a message boundary marker. If message boundaries aren't properly handled, several, a client might interpret partial commands as complete ones or combine multiple commands incorrectly. Also, if a message is larger than expected and no boundary is defined, it could overflow buffers, causing memory corruption or security vulnerabilities.

3. Describe the general differences between stateful and stateless protocols.

Stateful and Stateless protocols represent two fundamentally different approaches to managing communication between clients and servers in networked applications. Stateful protocols maintain information about the client's session across multiple requests. The server keeps track of the client's state, remembering previous interactions and using that context for future communications. Stateless protocols treat each request as an independent transaction without any knowledge of previous requests. The server doesn't store client session information between requests.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is labeled as "unreliable" because it lacks the built-in guarantees that TCP provides, but this characteristic doesn't make it useless—in fact, it makes UDP ideal for specific use cases where speed and efficiency are prioritized over perfect delivery. UDP is faster because it eliminates the overhead of connection establishment, acknowledgments, and retransmissions.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The operating system provides several interfaces and abstractions that enable applications to use network communications, with the most fundamental being the socket. Socket abstracts the complexities of the underlying network hardware and protocols, allowing applications to create communication endpoints (sockets), establish connections to remote hosts, send and receive data over the network and close connections when communication is complete.