#!/bin/bash

#Restart
/etc/init.d/networking restart

#Exp1 - Finished
ifconfig eth0 up
ifconfig eth0172.16.40.1/24

arp -d 172.16.40.254