#The software contained in this file is free and unencumbered software released into the public domain.
#Anyone is free to use the software contained in this file as they choose, including incorporating
#it into proprietary software.
import time
import sys
import threading
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
    self.pipe_created = False

    #Error handler thread
    self.running = True
    self.thErrorHandler = threading.Thread(target=self.errPlayerHandler, args=())
    self.thErrorHandler.daemon = True
    self.thErrorHandler.start()

  def errPlayerHandler(self):
    while (self.running):
      if(self.pipe_created):
        self.gstc.bus_timeout("p0", -1)
        self.gstc.bus_filter("p0", "error+eos+warning")
        resp = self.gstc.bus_read("p0")
        print(resp)

  def finish(self):
    self.running = False

  def openFile(self, videoPath):
    #Create pipeline object
    self.pipeline = "filesrc location="+videoPath+" ! decodebin ! videoconvert ! \
          videoscale ! capsfilter name=cf ! autovideosink"

  def playVideo(self):
    print("Playing")
    if (self.pipe_exists("p0")):
      self.gstc.pipeline_delete("p0")

    self.gstc.pipeline_create("p0", self.pipeline)
    self.pipe_created = True
    self.gstc.pipeline_play("p0")

  def pauseVideo(self):
    print("Playing")
    self.gstc.pipeline_pause("p0")

  def continueVideo(self):
    print("Playing")
    self.gstc.pipeline_play("p0")

  def stopVideo(self):
    print("Stop")
    if (self.pipe_exists("p0")):
      self.gstc.pipeline_stop("p0")
      self.gstc.pipeline_delete("p0")
      self.pipe_created = False

  def setRes(self, width, height):
    print("Changing video resolution")
    if (self.pipe_exists("p0")):
      self.gstc.pipeline_delete("p0")
    self.gstc.pipeline_create("p0", self.pipeline)
    self.gstc.element_set("p0", "cf", "caps", "video/x-raw,width="+width+",height="+height+"")
    self.gstc.pipeline_play("p0")

  def setSpeed(self, speed):
      self.gstc.event_seek("p0", rate=speed, format=3, flags=1, start_type=1, start=0, end_type=1, end=-1)

  def jumpTo(self, position):
    self.gstc.event_seek("p0", rate=1.0, format=3, flags=1, start_type=int(position*10^9), start=0, end_type=0, end=0)

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


if __name__ == "__main__":
  print("Sample PyGstC Video Player")

  if(len(sys.argv) != 2):
    printError()
    sys.exit(0)

  try:
    myPlayer = GstcPlayer()
    myPlayer.openFile(sys.argv[1])
    myPlayer.playVideo()
    action = ["running"]

    while (action[0] != "exit"):

      if (action[0]=="play"):
        myPlayer.playVideo()
      elif (action[0]=="pause"):
        myPlayer.pauseVideo()
      elif (action[0]=="continue"):
        myPlayer.continueVideo()
      elif (action[0]=="stop"):
        myPlayer.stopVideo()
      elif (action[0]=="set_speed"):
        if (len(action) == 2 and abs(float(action[1])) <= 1):
          myPlayer.setSpeed(float(action[1]))
        else:
          print("Playback speed valid range: [-1, 1]. 0 is not allowed")

      elif (action[0] == "play_fw"):
        myPlayer.setSpeed(float(1))

      elif (action[0] == "play_bw"):
        myPlayer.setSpeed(float(-1))

      elif (action[0]=="jump"):
        if (len(action) == 2 and action[1].isnumeric() and int(action[1]) >= 0):
          myPlayer.jumpTo(int(action[1]))
        else:
          print("jump $NANO_SECS")

      elif (action[0]=="set_res"):
        if (len(action) == 3 and action[1].isnumeric() and action[2].isnumeric()):
          myPlayer.setRes(action[1], action[2])
        else:
          print("Use set_res $X $Y")
      else:
        myPlayer.printUsage()

      #TODO react to messages from the bus

      action = input("Command\n").split()
      if ( len(action)==0):
        action = "running"

  except GstdError:
    print("GstdError: Gstd IPC failed")
  except GstcError:
    print("GstcError: Gstd python client failed")
  else:
    myPlayer.finish()
    print("PyGstc Video Player ended successfully")
