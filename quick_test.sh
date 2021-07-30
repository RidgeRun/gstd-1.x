#!/bin/sh

#gstd -e
gst-client pipeline_create testpipe videotestsrc name=vts ! autovideosink
gst-client pipeline_play testpipe
sleep $1
gst-client element_set testpipe vts pattern ball
sleep $1
gst-client  pipeline_stop testpipe
gst-client  pipeline_delete testpipe
echo "\n"
#gstd -k
sleep $1
clear
