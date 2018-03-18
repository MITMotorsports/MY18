#!/usr/bin/env python3

import can
import threading
import time

class ShutdownLoop(object):

    def __init__(self, stop_event):
        self.bus = can.interface.Bus(bustype='pcan', channel='PCAN_USBBUS1', bitrate=500000)
        self.stop_event = stop_event
        self.send_thread = threading.Thread(target=self.send, args=(self.bus, self.stop_event))
        self.recv_thread = threading.Thread(target=self.receive, args=(self.bus, self.stop_event))
        self.recv_thread.start()
        self.send_thread.start()

    def update(self):
        pass
    
    def receive(self, bus, stop_event):
        print("Start receiving messages")
        while not stop_event.is_set():
            rx_msg = bus.recv(1)
            if rx_msg is not None:
                print("rx: {}".format(rx_msg))
        print "Stopped receiving messages"
    
    def send(self, bus, stop_event):
        print("Start to send messages")
        while not stop_event.is_set():
            #bus.send(msg)
            #print("tx: {}".format(tx_msg))
            time.sleep(0.025)
        print("Stopped sending messages")

if __name__ == "__main__":
    
    stop_event = threading.Event()
    shutdownLoop = ShutdownLoop(stop_event)

    # Threading for sending and receiving messages
    try:
        while True:
            pass
    except KeyboardInterrupt:
        pass

    stop_event.set()
    shutdownLoop.bus.shutdown()
    print("Stopped script")
    

