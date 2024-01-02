#import matplotlib.pylab as plt
import cv2
import numpy as np
import time
import serial
import time
from pyzbar.pyzbar import *

serial_data = serial.Serial('com13', 115200)

width = 480
height = 480
command = ""

# scale_w = 7 / 16
# scale_h = 11 / 18
# left_bottom = [0, height - 1]
# right_bottom = [width - 1, height - 1]
# left_up = [scale_w * width, scale_h * height]
# right_up = [(1 - scale_w) * width, scale_h * height]

def ROI(img, vertices):
    mask = np.zeros_like(img)

    if len(img.shape) > 2:
        channel_count = img.shape[2]
        ignore_mask_color = (255,) * channel_count

    else:
        ignore_mask_color = 255

    cv2.fillPoly(mask, vertices, ignore_mask_color)

    masked_img = cv2.bitwise_and(img, mask)
    cv2.imshow('mask', mask)
    return masked_img

def draw_the_lines(img, lines):
    command = ""
    img = np.copy(img)
    blank_image = np.zeros((img.shape[0], img.shape[1], 3), dtype=np.uint8)

    if lines is not None:
        for line in lines:
            x1sum = x2sum = y1sum = y2sum = 0
            x1avg = x2avg = y1avg = y2avg = 0
            for i in range(len(lines)):
                x1sum += lines[i][0][0]
                x2sum += lines[i][0][1]
                y1sum += lines[i][0][2]
                y2sum += lines[i][0][3]

            if len(lines) != 0:
                x1avg = int(x1sum / len(lines))
                x2avg = int(x2sum / len(lines))
                y1avg = int(y1sum / len(lines))
                y2avg = int(y2sum / len(lines))
            for x1, y1, x2, y2 in line:
                #print(x1, y1)
                # cy = np.max(y1)
                # cx = np.max(x1)
                if x2avg != x1avg:
                    grad = (y2 - y1) / (x2 - x1)
                elif x2avg == x1avg:
                    grad = -1
                if abs(grad) > 1:
                    cv2.line(blank_image, (x1, y1), (x2, y2), (255, 0, 255), thickness=3)
                    #print("Koordinat X Y", x1avg, y1avg)
                    cv2.line(blank_image, (x1avg, 0), (x1avg, 480), (255, 0, 0), 3)
                    cv2.line(blank_image, (0, y1avg), (640, y1avg), (255, 0, 0), 3)
                    
                    if 160 < x1avg < 250 or 380 < x1avg < 480:
                        #print("INI X Y", x1avg, y1avg)
                        cv2.line(blank_image, (x1avg, 0), (x1avg, 480), (0, 0, 255), 3)
                        cv2.line(blank_image, (0, y1avg), (640, y1avg), (0, 0, 255), 3)
                        print(x1avg)

                        if 160 < x1avg < 250 :
                            #print("Turn left")
                            command = "Left" + '\r'
                        
                        elif 380 < x1avg < 480:
                            #print("Turn Right")
                            command = "Right" + '\r'

                    elif 250 < x1avg < 380:
                        command = "Straight" + '\r'
                            
                    
                else:
                    continue

    img = cv2.addWeighted(img, 0.8, blank_image, 1, 0.0)
    return img, command

def process(image):
    command = ""
    #print("shape", image.shape)
    height = image.shape[0]
    width = image.shape[1]
    ROI_vertices = [
        (2*width/7, height/8),
        (5*width/7, height/8),
        (5*width/7, 2*height/3),
        (2*width/7, 2*height/3)
    ]
    #ROI_vertices = [
    #    (2*width/5, height/8),
    #    (3*width/5, height/8),
    #    (3*width/5, 2*height/3),
    #    (2*width/5, 2*height/3)
    #]
    #vertices = np.array([[left_bottom, left_up, right_up, right_bottom]], dtype = np.int32)
    gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    img_hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    lower_yellow = np.array([0, 0, 170], dtype="uint8")
    upper_yellow = np.array([186, 45, 255], dtype="uint8")

    mask_yellow = cv2.inRange(img_hsv, lower_yellow, upper_yellow)
    mask_white = cv2.inRange(gray, 200, 255)

    mask_yw = cv2.bitwise_or(mask_white, mask_yellow)
    mask_yw_image = cv2.bitwise_and(gray, mask_yw)

    cv2.imshow("color", mask_yw_image)

    blur = cv2.GaussianBlur(mask_yw_image, (5, 5), 0)
    ret, thresh = cv2.threshold(blur, 60, 255, cv2.THRESH_BINARY)
    thresh = cv2.erode(thresh, None, iterations=2)
    thresh = cv2.dilate(thresh, None, iterations=2)
    contours, hierarchy = cv2.findContours(thresh.copy(), 1, cv2.CHAIN_APPROX_NONE)
    cont = np.zeros((480, 480, 3), np.uint8)
    cv2.drawContours(cont, contours, -1, (0, 255, 0), 1)
    cv2.imshow('cont', cont)

    canny = cv2.Canny(cont, 100, 120)
    crop = ROI(canny, np.array([ROI_vertices], dtype = np.int32))
    cv2.imshow('canny', canny)
    cv2.imshow('ROI', crop)
    lines = cv2.HoughLinesP(crop,
                            rho=2,
                            theta=np.pi/180,
                            threshold=50,
                            lines=np.array([]),
                            minLineLength=40,
                            maxLineGap=100)
    image_with_lines, command = draw_the_lines(image, lines)
    return image_with_lines, command

cap = cv2.VideoCapture('TesLine.mp4')
cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
cap.set(cv2.CAP_PROP_FPS, 60)

#Moving JPG (MJPG), Asterisk (*) is used to formatting MJPG
cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*'MJPG'))

while cap.isOpened():
    ret, frame = cap.read()
    frame_process, command = process(frame)
    for barcode in decode(frame):
        barcode_data = barcode.data.decode('utf-8')
        #print(barcode_data)
        pts = np.array([barcode.polygon], np.int32)
        pts = pts.reshape((-1, 1, 2))
        cv2.polylines(frame, [pts], True, (255, 0, 255), 5)
        cv2.putText(frame, barcode_data, (barcode.rect[0], barcode.rect[1]), cv2.FONT_HERSHEY_COMPLEX, 0.9, (255, 0, 0), 3)
        command = barcode_data+'\r'
        serial_data.write(command.encode())
    serial_data.write(command.encode())
    #print(command)
    cv2.imshow('frame', frame)
    cv2.imshow('frame process', frame_process)
    cv2.moveWindow('frame', 0, 400)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()