/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2017 Ridgerun, LLC (http://www.ridgerun.com)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __LIBGSTD_H__
#define __LIBGSTD_H__

#ifdef __cplusplus
extern "C"
{
#endif

void myPrint(void);

/*
 * GstDManager:
 * Opaque representation of GstD state.
 * This struct will have: Session, GstdIpc and num_ipcs (for now)
 */
typedef struct _GstDManager GstDManager;

/**
 * Supported_IPCs:
 * IPC options for libGstD
 */
typedef enum _SupportedIpcs SupportedIpcs;

enum _SupportedIpcs
{
    GSTD_IPC_TYPE_TCP,
    GSTD_IPC_TYPE_UNIX,
    GSTD_IPC_TYPE_HTTP,
};

/**
 * GstdStatus:
 * @gstd_OK: Everything went okay
 * @gstd_NULL_ARGUMENT: A mandatory argument was passed in as NULL
 * @gstd_OOM: The system has run out of memory
 * @gstd_TYPE_ERROR: An error occurred parsing a type from a string
 * @gstd_NOT_FOUND: The response is missing the field requested
 * @gstd_THREAD_ERROR: Unable to create a new thread
 * @gstd_BUS_TIMEOUT: A timeout was received while waiting on the bus
 * @gstd_LONG_RESPONSE: The response exceeds our maximum, typically
 * meaning a missing null terminator
 *
 * Return codes for the different libgstd operations
 */
typedef enum
{
  GSTD_LIB_OK = 0,
  GSTD_LIB_NULL_ARGUMENT = -1,
  GSTD_LIB_OOM = -4,
  GSTD_LIB_TYPE_ERROR = -5,
  GSTD_LIB_NOT_FOUND = -7,
  GSTD_LIB_THREAD_ERROR = -11,
  GSTD_LIB_BUS_TIMEOUT = -12,
  GSTD_LIB_SOCKET_TIMEOUT = -13,
  GSTD_LIB_LONG_RESPONSE = -14
} GstdStatus;


/**
 * gstd_manager_new:
 * 
 * @supported_ipcs: ipcs the user will use 
 * @num_ipcs: lenght of supported_ipcs
 * @out: placeholder for newly allocated gstd manager.
 * @gst_group: placeholder for GStreamer's argument specifications
 * @argc: arguments for gst_init
 * @argv: arguments for gst_init
 * 
 * Initializes gstd. If ipc array is not NULL
 * it will initialize the GstdIpc in GstDManager.
 * If it is NULL it will just initialize the session.
 *
 * Returns: GstdStatus indicating success or fail
 */
GstdStatus 
gstd_manager_new (SupportedIpcs supported_ipcs[], uint num_ipcs, 
    GstDManager ** out, void **gst_group, int argc, char *argv[]);


/**
 * gstd_manager_ipc_options:
 * @manager: The manager returned by gstd_manager_new()
 * @ipc_group: placeholder for IPCs specifications
 * 
 * Get IPCs information into a group 
 *
 */
void
gstd_manager_ipc_options (GstDManager * manager, void **ipc_group);

/**
 * gstd_manager_ipc_start:
 * @manager: The manager returned by gstd_manager_new()
 * 
 * Starts the ipc in GstdIpc array
 *
 * Returns: GstdStatus indicating success or fail
 */
int
gstd_manager_ipc_start (GstDManager * manager);

/**
 * gstd_manager_ipc_start:
 * @manager: The manager returned by gstd_manager_new()
 * 
 * Stops the ipc in GstdIpc array
 *
 * Returns: GstdStatus indicating success or fail
 */
void
gstd_manager_ipc_stop (GstDManager * manager);


/**
 * gstd_manager_free:
 * @manager: A valid manager allocated with gstd_new()
 *
 * Frees a previously allocated GstDManager.
 *
 * Returns: A newly allocated GstDManager. Use gstd_free() after
 * usage.
 */
void
gstd_manager_free (GstDManager * manager);

/**
 * gstd_manager_debug:
 * @threshold: the debug level takes a keyword and the debug level in the argument
 * recieving 0 as a level is equivalent to disabling debug
 * @colors: if non-zero ANSI color control escape sequences will be included in the debug output
 * @reset: if non-zero the debug threshold will be cleared each time, otherwise threshold 
 * is appended to previous threshold.
 *
 * Controls amount of GStreamer Daemon debug logging.  Typically the GStreamer Daemon debug log output is directed to the system log file.
 *
 * Returns: GstdStatus indicating success, daemon unreachable, daemon timeout
 */
GstdStatus gstd_manager_debug (GstDManager * manager, const char* threshold,
    const int colors, const int reset);

/**
 * gstd_pipeline_create:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name to associate to the pipeline
 * @pipeline_desc: The gst-launch style pipeline description to create
 *
 * Creates a new GStreamer pipeline that can be referred to using
 * @pipeline_name.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_create (GstDManager * manager, const char *pipeline_name,
    const char *pipeline_desc);
  
/**
 * gstd_pipeline_list:
 * @manager: The manager returned by gstd_manager_new()
 * @pipelines: List of existing pipelines names returned by the manager library
 * @list_lenght: Number of elements in the pipelines list
 *
 * Returns a list of the names of the existing pipelines.  Depending on the
 * deployment, another  application may have created some of the pipelines.
 * The manager application needs to do a free(*pipelines) and a
 * free(*pipelines[idx]) to release the resources used to hold the list and
 * its elements
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus 
gstd_pipeline_list(GstDManager * manager, 
    char **pipelines[], int *list_lenght);

/**
 * gstd_pipeline_delete:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Deletes a previously created GStreamer pipeline named @pipeline_name.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_delete(GstDManager * manager, const char *pipeline_name);

/**
 * gstd_pipeline_play:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Attempts to change the named pipeline to the play state.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_play(GstDManager * manager, const char *pipeline_name);

/**
 * gstd_pipeline_pause:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Attempts to change the named pipeline to the paused state.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_pause(GstDManager * manager, const char *pipeline_name);

/**
 * gstd_pipeline_stop:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Attempts to change the named pipeline to the null state.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_stop(GstDManager * manager, const char *pipeline_name);

/**
 * gstd_pipeline_get_graph:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 * @response: Pipeline graph description
 * 
 * Attempts to get the graph of the pipeline.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_get_graph(GstDManager * manager, const char *pipeline_name, char **response);

/**
 * gstd_pipeline_verbose:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 * @value: Pipeline verbose mode (true/false)
 * Attempts to update the verbose mode of the pipeline.
 * Only supported on GST Version >= 1.10
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus
gstd_pipeline_verbose(GstDManager * manager, const char *pipeline_name, int value);

/**
 * gstd_element_get:
 * @manager: The manager returned by gstd_manager_new()
 * @pname: Name associated with the pipeline
 * @element: Element name to be queried
 * @property: Parameter name to be queried
 * @format: sscanf style format string to turn the string representation of the
 * value into the datatype expected by the application
 * @...: One or more pointer arguments corresponding to the properly (after type
 *  promotion) matching conversion specifier in format.
 *
 * Retrieves the current value of the specified element property.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus gstd_element_get (GstDManager * manager, const char *pname,
  const char *element, const char *property, const char *format, ...);

/**
 * gstd_element_set:
 * @manager: The manager returned by gstd_manager_new()
 * @pname: Name associated with the pipeline
 * @element: Element name to be set
 * @property: Property name to be set
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
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus gstd_element_set(GstDManager * manager, const char *pname,
    const char *element, const char *parameter, const char *format, ...);
    
/**
 * gstd_element_properties_list:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 * @element: Element name to be queried
 * @properties: List of element properties in the specified pipeline
 * @list_lenght: Number of properties in the properties list
 *
 * Returns a list of pipeline element properties. The manager application needs
 * to do a free(*properties) to release the resources used to hold the list.
 * The GStreamer Daemon definition for this functionality returns both property
 * name and value, however, do to the lack of polymorphism in the C language,
 * only the property names are returned by gstd_element_properties_list(). To get the
 * values, use  gstd_element_get().
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus gstd_element_properties_list(GstDManager * manager, const char *pipeline_name, 
    char *element, char **properties[], int *list_lenght);

/**
 * gstd_pipeline_flush_start:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Causes elements in the pipeline to unblock and discard any pipeline data.
 * Returns: GstdStatus indicating success or some failure
 *
 */
GstdStatus gstd_pipeline_flush_start(GstDManager * manager, 
    const char *pipeline_name);

/**
 * gstd_pipeline_flush_stop:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 * @reset: if non-zero the running_time of the pipeline is set back to 0
 *
 *  Causes elements in the pipeline to process pipeline data normally.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus gstd_pipeline_flush_stop(GstDManager * manager, const char *pipeline_name,
  const int reset);

/**
 * gstd_pipeline_inject_eos:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 *
 * Injects an end-of-stream marker into the named pipeline. The EOS
 * event is useful for notifying the elements in a pipeline that no
 * further buffers are expected and they can wrap up any processing
 * required to properly close the stream. Effectively causes gstd to
 * invoke gst_event_new_eos().
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus gstd_pipeline_inject_eos (GstDManager * manager, const char *pipeline_name);

/**
 * gstd_pipeline_seek:
 * @manager: The manager returned by gstd_manager_new()
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
 * Returns: GstdStatus indicating success or some failure.
 */
GstdStatus gstd_pipeline_seek(GstDManager * manager, const char *pname,
    double rate, int format, int flags, int start_type, long long start,
    int stop_type, long long stop);


/**
 * Configures playback of the pipeline between @start to @stop at the speed
 * given in @rate.  Effectively causes gstd to invoke gst_element_seek().
 * gstd_pipeline_list_elements:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 * @elements: List of elements in the specified pipelines returned by the
 * manager library
 *
 * Returns a list of pipeline elements.  The manager application needs
 * to do a free(*elements) to release the resources used to hold the list.
 *
 * Returns: GstdStatus indicating success or some failure
 */
GstdStatus gstd_pipeline_list_elements(GstDManager * manager, const char *pipeline_name,
 char **elements[], int* list_lenght);

/**
 * gstdPipelineBusWaitCallback:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 * @message_name: The type of message to receive
 * @timeout: The amount of nanoseconds to wait for the event, or -1
 * for unlimited
 * @user_data: (allow none): A placeholder for custom data
 * 
 * The callback signature of the function to be registered in
 * gst_pipeline_bus_wait_async().
 *
 * Returns: GstdStatus indicating success or some failure
 */
typedef GstdStatus
(*gstdPipelineBusWaitCallback) (GstDManager * manager,
    const char *pipeline_name, const char *message_name,
    const long long timeout, char *message, void *user_data);

/**
 * gstd_pipeline_bus_wait_async:
 * @manager: The manager returned by gstd_manager_new()
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
 * Returns: GstdStatus indicating success, thread error or timeout.
 */
GstdStatus
gstd_pipeline_bus_wait_async (GstDManager * manager,
    const char *pipeline_name, const char *message_name,
    const long long timeout, gstdPipelineBusWaitCallback callback,
    void *user_data);

/**
 * gstd_pipeline_bus_wait:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 * @message_name: The type of message to receive
 * @timeout: The amount of nanoseconds to wait for the event, or -1
 * for unlimited
 * 
 * Block until a message of type @message_name is received in the bus
 * or a timeout occurs.
 *
 * Returns: GstdStatus indicating success, thread error or timeout.
 */
GstdStatus
gstd_pipeline_bus_wait (GstDManager * manager,
    const char *pipeline_name, const char *message_name,
    const long long timeout, char **message);

/**
 * gstd_pipeline_get_state:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 * @out: pointer to output string memory representing a pipeline state value,
 * this memory should be freed by the user.
 *
 * Attempts to get the state (e.g PLAYING) of the pipeline.
 *
 * Returns: GstdStatus indicating success, daemon unreachable, daemon
 * timeout, bad pipeline name, unable to get the pipeline state
 */
GstdStatus
gstd_pipeline_get_state (GstDManager * manager, const char *pipeline_name,
    char **out);


/**
 * gstd_pipeline_list_signals:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 * @element: Name associated with the element
 * @signals: List of existing signals for the given element in the pipeline
 * @list_lenght: Number of elements in the signals list
 *
 * Returns a list of the names of the existing signals in an element.The manager
 * application needs to do a free(*signals) and a free(*signals[idx]) to
 * release the resources used to hold the list and its elements
 *
 * Returns: GstdStatus indicating success, daemon unreachable, daemon timeout,
 * bad pipeline name
 */
GstdStatus
gstd_pipeline_list_signals (GstDManager * manager, const char *pipeline_name, const char* element, char **signals[], int *list_lenght);

/**
 * gstd_pipeline_signal_connect:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 * @element: Name associated with the element
 * @signal: Name associated with the signal
 * @timeout: Timeout for the signal
 * @response: pointer to output string memory representing a pipeline state value,
 * this memory should be freed by the user.
 *
 * Attempts to get a signal from a pipeline
 *
 * Returns: GstdStatus indicating success, daemon unreachable, daemon
 * timeout, bad pipeline name
 */
GstdStatus
gstd_pipeline_signal_connect (GstDManager * manager, const char *pipeline_name, const char* element, const char* signal, const int value, char **response);

/**
 * gstd_pipeline_signal_disconnect:
 * @manager: The manager returned by gstd_manager_new()
 * @pipeline_name: Name associated with the pipeline
 * @element: Name associated with the element
 * @signal: Name associated with the signal
 *
 * Attempts to disconnect from signal from a pipeline
 *
 * Returns: GstdStatus indicating success, daemon unreachable, daemon
 * timeout, bad pipeline name
 */
GstdStatus
gstd_pipeline_signal_disconnect (GstDManager * manager, const char *pipeline_name, const char* element, const char* signal);

#ifdef __cplusplus
}
#endif

#endif // __LIBGSTD_H__
