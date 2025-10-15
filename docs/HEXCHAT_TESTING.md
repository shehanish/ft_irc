# HexChat Testing Guide for ft_irc

## Overview

Your IRC server is now **compatible with HexChat**! The PASS/NICK/USER flow and CAP negotiation have been properly implemented.

---

## HexChat Connection Flow

HexChat uses the following sequence when connecting:

1. **CAP LS 302** - Request capability list (IRC v3 extension)
2. **PASS <password>** - Authenticate with server password
3. **NICK <nickname>** - Set nickname
4. **USER <username> 0 * :<realname>** - Set user information
5. **CAP END** - End capability negotiation

Your server handles all of these correctly! ✅

---

## Setup Instructions

### 1. Start Your IRC Server

```bash
cd /home/shkaruna/42project/ft_irc
make
./ircserv 6667 mypassword
```

Replace `mypassword` with your chosen server password.

---

### 2. Configure HexChat

#### Method 1: Using HexChat Network List

1. **Open HexChat**
2. **Go to**: HexChat → Network List (or press Ctrl+S)
3. **Click**: "Add" to create a new network
4. **Name it**: "ft_irc" (or any name you prefer)
5. **Click**: "Edit..."

#### Configure Server Details:

**In the "Edit" dialog:**

- **Click**: "Add" under servers
- **Enter**: `localhost/6667` (or `127.0.0.1/6667`)
- **Server Password**: Enter your server password (e.g., `mypassword`)

**User Information:**

- **Nick name**: Your desired nickname (e.g., `testuser`)
- **Second choice**: An alternative nickname
- **User name**: Your username (e.g., `testuser`)
- **Real name**: Your real/display name (e.g., `Test User`)

**Important Settings:**

- ✅ **Check**: "Use SSL for all servers on this network" - **UNCHECK THIS** (your server doesn't support SSL)
- ✅ **Check**: "Accept invalid SSL certificates" - **UNCHECK THIS**
- ✅ **Server Password**: Make sure this matches your server's password

6. **Click**: "Close"
7. **Click**: "Connect"

---

#### Method 2: Quick Connect

1. **Open HexChat**
2. **Go to**: HexChat → Network List
3. **Click**: "New Server Tab"
4. **In the server tab, type**:
   ```
   /server localhost 6667 mypassword
   /nick yournickname
   /user yourusername 0 * :Your Real Name
   ```

---

### 3. Expected Behavior

When you connect, you should see:

```
* Connected to localhost (127.0.0.1:6667)
* Welcome to the IRC server yournickname
* Your host is ircserv, running version 1.0
* This server was created today
* ircserv 1.0 o o
```

---

## Testing Commands

Once connected, try these IRC commands:

### Join a Channel
```
/join #test
```

### Send a Message
```
/msg #test Hello, world!
```

### Private Message
```
/msg othernick Hey there!
```

### Leave a Channel
```
/part #test
```

### Set Topic
```
/topic #test This is the channel topic
```

### Change Nickname
```
/nick newnickname
```

---

## Connection Sequence (Technical Details)

When HexChat connects, your server processes:

```
Client → Server: CAP LS 302
Server → Client: :localhost CAP * LS :

Client → Server: PASS mypassword
Server: (authenticates client internally)

Client → Server: NICK testuser
Server: (sets nickname)

Client → Server: USER testuser 0 * :Test User
Server: (sets username and completes registration)

Client → Server: CAP END
Server → Client: Registration complete, sends welcome messages (001-004)
```

---

## Troubleshooting

### Problem: "Password incorrect" error

**Solution:** Make sure the password in HexChat matches the server password exactly.

```bash
# Check what password you started the server with:
./ircserv 6667 mypassword
#                ^^^^^^^^^^
#                This must match HexChat's "Server Password"
```

### Problem: Connection refused

**Solution:** 
1. Check if server is running: `ps aux | grep ircserv`
2. Check if port is in use: `netstat -tuln | grep 6667`
3. Make sure you're using `localhost` or `127.0.0.1`

### Problem: "You have not registered" error when trying commands

**Solution:** Wait for the welcome messages (001-004) before sending commands. Registration happens automatically after PASS + NICK + USER + CAP END.

### Problem: Nickname already in use

**Solution:** 
1. The server remembers connected clients
2. Try a different nickname, or
3. Restart the server to clear all clients

### Problem: No response from server

**Solution:**
1. Check server console for errors
2. Verify the server is listening: `netstat -tuln | grep 6667`
3. Check firewall settings (though localhost usually bypasses firewalls)

---

## Debugging Tips

### Server Side (Terminal running ./ircserv)

Watch the server console output:
```
Server starting... 🟢
Server ready and listening on port 6667
New connection accepted!
Client is Authenticated
Nickname set to testuser
User set: testuser (Test User)
Client registered: testuser!testuser
```

### Client Side (HexChat)

Enable raw server messages in HexChat:
1. **Go to**: Settings → Preferences → Advanced → Text Events
2. **Find**: "Raw Modes" and enable it
3. Or simply watch the server tab for connection messages

---

## Compatible IRC Clients

The following IRC clients should work with your server:

- ✅ **HexChat** (tested)
- ✅ **irssi** (command-line)
- ✅ **WeeChat** (command-line)
- ✅ **mIRC** (Windows)
- ✅ **Textual** (macOS)
- ✅ **Pidgin** (with IRC plugin)
- ✅ **netcat** (`nc localhost 6667`)

All modern IRC clients that support IRCv3 CAP negotiation will work!

---

## Server Features Supported

✅ Client registration (PASS/NICK/USER)
✅ CAP negotiation (LS, END, REQ)
✅ JOIN channels
✅ PART channels
✅ PRIVMSG (channel and private messages)
✅ TOPIC (view and set)
✅ KICK users
✅ INVITE users
✅ MODE (partially implemented)
✅ Proper IRC numeric replies
✅ Multiple simultaneous clients
✅ Non-blocking I/O with poll()

---

## Next Steps

After successful connection:

1. **Test with multiple HexChat instances** - Open HexChat twice with different nicknames
2. **Create channels** - `/join #mychannel`
3. **Test private messages** - Between your two clients
4. **Test channel operations** - TOPIC, KICK, INVITE
5. **Test error handling** - Try wrong passwords, duplicate nicks, etc.

---

## Notes

- Your server uses **localhost** (127.0.0.1) by default
- Port **6667** is the standard IRC port
- No SSL/TLS support currently (plaintext only)
- Server password is **required** (sent via PASS command)
- CAP negotiation is supported but no IRCv3 capabilities are implemented yet

Happy testing! 🎉
