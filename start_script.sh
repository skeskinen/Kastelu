#!/bin/sh
cd /kastelu
fusermount -u sensors
owfs -uall --allow_other sensors
./kastelu --docroot . --http-address localhost --http-port 80 
