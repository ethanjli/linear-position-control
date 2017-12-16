import time

import serial

class SerialLineStream(object):
    def __init__(self):
        self.ser = None

    def connect(self, port='/dev/ttyACM0', baudrate=115200):
        print('Please connect the device now...')
        while True:
            try:
                self.ser = serial.Serial(port, baudrate)
                break
            except serial.serialutil.SerialException:
                time.sleep(0.2)
        print('Connected!')

    def get_line(self):
        return str(self.ser.readline(), 'ascii').rstrip()

    def write_byte(self, byte, length=1, byte_order='big', signed=False):
        self.ser.write(byte.to_bytes(length, byte_order, signed=signed))

    def close(self):
        self.ser.close()

    def reset(self):
        if self.ser.isOpen():
            self.ser.close()
        self.ser.open()
        self.ser.close()

class OutputFileLineStream(object):
    def __init__(self, filename):
        self.f = None
        self.filename = filename
        self.lines = 0

    def start(self):
        self.f = open(self.filename, 'w')

    def append(self, line):
        print(line, file=self.f)
        self.f.close()
        self.f = open(self.filename, 'a')
        self.lines += 1

    def append_as_csv(self, *args):
        line = ','.join(str(field) for field in args)
        self.append(line)

    def close(self):
        self.f.close()

