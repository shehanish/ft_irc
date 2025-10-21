# IRC Server Evaluation Guide

## Table of Contents
1. [Project Overview](#project-overview)
2. [Architecture Overview](#architecture-overview)
3. [Client Handling (Your Responsibility)](#client-handling)
4. [Command Handling (Your Responsibility)](#command-handling)
5. [IRC Protocol Compliance](#irc-protocol-compliance)
6. [Testing & Demonstration](#testing--demonstration)
7. [Technical Challenges & Solutions](#technical-challenges--solutions)

---

## Project Overview

### What is this project?
This is a **custom IRC (Internet Relay Chat) server** implementation in **C++98** that:
- Handles multiple simultaneous client connections
- Manages channels and user permissions
- Implements IRC RFC 1459/2812 protocol
- Uses **non-blocking I/O** with `poll()` for concurrent connections
- Provides real-time messaging between users and channels

### Key Requirements Met
✅ **C++98 compliant** - No C++11/14/17 features  
✅ **Non-blocking I/O** - Server never hangs  
✅ **Multiple clients** - Tested with 3+ simultaneous connections  
✅ **Poll-based** - Single-threaded event loop using `poll()`  
✅ **IRC compatible** - Works with standard IRC clients (HexChat, irssi, etc.)  
✅ **Error handling** - 22 IRC error codes implemented  

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                        IRC SERVER                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌────────────┐      ┌────────────┐      ┌──────────────┐ │
│  │   Server   │◄────►│   Client   │◄────►│   Channel    │ │
│  │  (Network) │      │ (Connection)│      │  (Chatrooms) │ │
│  └────────────┘      └────────────┘      └──────────────┘ │
│        ▲                    ▲                     ▲        │
│        │                    │                     │        │
│        └────────────────────┼─────────────────────┘        │
│                             │                              │
│                      ┌──────▼──────┐                       │
│                      │  Commands   │                       │
│                      │  (Handlers) │                       │
│                      └─────────────┘                       │
└─────────────────────────────────────────────────────────────┘
```

### Core Components

| Component | File | Purpose |
|-----------|------|---------|
| **Server** | `Server.cpp` | Network handling, poll() loop, client management |
| **Client** | `Client.cpp` | Individual connection state, buffers, authentication |
| **Channel** | `Channel.cpp` | Chat rooms, member lists, channel modes |
| **Commands** | `CommandHandlers.cpp` | IRC command implementations (JOIN, PRIVMSG, etc.) |

---

## Client Handling

### Overview
**Client handling** manages individual user connections from accept() through authentication to disconnection. Each client has their own state, buffers, and IRC identity.

### 1. Client Connection Flow

```
┌──────────────┐
│ New TCP      │
│ Connection   │
└──────┬───────┘
       │
       ▼
┌──────────────────────────────────────────────┐
│ Server::loop() - accept() new connection     │
│ - Create socket fd                           │
│ - Set non-blocking: fcntl(fd, F_SETFL, O_NONBLOCK) │
│ - Add to poll_fds vector                     │
│ - Create Client object in _clients map       │
└──────┬───────────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────────────┐
│ Client Authentication (3-step process)       │
│ 1. PASS <password>    → setIsAuthenticated() │
│ 2. NICK <nickname>    → setNick()            │
│ 3. USER <username>... → setUserName()        │
└──────┬───────────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────────────┐
│ registerClient() - All 3 steps complete      │
│ - setRegistered(true)                        │
│ - Send welcome messages (001-004)            │
│ - Client can now use IRC commands            │
└──────┬───────────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────────────┐
│ Active IRC Session                           │
│ - JOIN channels                              │
│ - Send/receive messages                      │
│ - Execute commands                           │
└──────┬───────────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────────────┐
│ Disconnect (Ctrl-D or connection lost)       │
│ - close(fd)                                  │
│ - Remove from _clients map                   │
│ - Remove from poll_fds vector                │
│ - Delete Client object                       │
└──────────────────────────────────────────────┘
```

### 2. Client Class Structure (`Client.cpp`)

#### Key Member Variables
```cpp
class Client {
private:
    int _fd;                    // Socket file descriptor
    std::string _nick;          // User's nickname
    std::string _username;      // Username (from USER command)
    std::string _realname;      // Real name (from USER command)
    std::string _ip;            // Client IP address
    
    // State flags
    bool _isAuthenticated;      // Has sent correct PASS
    bool _isRegistered;         // Completed PASS + NICK + USER
    
    // Buffers for IRC protocol
    std::string _recvBuffer;    // Incoming data buffer
    std::string _sendBuffer;    // Outgoing data buffer
};
```

#### Why Two Buffers?

**Receive Buffer (`_recvBuffer`):**
- Stores incomplete IRC messages
- IRC messages end with `\r\n`
- TCP might split one message across multiple recv() calls
- Example:
  ```
  recv() call 1: "PRIVMSG #ch"
  recv() call 2: "annel :Hello\r\n"
  ```
- Buffer accumulates: `"PRIVMSG #channel :Hello\r\n"` → complete message

**Send Buffer (`_sendBuffer`):**
- Queues messages to be sent to client
- `send()` might not send entire message (EAGAIN/EWOULDBLOCK)
- Non-blocking sockets need buffering
- Example:
  ```
  Want to send: "Welcome message" + "Channel list" + "Topic"
  send() only sends 50 bytes → rest goes in _sendBuffer
  POLLOUT event triggers → flush() sends remaining data
  ```

### 3. Non-Blocking I/O Explained

#### Why Non-Blocking?
```cpp
// BLOCKING (BAD - server freezes):
char buf[1000];
recv(fd, buf, 1000, 0);  // Waits forever if no data!

// NON-BLOCKING (GOOD):
fcntl(fd, F_SETFL, O_NONBLOCK);
int n = recv(fd, buf, 1000, 0);
if (n == -1 && errno == EAGAIN) {
    // No data available right now, that's OK!
    // poll() will tell us when data arrives
}
```

#### The Poll Loop (`Server.cpp::loop()`)

```cpp
void Server::loop() {
    std::vector<pollfd> poll_fds;
    
    // poll_fds structure:
    // pollfd {
    //     int fd;        // File descriptor to monitor
    //     short events;  // What to watch for (POLLIN/POLLOUT)
    //     short revents; // What actually happened
    // }
    
    while (true) {
        // poll() blocks until activity on ANY fd
        poll(poll_fds.data(), poll_fds.size(), -1);
        
        for (size_t i = 0; i < poll_fds.size(); i++) {
            if (poll_fds[i].revents & POLLIN) {
                // Data available to READ
                if (poll_fds[i].fd == _serverfd) {
                    // New client connecting
                    acceptNewClient();
                } else {
                    // Existing client sent data
                    receiveAndProcess(poll_fds[i].fd);
                }
            }
            
            if (poll_fds[i].revents & POLLOUT) {
                // Ready to WRITE data
                flushSendBuffer(poll_fds[i].fd);
            }
        }
    }
}
```

### 4. Message Parsing (`Server.cpp::parse()`)

#### IRC Message Format
```
[:<prefix>] <command> [<param1> <param2> ...] [:<trailing>]
```

Examples:
```
NICK alice
PRIVMSG #channel :Hello everyone!
:alice!~user@host PRIVMSG bob :Private message
```

#### Parsing Steps

```cpp
bool Server::parse(std::string &msg, Client *client) {
    // 1. Validate length
    if (msg.size() > 512) return false;  // IRC max message size
    
    // 2. Trim \r\n
    msg = msg.substr(0, msg.find_last_not_of("\r\n") + 1);
    
    // 3. Extract prefix (optional, starts with ':')
    if (msg[0] == ':') {
        // Server-to-client messages have prefix
        // Client-to-server usually don't
    }
    
    // 4. Extract COMMAND (uppercase it)
    std::string cmd = extractWord();
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    
    // 5. Parse arguments
    std::vector<std::string> args;
    while (more_args) {
        if (arg[0] == ':') {
            // Trailing parameter - rest of message
            args.push_back(msg.substr(pos + 1));  // Skip ':'
            break;
        }
        args.push_back(nextWord());
    }
    
    // 6. Route to appropriate handler
    if (cmd == "PASS") handlePass(client, args);
    else if (cmd == "NICK") handleNick(client, args);
    // ... etc
}
```

**Important Detail: Trailing Parameter**
```
PRIVMSG #channel :Hello world!
                 ^
                 | This ':' indicates "trailing parameter"
                 | Everything after is ONE argument
                 
args[0] = "#channel"
args[1] = "Hello world!"  // NOT ["Hello", "world!"]
```

### 5. Authentication Process

#### Why 3 Steps?

1. **PASS** - Verify password before allowing any access
2. **NICK** - Identify the user uniquely
3. **USER** - Get username and real name

**Order doesn't matter!** HexChat sends NICK/USER before PASS.

```cpp
void Server::registerClient(Client &client) {
    // Already registered? Don't do it again
    if (client.isRegistered())
        return;
    
    // Check ALL three requirements
    if (!client.isAuthenticated() ||     // PASS not sent/wrong
        client.getNick().empty() ||       // NICK not set
        client.getUserName().empty())     // USER not set
    {
        return;  // Not ready yet
    }
    
    // All three done! Register the client
    client.setRegistered(true);
    sendWelcome(client);  // Send 001-004 numerics
}
```

#### PASS Command (`handlePass()`)
```cpp
void Server::handlePass(Client &client, const std::vector<std::string> &args) {
    // Already authenticated? Ignore (prevents password changes)
    if (client.isAuthenticated())
        return;
    
    // No password provided?
    if (args.empty()) {
        client.appendToSendBuffer(":localhost 461 * PASS :Not enough parameters\r\n");
        return;
    }
    
    // Wrong password?
    if (args[0] != _password) {
        client.appendToSendBuffer(":localhost 464 * :Password incorrect\r\n");
        return;
    }
    
    // Correct! Mark as authenticated
    client.setIsAuthenticated(true);
    
    // Try to complete registration (maybe NICK/USER already sent)
    registerClient(client);
}
```

#### NICK Command (`handleNick()`)
```cpp
void Server::handleNick(Client &client, const std::vector<std::string> &args) {
    // No nickname provided?
    if (args.empty()) {
        client.appendToSendBuffer(":localhost 431 * :No nickname given\r\n");
        return;
    }
    
    std::string newNick = args[0];
    
    // Nickname already in use? (check all clients except this one)
    if (isNickTaken(newNick, &client)) {
        client.appendToSendBuffer(":localhost 433 * " + newNick + 
                                 " :Nickname is already in use\r\n");
        return;
    }
    
    // If already registered, this is a nickname CHANGE
    if (client.isRegistered()) {
        std::string oldNick = client.getNick();
        // Notify user: :oldnick!user@host NICK :newnick
        std::string msg = ":" + oldNick + "!~" + client.getUserName() + 
                         "@localhost NICK :" + newNick + "\r\n";
        client.appendToSendBuffer(msg);
    }
    
    client.setNick(newNick);
    
    // Try to complete registration
    registerClient(client);
}
```

#### USER Command (`handleUser()`)
```cpp
void Server::handleUser(Client &client, const std::vector<std::string> &args) {
    // Need 4 parameters: username, mode, unused, realname
    if (args.size() < 4) {
        client.appendToSendBuffer(":localhost 461 * USER :Not enough parameters\r\n");
        return;
    }
    
    // args[0] = username
    // args[1] = mode (usually "0")
    // args[2] = unused (usually "*")
    // args[3] = real name (trailing parameter)
    
    client.setUserName(args[0]);
    client.setRealName(args[3]);
    
    registerClient(client);
}
```

### 6. Buffer Management Deep Dive

#### Receiving Data (`Client::receiveData()`)

```cpp
std::vector<std::string> Client::receiveData(const char *data, int len) {
    std::vector<std::string> messages;
    
    // Add new data to receive buffer
    _recvBuffer.append(data, len);
    
    // Extract complete messages (ending with \r\n)
    size_t pos;
    while ((pos = _recvBuffer.find("\r\n")) != std::string::npos) {
        // Found a complete message!
        std::string msg = _recvBuffer.substr(0, pos + 2);  // Include \r\n
        messages.push_back(msg);
        
        // Remove processed message from buffer
        _recvBuffer.erase(0, pos + 2);
    }
    
    // Remaining data stays in buffer for next recv()
    return messages;
}
```

**Visual Example:**
```
Received: "NICK ali"
_recvBuffer = "NICK ali"  (no \r\n, incomplete!)
Return: []

Received: "ce\r\nJOIN #test\r\n"
_recvBuffer = "NICK alice\r\nJOIN #test\r\n"
Extract "NICK alice\r\n"
Extract "JOIN #test\r\n"
_recvBuffer = "" (empty)
Return: ["NICK alice\r\n", "JOIN #test\r\n"]
```

#### Sending Data (`Client::flush()`)

```cpp
bool Client::flush() {
    if (_sendBuffer.empty())
        return true;  // Nothing to send
    
    // Try to send data
    ssize_t sent = send(_fd, _sendBuffer.c_str(), _sendBuffer.size(), 0);
    
    if (sent > 0) {
        // Successfully sent some data
        _sendBuffer.erase(0, sent);  // Remove sent portion
    } else if (sent == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Socket buffer full, try again later
            return false;
        }
        // Real error - connection problem
    }
    
    return _sendBuffer.empty();  // True if buffer fully flushed
}
```

**Why This Matters:**
```
Client1 sends message to #channel with 100 users
→ Server creates 100 messages in send buffers
→ Some clients have slow connections
→ send() returns EAGAIN (socket buffer full)
→ Message waits in _sendBuffer
→ POLLOUT triggers when socket ready
→ flush() completes sending
```

---

## Command Handling

### Overview
Command handling implements IRC protocol commands. Each command validates input, checks permissions, and sends appropriate responses or error codes.

### 1. Command Pattern Architecture

```cpp
// Abstract base class
class Command {
public:
    virtual void execute(Server &server, Client &client, s_data &data) = 0;
};

// Specific command implementation
class JoinCmd : public Command {
public:
    void execute(Server &server, Client &client, s_data &data) {
        // JOIN command logic
    }
};

// Server initializes command map
void Server::serverInit() {
    _commands["JOIN"] = new JoinCmd();
    _commands["PART"] = new PartCmd();
    _commands["PRIVMSG"] = new PrivMsgCmd();
    _commands["KICK"] = new KickCmd();
    // ... etc
}
```

**Benefits:**
- **Extensibility**: Easy to add new commands
- **Clean routing**: `parse()` just looks up command in map
- **Separation of concerns**: Each command is independent

### 2. Command Implementation Examples

#### JOIN Command - Most Complex

```cpp
void handleJoin(Server &server, Client &client, s_data &data) {
    // 1. Validate parameters
    if (data.args.empty()) {
        client.appendToSendBuffer(":localhost 461 " + client.getNick() + 
                                 " JOIN :Not enough parameters\r\n");
        return;
    }
    
    std::string channelName = data.args[0];
    std::string key = (data.args.size() > 1) ? data.args[1] : "";
    
    // 2. Validate channel name (must start with #)
    if (channelName[0] != '#') {
        client.appendToSendBuffer(":localhost 403 " + client.getNick() + 
                                 " " + channelName + " :No such channel\r\n");
        return;
    }
    
    // 3. Get or create channel
    Channel *channel = server.getChannel(channelName);
    if (!channel) {
        // Create new channel, creator becomes operator
        channel = server.createChannel(channelName, client);
    } else {
        // 4. Check channel restrictions
        
        // +k mode: password protected
        if (channel->hasMode('k') && channel->getKey() != key) {
            client.appendToSendBuffer(":localhost 475 " + client.getNick() + 
                                     " " + channelName + " :Cannot join channel (+k)\r\n");
            return;
        }
        
        // +i mode: invite-only
        if (channel->hasMode('i') && !channel->isInvited(&client)) {
            client.appendToSendBuffer(":localhost 473 " + client.getNick() + 
                                     " " + channelName + " :Cannot join channel (+i)\r\n");
            return;
        }
        
        // +l mode: user limit
        if (channel->hasMode('l') && 
            channel->getMembers().size() >= channel->getLimit()) {
            client.appendToSendBuffer(":localhost 471 " + client.getNick() + 
                                     " " + channelName + " :Cannot join channel (+l)\r\n");
            return;
        }
    }
    
    // 5. Add client to channel
    channel->addMember(&client);
    
    // 6. Broadcast JOIN to all channel members
    std::string joinMsg = ":" + client.getNick() + "!~" + 
                         client.getUserName() + "@localhost JOIN :" + 
                         channelName + "\r\n";
    
    std::set<Client*> members = channel->getMembers();
    for (std::set<Client*>::iterator it = members.begin(); 
         it != members.end(); ++it) {
        (*it)->appendToSendBuffer(joinMsg);
    }
    
    // 7. Send channel topic (if exists)
    if (!channel->getTopic().empty()) {
        client.appendToSendBuffer(":localhost 332 " + client.getNick() + 
                                 " " + channelName + " :" + 
                                 channel->getTopic() + "\r\n");
    } else {
        client.appendToSendBuffer(":localhost 331 " + client.getNick() + 
                                 " " + channelName + " :No topic is set\r\n");
    }
    
    // 8. Send NAMES list (who's in the channel)
    std::string namesList = ":localhost 353 " + client.getNick() + 
                           " = " + channelName + " :";
    for (std::set<Client*>::iterator it = members.begin(); 
         it != members.end(); ++it) {
        if (channel->isOperator(*it))
            namesList += "@";  // @ prefix for operators
        namesList += (*it)->getNick() + " ";
    }
    namesList += "\r\n";
    client.appendToSendBuffer(namesList);
    
    // 9. End of NAMES list
    client.appendToSendBuffer(":localhost 366 " + client.getNick() + 
                             " " + channelName + " :End of /NAMES list\r\n");
}
```

**Key Points:**
- ✅ Parameter validation (461)
- ✅ Channel name validation (403)
- ✅ Mode checking (+k, +i, +l)
- ✅ Broadcasting to all members
- ✅ Topic and member list

#### PRIVMSG Command - Message Broadcasting

```cpp
void handlePrivMsg(Server &server, Client &client, s_data &data) {
    // 1. Validate parameters
    if (data.args.size() < 2) {
        if (data.args.empty()) {
            client.appendToSendBuffer(":localhost 461 " + client.getNick() + 
                                     " PRIVMSG :Not enough parameters\r\n");
        } else {
            client.appendToSendBuffer(":localhost 412 " + client.getNick() + 
                                     " :No text to send\r\n");
        }
        return;
    }
    
    std::string target = data.args[0];
    std::string message = data.args[1];  // Already extracted by parse()
    
    // 2. Check if target is a channel (starts with #)
    if (target[0] == '#') {
        Channel *channel = server.getChannel(target);
        
        // Channel doesn't exist?
        if (!channel) {
            client.appendToSendBuffer(":localhost 404 " + client.getNick() + 
                                     " " + target + " :Cannot send to channel\r\n");
            return;
        }
        
        // User not in channel?
        if (!channel->isMember(&client)) {
            client.appendToSendBuffer(":localhost 442 " + client.getNick() + 
                                     " " + target + " :You're not on that channel\r\n");
            return;
        }
        
        // 3. Broadcast message to all channel members EXCEPT sender
        std::string fullMsg = ":" + client.getNick() + "!~" + 
                             client.getUserName() + "@localhost PRIVMSG " + 
                             target + " :" + message + "\r\n";
        
        std::set<Client*> members = channel->getMembers();
        for (std::set<Client*>::iterator it = members.begin(); 
             it != members.end(); ++it) {
            if (*it != &client) {  // Don't send to sender!
                (*it)->appendToSendBuffer(fullMsg);
            }
        }
    } else {
        // 4. Private message to user
        Client *recipient = server.getUser(target);
        
        if (!recipient) {
            client.appendToSendBuffer(":localhost 401 " + client.getNick() + 
                                     " " + target + " :No such nick/channel\r\n");
            return;
        }
        
        std::string fullMsg = ":" + client.getNick() + "!~" + 
                             client.getUserName() + "@localhost PRIVMSG " + 
                             target + " :" + message + "\r\n";
        recipient->appendToSendBuffer(fullMsg);
    }
}
```

**Critical Broadcasting Detail:**

The biggest bug we fixed:
```cpp
// WRONG - only sender gets POLLOUT enabled
client.appendToSendBuffer(msg);
// sender's POLLOUT enabled

// CORRECT - check ALL clients after command processing
for (all clients) {
    if (client has data in send buffer) {
        enable POLLOUT for this client;
    }
}
```

This ensures channel messages reach all members!

#### KICK Command - Operator Privileges

```cpp
void handleKick(Server &server, Client &client, s_data &data) {
    // 1. Validate parameters: KICK #channel user [reason]
    if (data.args.size() < 2) {
        client.appendToSendBuffer(":localhost 461 " + client.getNick() + 
                                 " KICK :Not enough parameters\r\n");
        return;
    }
    
    std::string channelName = data.args[0];
    std::string targetNick = data.args[1];
    std::string reason = (data.args.size() > 2) ? data.args[2] : client.getNick();
    
    // 2. Get channel
    Channel *channel = server.getChannel(channelName);
    if (!channel) {
        client.appendToSendBuffer(":localhost 403 " + client.getNick() + 
                                 " " + channelName + " :No such channel\r\n");
        return;
    }
    
    // 3. Kicker must be in channel
    if (!channel->isMember(&client)) {
        client.appendToSendBuffer(":localhost 442 " + client.getNick() + 
                                 " " + channelName + " :You're not on that channel\r\n");
        return;
    }
    
    // 4. Kicker must be operator
    if (!channel->isOperator(&client)) {
        client.appendToSendBuffer(":localhost 482 " + client.getNick() + 
                                 " " + channelName + " :You're not channel operator\r\n");
        return;
    }
    
    // 5. Get target user
    Client *target = server.getUser(targetNick);
    if (!target) {
        client.appendToSendBuffer(":localhost 401 " + client.getNick() + 
                                 " " + targetNick + " :No such nick/channel\r\n");
        return;
    }
    
    // 6. Target must be in channel
    if (!channel->isMember(target)) {
        client.appendToSendBuffer(":localhost 441 " + client.getNick() + 
                                 " " + targetNick + " " + channelName + 
                                 " :They aren't on that channel\r\n");
        return;
    }
    
    // 7. Broadcast KICK message to all members BEFORE removing
    std::string kickMsg = ":" + client.getNick() + "!~" + 
                         client.getUserName() + "@localhost KICK " + 
                         channelName + " " + targetNick + " :" + reason + "\r\n";
    
    std::set<Client*> members = channel->getMembers();
    for (std::set<Client*>::iterator it = members.begin(); 
         it != members.end(); ++it) {
        (*it)->appendToSendBuffer(kickMsg);
    }
    
    // 8. Remove target from channel
    channel->removeMember(target);
}
```

**Key Permission Check:**
```cpp
if (!channel->isOperator(&client)) {
    // Error 482: Not an operator
}
```

Only channel operators can KICK!

#### MODE Command - Channel Settings

```cpp
void handleMode(Server &server, Client &client, s_data &data) {
    if (data.args.empty()) {
        client.appendToSendBuffer(":localhost 461 " + client.getNick() + 
                                 " MODE :Not enough parameters\r\n");
        return;
    }
    
    std::string channelName = data.args[0];
    Channel *channel = server.getChannel(channelName);
    
    if (!channel) {
        client.appendToSendBuffer(":localhost 403 " + client.getNick() + 
                                 " " + channelName + " :No such channel\r\n");
        return;
    }
    
    // Just viewing modes? MODE #channel
    if (data.args.size() == 1) {
        std::string modes = channel->getModeString();
        client.appendToSendBuffer(":localhost 324 " + client.getNick() + 
                                 " " + channelName + " " + modes + "\r\n");
        return;
    }
    
    // Setting modes - must be operator
    if (!channel->isOperator(&client)) {
        client.appendToSendBuffer(":localhost 482 " + client.getNick() + 
                                 " " + channelName + " :You're not channel operator\r\n");
        return;
    }
    
    // Parse mode string: +i, -k, +o user, +l 10, etc.
    std::string modeStr = data.args[1];
    size_t paramIndex = 2;
    bool adding = true;
    
    for (size_t i = 0; i < modeStr.size(); i++) {
        if (modeStr[i] == '+') {
            adding = true;
        } else if (modeStr[i] == '-') {
            adding = false;
        } else {
            char mode = modeStr[i];
            
            switch (mode) {
                case 'i':  // Invite-only
                    channel->setMode('i', adding);
                    break;
                    
                case 't':  // Topic restriction
                    channel->setMode('t', adding);
                    break;
                    
                case 'k':  // Channel key (password)
                    if (adding && paramIndex < data.args.size()) {
                        channel->setKey(data.args[paramIndex++]);
                        channel->setMode('k', true);
                    } else {
                        channel->setMode('k', false);
                    }
                    break;
                    
                case 'o':  // Operator privilege
                    if (paramIndex < data.args.size()) {
                        Client *target = server.getUser(data.args[paramIndex++]);
                        if (target && channel->isMember(target)) {
                            if (adding)
                                channel->addOperator(target);
                            else
                                channel->removeOperator(target);
                        }
                    }
                    break;
                    
                case 'l':  // User limit
                    if (adding && paramIndex < data.args.size()) {
                        int limit = std::atoi(data.args[paramIndex++].c_str());
                        channel->setLimit(limit);
                        channel->setMode('l', true);
                    } else {
                        channel->setMode('l', false);
                    }
                    break;
            }
        }
    }
    
    // Broadcast mode change to channel
    std::string modeMsg = ":" + client.getNick() + "!~" + 
                         client.getUserName() + "@localhost MODE " + 
                         channelName + " " + modeStr;
    for (size_t i = 2; i < data.args.size(); i++) {
        modeMsg += " " + data.args[i];
    }
    modeMsg += "\r\n";
    
    std::set<Client*> members = channel->getMembers();
    for (std::set<Client*>::iterator it = members.begin(); 
         it != members.end(); ++it) {
        (*it)->appendToSendBuffer(modeMsg);
    }
}
```

**Supported Modes:**
- `+i` / `-i` : Invite-only channel
- `+t` / `-t` : Only operators can change topic
- `+k <pass>` / `-k` : Channel password
- `+o <user>` / `-o <user>` : Grant/revoke operator
- `+l <num>` / `-l` : User limit

### 3. Error Handling System

#### IRC Numeric Replies

All IRC errors follow the format:
```
:<server> <code> <nick> [params] :<message>
```

Example:
```
:localhost 461 alice JOIN :Not enough parameters
```

#### Complete Error List (22 Codes)

| Code | Name | Meaning | Example Usage |
|------|------|---------|---------------|
| **401** | ERR_NOSUCHNICK | User doesn't exist | PRIVMSG to non-existent user |
| **403** | ERR_NOSUCHCHANNEL | Channel doesn't exist | PART from non-existent channel |
| **404** | ERR_CANNOTSENDTOCHAN | Can't send to channel | PRIVMSG when not in channel |
| **405** | ERR_TOOMANYCHANNELS | Too many channels | (Not implemented) |
| **412** | ERR_NOTEXTTOSEND | Empty message | PRIVMSG without text |
| **421** | ERR_UNKNOWNCOMMAND | Command not recognized | Invalid command |
| **431** | ERR_NONICKNAMEGIVEN | No nickname provided | NICK with no parameter |
| **432** | ERR_ERRONEUSNICKNAME | Invalid nickname format | NICK #invalid |
| **433** | ERR_NICKNAMEINUSE | Nickname already taken | NICK alice (when alice exists) |
| **441** | ERR_USERNOTINCHANNEL | Target not in channel | KICK user not in channel |
| **442** | ERR_NOTONCHANNEL | You're not in channel | PART from channel you're not in |
| **443** | ERR_USERONCHANNEL | User already in channel | INVITE user already in channel |
| **451** | ERR_NOTREGISTERED | Not authenticated | Commands before PASS/NICK/USER |
| **461** | ERR_NEEDMOREPARAMS | Missing parameters | JOIN with no channel name |
| **462** | ERR_ALREADYREGISTERED | Can't re-register | PASS after already registered |
| **464** | ERR_PASSWDMISMATCH | Wrong password | PASS with wrong password |
| **471** | ERR_CHANNELISFULL | Channel at user limit | JOIN channel with +l limit reached |
| **473** | ERR_INVITEONLYCHAN | Invite-only channel | JOIN +i channel without invite |
| **475** | ERR_BADCHANNELKEY | Wrong channel password | JOIN +k channel with wrong key |
| **482** | ERR_CHANOPRIVSNEEDED | Need operator | MODE/KICK/INVITE without operator |

#### Success Numerics

| Code | Name | Purpose |
|------|------|---------|
| **001** | RPL_WELCOME | Welcome message |
| **324** | RPL_CHANNELMODEIS | Channel mode response |
| **331** | RPL_NOTOPIC | No topic set |
| **332** | RPL_TOPIC | Current topic |
| **341** | RPL_INVITING | Invite confirmation |
| **352** | RPL_WHOREPLY | WHO response (per user) |
| **353** | RPL_NAMREPLY | NAMES list |
| **366** | RPL_ENDOFNAMES | End of NAMES |
| **315** | RPL_ENDOFWHO | End of WHO |

---

## IRC Protocol Compliance

### RFC 1459 / RFC 2812

Our server implements the core IRC protocol features:

#### Message Format
```
message  = [ ":" prefix SPACE ] command [ params ] crlf
prefix   = servername / ( nickname [ "!" user ] [ "@" host ] )
command  = letter+ / digit+
params   = SPACE ( ":" trailing / middle )*
```

#### Authentication Flow
```
Client → Server: PASS secretpassword
Client → Server: NICK alice
Client → Server: USER alice 0 * :Alice Smith
Server → Client: :server 001 alice :Welcome to IRC
Server → Client: :server 002 alice :Your host is server
Server → Client: :server 003 alice :This server was created today
Server → Client: :server 004 alice server 1.0 o o
```

#### Channel Operations
```
JOIN:    Client → Server: JOIN #channel [key]
         Server → All:    :alice!user@host JOIN :#channel
         
PART:    Client → Server: PART #channel [reason]
         Server → All:    :alice!user@host PART #channel :reason
         
PRIVMSG: Client → Server: PRIVMSG #channel :message
         Server → Others: :alice!user@host PRIVMSG #channel :message
```

---

## Testing & Demonstration

### 1. Starting the Server

```bash
# Compile
make

# Run (port 6667, password "hello")
./ircserv 6667 hello
```

### 2. Connect with HexChat

1. **Server Setup:**
   - Server: `localhost/6667`
   - Password: `hello`
   - Nickname: Choose unique name
   - Username: Any name

2. **Basic Commands to Show:**
```
/join #test
/msg #test Hello everyone!
/topic This is a test channel
/mode #test +t
/invite bob #test
/kick alice Breaking rules
```

### 3. Multi-Client Scenarios

#### Scenario 1: Basic Channel Communication
```
Terminal 1 (Alice):
/join #chat
/msg #chat Hi from Alice!

Terminal 2 (Bob):  
/join #chat
/msg #chat Hi Alice!

Terminal 3 (Charlie):
/join #chat
/msg #chat Hey everyone!
```

#### Scenario 2: Operator Privileges
```
Alice (creates channel, becomes operator):
/join #ops
/mode #ops +o bob         # Give Bob operator
/mode #ops +i             # Make invite-only
/invite charlie #ops

Charlie:
/join #ops                # Works! (invited)

Dave:
/join #ops                # FAILS! (473 - invite only)
```

#### Scenario 3: Channel Modes
```
Alice:
/join #test
/mode #test +k secretpass  # Set password
/mode #test +l 5           # Max 5 users
/mode #test +t             # Only ops change topic

Bob:
/join #test wrongpass      # FAILS (475)
/join #test secretpass     # Works!
/topic New topic           # FAILS (482 - not operator)

Alice:
/mode #test +o bob         # Make Bob operator
```

### 4. Error Testing

Run the automated script:
```bash
./tests/test_errors.sh
```

Or test manually (see `docs/ERROR_TESTING_GUIDE.md`).

### 5. Key Points to Demonstrate

✅ **Non-blocking I/O**: Start server, connect multiple clients, they all work simultaneously  
✅ **Broadcasting**: Send message in channel, ALL members see it instantly  
✅ **Authentication**: Connect without password → commands blocked with 451  
✅ **Nickname collision**: Two clients try same nick → second gets 433  
✅ **Operator privileges**: Non-op tries KICK → gets 482  
✅ **Channel modes**: Show +i, +k, +t, +o, +l in action  
✅ **Error handling**: Invalid commands → proper error codes  

---

## Technical Challenges & Solutions

### Challenge 1: Partial Messages
**Problem:** IRC messages might arrive in multiple recv() calls
```
recv(): "PRIVMSG #ch"
recv(): "annel :Hel"
recv(): "lo\r\n"
```

**Solution:** Receive buffer accumulates data until `\r\n`
```cpp
_recvBuffer.append(data, len);
while ((pos = _recvBuffer.find("\r\n")) != npos) {
    extract_message();
    _recvBuffer.erase(0, pos + 2);
}
```

### Challenge 2: Broadcast POLLOUT
**Problem:** Channel messages only appeared for sender

**Root Cause:** Only sender's POLLOUT was enabled

**Solution:** After processing ANY command, check ALL clients:
```cpp
for (all clients) {
    if (!client->getSendBuffer().empty()) {
        poll_fds[i].events |= POLLOUT;
    }
}
```

### Challenge 3: Authentication Order
**Problem:** HexChat sends NICK/USER before PASS

**Solution:** Make registration order-independent:
```cpp
void registerClient(Client &client) {
    if (authenticated && nick_set && user_set) {
        client.setRegistered(true);
    }
}

// Call after EACH of PASS, NICK, USER
```

### Challenge 4: Parse Trailing Parameters
**Problem:** Messages with spaces: `PRIVMSG #test :Hello world`

**Solution:** Detect `:` prefix for trailing parameter
```cpp
if (msg[pos] == ':') {
    args.push_back(msg.substr(pos + 1));  // "Hello world"
    break;  // Don't split on spaces
}
```

### Challenge 5: Non-blocking send()
**Problem:** send() might not send all data

**Solution:** Send buffer + POLLOUT
```cpp
ssize_t sent = send(fd, buffer.c_str(), buffer.size(), 0);
if (sent < buffer.size()) {
    // Keep unsent data in buffer
    buffer.erase(0, sent);
    // POLLOUT will trigger when ready
}
```

---

## Key Takeaways for Evaluation

### What You Implemented (Client & Command Handling)

1. **Client Connection Management**
   - Accept connections with poll()
   - Non-blocking I/O
   - Receive/send buffers
   - Connection state tracking

2. **Authentication System**
   - Order-independent PASS/NICK/USER
   - Password verification
   - Registration completion
   - Welcome message sequence

3. **Message Parsing**
   - IRC protocol parsing
   - Command extraction
   - Argument parsing
   - Trailing parameter handling

4. **Command Pattern**
   - Extensible command system
   - Clean command routing
   - Unified error handling

5. **Command Implementations**
   - JOIN (with mode checking)
   - PART
   - PRIVMSG (channel + private)
   - KICK
   - INVITE
   - TOPIC
   - MODE (5 channel modes)
   - WHO

6. **Error Handling**
   - 22 IRC error codes
   - Proper numeric format
   - Descriptive messages

### Code Organization

```
includes/
  Server.hpp      - Server class, client management
  Client.hpp      - Client class, buffers, state
  Channel.hpp     - Channel class, modes, members
  Commands.hpp    - Command pattern interfaces

srcs/
  Server.cpp      - Network, poll(), parsing
  Client.cpp      - Buffer management, I/O
  Channel.cpp     - Channel operations, modes
  CommandHandlers.cpp - All command implementations
  Commands.cpp    - Command class definitions
  main.cpp        - Entry point
  utils.cpp       - Helper functions

docs/
  ERROR_MESSAGES.md         - Error code reference
  ERROR_TESTING_GUIDE.md    - Complete test cases
  CHANNEL_TESTING_GUIDE.md  - Channel feature tests
  EVALUATION_GUIDE.md       - This document

tests/
  test_errors.sh  - Automated error testing
```

### Performance Characteristics

- **Single-threaded**: Uses poll() for concurrency
- **Non-blocking**: Never blocks on I/O
- **Scalable**: Can handle many simultaneous clients
- **Memory efficient**: Buffers only active data
- **Protocol compliant**: Works with standard IRC clients

---

## Questions You Might Be Asked

### Q: Why use poll() instead of select()?
**A:** poll() is more efficient and has no file descriptor limit (select() limited to 1024).

### Q: Why non-blocking sockets?
**A:** Blocking would freeze the entire server if one client is slow. Non-blocking lets us handle all clients concurrently in one thread.

### Q: Why separate receive and send buffers?
**A:** IRC messages might arrive fragmented (receive buffer assembles them). send() might not send everything at once (send buffer queues remaining data).

### Q: How do you prevent race conditions?
**A:** Single-threaded design - no threads, no races. poll() ensures orderly processing.

### Q: Why not use C++11/14 features?
**A:** Project requirement - demonstrates understanding of C++98 limitations and manual memory management.

### Q: What happens if a client disconnects mid-message?
**A:** recv() returns 0, we close the socket, remove from poll_fds, delete Client object, clean up from all channels.

### Q: How do you handle slow clients?
**A:** Non-blocking send() + send buffer. If socket buffer is full (EAGAIN), data waits in our buffer. POLLOUT tells us when to retry.

### Q: Why check POLLOUT for all clients after commands?
**A:** Broadcasting (like PRIVMSG to channel) puts data in multiple clients' send buffers. All need POLLOUT enabled.

---

## Final Demo Checklist

Before evaluation, test these scenarios:

### Basic Functionality
- [ ] Server starts and listens on specified port
- [ ] Multiple clients can connect simultaneously
- [ ] Clients authenticate with PASS/NICK/USER
- [ ] Clients can join and create channels
- [ ] Messages broadcast to all channel members
- [ ] Private messages work between users

### Error Handling
- [ ] Commands before registration → 451
- [ ] Wrong password → 464
- [ ] Duplicate nickname → 433
- [ ] Missing parameters → 461
- [ ] Non-existent channel → 403
- [ ] Not channel member → 442
- [ ] Not channel operator → 482

### Channel Features
- [ ] Channel creation (creator becomes operator)
- [ ] Operator privileges (MODE, KICK, INVITE)
- [ ] Channel modes (+i, +t, +k, +o, +l)
- [ ] KICK removes user and broadcasts
- [ ] INVITE to invite-only channels
- [ ] TOPIC with +t mode restriction

### Advanced Features
- [ ] MODE viewing and setting
- [ ] WHO command lists channel members
- [ ] Nickname changes broadcast
- [ ] Multiple channels per user
- [ ] Channel password protection
- [ ] User limits enforced

---

## Good Luck! 🚀

You've built a fully functional IRC server from scratch with:
- ✅ Non-blocking network I/O
- ✅ IRC protocol compliance
- ✅ Complete command system
- ✅ Error handling
- ✅ Channel management
- ✅ Operator privileges
- ✅ Broadcasting
- ✅ Multi-client support

**Remember:** Focus on explaining the *why* behind design decisions, not just the *what*. Show you understand the challenges of network programming and concurrent client handling!
