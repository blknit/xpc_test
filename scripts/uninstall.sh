#!/bin/sh
if (($EUID != 0)); then
    echo "Please run as root"
    exit
fi

# Unload the daemon
launchctl unload /Library/LaunchDaemons/com.launchd.xpc.plist

# Remove the daemon's launchd.plist
rm /Library/LaunchDaemons/com.launchd.xpc.plist

# Remove the daemon's executable
rm /Library/PrivilegedHelperTools/xpc_server

# Check the daemon is running
launchctl list | grep com.test.xpc
