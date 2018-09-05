/*
 * GStreamer Daemon - gst-launch on steroids
 * C client library abstracting gstd interprocess communication
 *
 * Copyright (c) 2015-2018 RidgeRun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * SECTION:libgstc
 * @short_description: Gst client library for C programming language
 * @title: Gstc Library
 * @see_also:#GstClient
 *
 *
 * The #GstClient library gives user applications access to the
 * GStreamer Daemon using the C programming language.
 *
 * A #GstClient is created along with one or more GStreamer pipelines.
 * The pipelines states are controlled, along with any element
 * properties.  Once an application is done with a pipeline, it can be
 * deleted.  To release all resources assoicated with the #GstClient,
 * the client can be released.
 *
 * Usage Notes:
 *
 * 1 ) None of the gstc_* functions block longer than the wait_time
 *     specified in the gstc_client_new() call.  The libgstc library
 *     is multi-threaded and will handle any transaction with
 *     GStreamer Daemon that takes longer than wait_time to complete.
 *     Call gst_client_release() to free the resources and threads
 *     used by the library.
 *
 * 2 ) The bus wait functions are asynchronous calls where a call back
 *     method is passed. To remove a function from the call back list,
 *     the call back function should return a value of zero.
 *
 * 3 ) Because of the lack of polymoriphism in the C programming
 *     language, values passed into or out of the API are essientally
 *     strings.  Because the calling application know the datatype,
 *     the calling application handles the data conversion.  To make
 *     this simple, when a value is passed into the libgstc API, a
 *     printf style format string is included to make it easy for the
 *     value to be turned into string.  When a value is passed back to
 *     the calling application, a scanf style format string is is
 *     included so the library can turn the string value back into the
 *     datatype most easily handled by the calling application.  The
 *     recommended format strings are listed below for each GStreamer
 *     datatype
 *
 *     GStreamer              printf           sscanf
 *     Datatype               format           format
 *                  REVISIT - fill out the table
 */

#ifndef __LIBGSTC_H__
#define __LIBGSTC_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * GstcStatus:
 * @GSTC_OK: Everything went okay
 * @GSTC_NULL_ARGUMENT: A mandatory argument was passed in as NULL
 * @GSTC_UNREACHABLE: Unable to reach the server
 * @GSTC_TIMEOUT: The server has timed out before responding
 * @GSTC_OOM: The system has run out of memory
 * @GSTC_TYPE_ERROR: An error occurred parsing a type from a string
 * @GSTC_MALFORMED: An attempt to parse a malformed JSON was made
 * @GSTC_NOT_FOUND: The response is missing the field requested
 * @GSTC_SEND_ERROR: There was a problem sending the request
 * @GSTC_RECV_ERROR: There was a problem receiving the response
 * @GSTC_SOCKET_ERROR: Unable to open the network socket
 * @GSTC_THREAD_ERROR: Unable to create a new thread
 * @GSTC_BUS_TIMEOUT: A timeout was received while waiting on the bus
 *
 * Return codes for the different libgstc operations
 */
typedef enum
{
  GSTC_OK,
  GSTC_NULL_ARGUMENT,
  GSTC_UNREACHABLE,
  GSTC_TIMEOUT,
  GSTC_OOM,
  GSTC_TYPE_ERROR,
  GSTC_MALFORMED,
  GSTC_NOT_FOUND,
  GSTC_SEND_ERROR,
  GSTC_RECV_ERROR,
  GSTC_SOCKET_ERROR,
  GSTC_THREAD_ERROR,
  GSTC_BUS_TIMEOUT
} GstcStatus;

/**
 * GstClient:
 * Opaque representation of the client state
 */
typedef struct _GstClient GstClient;

/**
 * gstc_client_new:
 * @address: The address to bind to
 * @port: The port to bind to
 * @wait_time: time to wait for a response from the daemon before
 * returning an error, applies to all non-blocking gstc_* methods.
 * Zero means wait forever.
 * @keep_connection_open: if non-zero the underlying network socket
 * will be kept open until gstc_client_free() is called
 * @client: placeholder for newly allocated client.
 *
 * Creates a new connection object to the GStreamer Daemon session in
 * @address and @port.  Does not exchange any data with the
 * daemon. When successful, a newly allocated client is returned
 * through @client. If an error occurs, the appropriate status is
 * returned and @client is not valid.
 *
 * Returns: GstcStatus indicating success, daemon unreachable, out of
 * memory.
 */
GstcStatus gstc_client_new (const char *address, const unsigned int port,
    const unsigned long wait_time, const int keep_connection_open, GstClient ** client);

/**
 * gstc_client_free:
 * @client: A valid client allocated with gstc_client_new()
 *
 * Frees a previously allocated GstClient.
 *
 * Returns: A newly allocated GstClient. Use gstc_client_free() after
 * usage.
 */
