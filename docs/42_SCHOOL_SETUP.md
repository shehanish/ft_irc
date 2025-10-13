# IRC Server - 42 School HexChat Setup Guide

## Problem at 42 School

At 42 school, HexChat is pre-configured with your **intra username** (e.g., `shkaruna`). When you connect to your IRC server, HexChat automatically:
1. Sends your intra username as NICK
2. Sends your intra username as USER
3. Auto-registers you immediately

This means you get **locked into your intra username** and can't change it... until now! ✅

---

## Solution Implemented

Your server now supports **nickname changes AFTER registration**! 

### What Was Fixed:

1. **`isNickTaken()` improved** - Now excludes yourself when checking if a nickname is taken
2. **`handleNick()` enhanced** - Allows nickname changes for registered clients
3. **NICK notifications** - Sends proper IRC protocol message when you change your nickname

---

## How to Use at 42 School

### 1. Start Your Server

```bash
cd ~/42project/ft_irc
make
./ircserv 6667 yourpassword
```

### 2. Connect with HexChat

The school's HexChat will automatically connect you with your intra name. You'll see:

```
* Connected to localhost
* Welcome to the IRC server shkaruna
* Your host is ircserv, running version 1.0
```

**Important:** You are now connected and registered with your intra username!

### 3. Change Your Nickname

After you're connected, simply type:

```
/nick yournewnickname
```

For example:
```
/nick sheha
/nick test123
/nick coolname
```

You'll see:
```
* shkaruna is now known as yournewnickname
```

✅ **Your nickname is now changed!**

---

## How It Works

### Before the Fix ❌

```
Client connects with intra name: shkaruna
Server registers: shkaruna
Client tries: /nick newname
Server checks: "shkaruna is taken!" (checking against yourself)
Result: 433 Nickname is already in use ❌
```

### After the Fix ✅

```
Client connects with intra name: shkaruna
Server registers: shkaruna
Client tries: /nick newname
Server checks: "newname is taken?" (excludes yourself from check)
Result: Nickname changed! ✅
Server sends: :shkaruna!~shkaruna@localhost NICK :newname
```

---

## Testing Multiple Clients

Since you're at 42 school, you can test with multiple computers or use the same computer:

### Option 1: Two Different Computers

**Computer 1:**
```bash
./ircserv 6667 testpass
```

**Computer 2:**
Open HexChat, connect to `computer1-hostname:6667`

### Option 2: Same Computer (Two HexChat Windows)

1. **Terminal:** Start server
   ```bash
   ./ircserv 6667 testpass
   ```

2. **First HexChat:** Connect normally (gets your intra name)
   ```
   /nick alice
   /join #test
   ```

3. **Second HexChat Instance:** Open another HexChat
   ```bash
   hexchat &
   ```
   Connect to `localhost:6667`, then:
   ```
   /nick bob
   /join #test
   ```

Now `alice` and `bob` can chat in `#test`!

---

## Important Notes

### ✅ You CAN Change:
- **Nickname** - Anytime after registration using `/nick newnick`

### ❌ You CANNOT Change:
- **Username** - Set once during registration (your intra name)
- **Realname** - Set once during registration

This is standard IRC behavior! Your **username** (shown in `nick!username@host`) stays the same, but your visible **nickname** can change.

### Example:

```
Initial:  shkaruna!~shkaruna@localhost
After:    coolname!~shkaruna@localhost
          ^^^^^^^^    ^^^^^^^^
          nickname    username (stays same)
```

---

## Common Commands After Connecting

### Change Your Nickname
```
/nick newnickname
```

### Join a Channel
```
/join #test
/join #42students
/join #anything
```

### Send Messages
```
/msg #test Hello everyone!
/msg othernick Private message
```

### Set Channel Topic
```
/topic #test Welcome to our channel!
```

### Leave a Channel
```
/part #test
/part #test Goodbye everyone!
```

### Invite Someone
```
/invite othernick #test
```

---

## Troubleshooting

### "Nickname is already in use" (433)

**Cause:** Someone else is already using that nickname.

**Solution:** 
- Try a different nickname: `/nick othername`
- Or disconnect the other client first

### "You may not reregister" (462)

**Cause:** You tried to change username with `/quote USER` after already being registered.

**Solution:** 
- Username cannot be changed after registration
- Use `/nick` to change nickname instead

### Can't change nickname at all

**Cause:** You might be using an old version of the code.

**Solution:**
- Make sure you've compiled the latest version:
  ```bash
  make fclean
  make
  ```

---

## Server-Side View

When a client changes their nickname, you'll see in the server console:

```
Server ready and listening on port 6667
New connection accepted!
Handled CAP LS for client 4
Client is Authenticated
Nickname set to shkaruna
User set: shkaruna (shkaruna)
Client 4 ended CAP negotiation.
Client registered: shkaruna!shkaruna
Nickname changed: shkaruna → coolname      ← This is the change!
```

---

## Technical Details

### IRC Protocol Messages

When you change your nickname, the server sends:

```
:oldnick!~username@localhost NICK :newnick
```

This tells HexChat (and any channels you're in) that you've changed your nickname.

### Code Changes

**In `Server.hpp`:**
```cpp
// Added optional parameter to exclude a client from the check
bool isNickTaken(const std::string &nickname, const Client *excludeClient = NULL) const;
```

**In `Server.cpp` - `handleNick()`:**
- Saves old nickname before changing
- Checks if new nickname is taken (excluding yourself)
- If registered, sends NICK change notification
- Updates nickname
- Allows registration for new clients

---

## Testing Checklist

- [ ] Start server successfully
- [ ] Connect with HexChat (auto-registered with intra name)
- [ ] Change nickname with `/nick newname`
- [ ] See confirmation: "oldnick is now known as newname"
- [ ] Join a channel
- [ ] Send messages with new nickname
- [ ] Have another person connect and verify they see your new nickname

---

## Why This Matters at 42

At 42 school:
- Everyone has the same username format (intra login)
- You can't easily distinguish between people in IRC
- **With nickname changes**, you can:
  - Use a nickname you prefer
  - Make yourself easily recognizable
  - Have fun with creative nicknames!

---

## Next Steps

1. **Test the server** with your partner/team
2. **Create channels** for your project discussions
3. **Use different nicknames** to make communication clearer
4. **Enjoy your IRC server!** 🎉

---

## Example Session at 42

```
[Terminal 1 - Server]
$ ./ircserv 6667 secret42
Server starting... 🟢
Server ready and listening on port 6667

[HexChat - You]
* Connected to localhost
* Welcome to the IRC server shkaruna
/nick sheha
* shkaruna is now known as sheha
/join #ft_irc_team
* Now talking in #ft_irc_team

[HexChat - Teammate]
* Connected to localhost  
* Welcome to the IRC server lde-taey
/nick lisa
* lde-taey is now known as lisa
/join #ft_irc_team
* Now talking in #ft_irc_team

[In #ft_irc_team]
<sheha> Hi Lisa! Ready to test our IRC server?
<lisa> Yes! This is awesome!
```

---

**Happy chatting at 42! 🚀**
