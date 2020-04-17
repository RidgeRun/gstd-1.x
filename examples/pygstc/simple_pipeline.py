import time
import sys
from pygstc.gstc import *
from pygstc.logger import *

#Create a custom logger with loglevel=DEBUG
gstd_logger = CustomLogger('simple_pipeline', loglevel='DEBUG')

#Create the client with the logger
gstd_client = GstdClient(logger=gstd_logger)

def printError():
    print("To play run: python3 simple_pipeline.py play VIDEO_PATH")
    print("To stop run: python3 simple_pipeline.py stop")
    print("To stop run: python3 simple_pipeline.py reverse")
    print("To stop run: python3 simple_pipeline.py slow_motion")

if(len(sys.argv) > 1):
  if(sys.argv[1]=="create"):

    FILE_SOURCE = sys.argv[2]
    #pipeline is the string with the pipeline description
    pipeline = "playbin uri=file:"+FILE_SOURCE

    #Following instructions create and play the pipeline
    gstd_client.pipeline_create ("p0", pipeline)
    
  elif(sys.argv[1]== "play"):
    gstd_client.pipeline_play ("p0")
    print("Playing")

  # Check this 
  # reverse and slow motion restart the pipeline
  elif(sys.argv[1]== "reverse"):
    gstd_client.event_seek("p0", rate=-1.0, format=3, flags=1, start_type=1, start=0, end_type=1, end=-1)
    print("Playing in reverse")

  elif(sys.argv[1]== "slow_motion"):
    gstd_client.event_seek("p0", rate=0.5, format=3, flags=1, start_type=1, start=0, end_type=1, end=-1)
    print("Playing in slow motion")

  elif(sys.argv[1]== "pause"):
    gstd_client.pipeline_pause ("p0")
    print("Pipeline paused")

  elif(sys.argv[1]== "stop"):
    gstd_client.pipeline_stop ("p0")
    print("Pipeline stoped")

  elif(sys.argv[1]== "delete"):
    gstd_client.pipeline_delete ("p0")
    print("Pipeline deleted")

  else:
    printError()
else:
    printError()
