#The software contained in this file is free and unencumbered software released into the public domain.
#Anyone is free to use the software contained in this file as they choose, including incorporating
#it into proprietary software.
import time
import sys
from pygstc.gstc import *
from pygstc.logger import *

def printError():
    print("To play run: python3 simple_pipeline.py create $VIDEO_PATH")
    print("To play run: python3 simple_pipeline.py play")
    print("To pause run: python3 simple_pipeline.py pause")
    print("To stop run: python3 simple_pipeline.py stop")
    print("To reverse play run: python3 simple_pipeline.py reverse")
    print("To play slow run: python3 simple_pipeline.py slow_motion")
    print("To stop run: python3 simple_pipeline.py delete")
    print("To read messages from the GStreamer Daemon: python3 simple_pipeline.py read_gstd")
    print("To read filtered messages from the GStreamer Daemon: python3 simple_pipeline.py read_f_gstd")
    print("To change resolution run: python3 simple_pipeline.py set_res $WIDTH $HEIGHT")


#Create a custom logger with loglevel=DEBUG
gstd_logger = CustomLogger('simple_playback', loglevel='DEBUG')

#Create the client with the logger
try:
  gstc = GstdClient(logger=gstd_logger)
  gstc.list_pipelines()
except:
  print("Error: The GstdClient could not be created")
else:
  try:
    if(len(sys.argv) > 1):
      status = 0
      if(sys.argv[1]=="create"):

        FILE_SOURCE = sys.argv[2]
        #pipeline is the string with the pipeline description
        pipeline = "filesrc location="+FILE_SOURCE+" ! decodebin ! videoconvert ! \
          videoscale ! capsfilter name=cf ! autovideosink"

        #Following instructions create and play the pipeline
        gstc.pipeline_create("p0", pipeline)

      elif(sys.argv[1]== "play"):
        gstc.pipeline_play("p0")
        print("Playing")

      #Reverse and slow motion restart the pipeline
      elif(sys.argv[1]== "reverse"):
        gstc.event_seek("p0", rate=-1.0, format=3, flags=1, start_type=1, start=0, end_type=1, end=-1)
        print("Playing in reverse")

      elif(sys.argv[1]== "slow_motion"):
        gstc.event_seek("p0", rate=0.5, format=3, flags=1, start_type=1, start=0, end_type=1, end=-1)
        print("Playing in slow motion")

      elif(sys.argv[1]== "pause"):
        gstc.pipeline_pause("p0")
        print("Pipeline paused")

      elif(sys.argv[1]== "stop"):
        gstc.pipeline_stop("p0")
        print("Pipeline stoped")

      elif(sys.argv[1]== "delete"):
        gstc.pipeline_delete ("p0")
        print("Pipeline deleted")

      elif(sys.argv[1] == "read_gstd"):
        #Timeout in nanoseconds or forever
        gstc.bus_timeout("p0", -1)
        resp = gstc.bus_read("p0")
        print(resp)

      elif(sys.argv[1] == "read_f_gstd"):
        #Serach EOF and react
        gstc.bus_filter("p0", "error+eos")
        resp = gstc.bus_read("p0")
        print(resp)

      elif(sys.argv[1] == "set_res" and len(sys.argv) == 4):
        width = sys.argv[2]
        height = sys.argv[3]
        gstc.element_set("p0", "cf", "caps", "video/x-raw,width="+width+",height="+height+"")

      else:
        status = -1
    else:
        status = -1
  except GstdError:
    print("GstdError: Gstd IPC failed")
  except GstcError:
    print("GstcError: Gstd python client failed")

if (status == -1):
  printError()