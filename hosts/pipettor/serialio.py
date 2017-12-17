import time
import threading

import serial

class Arduino(object):
    def __init__(self):
        self.ser = None
        self.listeners = []

        self.running = False
        self.thread = threading.Thread(
            name='Arduino serial communications', target=self.read_lines, daemon=True
        )

    # Setup

    def connect(self, port='/dev/ttyACM0', baudrate=115200):
        print('Please connect the device now...')
        while True:
            try:
                self.ser = serial.Serial(port, baudrate)
                break
            except serial.serialutil.SerialException:
                time.sleep(0.2)
        print('Connected!')

    def wait_for_handshake(self, poll_interval=200):
        while True:
            char = self.ser.read().decode('utf-8')
            if char == '~':
                self.write_string('\n')
                print('Completed handshake!')
                return
            time.sleep(poll_interval / 1000)

    # Teardown

    def close(self):
        self.ser.close()

    def reset(self):
        if self.ser.isOpen():
            self.ser.close()
        self.ser.open()
        self.ser.close()

    # RX

    def get_line(self):
        return str(self.ser.readline(), 'ascii').rstrip()

    def read_lines(self):
        self.running = True
        while self.running:
            line = self.get_line()
            for listener in self.listeners:
                listener.on_read_line(line)

    # TX

    def write_number(self, byte, length=1, byte_order='big', signed=False):
        self.ser.write(byte.to_bytes(length, byte_order, signed=signed))

    def write_line(self, line):
        self.ser.write(('{}\n'.format(line)).encode('utf-8'))

    def write_string(self, string):
        self.ser.write(string.encode('utf-8'))

    # Threading

    def start(self):
        self.thread.start()

    def stop(self):
        self.running = False
        self.thread.join(timeout=0.2)