void
gstc_client_free (GstClient *client);

/**
 * gstc_client_ping:
 * @client: The client returned by gstc_client_new()
 *
 * Verifies connectivy with GStreamer Daemon.
 *
 * Returns: GstcStatus indicating success, daemon unreachable, daemon timeout
 */
GstcStatus gstc_client_ping(GstClient *client);

/**
 * gstc_pipeline_create:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name to associate to the pipeline
 * @pipeline_desc: The gst-launch style pipeline description to create
 *
 * Creates a new GStreamer pipeline that can be referred to using
 * @pipeline_name.
 *
 * Returns: GstcStatus indicating success, daemon unreachable, daemon
 * timeout, bad pipeline
 */
GstcStatus
gstc_pipeline_create (GstClient *client, const char *pipeline_name,
    const char *pipeline_desc);
  
/**
 * gstc_pipeline_list:
 * @client: The client returned by gstc_client_new()
 * @pipelines: List of existing pipelines names returned by the client library
 * @list_lenght: Number of elements in the pipelines list
 *
 * Returns a list of the names of the existing pipelines.  Depending on the
 * deployment, another  application may have created some of the pipelines.
 * The client application needs to do a free(*pipelines) and a
 * free(*pipelines[idx]) to release the resources used to hold the list and
 * its elements
 *
 * Returns: GstcStatus indicating success, daemon unreachable, daemon timeout,
 * out of memory
 */
GstcStatus gstc_pipeline_list(GstClient *client, char **pipelines[],
  int *list_lenght);

/**
 * gstc_pipeline_delete:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name assoicated with the pipeline
 *
 * Deletes a previously created GStreamer pipeline named @pipeline_name.
 *
 * Returns: GstcStatus indicating success, daemon unreachable, daemon
 * timeout, bad pipeline name
 */
GstcStatus
gstc_pipeline_delete(GstClient *client, const char *pipeline_name);

/**
 * gstc_pipeline_play:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name assoicated with the pipeline
 *
 * Attempts to change the named pipeline to the play state.
 *
 * Returns: GstcStatus indicating success, daemon unreachable, daemon
 * timeout, bad pipeline name, unable to change pipeline state
 */
GstcStatus
gstc_pipeline_play(GstClient *client, const char *pipeline_name);

/**
 * gstc_pipeline_pause:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name assoicated with the pipeline
 *
 * Attempts to change the named pipeline to the paused state.
 *
 * Returns: GstcStatus indicating success, daemon unreachable, daemon
 * timeout, bad pipeline name, unable to change pipeline state
 */
GstcStatus
gstc_pipeline_pause(GstClient *client, const char *pipeline_name);

/**
 * gstc_pipeline_stop:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name assoicated with the pipeline
 *
 * Attempts to change the named pipeline to the null state.
 *
 * Returns: GstcStatus indicating success, daemon unreachable, daemon
 * timeout, bad pipeline name, unable to change pipeline state
 */
GstcStatus
gstc_pipeline_stop(GstClient *client, const char *pipeline_name);

/**
 * gstc_element_set:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name assoicated with the pipeline
 * @element_name: Element name to be set
 * @property_name: Property name to be set
 * @format: Zero or more directives: ordinary characters (not %), are
 * copied unchanged to the output stream; and conversion
 * specifications, each of which results in fetching zero or more
 * subsequent arguments.  Each conversion specification is introduced
 * by the % character.
 * @...: One or more arguments corresponding to properly (after type
 *  promotion) the matching conversion specifier in format.
 *
 * Sets the value of the specified element property.
 *
 * Returns: GstcStatus indicating success, daemon unreachable, daemon
 * timeout, bad pipeline name, bad element name, out of memory
 */
GstcStatus gstc_element_set(GstClient *client, const char *pname,
    const char *element, const char *parameter, const char *format, ...);

/**
 * gstc_pipeline_flush_start:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Causes elements in the pipeline to unblock and discard any pipeline data.
 * Returns: GstcStatus indicating success, daemon unreachable, daemon timeout,
 * bad pipeline name, out of memory
 *
 */
GstcStatus gstc_pipeline_flush_start(const GstClient *client, const char *pipeline_name);

/**
 * gstc_pipeline_flush_stop:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name associated with the pipeline
 * @reset: if non-zero the running_time of the pipeline is set back to 0
 *
 *  Causes elements in the pipeline to process pipeline data normally.
 *
 * Returns: GstcStatus indicating success, daemon unreachable, daemon timeout,
 * bad pipeline name, out of memory
 */
GstcStatus gstc_pipeline_flush_stop(const GstClient *client, const char *pipeline_name,
  const int reset);

