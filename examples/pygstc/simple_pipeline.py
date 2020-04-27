#The software contained in this file is free and unencumbered software released into the public domain.
#Anyone is free to use the software contained in this file as they choose, including incorporating
#it into proprietary software.
import time
import sys
from pygstc.gstc import GstcError, GstdError, GstdClient
from pygstc.logger import CustomLogger

class GstcPlayer:

  def __init__(self):
    #Create a custom logger with loglevel=DEBUG
    self.gstd_logger = CustomLogger('simple_playback', loglevel='DEBUG')
    #Create the client with the logger
    self.gstc = GstdClient(logger=self.gstd_logger)
    self.pipeline = "fakesrc ! fakesink"

    #Check if pipe is already created
    if ( self.pipe_exists("p0")):
      self.gstc.pipeline_delete("p0")

  def openFile(self, videoPath):
    #Create pipeline object
    self.pipeline = "filesrc location="+videoPath+" ! decodebin ! videoconvert ! \
          videoscale ! capsfilter name=cf ! autovideosink"

  def playVideo(self):
    print("Playing")
    if (self.pipe_exists("p0")):
      self.gstc.pipeline_delete("p0")

    self.gstc.pipeline_create("p0", self.pipeline)
    print(self.gstc.list_pipelines())
    self.gstc.pipeline_play("p0")

  def pauseVideo(self):
    print("Playing")
    if (self.pipe_exists("p0")):
      self.gstc.pipeline_pause("p0")

  def continueVideo(self):
    print("Playing")
    if (self.pipe_exists("p0")):
      self.gstc.pipeline_play("p0")

  def stopVideo(self):
    print("Stop")
    if (self.pipe_exists("p0")):
      self.gstc.pipeline_stop("p0")
      self.gstc.pipeline_delete("p0")

  def set_res(self, width, height):
    #self.gstc.pipeline_pause("p0")
    self.gstc.element_set("p0", "cf", "caps", "video/x-raw,width="+width+",height="+height+"")
    #self.gstc.pipeline_play("p0")

  def pipe_exists(self, pipe_name):
    #Check if pipe is already created
    existing_pipes = self.gstc.list_pipelines()
    if (existing_pipes == []):
      ret = False
    elif( existing_pipes[0]['name'] == pipe_name):
      ret = True
    else:
      ret = False
    return ret

  def printUsage(self):
    print("play: To play and run")
    print("pause: To pause the video")
    print("continue: To continue after paused")
    print("stop: To stop and close the playing")
    print("set_res $WIDTH $HEIGHT: To change the video resolution")

def printError():
  print("To use run: python3 simple_pipeline.py $VIDEO_PATH")


print("Sample PyGstC Video Player")

if(len(sys.argv) != 2):
  printError()
  sys.exit(0)

try:
  myPlayer = GstcPlayer()
  myPlayer.openFile(sys.argv[1])
  myPlayer.playVideo()
  action = 0

  while (action != "exit"):

    if (action=="play"):
      myPlayer.playVideo()
    elif (action=="pause"):
      myPlayer.pauseVideo()
    elif (action=="continue"):
      myPlayer.continueVideo()
    elif (action=="stop"):
      myPlayer.stopVideo()
    #elif (action=="set_speed"):
    #elif (action=="jump_to"):
    elif (action=="set_res"):
      myPlayer.set_res("900", "900")
    else:
      myPlayer.printUsage()

    #TODO react to messages from the bus

    action = input("Command\n")

except GstdError:
  print("GstdError: Gstd IPC failed")
except GstcError:
  print("GstcError: Gstd python client failed")
else:
  print("PyGstc Video Player ended successfully")
