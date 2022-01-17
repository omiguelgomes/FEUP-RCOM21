#!/bin/bash

#Restart
/etc/init.d/networking restart

#Exp2 - Finished
ifconfig eth0 up
ifconfig eth0 172.16.41.1/24

#Exp4
route add -net 172.16.40.0/24 gw 172.16.41.253

route add default gw 172.16.41.254