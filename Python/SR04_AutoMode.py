import serial
import threading
import struct

comm = serial.Serial(baudrate = 9600, timeout = 0.5)
comm.port = "/dev/ttyUSB0"
comm.baudrate = 9600
comm.timeout = 0.5
comm.open()

decode_state = 0
msg_cnt = 0
msg_buf = [ 0 ]
msg_sum = 0

def SR04_parse(byte):
    global decode_state, msg_cnt, msg_buf, msg_sum, quality

    if decode_state == 0: #Head Byte 0, 0xFF
        if byte == int("0xFF",16):
            decode_state = 1
            msg_cnt = 0
            msg_buf = 0
            msg_sum = 0
            quality = 0
            # print(" head!\n")

    elif decode_state == 1: #Data s (2 bytes)

        msg_cnt = msg_cnt + 1

        if msg_cnt == 1:
            msg_buf = [byte]
        else:
            msg_buf.append(byte)
            
        msg_sum += byte

        if msg_cnt >= 2:
            decode_state = 2
            
    elif decode_state == 2: #checksum
        if msg_sum == byte:
            #decode here
            SR04_decode(msg_buf)
        decode_state = 0
    else:
        decode_state = 0

def SR04_decode(buf):
    dist_array = [buf[0], buf[1]]
    dist = struct.unpack('>h', bytearray(dist_array))
    print(" Dist: ","%f " %float(dist[0]/10) ," %x "%buf[0]," %x \n"%buf[1])

def Read_UART():
    while 1:
        while comm.in_waiting:
            tx_buf = comm.read(comm.in_waiting)
            if not tx_buf: break
            length = len(tx_buf)
            for i in range(length):
                SR04_parse(tx_buf[i])

    
readThread = threading.Thread(target = Read_UART)

##Main here
readThread.start()