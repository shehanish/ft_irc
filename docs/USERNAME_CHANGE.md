# Username Change Feature

## Overview
Your IRC server now supports **changing username after registration**! This is in addition to nickname changes.

---

## How to Change Username

After connecting and registering, you can change your username using:

```
/quote USER newusername 0 * :New Real Name
```

Or in HexChat:
```
/raw USER newusername 0 * :New Real Name
```

### Example:
```
/quote USER johndoe 0 * :John Doe
```

You'll see:
```
-localhost- Username changed from shehani to johndoe
```

---

## How to Change Nickname

```
/nick newnickname
```

---

## What Changed

### Before ❌
- Username was locked after registration
- Attempting to change would get: "462 You may not reregister"

### After ✅
- Username can be changed anytime
- Server sends a NOTICE confirming the change
- Real name also updates

---

## Commands Summary

| Command | What it does | Example |
|---------|-------------|---------|
| `/nick name` | Change nickname | `/nick sheha` |
| `/quote USER user 0 * :real` | Change username & realname | `/quote USER john 0 * :John Smith` |

---

## Technical Details

### IRC Format:
```
USER <username> <mode> <unused> :<realname>
```

- **username**: Your new username
- **mode**: Usually 0 (not used in your server)
- **unused**: Usually * (placeholder)
- **realname**: Your display name (the text after `:`)

### What Gets Updated:
- ✅ Username (shown in `nick!username@host`)
- ✅ Real name (shown in WHOIS, etc.)

### Example:
```
Before: sheha!~shehani@localhost
After:  sheha!~johndoe@localhost
        ^^^^^    ^^^^^^^
        nick     username (changed!)
```

---

## Server Response

When you change your username, the server sends:
```
:localhost NOTICE yournick :Username changed from olduser to newuser
```

---

## Testing

1. **Connect to server**
2. **Change nickname:**
   ```
   /nick testname
   ```
3. **Change username:**
   ```
   /quote USER testuser 0 * :Test User
   ```
4. **Verify in server console** - you'll see:
   ```
   Username changed: shehani → testuser
   ```

---

## Notes

- ⚠️ Unlike standard IRC servers, yours allows username changes post-registration
- This is a **custom feature** for flexibility at 42 school
- Both nickname and username can be changed independently
- Changes are effective immediately

---

## Files Modified

- `srcs/Server.cpp` - `handleUser()` function
  - Removed registration block
  - Added username change notification
  - Allows updates for registered clients

---

**Enjoy the flexibility! 🎉**
