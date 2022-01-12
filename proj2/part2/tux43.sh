#!/bin/bash

#Restart
/etc/init.d/networking restart

#Exp1 - Finished
ifconfig eth0 up
ifconfig eth0 172.16.40.1/24

arp -d 172.16.40.254

#Exp4
route add -net 172.16.41.0/24 gw 172.16.40.254 

route add default gw 172.16.40.254