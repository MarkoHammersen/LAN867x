CC=gcc
CFLAGS=-O2 -Wall

TARGETS=throughput_sender throughput_receiver

all: $(TARGETS)

throughput_sender: throughput_sender.c
	$(CC) $(CFLAGS) -o $@ $<

throughput_receiver: throughput_receiver.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) *.o *.txt
