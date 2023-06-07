* This is the Compose Video Player

    - The CVP is intended to play a given set of images as video, this set of images have a place on the screen.


    - The manager is the part that loads all the images and start all the threads
        1. The communication object (it has a thread):
            + Has the outside world interface. It get a command and input into the system accordanly. I see 2 connections with the outside world. It is supposed to be implemented by a outside lib (not the core of the project)
                1.1 Connection 1: a mqtt listener that will receive a composed command and execute
                1.2 Connection 2: a http listener that will do the same
                1.3 Connection 3: a Serial interface that will receive a command and execute it accordanly.
        2. The player object (it has a thread):
            + This is the thread that will execute the video by playing the std sequence and add the overlays as requested by user
                2.1 sequences can be permanent (as they reach the end, they start from the beginning)
                2.2 sequences can have a not known number of images
        3. The general manager (main loop):
            + This thread monitors input from user (SIGINT and such), also monitors executions and keeps it clean after it ends.

Items that I know I need

* Show a PNG with alpha -- done -- it has no difference
* investigate the copy vs deep_copy of data
* show image full screen
