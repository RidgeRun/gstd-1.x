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
   * GstdClient Construtor
   * @param {String} ip
   * @param {Number} port
   * @throws {GstcErrorCode}
  */
  constructor(ip='http://localhost',port=5000) {
    this.ip = ip;
    this.port = port;
  }

  /**
   * Send Command
   * @param {String} url
   * @param {Array} request
   * @throws {GstdError} Error is triggered when Gstd IPC fails
   * @throws {GstcError} Error is triggered when GstClient fails
   */
  static async send_cmd(url, request) {
    try {
      var response = await fetch (url, request);
      var j_resp = await response.json();
      if (j_resp["code"] !== GstcErrorCode.GSTC_OK ) {
        throw new GstdError([j_resp["description"], j_resp["code"]]);
      }
      return j_resp;
    } catch (e) {
      if(e instanceof GstdError) {
        throw e;
      }
      throw new GstcError(['Server did not respond. Is it up?',
        GstcErrorCode.GSTC_UNREACHABLE]);
    }
  }

  /**
   * List Pipelines
   */
  async list_pipelines() {
    var url = this.ip + ":" + this.port + "/pipelines";
    var request = { method : "GET" };
    return await GstdClient.send_cmd(url, request);
  }

  /**
   * Pipeline Create
   * @param {String} pipe_name
   * @param {String} pipe_desc
   */
  async pipeline_create(pipe_name, pipe_desc) {

    var url = this.ip + ":" + this.port + "/pipelines?name=" + pipe_name +
      "&description=" + pipe_desc;
    var request = {
      method: 'POST'
    }

    return await GstdClient.send_cmd(url, request);
  }

  /**
   * Pipeline Play
   * @param {String} pipe_name
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

    return await GstdClient.send_cmd(url, request);
  }

  /**
   * Element Set
   * @param {String} pipe_name
   * @param {String} element
   * @param {String} prop
   * @param {String} value
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

    return await GstdClient.send_cmd(url, request);
  }

  /**
   * Pipeline Pause
   * @param {String} pipe_name
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

    return await GstdClient.send_cmd(url, request);
  }

  /**
   * Pipeline Stop
   * @param {String} pipe_name
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

    return await GstdClient.send_cmd(url, request);
  }

  /**
   * Pipeline Delete
   * @param {String} pipe_name
   */
  async pipeline_delete(pipe_name) {

    var url = this.ip + ":" + this.port + "/pipelines?name=" + pipe_name;
    var request = {
      method: 'DELETE',
      body: {
        name : pipe_name
      },
    }

    return await GstdClient.send_cmd(url, request);
  }
}

/** GstClient - GstcError Class */
class GstcError extends Error {

  /**
   * Constructor
   * @param  {...any} params List of Params
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
   * Constructor
   * @param  {...any} params List of Params
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