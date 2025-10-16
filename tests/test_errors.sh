#!/bin/bash

# IRC Server Error Message Test Script
# This script tests various error conditions using netcat

SERVER="127.0.0.1"
PORT="5506"
PASSWORD="hello"

echo "=========================================="
echo "IRC Server Error Message Testing Script"
echo "=========================================="
echo ""
echo "Make sure your server is running on $SERVER:$PORT"
echo "Press Enter to continue..."
read

# Function to send command and show response
test_command() {
    local description=$1
    local commands=$2
    local expected=$3
    
    echo ""
    echo "-------------------------------------------"
    echo "TEST $description"
    echo "Commands: $commands"
    echo "Expected: $expected"
    echo "-------------------------------------------"
    
    # Send commands and show response
    while IFS= read -r line; do
        printf "%s\r\n" "$line"
    done <<< "$commands" | nc -w 2 "$SERVER" "$PORT" | head -20
    
    echo ""
    echo "Press Enter for next test..."
    read
}

# Test 1: Command before registration (ERR_NOTREGISTERED - 451)
test_command \
   "1 Command before registration" \
   "JOIN #test" \
   "451 - ERR_NOTREGISTERED"

# Test 2: Wrong password (ERR_PASSWDMISMATCH - 464)
test_command \
    "2 Wrong password" \
    "PASS wrongpassword
    NICK testuser
    USER testuser 0 * :Test User" \
    "464 - ERR_PASSWDMISMATCH"

# Test 3: NICK without parameter (ERR_NONICKNAMEGIVEN - 431)
test_command \
    "3 NICK without parameter" \
    "PASS $PASSWORD
    NICK" \
    "431 - ERR_NONICKNAMEGIVEN"

# Test 4: Invalid nickname (ERR_ERRONEUSNICKNAME - 432)
test_command \
    "4 Invalid nickname" \
    "PASS $PASSWORD
    NICK #invalid
    USER testuser 0 * :Test User" \
    "432 - ERR_ERRONEUSNICKNAME"

# Test 5: JOIN without parameter (ERR_NEEDMOREPARAMS - 461)
test_command \
    "5 JOIN without parameter" \
    "PASS $PASSWORD
    NICK testuser5
    USER testuser5 0 * :Test User
    JOIN" \
    "461 - ERR_NEEDMOREPARAMS"

# Test 6: PRIVMSG without text (ERR_NOTEXTTOSEND - 412)
test_command \
    "6 PRIVMSG without text" \
    "PASS $PASSWORD
NICK testuser6
USER testuser6 0 * :Test User
PRIVMSG #test" \
    "412 - ERR_NOTEXTTOSEND"

# Test 7: Message to non-existent channel (ERR_NOSUCHNICK - 401)
test_command \
    "7 Message to non-existent channel" \
    "PASS $PASSWORD
NICK testuser7
USER testuser7 0 * :Test User
PRIVMSG #nonexistent :Hello" \
    "401 - ERR_NOSUCHNICK/NOSUCHCHANNEL"

# Test 8: PART from non-existent channel (ERR_NOSUCHCHANNEL - 403)
test_command \
    "8 PART from non-existent channel" \
    "PASS $PASSWORD
NICK testuser8
USER testuser8 0 * :Test User
PART #nonexistent" \
    "403 - ERR_NOSUCHCHANNEL"

# Test 9: Already registered (ERR_ALREADYREGISTERED - 462)
test_command \
    "9 Try to register twice" \
    "PASS $PASSWORD
NICK testuser9
USER testuser9 0 * :Test User
PASS $PASSWORD" \
    "462 - ERR_ALREADYREGISTERED"

# Test 10: Unknown command (ERR_UNKNOWNCOMMAND - 421)
test_command \
    "10 Unknown command" \
    "PASS $PASSWORD
NICK testuser10
USER testuser10 0 * :Test User
INVALIDCOMMAND" \
    "421 - ERR_UNKNOWNCOMMAND"

echo ""
echo "=========================================="
echo "Basic error testing complete!"
echo "=========================================="
echo ""
echo "For more complex tests (channel modes, kick, invite, etc.),"
echo "please use HexChat with multiple clients."
echo "See ERROR_TESTING_GUIDE.md for detailed test cases."
echo ""