/**
 * gstc_pipeline_inject_eos:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Injects an end-of-stream marker into the named pipeline. The EOS
 * event is useful for notifying the elements in a pipeline that no
 * further buffers are expected and they can wrap up any processing
 * required to properly close the stream. Effectively causes gstd to
 * invoke gst_event_new_eos().
 *
 * Returns: GstcStatus indicating success, daemon unreachable, daemon
 * timeout, bad pipeline name, out of memory
 */
GstcStatus gstc_pipeline_inject_eos (GstClient *client,
    const char *pipeline_name);

/**
 * gstc_pipeline_seek:
 * @client: The client returned by gstc_client_new()
 * @pname: Name associated with the pipeline
 * @rate: New playback rate, e.g. 1 is normal, 0.5 is half speed,
 * 2 is twice as fast as normal.  Negatives values means backwards playback.
 * @format: Format for seek values: undefined (0), default (1), bytes (2),
 * nanoseconds(3), buffers (4) or percent (5) where default means to use the
 * format of the pad/element, nanoseconds means time in nanoseconds and percentage
 * means percentage of the stream.
 * @flags: Seek flags: none (0), flush (1), accurate (2), key unit (3),
 * segment (4), trickmode (5), skip (6), snap before (7), snap after (8),
 * snap nearest (9), trickmode key units (10) or trickmode no audio (11)
 * where none means no flags, flush means that the pipeline should be flushed,
 * accurate means accurate position but might be slower for some formats,
 * key unit means seek to the nearest frame which is faster but less accurate,
 * segment means to performa a segment seek, trickmode means that it allows
 * elements to skip frames, snap before means go to a location before the requested
 * position, snap after means to go to a position after the requeste position,
 * snap nearest means to go to the closest to the request position, trickmode key
 * unit means that when doing fast forwarding or fast reverse playback it only
 * should only decode keyframes and skip all other content, trickmode no audio
 * means that the audio encoder elements be skipped
 * @start_type: none (0), set (1), or end (2) where none means no change in
 * position, set means absolute position, and end means relative position to duration
 * @start: time of the starting point of the segment
 * @stop_type: none (0), set (1), or end (2) where none means no change in
 * position, set means absolute position, and end means relative position to duration
 * @stop: time of the stopping point of the segment
 *
 * Configures playback of the pipeline between @start to @stop at the speed
 * given in @rate.  Effectively causes gstd to invoke gst_element_seek().
 *
 * Returns: GstcStatus indicating success, daemon unreachable, daemon timeout,
 * bad pipeline name, out of memory
 */
GstcStatus gstc_pipeline_seek(const GstClient *client, const char *pname,
    double rate, int format, int flags, int start_type, long long start,
    int stop_type, long long stop);

/**
 * GstcPipelineBusWaitCallback:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name associated with the pipeline
 * @message_name: The type of message to receive
 * @timeout: The amount of nanoseconds to wait for the event, or -1
 * for unlimited
 * @user_data: (allow none): A placeholder for custom data
 * 
 * The callback signature of the function to be registered in
 * gst_pipeline_bus_wait_async().
 *
 * Returns: GstcStatus indicating success, thread error or timeout.
 */
typedef GstcStatus
(*GstcPipelineBusWaitCallback) (GstClient *client,
    const char *pipeline_name, const char *message_name,
    const long long timeout, char *message, void *user_data);

/**
 * gstc_pipeline_bus_wait_async:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name associated with the pipeline
 * @message_name: The type of message to receive
 * @timeout: The amount of nanoseconds to wait for the event, or -1
 * for unlimited
 * @callback: The function to be called when the message (or timeout)
 * is received on the bus.
 * @user_data: (allow none): A placeholder for custom data
 * 
 * Register a callback function to be called when a specific message
 * is received on the bus or a timeout ocurred.
 *
 * Returns: GstcStatus indicating success, thread error or timeout.
 */
GstcStatus
gstc_pipeline_bus_wait_async (GstClient *client,
    const char *pipeline_name, const char *message_name,
    const long long timeout, GstcPipelineBusWaitCallback callback,
    void *user_data);

/**
 * gstc_pipeline_bus_wait:
 * @client: The client returned by gstc_client_new()
 * @pipeline_name: Name associated with the pipeline
 * @message_name: The type of message to receive
 * @timeout: The amount of nanoseconds to wait for the event, or -1
 * for unlimited
 * 
 * Block until a message of type @message_name is received in the bus
 * or a timeout occurs.
 *
 * Returns: GstcStatus indicating success, thread error or timeout.
 */
GstcStatus
gstc_pipeline_bus_wait (GstClient *client,
    const char *pipeline_name, const char *message_name,
    const long long timeout, char **message);

#ifdef __cplusplus
}
#endif

#endif // __LIBGSTC_H__
