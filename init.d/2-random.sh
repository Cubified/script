#!/bin/sh

RANDOM_SEED=/var/lib/misc/random-seed
if [ -s $RANDOM_SEED ]; then
  cat $RANDOM_SEED > /dev/urandom
else
  dd if=/dev/random of=/dev/urandom count=1 &>/dev/null
  touch $RANDOM_SEED
fi
chmod 600 $RANDOM_SEED
