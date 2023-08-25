# This file is part of GStreamer Daemon
# Python client library abstracting gstd interprocess communication
#
# Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import asyncio
import inspect
import json
import traceback

from pygstc.gstcerror import GstdError, GstcError, GstcErrorCode
from pygstc.logger import DummyLogger
from pygstc.tcp import Ipc

GSTD_PROCNAME = 'gstd'

"""
GstClient - GstdClient Class
"""


class GstdClient:

    """
    Class used as a client to communicate with the Gstd over
    an abstract inter-process communication class.

    Methods
    ----------
    ping_gstd ()
        Test if Gstd responds in the configured address and port
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
    pipeline_get_graph(self, pipe_name)
        Get the pipeline graph
    pipeline_verbose(self, pipe_name, value)
        Set the pipeline verbose mode
        Only supported on GST Version >= 1.10
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
        timeout=None,
    ):
        """
        Initialize new GstdClient.

        Parameters
        ----------
        ip : string
            IP where Gstd is running
        port : int
            Port where Gstd is running
        logger : CustomLogger
            Custom logger where all log messages from this class are going
            to be reported
        timeout : float
            Timeout in seconds to wait for a response. 0: non-blocking, None: blocking
        """

        if logger:
            self._logger = logger
        else:
            self._logger = DummyLogger()
        self._ip = ip
        self._port = port
        self._logger.info(
            'Starting GstClient with ip={} port={}'.format(
                self._ip, self._port))
        self._ipc = Ipc(self._logger, self._ip, self._port)
        self._timeout = timeout

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
                    "{} TypeError: parameter {}: expected {}, '{} found".format(
                        inspect.stack()[1].function,
                        i,
                        type_list[i],
                        type(parameter)),
                    GstcErrorCode.GSTC_MALFORMED)
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

    async def _send_cmd_line(self, cmd_line):
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
            jresult = await self._ipc.send(cmd_line, timeout=self._timeout)
            result = json.loads(jresult)
            if result['code'] != GstcErrorCode.GSTC_OK.value:
                self._logger.error(
                    '{} error: {}'.format(
                        cmd, result['description']))
                raise GstdError(result['description'],
                                result['code'])
            return result
        except ConnectionRefusedError as e:
            raise GstcError("Failed to communicate with Gstd",
                            GstcErrorCode.GSTC_UNREACHABLE)\
                from e
        except TypeError as e:
            raise GstcError('GstClient bad command',
                            GstcErrorCode.GSTC_TYPE_ERROR) from e
        except BufferError as e:
            raise GstcError('GstClient received a response bigger ' +
                            'than the maximum size allowed',
                            GstcErrorCode.GSTC_RECV_ERROR) from e
        except TimeoutError as e:
            raise GstcError('GstClient time out ocurred',
                            GstcErrorCode.GSTC_TIMEOUT) from e

    async def ping_gstd(self):
        """
        Test if Gstd responds in the configured address and port

        Raises
        ------
        GstcError
            Error is triggered when GstClient fails
        GstdError
            Error is triggered when Gstd IPC fails

        """
        self._logger.info('Sending ping to Gstd')
        try:
            jresult = await self._ipc.send(['list_pipelines'], timeout=1)
            # Verify correct data format
            result = json.loads(jresult)
            if ('description' in result and
               result['description'] != 'Success'):
                raise GstdError(result['description'],
                                result['code'])

        except json.JSONDecodeError as e:
            err_msg = 'Gstd corrupted response'
            self._logger.error(err_msg)
            raise GstcError(err_msg,
                            GstcErrorCode.GSTC_MALFORMED) from e
        except ConnectionRefusedError as e:
            err_msg = 'Error contacting Gstd'
            self._logger.error(err_msg)
            raise GstcError(err_msg,
                            GstcErrorCode.GSTC_UNREACHABLE) from e
        except BufferError as e:
            raise GstcError('GstClient received a buffer bigger ' +
                            'than the maximum size allowed',
                            GstcErrorCode.GSTC_RECV_ERROR) from e
        except TimeoutError as e:
            raise GstcError('GstClient time out ocurred',
                            GstcErrorCode.GSTC_TIMEOUT) from e

    async def bus_filter(self, pipe_name, filter):
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

        self._logger.info(
            'Setting bus read filter of pipeline {} to {}'.format(
                pipe_name, filter))
        parameters = self._check_parameters([pipe_name, filter], [str, str])
        await self._send_cmd_line(['bus_filter'] + parameters)

    async def bus_read(self, pipe_name):
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

        self._logger.info('Reading bus of pipeline {}'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        result = await self._send_cmd_line(['bus_read'] + parameters)
        return result['response']

    async def bus_timeout(self, pipe_name, timeout):
        """
        Apply a timeout for the bus polling.
        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        timeout: int
            Timeout in nanoseconds. -1: forever, 0: return
            immediately, n: wait n nanoseconds.

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info(
            'Setting bus read timeout of pipeline {} to {}'.format(
                pipe_name, timeout))
        parameters = self._check_parameters([pipe_name, timeout], [str, int])
        await self._send_cmd_line(['bus_timeout'] + parameters)

    async def create(
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

        self._logger.info(
            'Creating property {} in uri {} with value "{}"'.format(
                property, uri, value))
        parameters = self._check_parameters(
            [uri, property, value], [str, str, str])
        await self._send_cmd_line(['create'] + parameters)

    async def debug_color(self, colors):
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
        await self._send_cmd_line(['debug_color'] + parameters)

    async def debug_enable(self, enable):
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
        await self._send_cmd_line(['debug_enable'] + parameters)

    async def debug_reset(self, reset):
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
        await self._send_cmd_line(['debug_reset'] + parameters)

    async def debug_threshold(self, threshold):
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

        self._logger.info(
            'Setting GStreamer debug threshold to {}'.format(threshold))
        parameters = self._check_parameters([threshold], [str])
        await self._send_cmd_line(['debug_threshold'] + parameters)

    async def delete(self, uri, name):
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

        self._logger.info('Deleting name {} at uri "{}"'.format(name, uri))
        parameters = self._check_parameters([uri, name], [str, str])
        await self._send_cmd_line(['delete'] + parameters)

    async def element_get(
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
            'Getting value of element {} {} property in pipeline {}'.format(
                element, prop, pipe_name))
        parameters = self._check_parameters(
            [pipe_name, element, prop], [str, str, str])
        result = await self._send_cmd_line(['element_get'] + parameters)
        return result['response']['value']

    async def element_set(
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

        self._logger.info(
            'Setting element {} {} property in pipeline {} to:{}'.format(
                element, prop, pipe_name, value))
        parameters = self._check_parameters(
            [pipe_name, element, prop, value], [str, str, str, str])
        await self._send_cmd_line(['element_set'] + parameters)

    async def event_eos(self, pipe_name):
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

        self._logger.info(
            'Sending end-of-stream event to pipeline {}'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        await self._send_cmd_line(['event_eos'] + parameters)

    async def event_flush_start(self, pipe_name):
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

        self._logger.info(
            'Putting pipeline {} in flushing mode'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        await self._send_cmd_line(['event_flush_start'] + parameters)

    async def event_flush_stop(self, pipe_name, reset=True):
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

        self._logger.info(
            'Taking pipeline {} out of flushing mode'.format(pipe_name))
        parameters = self._check_parameters([pipe_name, reset], [str, bool])
        await self._send_cmd_line(['event_flush_stop'] + parameters)

    async def event_seek(
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

        self._logger.info(
            'Performing event seek in pipeline {}'.format(pipe_name))
        parameters = self._check_parameters(
            [
                pipe_name, rate, format, flags, start_type, start, end_type,
                end],
            [
                str, float, int, int, int, int, int, int])
        await self._send_cmd_line(['event_seek'] + parameters)

    async def list_elements(self, pipe_name):
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

        self._logger.info('Listing elements of pipeline {}'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        result = await self._send_cmd_line(['list_elements'] + parameters)
        return result['response']['nodes']

    async def list_pipelines(self):
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
        result = await self._send_cmd_line(['list_pipelines'])
        return result['response']['nodes']

    async def list_properties(self, pipe_name, element):
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

        self._logger.info(
            'Listing properties of  element {} from pipeline {}'.format(
                element, pipe_name))
        parameters = self._check_parameters([pipe_name, element], [str, str])
        result = await self._send_cmd_line(['list_properties'] + parameters)
        return result['response']['nodes']

    async def list_signals(self, pipe_name, element):
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

        self._logger.info(
            'Listing signals of  element {} from pipeline {}'.format(
                element, pipe_name))
        parameters = self._check_parameters([pipe_name, element], [str, str])
        result = await self._send_cmd_line(['list_signals'] + parameters)
        return result['response']['nodes']

    async def pipeline_create(self, pipe_name, pipe_desc):
        """
        Create a new pipeline based on the name and description.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        pipe_desc: string
            Pipeline description (same as gst-launch-1.0)
        """

        self._logger.info(
            'Creating pipeline {} with description "{}"'.format(
                pipe_name, pipe_desc))
        parameters = self._check_parameters([pipe_name, pipe_desc], [str, str])
        await self._send_cmd_line(['pipeline_create'] + parameters)

    async def pipeline_create_ref(self, pipe_name, pipe_desc):
        """
        Create a new pipeline based on the name and description using refcount.
        The refcount works similarly to GObject references. If the command
        is called but the refcount is greater than 0 nothing will happen
        and the refcount will increment.

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        pipe_desc: string
            Pipeline description (same as gst-launch-1.0)
        """

        self._logger.info(
            'Creating pipeline by reference {} with description "{}"'.format(
                pipe_name, pipe_desc))
        parameters = self._check_parameters([pipe_name, pipe_desc], [str, str])
        await self._send_cmd_line(['pipeline_create_ref'] + parameters)

    async def pipeline_delete(self, pipe_name):
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

        self._logger.info('Deleting pipeline {}'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        await self._send_cmd_line(['pipeline_delete'] + parameters)

    async def pipeline_delete_ref(self, pipe_name):
        """
        Delete the pipeline with the given name using refcount.
        The refcount works similarly to GObject references. If the command
        is called but the refcount is greater than 1 nothing will happen
        and the refcount will decrement.

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

        self._logger.info(
            'Deleting pipeline by reference {}'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        await self._send_cmd_line(['pipeline_delete_ref'] + parameters)

    async def pipeline_pause(self, pipe_name):
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

        self._logger.info('Pausing pipeline {}'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        await self._send_cmd_line(['pipeline_pause'] + parameters)

    async def pipeline_play(self, pipe_name):
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

        self._logger.info('Playing pipeline {}'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        await self._send_cmd_line(['pipeline_play'] + parameters)

    async def pipeline_play_ref(self, pipe_name):
        """
        Set the pipeline to playing using refcount.
        The refcount works similarly to GObject references. If the command
        is called but the refcount is greater than 0 nothing will happen
        and the refcount will increment.

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

        self._logger.info('Playing pipeline by reference {}'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        await self._send_cmd_line(['pipeline_play_ref'] + parameters)

    async def pipeline_stop(self, pipe_name):
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

        self._logger.info('Stoping pipeline {}'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        await self._send_cmd_line(['pipeline_stop'] + parameters)

    async def pipeline_stop_ref(self, pipe_name):
        """
        Set the pipeline to null using refcount.
        The refcount works similarly to GObject references. If the command
        is called but the refcount is greater than 1 nothing will happen
        and the refcount will decrement.

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

        self._logger.info('Stoping pipeline by reference {}'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        await self._send_cmd_line(['pipeline_stop_ref'] + parameters)

    async def pipeline_get_graph(self, pipe_name):
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

        self._logger.info('Getting the pipeline {} graph'.format(pipe_name))
        parameters = self._check_parameters([pipe_name], [str])
        result = await self._send_cmd_line(['pipeline_get_graph'] + parameters)
        return result

    async def pipeline_verbose(self, pipe_name, value):
        """
        Set the pipeline verbose mode.
        Only supported on GST Version >= 1.10

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

        self._logger.info(
            'Setting the pipeline {} verbose mode to {}'.format(
                pipe_name, value))
        parameters = self._check_parameters([pipe_name, value], [str, bool])
        await self._send_cmd_line(['pipeline_verbose'] + parameters)

    async def read(self, uri):
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

        self._logger.info('Reading uri {}'.format(uri))
        parameters = self._check_parameters([uri], [str])
        result = await self._send_cmd_line(['read'] + parameters)
        return result['response']

    async def signal_connect(
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
            'Connecting to signal {} of element {} from pipeline {}'.format(
                signal, element, pipe_name))
        parameters = self._check_parameters(
            [pipe_name, element, signal], [str, str, str])
        result = await self._send_cmd_line(['signal_connect'] + parameters)
        return result['response']

    async def signal_disconnect(
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
            'Disconnecting from signal {} of element {} from pipeline {}'.format(
                signal, element, pipe_name))
        parameters = self._check_parameters(
            [pipe_name, element, signal], [str, str, str])
        await self._send_cmd_line(['signal_disconnect'] + parameters)

    async def signal_timeout(
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
            Timeout in nanoseconds.  -1: forever, 0: return
            immediately, n: wait n microseconds.

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info(
            'Connecting to signal {} of element {} from pipeline {} with \
                timeout {}'.format(signal, element, pipe_name, timeout))
        parameters = self._check_parameters(
            [pipe_name, element, signal, timeout], [str, str, str, int])
        await self._send_cmd_line(['signal_timeout'] + parameters)

    async def action_emit(self, pipe_name, element, action):
        """
        Emits an action with no-parameters

        Parameters
        ----------
        pipe_name: string
            The name of the pipeline
        element: string
            The name of the element
        action: string
            The name of the action

        Raises
        ------
        GstdError
            Error is triggered when Gstd IPC fails
        GstcError
            Error is triggered when the Gstd python client fails internally
        """

        self._logger.info(
            'Triggering action {} of element {} from pipeline {}'.format(
                action, element, pipe_name))
        parameters = self._check_parameters(
            [pipe_name, element, action], [str, str, str])
        await self._send_cmd_line(['action_emit'] + parameters)

    async def update(self, uri, value):
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

        self._logger.info('Updating uri {} with value "{}"'.format(uri, value))
        parameters = self._check_parameters([uri, value], [str, str])
        await self._send_cmd_line(['update'] + parameters)
