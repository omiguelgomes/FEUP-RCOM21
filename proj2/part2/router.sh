#mafalda_t5_router
#copy flash:mafalda_t5_router running-config

conf t
interface gigabitethernet 0/0
ip address 172.16.41.254 255.255.255.0
no shutdown
ip nat inside
exit

interface gigabitethernet 0/1
ip address 172.16.1.49 255.255.255.0
no shutdown
ip nat outside
exit

ip nat pool ovrld 172.16.1.49 prefix 24
ip nat inside source list 1 pool ovrld overload

access-list 1 permit 172.16.40.0 0.0.0.7
access-list 1 permit 172.16.41.0 0.0.0.7

ip route 0.0.0.0 0.0.0.0 172.16.1.254
ip route 172.16.40.0 255.255.255.0 172.16.41.253