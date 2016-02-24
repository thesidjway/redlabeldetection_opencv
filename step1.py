import cv2
import numpy as np

while(1):
     # Take each frame
     frame = cv2.imread("testimage.png")
     # Convert BGR to HSV
     hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
     # define range of blue color in HSV
     lower_red = np.array([150,50,50])
     upper_red = np.array([180,255,255])
     lower_red1 = np.array([0,50,50])
     upper_red1 = np.array([15,255,255])
     # Threshold the HSV image to get only blue colors
     mask = cv2.inRange(hsv, lower_red, upper_red)
     mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
     # Bitwise-AND mask and original image
     res = cv2.bitwise_or(mask,mask1)
     #res = cv2.bitwise_and(frame,frame, mask= mask)
     cv2.imshow('mask',res)
     k = cv2.waitKey(5) & 0xFF
     if k == 27:
          break
cv2.destroyAllWindows()