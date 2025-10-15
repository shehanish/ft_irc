# Quick HexChat Connection Reference

## TL;DR - Connect to Your Server

1. **Start server:**
   ```bash
   ./ircserv 6667 yourpassword
   ```

2. **In HexChat:**
   - Add new network: `localhost/6667`
   - Server password: `yourpassword`
   - Set nickname and username
   - Connect!

---

## What Was Fixed for HexChat

### ✅ Changes Made:

1. **CAP command buffering** - Changed from direct `send()` to `appendToSendBuffer()`
2. **CAP END triggers registration** - After CAP END, server attempts to complete registration
3. **CAP REQ support** - Server now responds with NAK (not acknowledged) for unsupported capabilities
4. **Registration flow** - Works with HexChat's sequence: CAP LS → PASS → NICK → USER → CAP END

### Why It Works Now:

**HexChat's connection sequence:**
```
1. CAP LS 302          → Server replies: CAP * LS :
2. PASS yourpassword   → Server authenticates
3. NICK yournick       → Server sets nickname
4. USER username ...   → Server sets username
5. CAP END             → Server completes registration, sends 001-004 welcome
```

**Your server now:**
- ✅ Handles CAP negotiation properly
- ✅ Uses consistent buffering (appendToSendBuffer)
- ✅ Completes registration after CAP END
- ✅ Sends proper IRC numeric replies

---

## Test Commands

After connecting:

```irc
/join #test              → Join channel
/msg #test Hello!        → Send message to channel
/msg othernick Hi!       → Private message
/part #test              → Leave channel
/topic #test New topic   → Set channel topic
/nick newnick            → Change nickname
```

---

## Expected Server Console Output

```
Server starting... 🟢
Server ready and listening on port 6667
New connection accepted!
Handled CAP LS for client 4
Client is Authenticated
Nickname set to yournick
User set: username (Real Name)
Client 4 ended CAP negotiation.
Client registered: yournick!username
```

---

## Expected HexChat Output

```
* Connected to localhost (127.0.0.1:6667)
* Welcome to the IRC server yournick
* Your host is ircserv, running version 1.0
* This server was created today
* ircserv 1.0 o o
```

---

## Common Issues

| Issue | Solution |
|-------|----------|
| Password incorrect | Check server password matches HexChat's "Server Password" field |
| Connection refused | Verify server is running: `ps aux \| grep ircserv` |
| You have not registered | Wait for welcome messages (001-004) before sending commands |
| Nickname in use | Try different nickname or restart server |

---

## Files Modified

- `/home/shkaruna/42project/ft_irc/srcs/Server.cpp`
  - `handleCap()` - Now uses buffer, handles CAP REQ, triggers registration on CAP END
  - `handleNick()` - Added comment about PASS ordering

---

## Compatibility

✅ **HexChat** - Full support with CAP negotiation
✅ **irssi, WeeChat** - Full support
✅ **mIRC** - Should work (uses similar flow)
✅ **Modern IRC clients** - Any client supporting IRCv3 CAP
✅ **netcat/telnet** - Manual testing still works

---

For detailed information, see: `docs/HEXCHAT_TESTING.md`
