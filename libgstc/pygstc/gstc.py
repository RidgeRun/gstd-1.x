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

import inspect
import json
import traceback

from pygstc.gstcerror import GstdError, GstcError
from pygstc.logger import DummyLogger
from pygstc.tcp import Ipc

GSTD_PROCNAME = 'gstd'

"""
GSTC - GstdClient Class
"""


class GstdClient:

    """
    Class used as client to comunicate with the GStreamer Daemon over
    an abstract inter-process communication class.

    Methods
    ----------
    ping_gstd ()
        Test if GSTD responds in the configured address and port
    bus_filter(pipe_name, filter)
        Select the types of message to be read from the bus. Separate
        with a '+', i.e.: eos+warning+error
    bus_read(pipe_name)
        Read the bus and wait
    bus_timeout(pipe_name, timeout)
        Apply a timeout for the bus polling. -1: forever, 0: return
        immediately, n: wait n nanoseconds
    create(uri, property, value)
        Create a resource at the given URI
    debug_color(colors)
        Enable/Disable colors in the debug logging
    debug_enable(enable)
        Enable/Disable GStreamer debug
    debug_reset(reset)
        Enable/Disable debug threshold reset
    debug_threshold(threshold)
        The debug filter to apply (as you would use with gst-launch)
    delete(uri, name)
        Delete the resource held at the given URI with the given name
    element_get(pipe_name, element, prop)
        Queries a property in an element of a given pipeline
    element_set(pipe_name, element, prop, value)
        Set a property in an element of a given pipeline
    event_eos(pipe_name)
        Send an end-of-stream event
    event_flush_start(pipe_name)
        Put the pipeline in flushing mode
    event_flush_stop(pipe_name, reset='true')
        Take the pipeline out from flushing mode
    event_seek(
        self,
        pipe_name,
        rate=1.0,
        format=3,
        flags=1,
        start_type=1,
        start=0,
        end_type=1,
        end=-1,
        )
        Perform a seek in the given pipeline
    list_elements(pipe_name)
        List the elements in a given pipeline
    list_pipelines( )
        List the existing pipelines
    list_properties(pipe_name, element)
        List the properties of an element in a given pipeline
    list_signals(pipe_name, element)
        List the signals of an element in a given pipeline
    pipeline_create(pipe_name,  pipe_desc)
        Create a new pipeline based on the name and description
    pipeline_delete(pipe_name)
        Delete the pipeline with the given name
    pipeline_pause(pipe_name)
        Set the pipeline to paused
    pipeline_play(pipe_name)
        Set the pipeline to playing
    pipeline_stop(pipe_name)
        Set the pipeline to null
    read(uri)
        Read the resource held at the given URI with the given name
    signal_connect(pipe_name, element, signal)
        Connect to signal and wait
    signal_disconnect(pipe_name, element, signal)
        Disconnect from signal
    signal_timeout(pipe_name, element, signal, timeout)
        Apply a timeout for the signal waiting. -1: forever, 0: return
        immediately, n: wait n microseconds
    update(uri, value)
        Update the resource at the given URI
    """

    def __init__(
        self,
        ip='localhost',
        port=5000,
        logger=None,
        timeout=0,
    ):
        """
        Initialize new GstdClient.

        Parameters
        ----------
        ip : string
            IP where GSTD is running
        port : int
            Port where GSTD is running
        logger : CustomLogger
            Custom logger where all log messages from this class are going
            to be reported
        """

        if logger:
            self._logger = logger
        else:
            self._logger = DummyLogger()
        self._ip = ip
        self._port = port
        self._logger.info('Starting GStreamer Daemon Client with ip=%s port=%d'
                          % (self._ip, self._port))
        self._ipc = Ipc(self._logger, self._ip, self._port)
        self._timeout = timeout
        self.ping_gstd()

    def _check_parameters(self, parameter_list, type_list):
        """
        Checks that every parameter in the parameter list corresponds to the
        type in type_list. Then returns an array with the string conversion of
        each value.

        Parameters
        ----------
        parameter_list: list
            The parameters to check and convert
        type_list: list
            List of types for each parameter

        Returns
        -------
        parameter_string_list : list
            List of string conversions of each parameter
        """
        parameter_string_list = []
        for i, parameter in enumerate(parameter_list):
            if not isinstance(parameter, type_list[i]):
                raise GstcError(
                    "%s TypeError: parameter %i: expected %s, '%s found" %
                    (inspect.stack()[1].function, i, type_list[i],
                     type(parameter)), -6)
            if type_list[i] == str:
                parameter_string_list += [parameter]
            elif type_list[i] == bool:
                if parameter:
                    parameter_string_list += ['true']
                else:
                    parameter_string_list += ['false']
            else:
                parameter_string_list += [str(parameter)]
        return parameter_string_list

    def _send_cmd_line(self, cmd_line):
        """
        Send a command using an abstract IPC and wait for the response.

        Parameters
        ----------
        cmd_line : string list
            Command to be send

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally

        Returns
        -------
        result : dictionary
            Response from the IPC
        """
        try:
            cmd = cmd_line[0]
            jresult = self._ipc.send(cmd_line, timeout=self._timeout)
            result = json.loads(jresult)
            if result['code'] != 0:
                self._logger.error('%s error: %s' % (cmd,
                                                     result['description']))
                raise GstcError(result['description'], -7)
            return result
        except ConnectionRefusedError as e:
            raise GstdError("Failed to communicate with GSTD", 15)\
                from e
        except TypeError as e:
            raise GstcError('Bad command', -1) from e

    def ping_gstd(self):
        """
        Test if GSTD responds in the configured address and port

        Raises
        ------
        GstcError
            Error is triggered when Gst Client fails
        GstdError
            Error is triggered when Gstd IPC fails

        """
        self._logger.info('Sending ping to GStreamer Daemon')
        try:
            jresult = self._ipc.send(['list_pipelines'], timeout=1)
            # Verify correct data format
            result = json.loads(jresult)
            if ('description' in result and
               result['description'] != 'Success'):
                raise GstdError("GStreamer Daemon bad response", 15)

        except json.JSONDecodeError as e:
            self._logger.error('GStreamer Daemon corrupted response')
            raise GstcError("GStreamer Daemon corrupted response", 13) from e
        except ConnectionRefusedError as e:
            self._logger.error('Error contacting GST Daemon')
            raise GstdError('Error contacting GST Daemon', 15) from e

    def bus_filter(self, pipe_name, filter):
        """
        Select the types of message to be read from the bus. Separate
        with a '+', i.e.: eos+warning+error.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        filter: string
            Filter to be applied to the bus. '+' reparated strings

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Setting bus read filter of pipeline %s to %s'
                          % (pipe_name, filter))
        parameters = self._check_parameters([pipe_name, filter], [str, str])
        self._send_cmd_line(['bus_filter'] + parameters)

    def bus_read(self, pipe_name):
        """
        Read the bus and wait.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally

        Returns
        -------
        result : dictionary
            Command response
        """

        self._logger.info('Reading bus of pipeline %s' % pipe_name)
        parameters = self._check_parameters([pipe_name], [str])
        result = self._send_cmd_line(['bus_read'] + parameters)
        return result['response']

    def bus_timeout(self, pipe_name, timeout):
        """
        Apply a timeout for the bus polling.
        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        timeout: int
            Timeout in nanosecons. -1: forever, 0: return
            immediately, n: wait n nanoseconds.

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Setting bus read timeout of pipeline %s to %s'
                          % (pipe_name, timeout))
        parameters = self._check_parameters([pipe_name, timeout], [str, int])
        self._send_cmd_line(['bus_timeout'] + parameters)

    def create(
        self,
        uri,
        property,
        value,
    ):
        """
        Create a resource at the given URI.

        Parameters
        ----------
        uri: string
            Resource identifier
        property: string
            The name of the property
        value: string
            The initial value to be set

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Creating property %s in uri %s with value "%s"'
                          % (property, uri, value))
        parameters = self._check_parameters(
            [uri, property, value], [str, str, str])
        self._send_cmd_line(['create'] + parameters)

    def debug_color(self, colors):
        """
        Enable/Disable colors in the debug logging.

        Parameters
        ----------
        colors: boolean
            Enable color in the debug

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Enabling/Disabling GStreamer debug colors')
        parameters = self._check_parameters([colors], [bool])
        self._send_cmd_line(['debug_color'] + parameters)

    def debug_enable(self, enable):
        """
        Enable/Disable GStreamer debug.

        Parameters
        ----------
        enable: boolean
            Enable GStreamer debug

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Enabling/Disabling GStreamer debug')
        parameters = self._check_parameters([enable], [bool])
        self._send_cmd_line(['debug_enable'] + parameters)

    def debug_reset(self, reset):
        """
        Enable/Disable debug threshold reset.

        Parameters
        ----------
        reset: boolean
            Reset the debug threshold

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Enabling/Disabling GStreamer debug threshold reset')
        parameters = self._check_parameters([reset], [bool])
        self._send_cmd_line(['debug_reset'] + parameters)

    def debug_threshold(self, threshold):
        """
        The debug filter to apply (as you would use with gst-launch).

        Parameters
        ----------
        threshold: string
            Debug threshold:
            0   none    No debug information is output.
            1   ERROR   Logs all fatal errors.
            2   WARNING Logs all warnings.
            3   FIXME   Logs all "fixme" messages.
            4   INFO    Logs all informational messages.
            5   DEBUG   Logs all debug messages.
            6   LOG     Logs all log messages.
            7   TRACE   Logs all trace messages.
            9   MEMDUMP Logs all memory dump messages.

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Setting GStreamer debug threshold to %s'
                          % threshold)
        parameters = self._check_parameters([threshold], [str])
        self._send_cmd_line(['debug_threshold'] + parameters)

    def delete(self, uri, name):
        """
        Delete the resource held at the given URI with the given name.

        Parameters
        ----------
        uri: string
            Resource identifier
        name: string
            The name of the resource to delete

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Deleting name %s at uri "%s"' % (name, uri))
        parameters = self._check_parameters([uri, name], [str, str])
        self._send_cmd_line(['delete'] + parameters)

    def element_get(
        self,
        pipe_name,
        element,
        prop,
    ):
        """
        Queries a property in an element of a given pipeline.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        element: string
            The name of the element
        prop: string
            The name of the property

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally

        Returns
        -------
        result : string
            Command response
        """

        self._logger.info(
            'Getting value of element %s %s property in pipeline %s' %
            (element, prop, pipe_name))
        parameters = self._check_parameters(
            [pipe_name, element, prop], [str, str, str])
        result = self._send_cmd_line(['element_get'] + parameters)
        return result['response']['value']

    def element_set(
        self,
        pipe_name,
        element,
        prop,
        value,
    ):
        """
        Set a property in an element of a given pipeline.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        element: string
            The name of the element
        prop: string
            The name of the property
        value: string
            The value to set

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Setting element %s %s property in pipeline %s to:%s'
                          % (element, prop, pipe_name, value))
        parameters = self._check_parameters(
            [pipe_name, element, prop, value], [str, str, str, str])
        self._send_cmd_line(['element_set'] + parameters)

    def event_eos(self, pipe_name):
        """
        Send an end-of-stream event.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Sending end-of-stream event to pipeline %s'
                          % pipe_name)
        parameters = self._check_parameters([pipe_name], [str])
        self._send_cmd_line(['event_eos'] + parameters)

    def event_flush_start(self, pipe_name):
        """
        Put the pipeline in flushing mode.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Putting pipeline %s in flushing mode'
                          % pipe_name)
        parameters = self._check_parameters([pipe_name], [str])
        self._send_cmd_line(['event_flush_start'] + parameters)

    def event_flush_stop(self, pipe_name, reset=True):
        """
        Take the pipeline out from flushing mode.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        reset: boolean
            Reset the event flush

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Taking pipeline %s out of flushing mode'
                          % pipe_name)
        parameters = self._check_parameters([pipe_name, reset], [str, bool])
        self._send_cmd_line(['event_flush_stop'] + parameters)

    def event_seek(
        self,
        pipe_name,
        rate=1.0,
        format=3,
        flags=1,
        start_type=1,
        start=0,
        end_type=1,
        end=-1,
    ):
        """
        Perform a seek in the given pipeline

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        rate: float
            The new playback rate. Default value: 1.0.
        format: int
            The format of the seek values. Default value: 3.
        flags: int
            The optional seek flags. Default value: 1.
        start_type: int
            The type and flags for the new start position. Default value: 1.
        start: int
            The value of the new start position. Default value: 0.
        end_type: int
            The type and flags for the new end position. Default value: 1.
        end: int
            The value of the new end position. Default value: -1.

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Performing event seek in pipeline %s'
                          % pipe_name)
        parameters = self._check_parameters(
            [
                pipe_name, rate, format, flags, start_type, start, end_type,
                end],
            [
                str, float, int, int, int, int, int, int])
        self._send_cmd_line(['event_seek'] + parameters)

    def list_elements(self, pipe_name):
        """
        List the elements in a given pipeline.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally

        Returns
        -------
        result : string
            List of elements
        """

        self._logger.info('Listing elements of pipeline %s' % pipe_name)
        parameters = self._check_parameters([pipe_name], [str])
        result = self._send_cmd_line(['list_elements'] + parameters)
        return result['response']['nodes']

    def list_pipelines(self):
        """
        List the existing pipelines

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally

        Returns
        -------
        result : string
            List of pipelines
        """

        self._logger.info('Listing pipelines')
        result = self._send_cmd_line(['list_pipelines'])
        return result['response']['nodes']

    def list_properties(self, pipe_name, element):
        """
        List the properties of an element in a given pipeline.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        element: string
            The name of the element

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally

        Returns
        -------
        result : string
            List of properties
        """

        self._logger.info('Listing properties of  element %s from pipeline %s'
                          % (element, pipe_name))
        parameters = self._check_parameters([pipe_name, element], [str, str])
        result = self._send_cmd_line(['list_properties'] + parameters)
        return result['response']['nodes']

    def list_signals(self, pipe_name, element):
        """
        List the signals of an element in a given pipeline.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        element: string
            The name of the element

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally

        Returns
        -------
        result : string
            List of signals
        """

        self._logger.info('Listing signals of  element %s from pipeline %s'
                          % (element, pipe_name))
        parameters = self._check_parameters([pipe_name, element], [str, str])
        result = self._send_cmd_line(['list_signals'] + parameters)
        return result['response']['nodes']

    def pipeline_create(self, pipe_name, pipe_desc):
        """
        Create a new pipeline based on the name and description.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        pipe_desc: string
            Pipeline description (same as gst-launch-1.0)
        """

        self._logger.info('Creating pipeline %s with description "%s"'
                          % (pipe_name, pipe_desc))
        parameters = self._check_parameters([pipe_name, pipe_desc], [str, str])
        self._send_cmd_line(['pipeline_create'] + parameters)

    def pipeline_delete(self, pipe_name):
        """
        Delete the pipeline with the given name.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Deleting pipeline %s' % pipe_name)
        parameters = self._check_parameters([pipe_name], [str])
        self._send_cmd_line(['pipeline_delete'] + parameters)

    def pipeline_pause(self, pipe_name):
        """
        Set the pipeline to paused.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Pausing pipeline %s' % pipe_name)
        parameters = self._check_parameters([pipe_name], [str])
        self._send_cmd_line(['pipeline_pause'] + parameters)

    def pipeline_play(self, pipe_name):
        """
        Set the pipeline to playing.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Playing pipeline %s' % pipe_name)
        parameters = self._check_parameters([pipe_name], [str])
        self._send_cmd_line(['pipeline_play'] + parameters)

    def pipeline_stop(self, pipe_name):
        """
        Set the pipeline to null.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Stoping pipeline %s' % pipe_name)
        parameters = self._check_parameters([pipe_name], [str])
        self._send_cmd_line(['pipeline_stop'] + parameters)

    def pipeline_get_graph(self, pipe_name):
        """
        Get the pipeline graph.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally

        Returns
        -------
        result : string
            Pipeline graph in GraphViz dot format
        """

        self._logger.info('Getting the pipeline %s graph' % pipe_name)
        parameters = self._check_parameters([pipe_name], [str])
        result = self._send_cmd_line(['pipeline_get_graph'] + parameters)
        return result

    def pipeline_verbose(self, pipe_name, value):
        """
        Set the pipeline verbose mode.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        value: boolean
            True or False to activate or deactivate

        Raises
        ------
        GstdError
            Error is triggered when the Gstd sever fails internally
        GstcError
            Error is triggered when Gstd IPC fails
        """

        self._logger.info('Setting the pipeline %s verbose mode to %s'
                          % (pipe_name, value))
        parameters = self._check_parameters([pipe_name, value], [str, bool])
        self._send_cmd_line(['pipeline_verbose'] + parameters)

    def read(self, uri):
        """
        Read the resource held at the given URI with the given name.

        Parameters
        ----------
        uri: string
            Resource identifier

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally

        Returns
        -------
        result : string
            Command response
        """

        self._logger.info('Reading uri %s' % uri)
        parameters = self._check_parameters([uri], [str])
        result = self._send_cmd_line(['read'] + parameters)
        return result['response']

    def signal_connect(
        self,
        pipe_name,
        element,
        signal,
    ):
        """
        Connect to signal and wait.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        element: string
            The name of the element
        signal: string
            The name of the signal

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally

        Returns
        -------
        result : string
            Command response
        """

        self._logger.info(
            'Connecting to signal %s of element %s from pipeline %s' %
            (signal, element, pipe_name))
        parameters = self._check_parameters(
            [pipe_name, element, signal], [str, str, str])
        result = self._send_cmd_line(['signal_connect'] + parameters)
        return result['response']

    def signal_disconnect(
        self,
        pipe_name,
        element,
        signal,
    ):
        """
        Disconnect from signal.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        element: string
            The name of the element
        signal: string
            The name of the signal

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info(
            'Disonnecting from signal %s of element %s from pipeline %s' %
            (signal, element, pipe_name))
        parameters = self._check_parameters(
            [pipe_name, element, signal], [str, str, str])
        self._send_cmd_line(['signal_disconnect'] + parameters)

    def signal_timeout(
        self,
        pipe_name,
        element,
        signal,
        timeout,
    ):
        """
        Apply a timeout for the signal waiting.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        element: string
            The name of the element
        signal: string
            The name of the signal
        timeout: int
            Timeout in nanosecons.  -1: forever, 0: return
            immediately, n: wait n microseconds.

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info(
            'Connecting to signal %s of element %s from pipeline %s with \
                timeout %s' % (signal, element, pipe_name, timeout))
        parameters = self._check_parameters(
            [pipe_name, element, signal, timeout], [str, str, str, int])
        self._send_cmd_line(['signal_timeout'] + parameters)

    def update(self, uri, value):
        """
        Update the resource at the given URI.

        Parameters
        ----------
        uri: string
            Resource identifier
        value: string
            The value to set

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info('Updating uri %s with value "%s"' % (uri,
                                                               value))
        parameters = self._check_parameters([uri, value], [str, str])
        self._send_cmd_line(['update'] + parameters)
