#!/bin/sh

CFGFILE=/etc/network/interfaces

ret=0
for iface in $(find_ifaces); do
  if [ ! ifup -i $CFGFILE $iface >/dev/null ]; then
    ifdown -i $CFGFILE $iface >/dev/null 2>&1
    ret=1
  fi
done

exit $ret
