# IRC Server Error Messages Testing Guide

This guide provides test cases for all error messages implemented in the IRC server.

## Setup
1. Start server: `./ircserv 6667 apple`
2. Connect 3 HexChat clients (sheha, Lotte, Saby)
3. All clients should authenticate and join #test

---

## Authentication Error Tests

### Test ERR_PASSWDMISMATCH (464)
**What to test:** Wrong password
```
1. Connect with HexChat
2. In server password field, enter: wrongpassword
Expected: :localhost 464 * :Password incorrect
```

### Test ERR_NOTREGISTERED (451)
**What to test:** Command before registration
```
1. Connect with netcat: nc localhost 6667
2. Type: JOIN #test
Expected: :localhost 451 * :You have not registered
```

### Test ERR_ALREADYREGISTERED (462)
**What to test:** Try to register twice
```
1. After connecting and registering
2. Type: /quote PASS apple
Expected: :localhost 462 sheha :You may not reregister
```

---

## NICK Command Error Tests

### Test ERR_NONICKNAMEGIVEN (431)
**What to test:** NICK without parameter
```
/quote NICK
Expected: :localhost 431 :No nickname given
```

### Test ERR_ERRONEUSNICKNAME (432)
**What to test:** Invalid nickname format
```
/nick #invalid
/nick @bad
/nick with spaces
Expected: :localhost 432 sheha <badnick> :Erroneous nickname
```

### Test ERR_NICKNAMEINUSE (433)
**What to test:** Nickname already taken
```
Client 1: (already using nick "sheha")
Client 2: /nick sheha
Expected: :localhost 433 Lotte sheha :Nickname is already in use
```

---

## JOIN Command Error Tests

### Test ERR_NEEDMOREPARAMS (461) - JOIN
**What to test:** JOIN without channel name
```
/quote JOIN
Expected: :localhost 461 sheha JOIN :Not enough parameters
```

### Test ERR_TOOMANYCHANNELS (405)
**What to test:** Exceeded max channels
```
/join #channel1
/join #channel2
/join #channel3
... (continue until limit - default is 10)
/join #channel11
Expected: :localhost 405 sheha #channel11 :You have joined too many channels
```

### Test ERR_BADCHANNELKEY (475)
**What to test:** Wrong channel password
```
Client 1: /join #private
Client 1: /mode #private +k secret123
Client 2: /join #private
Expected: :localhost 475 Lotte #private :Cannot join channel (+k) - bad key

Client 2: /join #private wrongkey
Expected: :localhost 475 Lotte #private :Cannot join channel (+k) - bad key

Client 2: /join #private secret123
Expected: Success (joins channel)
```

### Test ERR_INVITEONLYCHAN (473)
**What to test:** Join invite-only channel without invite
```
Client 1: /join #exclusive
Client 1: /mode #exclusive +i
Client 2: /join #exclusive
Expected: :localhost 473 Lotte #exclusive :Cannot join channel (+i)
```

### Test ERR_CHANNELISFULL (471)
**What to test:** Channel user limit reached
```
Client 1: /join #limited
Client 1: /mode #limited +l 2
Client 2: /join #limited
Client 3: /join #limited
Expected: :localhost 471 Saby #limited :Cannot join channel (+l)
```

---

## PART Command Error Tests

### Test ERR_NEEDMOREPARAMS (461) - PART
**What to test:** PART without channel name
```
/quote PART
Expected: :localhost 461 sheha PART :Not enough parameters
```

### Test ERR_NOSUCHCHANNEL (403) - PART
**What to test:** PART from non-existent channel
```
/part #nonexistent
Expected: :localhost 403 sheha #nonexistent :No such channel
```

### Test ERR_NOTONCHANNEL (442) - PART
**What to test:** PART from channel you're not in
```
Client 1: /join #test
Client 2: /part #test (without joining first)
Expected: :localhost 442 Lotte #test :You're not on that channel
```

---

## PRIVMSG Command Error Tests

### Test ERR_NEEDMOREPARAMS (461) - PRIVMSG
**What to test:** PRIVMSG without parameters
```
/quote PRIVMSG
Expected: :localhost 461 sheha PRIVMSG :Not enough parameters
```

### Test ERR_NOTEXTTOSEND (412)
**What to test:** PRIVMSG without message
```
/quote PRIVMSG #test
Expected: :localhost 412 sheha :No text to send
```

