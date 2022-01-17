#mafalda_t5_switch
#copy flash:mafalda_t5_switch running-config

#Access
enable
8nortel

#Reset switch
#See in class

#Exp 2 - Finished
configure terminal
vlan 40
end

configure terminal
interface fastethernet 0/1 #tux43
switchport mode access
switchport access vlan 40

interface fastethernet 0/2 #tux44 E0
switchport mode access
switchport access vlan 40
end

configure terminal
vlan 41
end

configure terminal
interface fastethernet 0/3 #tux42
switchport mode access
switchport access vlan 41
end

#Exp4 - Until Step 4
configure terminal
interface fastethernet 0/4 #tux44 E1
switchport mode access
switchport access vlan 41
end

configure terminal
interface fastethernet 0/5 #Router GE0/0
switchport mode access
switchport access vlan 41
end

#Router GE 0/1 -> 4.1 acima

