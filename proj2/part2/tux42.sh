#!/bin/bash

#Restart
/etc/init.d/networking restart

#Exp2 - Finished
ifconfig eth0 up
ifconfig eth0 172.16.41.1/24