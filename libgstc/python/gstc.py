import json
import logging
import socket
import subprocess
import psutil
import time
import traceback
import sys

GSTD_PROCNAME = 'gstd'
terminator = '\x00'.encode('utf-8')
max_size = 8192
def recvall(sock):
    buf = b''
    count = max_size
    try:
        while count:
            newbuf = sock.recv(max_size//8)
            if not newbuf: return None
            if terminator in newbuf:
                # this is the last item
                buf += newbuf[:newbuf.find(terminator)]
                break
            else:
                buf += newbuf
                count -= len(newbuf)
    except socket.error:
        buf = json.dumps({"error":"socket error", "msg": traceback.format_exc(), "code": -1 })
    return buf

# Add color to logging output
COLORS = {
    'WARNING': '33m',
    'INFO': '37m',
    'DEBUG': '34m',
    'CRITICAL': '35m',
    'ERROR': '31m'
}
class colorFormatter(logging.Formatter):
    def __init__(self, msg):
        logging.Formatter.__init__(self, msg)

    def format(self, record):
        if record.levelname in COLORS:
            record.levelname = "\033[1;" + COLORS[record.levelname] + record.levelname + "\033[0m"
        return logging.Formatter.format(self, record)

class GstdError(Exception):
    def __init__(self,*args,**kwargs):
        Exception.__init__(self,*args,**kwargs)
class GstcError(Exception):
    def __init__(self,*args,**kwargs):
        Exception.__init__(self,*args,**kwargs)

class client(object):
    def __init__(self, ip='localhost', port=5000, nports=1, logfile=None, loglevel='ERROR'):
        
        # Init the logger
        self.logger = logging.getLogger('GSTD')
        self.logger.setLevel(logging.DEBUG)
        # Select to log in a file or console
        if logfile:
            # log in file
            self.log = logging.FileHandler(logfile)
        else:
            # log in console
            self.log = logging.StreamHandler()
        # Set log format with colors
        self.log.setFormatter(colorFormatter("%(asctime)22s  %(levelname)s    \t%(message)s"))
        # Set log level
        numeric_level = getattr(logging, loglevel.upper(), None)
        if isinstance(numeric_level, int):
            self.log.setLevel(numeric_level)
        else:
            self.log.setLevel(logging.ERROR)
        self.logger.addHandler(self.log)

        self.ip = ip
        self.port = port
        self.nports = nports
        self.proc = None
        self.gstd_started = False
        self.logger.info('Starting GSTD instance with ip=%s port=%d nports=%d logfile=%s loglevel=%s', self.ip, self.port, self.nports, logfile, loglevel)
        self.test_gstd()

    def __del__(self):
        self.logger.info('Destroying GSTD instance with ip=%s port=%d', self.ip, self.port)
        if (self.gstd_started):
            self.logger.info('Killing GStreamer Daemon process...')
            self.proc.kill()
        self.logger.removeHandler(self.log)

    def socket_send(self, line):
        self.logger.debug('GSTD socket sending line: %s', line)
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((self.ip, self.port))
            s.send(' '.join(line).encode('utf-8'))
            data = recvall(s)
            data = data.decode('utf-8')
            s.close()
        except socket.error:
            s.close()
            self.logger.error('GSTD socket error')
            data = None
        self.logger.debug('GSTD socket received answer:\n %s', data)
        return data

    def start_gstd(self):
        try:
            gstd_bin = subprocess.check_output(['which',GSTD_PROCNAME])
            gstd_bin = gstd_bin.rstrip()
            self.logger.info('Startting GStreamer Daemon...')
            subprocess.Popen([gstd_bin, "-p", str(self.port), "-n", str(self.nports)])
            time.sleep(3)
            if self.test_gstd():
                self.gstd_started = True
                self.logger.info("GStreamer Daemon started successfully!")
                return True
            else:
                self.logger.info("GStreamer Daemon did not start correctly...")
                return False
        except subprocess.CalledProcessError:
            # Did not find gstd
            self.logger.error("GStreamer Daemon is not running and it is not installed.")
            self.logger.error("To get GStreamer Daemon, visit https://www.ridgerun.com/gstd.")
        return False

    def test_gstd(self):
        if self.ip not in ['localhost', '127.0.0.1']:
            # bypass process check, we don't know how to start gstd remotely
            self.logger.warning("Assuming GSTD is running in the remote host at %s" % self.ip )
            return True
        for proc in psutil.process_iter():
        # check whether the process name matches
            if proc.name() == GSTD_PROCNAME:
                self.proc = proc
        if self.proc or self.start_gstd():
            # we already had gstd or were able to start it.
            return True
        else:
            self.logger.error("GStreamer Daemon is not running and couldn't be started")
            return False

    def create(self, uri, property, value):
        self.logger.info('Creating property %s in uri %s with value "%s"', property, uri, value)
        self.send_cmd_line(['create', uri, property, value])

    def read(self, uri):
        self.logger.info('Reading uri %s', uri)
        result = self.send_cmd_line(['read', uri])
        return result['response']

    def update(self, uri, value):
        self.logger.info('Updating uri %s with value "%s"', uri, value)
        self.send_cmd_line(['update', uri, value])

    def delete(self, uri, name):
        self.logger.info('Deleting name %s at uri "%s"', name, uri)
        self.send_cmd_line(['delete', uri, name])

    def pipeline_create(self, pipe_name,  pipe_desc):
        self.logger.info('Creating pipeline %s with description "%s"', pipe_name, pipe_desc)
        self.send_cmd_line(['pipeline_create', pipe_name, pipe_desc])

    def pipeline_delete(self, pipe_name):
        self.logger.info('Deleting pipeline %s', pipe_name)
        self.send_cmd_line(['pipeline_delete', pipe_name])

    def pipeline_play(self, pipe_name):
        self.logger.info('Playing pipeline %s', pipe_name)
        self.send_cmd_line(['pipeline_play', pipe_name])

    def pipeline_pause(self, pipe_name):
        self.logger.info('Pausing pipeline %s', pipe_name)
        self.send_cmd_line(['pipeline_pause', pipe_name])

    def pipeline_stop(self, pipe_name):
        self.logger.info('Stoping pipeline %s', pipe_name)
        self.send_cmd_line(['pipeline_stop', pipe_name])

    def element_set(self, pipe_name, element, prop, value):
        self.logger.info('Setting element %s %s property in pipeline %s to:%s', element, prop, pipe_name, value)
        self.send_cmd_line(['element_set', pipe_name, "%s %s %s" % (element, prop, value) ])

    def element_get(self, pipe_name, element, prop):
        self.logger.info('Getting value of element %s %s property in pipeline %s', element, prop, pipe_name)
        result = self.send_cmd_line(cmd_line = ['element_get', pipe_name, "%s %s" % (element, prop) ])
        return result['response']['value']

    def list_pipelines(self):
        self.logger.info('Listing pipelines')
        result = self.send_cmd_line(cmd_line = ['list_pipelines'])
        return result['response']['nodes']

    def list_elements(self, pipe_name):
        self.logger.info('Listing elements of pipeline %s', pipe_name)
        result = self.send_cmd_line(cmd_line = ['list_elements', pipe_name])
        return result['response']['nodes']

    def list_properties(self, pipe_name, element):
        self.logger.info('Listing properties of  element %s from pipeline %s', element, pipe_name)
        result = self.send_cmd_line(['list_properties', pipe_name, element])
        return result['response']['nodes']

    def list_signals(self, pipe_name, element):
        self.logger.info('Listing signals of  element %s from pipeline %s', element, pipe_name)
        result = self.send_cmd_line(['list_signals', pipe_name, element])
        return result['response']['nodes']

    def bus_read(self, pipe_name):
        self.logger.info('Reading bus of pipeline %s', pipe_name)
        result = self.send_cmd_line(['bus_read', pipe_name])
        return result['response']

    def bus_filter(self, pipe_name, filter):
        self.logger.info('Setting bus read filter of pipeline %s to %s', pipe_name, filter)
        self.send_cmd_line(['bus_filter', pipe_name, filter])

    def bus_timeout(self, pipe_name, timeout):
        self.logger.info('Setting bus read timeout of pipeline %s to %s', pipe_name, timeout)
        self.send_cmd_line(['bus_timeout', pipe_name, timeout])

    def event_eos(self, pipe_name):
        self.logger.info('Sending end-of-stream event to pipeline %s', pipe_name)
        self.send_cmd_line(['event_eos', pipe_name])

    def event_seek (self, pipe_name, rate='1.0', format='3', flags='1', start_type='1', start='0', end_type='1', end='-1'):
        self.logger.info('Performing event seek in pipeline %s', pipe_name)
        self.send_cmd_line(['event_seek', pipe_name, rate, format, flags, start_type, start, end_type, end])

    def event_flush_start(self, pipe_name):
        self.logger.info('Putting pipeline %s in flushing mode', pipe_name)
        self.send_cmd_line(['event_flush_start', pipe_name])

    def event_flush_stop(self, pipe_name, reset='true'):
        self.logger.info('Taking pipeline %s out of flushing mode', pipe_name)
        self.send_cmd_line(['event_flush_stop', pipe_name, reset])

    def signal_connect(self, pipe_name, element, signal):
        self.logger.info('Connecting to signal %s of element %s from pipeline %s', signal, element, pipe_name)
        result = self.send_cmd_line(['signal_connect', pipe_name, element, signal])
        return result['response']

    def signal_timeout(self, pipe_name, element, signal, timeout):
        self.logger.info('Connecting to signal %s of element %s from pipeline %s with timeout %s', signal, element, pipe_name, timeout)
        self.send_cmd_line(['signal_timeout', pipe_name, element, signal, timeout])

    def signal_disconnect(self, pipe_name, element, signal):
        self.logger.info('Disonnecting from signal %s of element %s from pipeline %s', signal, element, pipe_name)
        self.send_cmd_line(['signal_disconnect', pipe_name, element, signal])

    def debug_enable(self, enable):
        self.logger.info('Enabling/Disabling GStreamer debug')
        self.send_cmd_line(['debug_enable', enable])

    def debug_threshold(self, threshold):
        self.logger.info('Setting GStreamer debug threshold to %s', threshold)
        self.send_cmd_line(['debug_threshold', threshold])

    def debug_color(self, colors):
        self.logger.info('Enabling/Disabling GStreamer debug colors')
        self.send_cmd_line(['debug_color', colors])

    def debug_reset(self, reset):
        self.logger.info('Enabling/Disabling GStreamer debug threshold reset')
        self.send_cmd_line(['debug_reset', reset])

    def send_cmd_line(self, cmd_line):
        cmd = cmd_line[0]
        try:
            jresult = self.socket_send(cmd_line)
            result = json.loads(jresult)
        except Exception as exception:
            self.logger.error('%s error: %s', cmd, type(exception).__name__)
            traceback.print_exc()
            raise GstcError(type(exception).__name__)
            return None
        if (result['code'] != 0):
            self.logger.error('%s error: %s', cmd, result['description'])
            raise GstdError(result['description'])
        return result
