#!/bin/sh
cd /kastelu
fusermount -u sensors
owfs -uall --allow_other sensor
./kastelu --docroot . --http-address 0.0.0.0 --http-port 80 
