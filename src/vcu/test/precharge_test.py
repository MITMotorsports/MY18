#!/usr/bin/env python3

# is not really python 3 compatible yet

import can
import threading
import time

def pauseForUser():
    raw_input('HIT ENTER TO CONTINUE:')

class PrechargeTest(object):

    START                   = "START"
    DRIVER_RESET            = "DRIVER_RESET"
    CHARGING                = "CHARGING"
    READY_TO_DRIVE          = "READY_TO_DRIVE"
    PRESSING_PEDALS         = "PRESSING_PEDALS"
    STOPPED                 = "STOPPED"

    def __init__(self, stop_event):
        self.state = None
        self.state_start_time = time.time()
        self.bus = can.interface.Bus(bustype='pcan', channel='PCAN_USBBUS1', bitrate=500000)
        self.stop_event = stop_event
        self.recv_thread = threading.Thread(target=self.receive, args=(self.bus, self.stop_event))
        self.recv_thread.start()

        # Initialize
        pauseForUser()
        self.changeState(PrechargeTest.START)

    def timeElapsed(self):
        return time.time() - self.state_start_time

    def resetClock(self):
        self.state_start_time = time.time()

    # UPDATE FUNCTIONS FOR THE TEST THAT ARE RUN IN AN UPDATE LOOP CONTINUOUSLY

    def changeState(self, newState):
        print "NOW ENTERING THE %d STATE" % self.state
        self.state = newState
        self.resetClock()
        if newState == PrechargeTest.START:
            self.initStart()
        elif newState == PrechargeTest.DRIVER_RESET:
            self.initDriverReset()
        elif newState == PrechargeTest.CHARGING:
            self.initCharging()
        elif newState == PrechargeTest.READY_TO_DRIVE:
            self.enter_vcu_state_rtd()
        elif newState == PrechargeTest.PRESSING_PEDALS:
            self.initPressingPedals()
        elif newState == PrechargeTest.STOPPED:
            self.initStopped()

    def update(self):
        if self.state == PrechargeTest.START:
            self.updateStart()
        elif self.state == PrechargeTest.DRIVER_RESET:
            self.updateDriverReset()
        elif self.state == PrechargeTest.CHARGING:
            self.updateCharging()
        elif self.state == PrechargeTest.READY_TO_DRIVE:
            self.updateReadyToDrive()
        elif self.state == PrechargeTest.PRESSING_PEDALS:
            self.updatePressingPedals()
        elif self.state == PrechargeTest.STOPPED:
            self.updateStopped()

    # INIT FUNCTIONS FOR THE STATES
    def initStart(self):
        pauseForUser()
        print "--> STARTING TEST CAR (LIGHTNING MCQUEEN)"

    def initDriverReset(self):
        pauseForUser()

    def initCharging(self):
        pauseForUser()

    def enter_vcu_state_rtd(self):
        pauseForUser()

    def initPressingPedals(self):
        pauseForUser()

    def initStopped(self):
        pauseForUser()

    # UPDATE FUNCTIONS FOR THE STATES
    def updateStart(self):
        pass

    def updateDriverReset(self):
        pass

    def updateCharging(self):
        pass

    def updateReadyToDrive(self):
        pass

    def updatePressingPedals(self):
        pass

    def updateStopped(self):
        pass


    # THREADED RECV FUNCTION TO GET THE MESSAGES FROM THE CAN BUS
    def receive(self, bus, stop_event):
        print("Start receiving messages")
        while not stop_event.is_set():
            rx_msg = bus.recv(1)
            if rx_msg is not None:
                print("rx: {}".format(rx_msg))
        print "Stopped receiving messages"

    # def send(self, bus, stop_event):
    #     print("Start to send messages")
    #     while not stop_event.is_set():
    #         #bus.send(msg)
    #         #print("tx: {}".format(tx_msg))
    #         time.sleep(0.025)
    #     print("Stopped sending messages")

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
