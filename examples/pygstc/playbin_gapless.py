# Copyright (c) RidgeRun, 2020 <support@ridgerun.com>
#
# The software contained in this file is free and unencumbered software
# released into the public domain. Anyone is free to use the software contained
# in this file as they choose, including incorporating it into proprietary
# software.

import sys
import threading
from pygstc.gstc import GstcError, GstdError, GstdClient
from pygstc.logger import CustomLogger

BUS_TIMEOUT = 200000000
BUS_FILTER = "error+warning"
PLAYBIN_NAME = "playbin"
PLAYBIN_PROP = "uri"
PLAYBIN_SIGNAL = "about-to-finish"
PIPE_NAME = "playbin_gapless"
# The identity is a workaround to force parse_launch to avoid setting
# the playbin as the top-level pipeline. It doesn't process anything
PIPE_DESC = "playbin name=" + PLAYBIN_NAME + " identity"

def bus_callback(player):
    player.bus_timeout(PIPE_NAME, BUS_TIMEOUT)
    player.bus_filter(PIPE_NAME, BUS_FILTER)

    while not player.done:
        resp = player.bus_read(PIPE_NAME)
        if not resp:
            continue

        if resp["type"] == "warning":
            sys.stderr.write("Warning: "+ str(resp["message"] + "\n"))
            continue

        if resp["type"] == "error":
            sys.stderr.write("Error: "+ str(resp["message"] + "\n"))
            player.is_error = True
            player.signal_disconnect(PIPE_NAME, PLAYBIN_NAME, PLAYBIN_SIGNAL)
            break

def wait_for_file_completion(player):
    # will block until the signal is received
    player.signal_connect(PIPE_NAME, PLAYBIN_NAME, PLAYBIN_SIGNAL)

def play_first_file(player, files):
    index = configure_next_file(player, files, 0)
    player.pipeline_play(PIPE_NAME)
    return index

def configure_next_file(player, files, index):
    nextfile = files[index]
    print ("Setting up new file to " + nextfile)
    player.element_set(PIPE_NAME, PLAYBIN_NAME, PLAYBIN_PROP, nextfile)

    # rotate the index in the list of files.
    return (index + 1) % len(files)

def cleanup_player(player):
    player.done = True
    player.bus_thread.join()
    player.signal_disconnect(PIPE_NAME, PLAYBIN_NAME, PLAYBIN_SIGNAL)
    player.pipeline_stop(PIPE_NAME)
    player.pipeline_delete(PIPE_NAME)

def prepare_player():
    logger = CustomLogger('playbin_gapless', loglevel='WARNING')
    player = GstdClient(logger=logger)
    player.pipeline_create(PIPE_NAME, PIPE_DESC)

    # Setup async bus callback
    player.is_error = False
    player.done = False
    player.bus_thread = threading.Thread(target=bus_callback, args=(player,))
    player.bus_thread.start()

    return player

def print_usage(proc):
    sys.stderr.write("Usage:\n")
    sys.stderr.write("\t" + proc + " FILE [FILE2 FILE3 ...]\n")
    sys.stderr.write("FILE(N) must be in the URI format. At least one file is needed.\nExample:\n")
    sys.stderr.write("\t" + proc + "file:///home/nvidia/file1.mp4 file:///opt/another_file.avi")

if __name__ == "__main__":
    print("Playbin Gapless Playback")

    if len(sys.argv) < 2:
        print_usage(sys.argv[0])
        sys.exit(1)

    player = None
    files = sys.argv[1:]

    try:
        player = prepare_player()
        index = play_first_file(player, files)

        # Main loop: wait for the playbin to emit the "about-to-finish" signal
        # and configure the next file
        while not player.is_error:
            # Configure the new file BEFORE receiving the
            # signal. Otherwise the new file name will not be queued
            # This is a limitation when using gstc.
            index = configure_next_file(player, files, index)

            if player.is_error:
                break

            wait_for_file_completion(player)

    except (GstdError, GstcError) as err:
        sys.stderr.write("Gstd error: " + str(err))
    except KeyboardInterrupt:
        pass
    except:
        sys.stderr.write("Unexpected error\n")

    if player:
        print("Cleaning up!")
        cleanup_player(player)
