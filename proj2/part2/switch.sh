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
interface fastethernet 0/1
switchport mode access
switchport access vlan 40

interface fastethernet 0/2
switchport mode access
switchport access vlan 40
end

configure terminal
vlan 41
end

configure terminal
interface fastethernet 0/3
switchport mode access
switchport access vlan 41
end

#Exp4 - Until Step 4
configure terminal
interface fastethernet 0/4
switchport mode access
switchport access vlan 41
end