### Test ERR_NOSUCHNICK (401) - PRIVMSG
**What to test:** Message to non-existent user
```
/msg nonexistentuser hello
Expected: :localhost 401 sheha nonexistentuser :No such nick/channel
```

### Test ERR_NOSUCHCHANNEL (401) - PRIVMSG
**What to test:** Message to non-existent channel
```
/msg #nonexistent hello
Expected: :localhost 401 sheha #nonexistent :No such nick/channel
```

### Test ERR_CANNOTSENDTOCHAN (404)
**What to test:** Send message to channel you're not in
```
Client 1: /join #test
Client 2: (not in #test)
Client 2: /msg #test hello
Expected: :localhost 404 Lotte #test :Cannot send to channel
```

---

## KICK Command Error Tests

### Test ERR_NEEDMOREPARAMS (461) - KICK
**What to test:** KICK without enough parameters
```
/quote KICK
/quote KICK #test
Expected: :localhost 461 sheha KICK :Not enough parameters
```

### Test ERR_NOSUCHCHANNEL (403) - KICK
**What to test:** KICK from non-existent channel
```
/kick #nonexistent Lotte
Expected: :localhost 403 sheha #nonexistent :No such channel
```

### Test ERR_NOTONCHANNEL (442) - KICK
**What to test:** KICK when not on channel
```
Client 1: (not in #test)
Client 1: /kick #test Lotte
Expected: :localhost 442 sheha #test :You're not on that channel
```

### Test ERR_CHANOPRIVSNEEDED (482) - KICK
**What to test:** KICK without operator status
```
Client 1: /join #test (creator, has ops)
Client 2: /join #test (regular user)
Client 2: /kick #test Saby
Expected: :localhost 482 Lotte #test :You're not channel operator
```

### Test ERR_NOSUCHNICK (401) - KICK
**What to test:** KICK non-existent user
```
/kick #test nonexistentuser
Expected: :localhost 401 sheha nonexistentuser :No such nick/channel
```

### Test ERR_USERNOTINCHANNEL (441)
**What to test:** KICK user not on channel
```
Client 1: /join #test
Client 2: /join #other
Client 1: /kick #test Lotte (Lotte is not in #test)
Expected: :localhost 441 sheha Lotte #test :They aren't on that channel
```

---

## INVITE Command Error Tests

### Test ERR_NEEDMOREPARAMS (461) - INVITE
**What to test:** INVITE without parameters
```
/quote INVITE
/quote INVITE Lotte
Expected: :localhost 461 sheha INVITE :Not enough parameters
```

### Test ERR_NOSUCHNICK (401) - INVITE
**What to test:** Invite non-existent user
```
/invite nonexistentuser #test
Expected: :localhost 401 sheha nonexistentuser :No such nick/channel
```

### Test ERR_NOSUCHCHANNEL (403) - INVITE
**What to test:** Invite to non-existent channel
```
/invite Lotte #nonexistent
Expected: :localhost 403 sheha #nonexistent :No such channel
```

### Test ERR_NOTONCHANNEL (442) - INVITE
**What to test:** Invite when not on channel
```
Client 1: (not in #test)
Client 1: /invite Lotte #test
Expected: :localhost 442 sheha #test :You're not on that channel
```

### Test ERR_USERONCHANNEL (443)
**What to test:** Invite user already on channel
```
Client 1: /join #test
Client 2: /join #test
Client 1: /invite Lotte #test
Expected: :localhost 443 sheha Lotte #test :is already on channel
```

### Test ERR_CHANOPRIVSNEEDED (482) - INVITE
**What to test:** Invite to +i channel without ops
```
Client 1: /join #private
Client 1: /mode #private +i
Client 2: /join #private (fails, but gets invited)
Client 1: /invite Lotte #private
Client 2: /join #private (success)
Client 3: /join #private (also gets in through invite)
Client 2: /invite Saby #private
Expected: :localhost 482 Lotte #private :You're not channel operator
```

---

## TOPIC Command Error Tests

### Test ERR_NEEDMOREPARAMS (461) - TOPIC
**What to test:** TOPIC without parameters
```
/quote TOPIC
Expected: :localhost 461 sheha TOPIC :Not enough parameters
```

