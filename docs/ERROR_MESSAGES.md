# IRC Error Messages Implementation

## Summary of IRC Numeric Replies

This document lists all the error messages and numeric replies implemented in the IRC server.

---

## Success Messages

| Code | Name | Format | Description |
|------|------|--------|-------------|
| 001 | RPL_WELCOME | `:localhost 001 <nick> :Welcome to the IRC Network <nick>!<user>@localhost` | Welcome message after registration |
| 324 | RPL_CHANNELMODEIS | `:localhost 324 <nick> <channel> <modes> [params]` | Channel mode response |
| 331 | RPL_NOTOPIC | `:localhost 331 <nick> <channel> :No topic is set` | No topic set for channel |
| 332 | RPL_TOPIC | `:localhost 332 <nick> <channel> :<topic>` | Channel topic |
| 341 | RPL_INVITING | `:localhost 341 <nick> <target> <channel>` | Invite confirmation |
| 353 | RPL_NAMREPLY | `:localhost 353 <nick> = <channel> :<names>` | Names list for channel |
| 366 | RPL_ENDOFNAMES | `:localhost 366 <nick> <channel> :End of /NAMES list` | End of names list |
| 315 | RPL_ENDOFWHO | `:localhost 315 <nick> <mask> :End of WHO list` | End of WHO list |
| 352 | RPL_WHOREPLY | `:localhost 352 <nick> <channel> <user> <host> <server> <nick> <flags> :<hopcount> <realname>` | WHO reply for user |

---

## Error Messages

### Authentication Errors

| Code | Name | Format | Description | Used In |
|------|------|--------|-------------|---------|
| 451 | ERR_NOTREGISTERED | `:localhost 451 <nick> :You have not registered` | User tried command before registration | All commands (registration check) |
| 462 | ERR_ALREADYREGISTERED | `:localhost 462 <nick> :You may not reregister` | User tried to register twice | PASS, NICK, USER |
| 464 | ERR_PASSWDMISMATCH | `:localhost 464 <nick> :Password incorrect` | Wrong password | PASS |

---

### Command Parameter Errors

| Code | Name | Format | Description | Used In |
|------|------|--------|-------------|---------|
| 431 | ERR_NONICKNAMEGIVEN | `:localhost 431 :No nickname given` | NICK without parameter | NICK |
| 461 | ERR_NEEDMOREPARAMS | `:localhost 461 <nick> <command> :Not enough parameters` | Command needs more parameters | JOIN, KICK, INVITE, TOPIC, MODE, PRIVMSG |

---

### Nickname Errors

| Code | Name | Format | Description | Used In |
|------|------|--------|-------------|---------|
| 432 | ERR_ERRONEUSNICKNAME | `:localhost 432 <nick> <badnick> :Erroneous nickname` | Invalid nickname format | NICK |
| 433 | ERR_NICKNAMEINUSE | `:localhost 433 <nick> <badnick> :Nickname is already in use` | Nickname already taken | NICK |

---

### Channel Errors

| Code | Name | Format | Description | Used In |
|------|------|--------|-------------|---------|
| 401 | ERR_NOSUCHNICK | `:localhost 401 <nick> <target> :No such nick/channel` | Target user/channel doesn't exist | PRIVMSG, KICK, INVITE |
| 403 | ERR_NOSUCHCHANNEL | `:localhost 403 <nick> <channel> :No such channel` | Channel doesn't exist | MODE, TOPIC, INVITE |
| 404 | ERR_CANNOTSENDTOCHAN | `:localhost 404 <nick> <channel> :Cannot send to channel` | User not on channel | PRIVMSG |
| 405 | ERR_TOOMANYCHANNELS | `:localhost 405 <nick> <channel> :You have joined too many channels` | Exceeded max channels | JOIN |
| 412 | ERR_NOTEXTTOSEND | `:localhost 412 <nick> :No text to send` | PRIVMSG with no message | PRIVMSG |
| 421 | ERR_UNKNOWNCOMMAND | `:localhost 421 <nick> <command> :Unknown command` | Command not recognized | parse() |
| 441 | ERR_USERNOTINCHANNEL | `:localhost 441 <nick> <target> <channel> :They aren't on that channel` | Target not on channel | KICK |
| 442 | ERR_NOTONCHANNEL | `:localhost 442 <nick> <channel> :You're not on that channel` | User not on channel | PART, TOPIC, INVITE |
| 443 | ERR_USERONCHANNEL | `:localhost 443 <nick> <target> <channel> :is already on channel` | User already on channel | INVITE |
| 471 | ERR_CHANNELISFULL | `:localhost 471 <nick> <channel> :Cannot join channel (+l)` | Channel has reached user limit | JOIN |
| 473 | ERR_INVITEONLYCHAN | `:localhost 473 <nick> <channel> :Cannot join channel (+i)` | Channel is invite-only | JOIN |
| 475 | ERR_BADCHANNELKEY | `:localhost 475 <nick> <channel> :Cannot join channel (+k) - bad key` | Wrong channel password | JOIN |
| 482 | ERR_CHANOPRIVSNEEDED | `:localhost 482 <nick> <channel> :You're not channel operator` | Need operator privileges | KICK, MODE, TOPIC (+t), INVITE (+i) |

---

## Command Implementations

### JOIN
- **461**: Not enough parameters
- **405**: Too many channels
- **475**: Bad channel key (wrong password)
- **473**: Invite-only channel (not invited)
- **471**: Channel is full (user limit reached)

### PART
- **461**: Not enough parameters
- **403**: No such channel
- **442**: Not on that channel

### PRIVMSG
- **461**: Not enough parameters
- **412**: No text to send
- **401**: No such nick/channel (target doesn't exist)
- **404**: Cannot send to channel (not a member)

### KICK
- **461**: Not enough parameters
- **403**: No such channel
- **442**: Not on that channel (kicker)
- **482**: Not channel operator
- **401**: No such nick (target doesn't exist)
- **441**: Target not on channel

### INVITE
- **461**: Not enough parameters
- **401**: No such nick (target doesn't exist)
- **403**: No such channel
- **442**: Not on that channel
- **443**: User already on channel
- **482**: Not channel operator (for +i channels)

### TOPIC
- **461**: Not enough parameters
- **403**: No such channel
- **442**: Not on that channel
- **482**: Not channel operator (for +t channels)

### MODE
- **461**: Not enough parameters
- **403**: No such channel
- **482**: Not channel operator

### NICK
- **431**: No nickname given
- **432**: Erroneous nickname (invalid format)
- **433**: Nickname already in use
- **462**: Already registered (can change after registration)

### PASS
- **464**: Password incorrect
- **462**: Already registered

### USER
- **461**: Not enough parameters
- **462**: Already registered

### WHO
- Returns WHO list for channel or user

---

## Notes

- All error messages follow IRC RFC 1459/2812 format
- Server name is always "localhost" in responses
- Error messages are sent immediately to the client's send buffer
- Client nick is included in all error messages (or "*" if not registered)
