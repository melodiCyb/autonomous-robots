import cv2
import imutils
from imutils.video import VideoStream
import datetime
import time
import math
from collections import deque
import numpy as np


def detect_positions(pinkLower, pinkUpper, radius_upper, radius_lower, video, save_samples):


    # initialize frame counter,
    # and the coordinate deltas
    counter = 0
    (dX, dY) = (0, 0)

    # if runs on a recorded video
    if video:
        vs = cv2.VideoCapture('recording2.mp4')
    # from camera
    else:
        vs = VideoStream(src=0).start()
        # allow the camera to warm up
        time.sleep(2.0)
        
    # keep looping
    while True:
        # grab the current frame
        # frame = cv2.imread("test.png")
        frame = vs.read()
    
        text = "Unaligned"
        # resize the frame, blur it, and convert it to the HSV
        # color space
        frame = imutils.resize(frame, width=1000)
        height = frame.shape[0]
        width = frame.shape[1]
        cX = width // 2
        cY = height // 2
        cv2.circle(frame, (cX, cY), 5, (255, 255, 255), -1)
        cv2.putText(frame, "centroid", (cX-25, cY-25),cv2.FONT_HERSHEY_SIMPLEX, 0.50, (255, 255, 255), 2)
        blurred = cv2.GaussianBlur(frame, (11, 11), 0)
        hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

        # construct a mask for the color "pink", then perform
        # a series of dilations and erosions to remove any small
        # blobs left in the mask


        mask = cv2.inRange(hsv, pinkLower, pinkUpper)
        mask = cv2.erode(mask, None, iterations=2)
        mask = cv2.dilate(mask, None, iterations=2)
        # find contours in the mask and initialize the current
        # (x, y) center of the target
        cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL,
            cv2.CHAIN_APPROX_SIMPLE)
        cnts = imutils.grab_contours(cnts)
        center = None

        # only proceed if at least one contour was found
        if len(cnts) > 0:      
            # find the largest contour in the mask, then use
            # it to compute the minimum enclosing circle and
            # centroid
            c = max(cnts, key=cv2.contourArea)
            ((x, y), radius) = cv2.minEnclosingCircle(c)
            M = cv2.moments(c)
            center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
            # only proceed if the radius meets a minimum size

            if radius > radius_lower and radius < radius_upper:
                cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 255), 2)
                cv2.circle(frame, (center[0], center[1]), 2, (255, 255, 255), -1)
                
            # calculate x,y diff of centroid and the object
            x_diff = cX - center[0]
            y_diff = cY - center[1]
            # calculate distance
            distance = math.sqrt((x_diff**2)+(y_diff**2))
            if distance < 10:
                text = 'Aligned'
                
            # draw object status and x, y diff
            cv2.putText(frame, "Object Status: {}".format(text), (10, 20),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
            cv2.putText(frame, "dx: {}, dy: {}".format(x_diff, y_diff),
                (10, 40), cv2.FONT_HERSHEY_SIMPLEX,
                0.5, (0, 0, 255), 1)
            
        if save_samples:  
            if counter % 20 == 0:
            # save sample frames
                cv2.imwrite("frame_%d.jpg" % counter, frame)

        # show the frame to our screen and increment the frame counter
        cv2.imshow("Frame", frame)
        key = cv2.waitKey(1) & 0xFF
        counter += 1

        # if the 'q' key is pressed, stop the loop
        if key == ord("q"):
            break
    if not video:
        # stop the camera video stream

        vs.stop()

    # close all windows
    cv2.destroyAllWindows()

if __name__ == '__main__':
    # define the lower and upper boundaries of the "pink"
    # ball in the HSV color space
    pinkLower = (0, 100, 100)
    pinkUpper = (168, 255, 255)
    radius_upper = 15
    radius_lower = 5
    video = True
    save_samples = False
    detect_positions(pinkLower, pinkUpper, radius_upper, radius_lower, video, save_samples)