### Test ERR_NOSUCHCHANNEL (403) - TOPIC
**What to test:** TOPIC on non-existent channel
```
/topic #nonexistent
Expected: :localhost 403 sheha #nonexistent :No such channel
```

### Test ERR_NOTONCHANNEL (442) - TOPIC
**What to test:** TOPIC when not on channel
```
Client 1: (not in #test)
Client 1: /topic #test
Expected: :localhost 442 sheha #test :You're not on that channel
```

### Test ERR_CHANOPRIVSNEEDED (482) - TOPIC
**What to test:** Set topic on +t channel without ops
```
Client 1: /join #test
Client 1: /mode #test +t
Client 2: /join #test
Client 2: /topic #test :New topic
Expected: :localhost 482 Lotte #test :You're not channel operator
```

---

## MODE Command Error Tests

### Test ERR_NEEDMOREPARAMS (461) - MODE
**What to test:** MODE without parameters
```
/quote MODE
Expected: :localhost 461 sheha MODE :Not enough parameters
```

### Test ERR_NOSUCHCHANNEL (403) - MODE
**What to test:** MODE on non-existent channel
```
/mode #nonexistent +i
Expected: :localhost 403 sheha #nonexistent :No such channel
```

### Test ERR_CHANOPRIVSNEEDED (482) - MODE
**What to test:** MODE without operator status
```
Client 1: /join #test (creator, has ops)
Client 2: /join #test (regular user)
Client 2: /mode #test +i
Expected: :localhost 482 Lotte #test :You're not channel operator
```

---

## Success Cases (No Errors)

### Successful Commands
```
# Successful JOIN
/join #test

# Successful PART
/part #test

# Successful PRIVMSG
/msg #test Hello everyone!
/msg Lotte Hi there!

# Successful KICK (as operator)
/kick #test Lotte Spamming

# Successful INVITE
/invite Lotte #test

# Successful TOPIC (view)
/topic #test

# Successful TOPIC (set, as operator)
/topic #test :Welcome to our channel!

# Successful MODE (view)
/mode #test

# Successful MODE (set, as operator)
/mode #test +i
/mode #test +t
/mode #test +k password123
/mode #test +l 10
/mode #test +o Lotte

# Successful WHO
/who #test

# Successful NICK change
/nick newnickname
```

---

## Quick Test Script

For quick testing, copy/paste these commands one by one:

```irc
# Test authentication
/quote PASS wrongpass

# Test NICK errors
/quote NICK
/nick #bad

# Test JOIN errors
/quote JOIN
/join #test
/mode #test +k secret
/part #test
/join #test wrongkey

# Test PRIVMSG errors
/quote PRIVMSG
/msg #nonexistent hello
/msg nonexistentuser hello

# Test KICK errors
/kick #test
/kick #nonexistent Lotte

# Test INVITE errors
/quote INVITE
/invite nonexistentuser #test

# Test TOPIC errors
/quote TOPIC
/topic #nonexistent

# Test MODE errors
/quote MODE
/mode #nonexistent +i
```

---

## Expected Behavior Summary

| Error Code | Total Uses | Commands |
|------------|------------|----------|
| 401 | 5 | PRIVMSG, KICK, INVITE |
| 403 | 5 | PART, KICK, INVITE, TOPIC, MODE |
| 404 | 1 | PRIVMSG |
| 405 | 1 | JOIN |
| 412 | 1 | PRIVMSG |
| 421 | 1 | Unknown commands |
| 431 | 1 | NICK |
| 432 | 1 | NICK |
| 433 | 1 | NICK |
| 441 | 1 | KICK |
| 442 | 5 | PART, KICK, INVITE, TOPIC |
| 443 | 1 | INVITE |
| 451 | All | All commands (before registration) |
| 461 | 8 | JOIN, PART, KICK, INVITE, TOPIC, MODE, PRIVMSG, USER |
| 462 | 2 | PASS, NICK, USER (after registration) |
| 464 | 1 | PASS |
| 471 | 1 | JOIN |
| 473 | 1 | JOIN |
| 475 | 1 | JOIN |
| 482 | 5 | KICK, MODE, TOPIC (+t), INVITE (+i) |

---

## Notes

- Use `/quote COMMAND` to send raw IRC commands
- Some tests require multiple clients
- Channel operator (@) is automatically given to channel creator
- Default max channels per user: 10 (MAX_CHANNELS in Client.hpp)
