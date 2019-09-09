import socket

class ipc(object):
    def __init__(self, logger, ip, port, maxsize=8192, terminator='\x00'.encode('utf-8')):
        self.logger = logger
        self.ip = ip
        self.port = port
        self.maxsize = maxsize
        self.terminator = terminator

    def send(self, line):
        self.logger.debug('GSTD socket sending line: %s', line)
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((self.ip, self.port))
            s.send(' '.join(line).encode('utf-8'))
            data = self.recvall(s)
            data = data.decode('utf-8')
            s.close()
        except socket.error:
            s.close()
            self.logger.error('GSTD socket error')
            data = None
        self.logger.debug('GSTD socket received answer:\n %s', data)
        return data

    def recvall(self, sock):
        buf = b''
        count = self.maxsize
        try:
            while count:
                newbuf = sock.recv(self.maxsize//8)
                if not newbuf: return None
                if self.terminator in newbuf:
                    # this is the last item
                    buf += newbuf[:newbuf.find(self.terminator)]
                    break
                else:
                    buf += newbuf
                    count -= len(newbuf)
        except socket.error:
            buf = json.dumps({"error":"socket error", "msg": traceback.format_exc(), "code": -1 })
        return buf
