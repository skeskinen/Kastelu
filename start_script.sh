#!/bin/sh
cd /kastelu
fusermount -u sensors
owfs -uall --allow_other sensor
killall kastelu
./kastelu --docroot . --http-address 0.0.0.0 --http-port 80 &
echo $! > /var/run/kastelu.pid
