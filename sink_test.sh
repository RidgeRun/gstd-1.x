#!/bin/sh

#gstd -e
gst-client pipeline_create testpipe videotestsrc name=vts ! appsink name=mysink
gst-client element_set testpipe mysink emit-signals true
# gst-client signal_connect testpipe mysink new-sample
gst-client pipeline_play testpipe

