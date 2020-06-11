/*
 * GStreamer Daemon - gst-launch on steroids
 * JavaScript client library abstracting gstd interprocess communication
 *
 * Copyright (c) 2015-2020 RidgeRun, LLC (http://www.ridgerun.com)
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

/** GstClient - GstdClient Class */
class GstdClient {

  /**
   * GstdClient Construtor.
   *
   * @param {String} ip.
   * @param {Number} port.
  */
  constructor(ip='http://localhost',port=5000) {
    this.ip = ip;
    this.port = port;
  }

  /**
   * Send Command.
   *
   * @param {String} url.
   * @param {Array} request.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  static async send_cmd(url, request) {
    try {
      var response = await fetch (url, request);
      var j_resp = await response.json();
    } catch (e) {
      if(e instanceof TypeError &&
         e.message.includes("NetworkError")) {
        throw new GstcError(['Gstd did not respond. Is it up?',
        GstcErrorCode.GSTC_UNREACHABLE]);
      } else {
        throw new GstcError(['Gstd corrupted response',
        GstcErrorCode.GSTC_RECV_ERROR]);
      }
    }
    if (j_resp["code"] !== GstcErrorCode.GSTC_OK ) {
      throw new GstdError([j_resp["description"], j_resp["code"]]);
    }
    return j_resp;
  }

  /**
   * List Pipelines.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async list_pipelines() {
    var url = this.ip + ":" + this.port + "/pipelines";
    var request = { method : "GET" };
    return GstdClient.send_cmd(url, request);
  }

  /**
   * Pipeline Create.
   *
   * @param {String} pipe_name.
   * @param {String} pipe_desc.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async pipeline_create(pipe_name, pipe_desc) {

    var url = this.ip + ":" + this.port + "/pipelines?name=" + pipe_name +
      "&description=" + pipe_desc;
    var request = {
      method: 'POST',
      body : {
        name : pipe_name,
        description : pipe_desc
      }
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Pipeline Play.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async pipeline_play(pipe_name) {

    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/state?name=playing";
    var request = {
      method: 'PUT',
      body: {
        name : pipe_name,
        status : 'playing'
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Element Set.
   *
   * @param {String} pipe_name.
   * @param {String} element.
   * @param {String} prop.
   * @param {String} value.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async element_set(pipe_name, element, prop, value) {

    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
    "/elements/" + element + "/properties/" + prop + "?name=" + value;
    var request = {
      method: 'PUT',
      body: {
        name : pipe_name,
        element : element,
        prop : prop,
        value : value
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Pipeline Pause.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async pipeline_pause(pipe_name) {

    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
    "/state?name=paused";
    var request = {
      method: 'PUT',
      body: {
        name : pipe_name,
        status : 'paused'
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Pipeline Stop.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async pipeline_stop(pipe_name) {

    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/state?name=null";
    var request = {
      method: 'PUT',
      body: {
        name : pipe_name,
        status : 'null'
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Pipeline Delete.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async pipeline_delete(pipe_name) {

    var url = this.ip + ":" + this.port + "/pipelines?name=" + pipe_name;
    var request = {
      method: 'DELETE',
      body: {
        name : pipe_name
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Bus Filter
   *
   * @param {String} pipe_name.
   * @param {String} filter.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async bus_filter(pipe_name, filter) {
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/bus/types?name=" + filter;
    var request = {
      method: 'PUT',
      body: {
        name : pipe_name,
        filter : filter
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Bus Read.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async bus_read(pipe_name) {
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/bus/message";
    var request = {
      method: 'GET'
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Apply a timeout for the bus polling.
   *
   * @param {String} pipe_name.
   * @param {Integer} timeout.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async bus_timeout(pipe_name, timeout) {
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/bus/timeout?name=" + timeout;
    var request = {
      method: 'PUT',
      body: {
        name : pipe_name,
        timeout : timeout
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Perform a seek in the given pipeline
   *
   * @param {String} pipe_name
   * @param {Integer} rate
   * @param {Integer} format
   * @param {Integer} flags
   * @param {Integer} start_type
   * @param {Integer} start
   * @param {Integer} end_type
   * @param {Integer} end
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async event_seek(pipe_name, rate=1.0, format=3, flags=1, start_type=1,
    start=0, end_type=1, end=-1) {

      var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/event?name=seek&description=" + rate + "%20" + format + "%20" +
      flags + "%20" + start_type + "%20" + start + "%20" + end_type + "%20" + end;
    var request = {
      method : 'POST',
      body : {
        event: 'seek',
        name : pipe_name,
        rate : rate,
        format : format,
        flags : flags,
        start_type : start_type,
        end_type : end_type,
        end : end
      }
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Enable/Disable colors in the debug logging.
   *
   * @param {Boolean} enable.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async debug_color (enable) {
    var url = this.ip + ":" + this.port + "/debug/color?name=" + enable;
    var request = {
      method: 'PUT',
      body: {
        name : enable,
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Enable/Disable GStreamer debug.
   *
   * @param {Boolean} enable.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async debug_enable(enable) {
    var url = this.ip + ":" + this.port + "/debug/enable?name=" + enable;
    var request = {
      method: 'PUT',
      body: {
        name : enable,
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Enable/Disable debug threshold reset.
   *
   * @param {Boolean} reset
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async debug_reset(enable) {
    var url = this.ip + ":" + this.port + "/debug/reset?name=" + enable;
    var request = {
      method: 'PUT',
      body: {
        name : enable,
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * The debug filter to apply (as you would use with gst-launch).
   *
   * Debug threshold:
   *     0   none    No debug information is output.
   *     1   ERROR   Logs all fatal errors.
   *     2   WARNING Logs all warnings.
   *     3   FIXME   Logs all "fixme" messages.
   *     4   INFO    Logs all informational messages.
   *     5   DEBUG   Logs all debug messages.
   *     6   LOG     Logs all log messages.
   *     7   TRACE   Logs all trace messages.
   *     9   MEMDUMP Logs all memory dump messages.
   *
   * @param {String} threshold.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async debug_threshold(threshold) {
    var url = this.ip + ":" + this.port + "/debug/threshold?name=" + threshold;
    var request = {
      method: 'PUT',
      body: {
        threshold : threshold,
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Queries a property in an element of a given pipeline.
   *
   * @param {String} pipe_name
   * @param {String} element
   * @param {String} prop
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async element_get(pipe_name, element, prop) {
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/elements/" + element + "/properties/" + prop;
    var request = {
      method: 'GET'
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * List the elements in a given pipeline.
   *
   * @param {String} pipe_name
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async list_elements(pipe_name) {
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/elements/";
    var request = {
      method: 'GET'
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * List the properties of an element in a given pipeline.
   *
   * @param {String} pipe_name
   * @param {String} element
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async list_properties(pipe_name, element) {
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/elements/" + element + "/properties";
    var request = {
      method: 'GET'
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * List the signals of an element in a given pipeline.
   *
   * @param {String} pipe_name
   * @param {String} element
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async list_signals(pipe_name, element) {
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/elements/" + element + "/signals";
    var request = {
      method: 'GET'
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Connect to signal and wait.
   *
   * @param {String} pipe_name
   * @param {String} element
   * @param {String} signal
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async signal_connect(pipe_name, element, signal) {
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/elements/" + element + "/signals/" + signal + "/callback";
    var request = {
      method: 'GET'
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Disconnect from signal.
   *
   * @param {String} pipe_name
   * @param {String} element
   * @param {String} signal
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async signal_disconnect(pipe_name, element, signal) {
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/elements/" + element + "/signals/" + signal + "/disconnect";
    var request = {
      method: 'GET'
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Apply a timeout for the signal waiting.
   *
   * @param {String} pipe_name
   * @param {String} element
   * @param {String} signal
   * @param {String} timeout
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async signal_timeout(pipe_name, element, signal, timeout) {
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/elements/" + element + "/signals/" + signal + "timeout?name=" +
      timeout;
    var request = {
      method: 'PUT',
      body: {
        name : pipe_name,
        element : element,
        signal : signal,
        timeout : timeout
      },
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Send an end-of-stream event.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async event_eos(pipe_name) {

    var event_name = "eos";
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/event?name=" + event_name;
    var request = {
      method: 'POST',
      body : {
        name : pipe_name,
        event : event_name
      }
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Put the pipeline in flushing mode.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async event_flush_start(pipe_name) {

    var event_name = "flush_start";
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/event?name=" + event_name;
    var request = {
      method: 'POST',
      body : {
        name : pipe_name,
        event : event_name
      }
    }

    return GstdClient.send_cmd(url, request);
  }

  /**
   * Take the pipeline out from flushing mode.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Error is triggered when Gstd IPC fails.
   * @throws {GstcError} Error is triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async event_flush_stop(pipe_name) {

    var event_name = "flush_stop";
    var event_desc = "true";
    var url = this.ip + ":" + this.port + "/pipelines/" + pipe_name +
      "/event?name=" + event_name + "&description=" + event_desc;
    var request = {
      method: 'POST',
      body : {
        name : pipe_name,
        event : event_name,
        description : event_desc
      }
    }

    return GstdClient.send_cmd(url, request);
  }

}

/** GstClient - GstcError Class */
class GstcError extends Error {

