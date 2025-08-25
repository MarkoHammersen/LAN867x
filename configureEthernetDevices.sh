#!/bin/bash
# Setup der 10BASE-T1 Interfaces mit PLCA

# --- Interface 1 ---coordinator
IF1="enx6827199267cd"
IP1="192.168.10.11/24"
NODE_ID1=0

# --- Interface 2 ---follower
IF2="enx6827199270d1"
IP2="192.168.10.12/24"
NODE_ID2=1

# --- Gemeinsame PLCA-Parameter ---
NODE_CNT=8
TO_TMR=0x20
BURST_CNT=0x0
BURST_TMR=0x80

# Funktion zum Setzen eines Interfaces
setup_interface() {
    local IFACE=$1
    local IP_ADDR=$2
    local NODE_ID=$3

    echo "Konfiguriere Interface $IFACE mit IP $IP_ADDR und PLCA Node-ID $NODE_ID..."
    sudo ip addr add dev $IFACE $IP_ADDR
    sudo ip link set $IFACE up
    sudo ethtool --set-plca-cfg $IFACE enable on node-id $NODE_ID node-cnt $NODE_CNT to-tmr $TO_TMR burst-cnt $BURST_CNT burst-tmr $BURST_TMR
    sudo ethtool --get-plca-cfg $IFACE
    echo ""
}

# Interfaces konfigurieren
setup_interface $IF1 $IP1 $NODE_ID1
setup_interface $IF2 $IP2 $NODE_ID2
