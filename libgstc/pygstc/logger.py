# GStreamer Daemon - gst-launch on steroids
# Python client library abstracting gstd interprocess communication

# Copyright (c) 2015-2020 RidgeRun, LLC (http://www.ridgerun.com)

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:

# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.

# 2. Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following
# disclaimer in the documentation and/or other materials provided
# with the distribution.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.

import logging

# Add color to logging output

COLORS = {
    'WARNING': '33m',
    'INFO': '37m',
    'DEBUG': '34m',
    'CRITICAL': '35m',
    'ERROR': '31m',
}

"""
GSTC - ColorFormatter Class
"""


class ColorFormatter(logging.Formatter):

    """
    Custom implementation of logging Formater class.

    Methods
    ----------
    format(record)
        Formats the log and adds colors according to the log level
    """

    def __init__(self, msg):
        """
        Initialize new ColorFormatter.

        Parameters
        ----------
        msg : string
            Message to format
        """

        logging.Formatter.__init__(self, msg)

    def format(self, record):
        """
        Formats the log and adds colors according to the log level.

        Parameters
        ----------
        record : string
            Message to format
        """

        if record.levelname in COLORS:
            record.levelname = "\033[1;" + COLORS[record.levelname] \
                + record.levelname + "\033[0m"
        return logging.Formatter.format(self, record)


"""
GSTC - CustomLogger Class
"""


class CustomLogger:

    """
    Custom implementation of logging Logger class.

    Methods
    ----------
    warning(log)
        Logs a warning level message
    info(log)
        Logs an info level message
    debug(log)
        Logs a debug level message
    critical(log)
        Logs a critical level message
    error(log)
        Logs an error level message
    """

    def __init__(
        self,
        logname,
        logfile=None,
        loglevel='ERROR',
    ):
        """
        Initialize new CustomLogger.

        Parameters
        ----------
        logname : string
            Name of the logger
        logfile : string
            Path to save the logs. Forward to stdout if empty
        loglevel : string
            Level of the logger
        """

        # Init the logger

        self._logger = logging.getLogger(logname)
        self._logger.setLevel(logging.DEBUG)

        # Select to log in a file or console

        if logfile:

            # log in file

            self._log = logging.FileHandler(logfile)
        else:

            # log in console

            self._log = logging.StreamHandler()

        # Set log format with colors

        self._log.setFormatter(ColorFormatter(
            '%(asctime)22s  %(levelname)s    \t%(message)s'))

        # Set log level

        numeric_level = getattr(logging, loglevel.upper(), None)
        if isinstance(numeric_level, int):
            self._log_level = numeric_level
        else:
            self._log_level = logging.ERROR
        self._log.setLevel(self._log_level)
        self._logger.addHandler(self._log)

    def __del__(self):
        """
        Destroy a CustomLogger
        """

        if self._log:
            self._log.close()
            self._logger.removeHandler(self._log)

    def set_handler(self, handler):
        """
        Changes the default file or console handler to a custom one passed as a
        parameter.

        Parameters
        ----------
        handler : object
            The logging handler to use
        """

        # Remove the previous handler

        self._log.close()
        self._logger.removeHandler(self._log)

        # Add the new handler

        self._log = handler
        self._log.setFormatter(ColorFormatter(
            '%(asctime)22s  %(levelname)s    \t%(message)s'))
        self._log.setLevel(self._log_level)
        self._logger.addHandler(self._log)

    def warning(self, log):
        """
        Logs a warning level message.

        Parameters
        ----------
        log : string
            The message to log
        """

        return self._logger.warning(log)

    def info(self, log):
        """
        Logs an info level message.

        Parameters
        ----------
        log : string
            The message to log
        """

        return self._logger.info(log)

    def debug(self, log):
        """
        Logs a debug level message.

        Parameters
        ----------
        log : string
            The message to log
        """

        return self._logger.debug(log)

    def critical(self, log):
        """
        Logs a critical level message.

        Parameters
        ----------
        log : string
            The message to log
        """

        return self._logger.critical(log)

    def error(self, log):
        """
        Logs an error level message.

        Parameters
        ----------
        log : string
            The message to log
        """

        return self._logger.error(log)


"""
GSTC - DummyLogger Class
"""


class DummyLogger:

    """
    Dummy implementation of the logging Logger that discards all logs.

    Methods
    ----------
    warning(log)
        Discards the log
    info(log)
        Discards the log
    debug(log)
        Discards the log
    critical(log)
        Discards the log
    error(log)
        Discards the log
    """

    def __init__(self):
        """
        Initialize DummyLogger.
        """

        pass

    def __del__(self):
        """
        Destroy DummyLogger.
        """

        pass

    def warning(self, log):
        """
        Discards the log.
        """

        pass

    def info(self, log):
        """
        Discards the log.
        """

        pass

    def debug(self, log):
        """
        Discards the log.
        """

        pass

    def critical(self, log):
        """
        Discards the log.
        """

        pass

    def error(self, log):
        """
        Discards the log.
        """

        pass
