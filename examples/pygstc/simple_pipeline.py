# The software contained in this file is free and unencumbered software
# released into the public domain. Anyone is free to use the software contained
# in this file as they choose, including incorporating it into proprietary
# software.
import sys
import threading
from pygstc.gstc import GstcError, GstdError, GstdClient
from pygstc.logger import CustomLogger


class GstcPlayer:

    def __init__(self):
        # Create a custom logger with loglevel=WARNING
        self.gstd_logger = CustomLogger('simple_playback', loglevel='WARNING')
        # Create the client with the logger
        self.gstc = GstdClient(logger=self.gstd_logger)
        self.pipeline = ""
        self.pipeName = "pygstc_player"
        self.playingSpeed = 1

    def openVideo(self, videoPath):
        ret = True
        # Check if pipe is already created
        if (self.pipe_exists(self.pipeName)):
            ret = False
        else:
            self.openFile(videoPath)
            self.playVideo()

        return ret

    def errPlayerHandler(self):
        self.gstc.bus_timeout(self.pipeName, 200000000)
        self.gstc.bus_filter(self.pipeName, "error+eos+warning")

        while (True):
            try:
                resp = self.gstc.bus_read(self.pipeName)
                if (resp is not None and resp["type"] == "error"):
                    print("Player Error. Playing stopped")
                elif (resp is not None and resp["type"] == "eos"):
                    print("Player reached end of stream")
            except GstdError:
                break
            except GstcError:
                break

    def openFile(self, videoPath):
        # Fill pipeline
        self.pipeline = "filesrc location="+videoPath+" ! decodebin ! \
          videoconvert ! videoscale ! autovideosink"

    def playVideo(self):
        print("Video playing")
        if (not self.pipe_exists(self.pipeName)):
            # Create pipeline object
            self.gstc.pipeline_create(self.pipeName, self.pipeline)
            self.gstc.pipeline_play(self.pipeName)
            self.thErrorHandler = threading.Thread(
                                    target=self.errPlayerHandler,
                                    args=())
            self.thErrorHandler.start()

        else:
            # Continue playing after pause
            self.gstc.pipeline_play(self.pipeName)

    def pauseVideo(self):
        print("Video paused")
        self.gstc.pipeline_pause(self.pipeName)

    def stopVideo(self):
        print("Video stopped")
        if (self.pipe_exists(self.pipeName)):
            self.gstc.pipeline_stop(self.pipeName)
            self.gstc.pipeline_delete(self.pipeName)
            self.thErrorHandler.join()

    def setSpeed(self, speed):
        print("Setting play speed to: "+str(speed))
        self.playingSpeed = speed
        self.gstc.event_seek(self.pipeName,
                             rate=self.playingSpeed,
                             format=3,
                             flags=1,
                             start_type=1,
                             start=0,
                             end_type=1,
                             end=-1)

    def jumpTo(self, position):
        print("Jump to time: "+str(position))
        self.gstc.event_seek(self.pipeName,
                             rate=float(self.playingSpeed),
                             format=3,
                             flags=1,
                             start_type=1,
                             start=(position*(1000000000)),
                             end_type=0,
                             end=-1)

    def pipe_exists(self, pipe_name):
        # Check if pipe is already created
        existing_pipes = self.gstc.list_pipelines()
        if (existing_pipes == []):
            ret = False
        elif(existing_pipes[0]['name'] == pipe_name):
            ret = True
        else:
            ret = False
        return ret


def printUsage():
    print("play: To play and run")
    print("pause: To pause the video")
    print("stop: To stop and close the playing")
    print("set_speed $SPEED")
    print("jump $TIME [in seconds]")


def printError():
    print("To use run: python3 simple_pipeline.py $VIDEO_PATH",
          file=sys.stderr)


if __name__ == "__main__":
    print("Sample PyGstC Video Player")

    if(len(sys.argv) != 2):
        printError()
        sys.exit(0)
    try:
        myPlayer = GstcPlayer()

        if(not myPlayer.openVideo(sys.argv[1])):
            print("Pipeline already exits", file=sys.stderr)
            sys.exit(0)

        action = ["running"]
        while (action[0] != "exit"):

            if (action[0] == "play"):
                myPlayer.playVideo()
            elif (action[0] == "pause"):
                myPlayer.pauseVideo()
            elif (action[0] == "stop"):
                myPlayer.stopVideo()
            elif (action[0] == "set_speed"):
                if (len(action) == 2):
                    myPlayer.setSpeed(float(action[1]))
                else:
                    print("Playback speed valid range: [-1, 1]. 0 is not \
                           allowed")

            elif (action[0] == "jump"):
                if (len(action) == 2 and
                   action[1].isnumeric() and
                   int(action[1]) >= 0):
                    myPlayer.jumpTo(int(action[1]))
                else:
                    print("jump $SECS")

            else:
                printUsage()

            action = input("Command\n").split()
            if(len(action) == 0):
                action = "running"

    except GstdError as err:
        # GstdError: Gstd IPC failed
        print("GStreamer Daemon failed with code: "+str(err), file=sys.stderr)
    except GstcError as err:
        # GstcError: Gstd python client failed
        print("GStreamer Client failed with code: "+str(err), file=sys.stderr)
    else:
        myPlayer.stopVideo()
        print("PyGstc Video Player ended successfully")
