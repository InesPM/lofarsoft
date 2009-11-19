import cPickle
import logging.handlers
import SocketServer
import struct
import threading
import select
import socket
from contextlib import contextmanager

@contextmanager
def clusterlogger(
    logger,
    host=None,
    port=logging.handlers.DEFAULT_TCP_LOGGING_PORT
):
    if not host:
        host = scoket.gethostname()
    logserver = LogRecordSocketReceiver(logger)
    logserver.start()
    yield logserver.server_address
    logserver.stop()

class LogRecordStreamHandler(SocketServer.StreamRequestHandler):
    """
    Handler for a streaming logging request.

    This basically logs the record using whatever logging policy is
    configured locally.
    """

    def handle(self):
        """
        Handle multiple requests - each expected to be a 4-byte length,
        followed by the LogRecord in pickle format. Logs the record
        according to whatever policy is configured locally.
        """
        while 1:
            chunk = self.connection.recv(4)
            if len(chunk) < 4:
                break
            slen = struct.unpack(">L", chunk)[0]
            chunk = self.connection.recv(slen)
            while len(chunk) < slen:
                chunk = chunk + self.connection.recv(slen - len(chunk))
            obj = self.unPickle(chunk)
            record = logging.makeLogRecord(obj)
            self.handleLogRecord(record)

    def unPickle(self, data):
        return cPickle.loads(data)

    def handleLogRecord(self, record):
        self.server.logger.handle(record)

class LogRecordSocketReceiver(SocketServer.ThreadingTCPServer):
    """
    simple TCP socket-based logging receiver suitable for testing.
    """

    allow_reuse_address = 1

    def __init__(self,
        logger,
        host=None,
        port=logging.handlers.DEFAULT_TCP_LOGGING_PORT,
        handler=LogRecordStreamHandler
    ):
        # If not specified, asssume local hostname
        if not host:
            host = socket.gethostname()
        SocketServer.ThreadingTCPServer.__init__(self, (host, port), handler)
        self.abort = False
        self.timeout = 1
        self.logger = logger

    def start(self):
        def loop_in_thread():
            while not self.abort:
                rd, wr, ex = select.select(
                    [self.socket.fileno()], [], [], self.timeout
                )
                if rd:
                    self.handle_request()
        self.runthread = threading.Thread(target=loop_in_thread)
        self.runthread.start()

    def stop(self):
        self.abort = True
        self.runthread.join()
        self.server_close()
