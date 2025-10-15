# Testing Channels with Multiple Clients

## You Have 3 Clients Connected! 🎉

Now let's test channel functionality:

---

## Basic Channel Commands

### 1. **Join a Channel**

In **Client 1:**
```
/join #test
```

In **Client 2:**
```
/join #test
```

In **Client 3:**
```
/join #test
```

✅ All three clients are now in the same channel!

---

### 2. **Send Messages to Channel**

In **Client 1:**
```
Hello everyone!
```

In **Client 2:**
```
Hi there!
```

✅ All clients in #test should see these messages!

---

### 3. **Send Private Messages**

If Client 1 wants to message Client 2 privately:

In **Client 1:**
```
/msg client2nick Hey, private message!
```

✅ Only Client 2 will see this message

---

### 4. **List Who's in Channel**

```
/names #test
```

Or:
```
/who #test
```

---

### 5. **Set Channel Topic**

In **Client 1:**
```
/topic #test Welcome to our test channel!
```

✅ All clients will see the topic change

---

### 6. **Leave a Channel**

In **Client 3:**
```
/part #test
```

Or with a message:
```
/part #test Goodbye everyone!
```

✅ Client 3 leaves, others remain in channel

---

### 7. **Kick a User** (if you're channel operator)

In **Client 1** (channel creator is usually operator):
```
/kick #test client2nick
```

Or with a reason:
```
/kick #test client2nick Please behave!
```

---

### 8. **Invite Someone to Channel**

In **Client 1:**
```
/invite client3nick #test
```

---

## Full Test Scenario

### Step-by-Step Test:

#### **Client 1 (Alice):**
```
/nick alice
/join #mychannel
```
You'll see: `* Now talking in #mychannel`

#### **Client 2 (Bob):**
```
/nick bob
/join #mychannel
```
You'll see: 
- `* Now talking in #mychannel`
- Alice sees: `* bob has joined #mychannel`

#### **Client 3 (Charlie):**
```
/nick charlie
/join #mychannel
```
Everyone sees: `* charlie has joined #mychannel`

#### **In #mychannel (everyone can type):**

**Alice:**
```
Hello everyone!
```

**Bob:**
```
Hi Alice!
```

**Charlie:**
```
Hey folks!
```

✅ Everyone sees all messages!

---

## Set Topic

**Alice (as channel creator):**
```
/topic #mychannel This is our test channel
```

Everyone sees: `* Topic for #mychannel is: This is our test channel`

---

## Private Message Test

**Alice (send private message to Bob):**
```
/msg bob This is a secret message
```

✅ Only Bob sees it in a separate window/tab

---

## Test PART (Leave Channel)

**Charlie:**
```
/part #mychannel See you later!
```

Alice and Bob see: `* charlie has left #mychannel (See you later!)`

---

## Test Multiple Channels

**Alice:**
```
/join #channel1
/join #channel2
```

**Bob:**
```
/join #channel1
```

**Charlie:**
```
/join #channel2
```

Now Alice is in both, Bob in #channel1, Charlie in #channel2!

Switch between channels in HexChat by clicking the tabs.

---

## Advanced Tests

### Test KICK:

**Alice (in #mychannel):**
```
/kick #mychannel bob You've been kicked!
```

Bob gets removed from the channel.

---

### Test INVITE:

**Alice:**
```
/invite charlie #mychannel
```

Charlie receives an invite to join #mychannel.

---

## What to Look For

### ✅ **Working Features:**
- Creating channels (first person to JOIN creates it)
- Multiple people joining the same channel
- Broadcasting messages to everyone in channel
- Private messages between users
- Topic setting and viewing
- Leaving channels (PART)
- Kicking users (KICK)
- Inviting users (INVITE)

### 🔍 **Check in Server Console:**

You should see output like:
```
Nickname set to alice
Nickname set to bob
Nickname set to charlie
Client registered: alice!alice
Client registered: bob!bob
Client registered: charlie!charlie
```

---

## Quick Test Commands Cheat Sheet

| Command | What it does | Example |
|---------|-------------|---------|
| `/join #name` | Join/create channel | `/join #test` |
| `/part #name` | Leave channel | `/part #test` |
| `/msg nick text` | Private message | `/msg bob hi` |
| `/topic #name text` | Set topic | `/topic #test Welcome!` |
| `/kick #name nick` | Kick user | `/kick #test bob` |
| `/invite nick #name` | Invite user | `/invite charlie #test` |
| `/names #name` | List users | `/names #test` |
| `/who #name` | List users (detailed) | `/who #test` |

---

## Expected Behavior

### When someone joins:
```
* bob has joined #test
```

### When someone leaves:
```
* bob has left #test
```

### When someone is kicked:
```
* bob was kicked by alice (reason)
```

### When topic changes:
```
* alice changed the topic to: New topic here
```

---

## Testing Checklist

- [ ] All 3 clients can join the same channel
- [ ] Messages sent in channel appear for everyone
- [ ] Private messages work between clients
- [ ] Topic can be set and seen by all
- [ ] Users can leave channels (PART)
- [ ] Channel creator can kick users
- [ ] Invites work
- [ ] Multiple channels can exist simultaneously
- [ ] Users can be in multiple channels

---

## Troubleshooting

### "No such channel" error:
- Make sure you typed the channel name correctly
- Channels start with `#` (e.g., `#test`, not `test`)

### Messages not appearing:
- Check you're in the right channel tab in HexChat
- Verify all clients are in the same channel with `/names #channelname`

### Can't kick users:
- Only channel creators/operators can kick
- The first person to join a channel is usually the operator

---

## Have Fun! 🎊

Try creating different scenarios:
- Create a main channel for all 3 clients
- Create private channels for 2 clients
- Test kicking and re-joining
- Test multiple channel memberships

Your IRC server is working! 🚀
