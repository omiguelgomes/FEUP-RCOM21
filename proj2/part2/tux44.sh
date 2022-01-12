#!/bin/bash

#Reset
/etc/init.d/networking restart

#Exp1 - Finished
ifconfig eth0 up
ifconfig eth0 172.16.40.254/24

#Exp4 - Until step 4
ifconfig eth1 up
ifconfig
ifconfig eth1 172.16.31.253/24

echo 1 > /proc/sys/net/ipv4/ip_forward

echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts