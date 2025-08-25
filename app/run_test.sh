#!/bin/bash
# Automatischer Durchsatz-Test über Raw-Sockets (10BASE-T1)
# Root-Rechte werden benötigt, da PF_PACKET benutzt wird

RECEIVER_IF="enx6827199270d1"
SENDER_IF="enx6827199267cd"
DEST_MAC="68:27:19:92:70:d1"  # MAC-Adresse des Receivers
PACKET_SIZE=1400
DURATION=10

echo "Starte Receiver auf ${RECEIVER_IF} ..."
sudo ./throughput_receiver ${RECEIVER_IF} > receiver_output.txt 2>&1 &

# kleinen Moment warten, damit Receiver läuft
sleep 2

echo "Starte Sender auf ${SENDER_IF} ..."
sudo ./throughput_sender ${SENDER_IF} ${DEST_MAC} ${PACKET_SIZE} ${DURATION} > sender_output.txt 2>&1

# warten, bis Receiver beendet ist
sleep $((DURATION+3))

echo ""
echo "===== Ergebnis Sender (auf ${SENDER_IF}) ====="
cat sender_output.txt
echo ""
echo "===== Ergebnis Receiver (auf ${RECEIVER_IF}) ====="
cat receiver_output.txt
