/*
 * This file is part of GStreamer Daemon
 * JavaScript client library abstracting gstd interprocess communication
 *
 * Copyright 2015-2022 Ridgerun, LLC (http://www.ridgerun.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** GstClient - GstdClient Class */
class GstdClient {

  /**
   * GstdClient Construtor.
   *
   * @param {String} ip.
   * @param {Number} port.
  */
  constructor(ip = '127.0.0.1', port = 5000) {

    this.ip = ip;
    this.port = port;
  }

  /**
   * Send Command.
   *
   * @param {String} address.
   * @param {Array} request.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  static async send_cmd(address, request) {
    /* Add scheme to the address (authority + path + query) */
    const url = `http://${address}`;
    try {
      var response = await fetch(url, request);
      var j_resp = await response.json();
    } catch (e) {
      if (e instanceof TypeError &&
        e.message.includes("NetworkError")) {
        throw new GstcError(['Gstd did not respond. Is it up?',
          GstcErrorCode.GSTC_UNREACHABLE]);
      } else {
        throw new GstcError(['Gstd corrupted response',
          GstcErrorCode.GSTC_RECV_ERROR]);
      }
    }
    if (j_resp["code"] !== GstcErrorCode.GSTC_OK) {
      throw new GstdError([j_resp["description"], j_resp["code"]]);
    }
    return j_resp;
  }

  /**
   * Create a resource at the given URI.
   *
   * @param {String} uri.
   * @param {String} name.
   * @param {String} description.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async create(uri, name, description) {

    var complete_address = this.ip + ":" + this.port + uri + "?name=" +
      name;

    /* Allow create without description */
    if (description != null) {
      complete_address = complete_address + "&description=" +
        description;
    }

    var request = {
      method: 'POST',
      body: {
        uri: uri,
        name: name,
        description: description
      }
    }

    return GstdClient.send_cmd(complete_address, request);
  }

  /**
   * Read the resource held at the given URI with the given name.
   *
   * @param {String} uri.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async read(uri) {

    var complete_address = this.ip + ":" + this.port + uri;
    var request = { method: "GET" };
    return GstdClient.send_cmd(complete_address, request);
  }

  /**
   * Update the resource at the given URI.
   *
   * @param {String} uri.
   * @param {String} description.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async update(uri, description) {

    var complete_address = this.ip + ":" + this.port + uri + "?name=" +
      description;
    var request = {
      method: 'PUT',
      body: {
        uri: uri,
        description: description
      },
    }

    return GstdClient.send_cmd(complete_address, request);
  }

  /**
   * Delete the resource held at the given URI with the given name.
   *
   * @param {String} uri.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async delete(uri, name) {
    var complete_address = this.ip + ":" + this.port + uri + "?name="
      + name;
    var request = {
      method: 'DELETE',
      body: {
        uri: uri,
        name: name
      },
    }

    return GstdClient.send_cmd(complete_address, request);
  }

  /**
   * List Pipelines.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async list_pipelines() {

    return this.read("/pipelines");
  }

  /**
   * Pipeline Create.
   *
   * @param {String} pipe_name.
   * @param {String} pipe_desc.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async pipeline_create(pipe_name, pipe_desc) {

    return this.create("/pipelines", pipe_name, pipe_desc);
  }

  /**
   * Pipeline Play.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async pipeline_play(pipe_name) {

    return this.update("/pipelines/" + pipe_name + "/state", "playing");
  }

  /**
   * Element Set.
   *
   * @param {String} pipe_name.
   * @param {String} element.
   * @param {String} prop.
   * @param {String} value.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async element_set(pipe_name, element, prop, value) {

    var uri = "/pipelines/" + pipe_name + "/elements/" + element +
      "/properties/" + prop;
    return this.update(uri, value);
  }

  /**
   * Pipeline Pause.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async pipeline_pause(pipe_name) {

    return this.update("/pipelines/" + pipe_name + "/state", "paused");
  }

  /**
   * Pipeline Stop.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async pipeline_stop(pipe_name) {

    return this.update("/pipelines/" + pipe_name + "/state", "null");
  }

  /**
   * Pipeline Delete.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async pipeline_delete(pipe_name) {

    return this.delete("/pipelines", pipe_name);
  }

  /**
   * Bus Filter
   *
   * @param {String} pipe_name.
   * @param {String} filter.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async bus_filter(pipe_name, filter) {

    return this.update("/pipelines/" + pipe_name + "/bus/types", filter);
  }

  /**
   * Bus Read.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async bus_read(pipe_name) {

    return this.read("/pipelines/" + pipe_name + "/bus/message");
  }

  /**
   * Apply a timeout for the bus polling.
   *
   * @param {String} pipe_name.
   * @param {Integer} timeout.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async bus_timeout(pipe_name, timeout) {

    return this.update("/pipelines/" + pipe_name + "/bus/timeout", timeout);
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
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async event_seek(pipe_name, rate = 1.0, format = 3, flags = 1,
    start_type = 1, start = 0, end_type = 1, end = -1) {

    var uri = "/pipelines/" + pipe_name + "/event";
    var description = rate + "%20" + format + "%20" + flags + "%20" +
      start_type + "%20" + start + "%20" + end_type + "%20" + end;
    return this.create(uri, "seek", description);
  }

  /**
   * Enable/Disable colors in the debug logging.
   *
   * @param {Boolean} enable.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async debug_color(enable) {

    return this.update("/debug/color", enable);
  }

  /**
   * Enable/Disable GStreamer debug.
   *
   * @param {Boolean} enable.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async debug_enable(enable) {

    return this.update("/debug/enable", enable);
  }

  /**
   * Enable/Disable debug threshold reset.
   *
   * @param {Boolean} reset
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async debug_reset(enable) {

    return this.update("/debug/reset", enable);
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
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async debug_threshold(threshold) {

    return this.update("/debug/threshold", threshold);
  }

  /**
   * Queries a property in an element of a given pipeline.
   *
   * @param {String} pipe_name
   * @param {String} element
   * @param {String} prop
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async element_get(pipe_name, element, prop) {

    var uri = "/pipelines/" + pipe_name + "/elements/" + element +
      "/properties/" + prop;
    return this.read(uri);
  }

  /**
   * List the elements in a given pipeline.
   *
   * @param {String} pipe_name
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async list_elements(pipe_name) {

    return this.read("/pipelines/" + pipe_name + "/elements/");
  }

  /**
   * List the properties of an element in a given pipeline.
   *
   * @param {String} pipe_name
   * @param {String} element
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async list_properties(pipe_name, element) {

    var uri = "/pipelines/" + pipe_name + "/elements/" + element +
      "/properties";
    return this.read(uri);
  }

  /**
   * List the signals of an element in a given pipeline.
   *
   * @param {String} pipe_name
   * @param {String} element
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async list_signals(pipe_name, element) {

    var uri = "/pipelines/" + pipe_name + "/elements/" + element +
      "/signals";
    return this.read(uri);
  }

  /**
   * Connect to signal and wait.
   *
   * @param {String} pipe_name
   * @param {String} element
   * @param {String} signal
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async signal_connect(pipe_name, element, signal) {

    var uri = "/pipelines/" + pipe_name + "/elements/" + element +
      "/signals/" + signal + "/callback";
    return this.read(uri);
  }

  /**
   * Disconnect from signal.
   *
   * @param {String} pipe_name
   * @param {String} element
   * @param {String} signal
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async signal_disconnect(pipe_name, element, signal) {

    var uri = "/pipelines/" + pipe_name + "/elements/" + element +
      "/signals/" + signal + "/disconnect";
    return this.read(uri);
  }

  /**
   * Apply a timeout for the signal waiting.
   *
   * @param {String} pipe_name
   * @param {String} element
   * @param {String} signal
   * @param {String} timeout
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async signal_timeout(pipe_name, element, signal, timeout) {

    var uri = "/pipelines/" + pipe_name + "/elements/" + element +
      "/signals/" + signal + "/timeout";
    return this.update(uri, timeout);
  }

  /**
   * Send an end-of-stream event.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async event_eos(pipe_name) {

    return this.create("/pipelines/" + pipe_name + "/event", "eos", null);
  }

  /**
   * Put the pipeline in flushing mode.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async event_flush_start(pipe_name) {

    var uri = "/pipelines/" + pipe_name + "/event";
    return this.create(uri, "flush_start", null);
  }

  /**
   * Take the pipeline out from flushing mode.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  async event_flush_stop(pipe_name) {

    var uri = "/pipelines/" + pipe_name + "/event";
    return this.create(uri, "flush_stop", "true");
  }

  /**
   * Get the pipeline graph.
   *
   * @param {String} pipe_name.
   *
   * @throws {GstdError} Triggered when Gstd fails to process a request.
   * @throws {GstcError} Triggered when GstClient fails.
   *
   * @return {object} Response from Gstd.
   */
  pipeline_get_graph(pipe_name) {
    return this.read("/pipelines/" + pipe_name + "/graph");
  }

/**
 * Set the pipeline verbose mode.
 * Only supported on GST Version >= 1.10
 *
 * @param {String} pipe_name.
 *
 * @throws {GstdError} Triggered when Gstd fails to process a request.
 * @throws {Boolean} Triggered when GstClient fails.
 *
 * @return {object} Response from Gstd.
 */
  pipeline_verbose(pipe_name, enable) {
    return this.update("/pipelines/" + pipe_name + "/verbose", enable);
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

    /* Maintains proper stack trace */
    if (Error.captureStackTrace) {
      Error.captureStackTrace(this, GstcError)
    }

    this.name = 'GstcError'
    /* Custom debugging information */
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

    /* Maintains proper stack trace */
    if (Error.captureStackTrace) {
      Error.captureStackTrace(this, GstdError)
    }

    this.name = 'GstdError'
    /* Custom debugging information */
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