  /**
   * Constructor.
   *
   * @param  {...any} params List of Params.
   */
  constructor(...params) {

    super(...params)

    /** Maintains proper stack trace */
    if (Error.captureStackTrace) {
      Error.captureStackTrace(this, GstcError)
    }

    this.name = 'GstcError'
    /** Custom debugging information */
    this.date = new Date()
  }
}

/** GstClient - GstdError Class */
class GstdError extends Error {

  /**
   * Constructor.
   *
   * @param  {...any} params List of Params.
   */
  constructor(...params) {

    super(...params)

    /** Maintains proper stack trace */
    if (Error.captureStackTrace) {
      Error.captureStackTrace(this, GstdError)
    }

    this.name = 'GstdError'
    /** Custom debugging information */
    this.date = new Date()
  }
}

const GstcErrorCode = {
  GSTC_OK: 0,
  GSTC_NULL_ARGUMENT: -1,
  GSTC_UNREACHABLE: -2,
  GSTC_TIMEOUT: -3,
  GSTC_OOM: -4,
  GSTC_TYPE_ERROR: -5,
  GSTC_MALFORMED: -6,
  GSTC_NOT_FOUND: -7,
  GSTC_SEND_ERROR: -8,
  GSTC_RECV_ERROR: -9,
  GSTC_SOCKET_ERROR: -10,
  GSTC_THREAD_ERROR: -11,
  GSTC_BUS_TIMEOUT: -12,
  GSTC_SOCKET_TIMEOUT: -13,
}

