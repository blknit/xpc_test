#!/bin/sh
if (($EUID != 0)); then
    echo "Please run as root"
    exit
fi

# Copy the daemon's launchd.plist into /Library/LaunchDaemons
mkdir -p /Library/LaunchDaemons
for plist in com.launchd.xpc.plist
do
  trg=/Library/LaunchDaemons/com.launchd.xpc.plist
  cp ${plist} ${trg}
  chown root:wheel ${trg}
  chmod 644 ${trg}
done

# Copy the daemon's executable into /Library/PrivilegedHelperTools
mkdir -p /Library/PrivilegedHelperTools
for executable in xpc_server
do
  trg=/Library/PrivilegedHelperTools/${executable}
  cp ../build/Debug/${executable} ${trg}
  chown -R root:admin ${trg}
done

# Load the new daemon
launchctl load /Library/LaunchDaemons/com.launchd.xpc.plist

# Check the daemon is running
launchctl list | grep com.test.xpc
