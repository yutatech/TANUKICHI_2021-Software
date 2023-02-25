#
# TANUKICHI 2021
#
# Copyright (c) 2018~2023 Yuta Fujiyama
#
# This software is released under the MIT license.
# see http://opensource.org/licenses/mit-license.php

import sensor, image, time, math
from pyb import UART
from pyb import USB_VCP
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_vflip(True)

Orange = (42, 75, 34, 79, 21, 71)
Yellow = (48, 97, -11, 33, 32, 71)
Blue   = (29, 36, -19, -3, -22, -3)
#thresholds = [Orange, Blue]
thresholds = [Orange, Yellow]
centerx = 174
centery = 124
visionR = 105
visionr = 45
sensor.set_auto_gain(False, gain_db = 8)
sensor.set_auto_exposure(False, exposure_us = 2500)
sensor.set_auto_whitebal(False, rgb_gain_db = [-5, -5, -1.5])
sensor.set_gainceiling(128)
sensor.set_contrast(3)
sensor.set_brightness(2)
sensor.set_saturation(3)

sensor.skip_frames()

uart = UART(3, 115200 ,timeout=10, timeout_char = 1000)
usb_vcp = USB_VCP()



bAngle = 0
bDistance = 1
gAngleL = 2
gAngleC = 3
gAngleR = 4
gDistance = 5
FPS = 6
X = 0
Y = 1
W = 2
H = 3

data = [0, 0, 0, 0, 0, 0, 0, 254]

def compress(data):
    if data > 73:
        data =  (data+73)/2
    elif data < -73:
        data = (data-73)/2
    data+=126
    return int(data)

def decompress(data):
    data -= 126
    if data > 73:
        data = data*2 - 73
    elif data < -73:
        data = data*2 + 73
    return data

class Ball:
    x = 0
    y = 0
    rect = [0, 0, 0, 0]

print("gain : ", sensor.get_gain_db())
print("exposure : ", sensor.get_exposure_us())
print("whitebal : ", sensor.get_rgb_gain_db())
clock = time.clock()

while(True):
    clock.tick()
    img = sensor.snapshot()
    img.draw_circle(centerx,centery, visionr, color = [0,0,0], thickness = 1, fill = True)

    ballCount = 0
    goalCount = 0
    maxBall = Ball()
    maxGoal = [0, 0, 0, 0]
    secGoal = [0, 0, 0, 0]
    for blob in img.find_blobs(thresholds, roi = [centerx-105,centery-105,210,210], pixels_threshold = 5, area_threshold = 5, marge = False, margein = 2):
        if blob.code() == 1 << 0:
            if ballCount == 0 or maxBall.rect[W] * maxBall.rect[H] < blob.w() * blob.h():
                maxBall.rect = blob.rect()
                maxBall.x = blob.cx()
                maxBall.y = blob.cy()
                ballCount += 1
        elif blob.code() == 1 << 1:
            if blob.w() * blob.h() > 50:
                if goalCount == 0:
                    maxGoal = blob.rect()
                else:
                    if maxGoal[W] * maxGoal[H] < blob.w() * blob.h():
                        secGoal = maxGoal
                        maxGoal = blob.rect()
                    elif secGoal[W] * secGoal[H] < blob.w() * blob.h():
                        secGoal = blob.rect()
                goalCount += 1
                if usb_vcp.isconnected():
                    img.draw_rectangle(blob.rect(), color = [0,0,255])

    if ballCount == 0:
        data[bAngle] = 255
        data[bDistance] = 255
    else:
        blobX = maxBall.x - centerx
        blobY = centery - maxBall.y
        data[bAngle] = compress(math.atan2(blobX, blobY) * 57.3)
        data[bDistance] = int(math.sqrt(blobX * blobX + blobY * blobY))
        if usb_vcp.isconnected():
            img.draw_rectangle(maxBall.rect, color = [255,0,0])
            img.draw_line([centerx, centery, maxBall.x, maxBall.y], color = [255, 0, 0])

    if goalCount == 0:
        data[gAngleL] = 255
        data[gAngleC] = 255
        data[gAngleR] = 255
        data[gDistance] = 255
    else:
        if goalCount == 1 or secGoal[W] * secGoal[H] == 0 or abs(maxGoal[X] - secGoal[X] + (maxGoal[W] - secGoal[W])/2) - 10 > (maxGoal[W] + secGoal[W]) / 2 or abs(maxGoal[Y] - secGoal[Y] + (maxGoal[H] - secGoal[H])/2) - 10 > (maxGoal[H] + secGoal[H]) / 2:
            tar = maxGoal
        else:
            tar = [0, 0, 0, 0]
            tar[X] = min(maxGoal[X], secGoal[X])
            tar[Y] = min(maxGoal[Y], secGoal[Y])
            tar[W] = max(maxGoal[X] + maxGoal[W], secGoal[X] + secGoal[W]) - min(maxGoal[X], secGoal[X])
            tar[H] = max(maxGoal[Y] + maxGoal[H], secGoal[Y] + secGoal[H]) - min(maxGoal[Y], secGoal[Y])

        blobX = tar[X] + tar[W]/2 - centerx
        blobY = centery - tar[Y] - tar[H]/2
        data[gAngleC] = int(math.atan2(blobX, blobY) * 57.3)
        data[gDistance] = int(math.sqrt(blobX*blobX+blobY*blobY))

        data[gAngleL]=0
        data[gAngleR]=0
        #AngleL
        if (centerx <= tar[X] and tar[Y] <= centery) or (centery <= tar[Y]+tar[H]/2 and centerx <= tar[X]+tar[W]/2 and tar[Y] <= centery):
            ratio = (math.cos((data[gAngleC]-45)*0.0349+3.14)+1)*0.5
            data[gAngleL] = math.atan2(tar[X]+tar[W]*ratio-centerx, centery-tar[Y])*57.3
        elif (centery <= tar[Y] and centerx <= tar[X]+tar[W]) or (tar[X]+tar[W]/2 <= centerx and centery <= tar[Y]+tar[H]/2 and centerx <= tar[X]+tar[W]):
            ratio = (math.cos((data[gAngleC]-135)*0.0349+3.14)+1)*0.5
            data[gAngleL] = math.atan2(tar[X]+tar[W]-centerx, centery-tar[Y]-tar[H]*ratio)*57.3
        elif (tar[X]+tar[W] <= centerx and centery <= tar[Y]+tar[H]) or (tar[Y]+tar[H]/2 <= centery and tar[X]+tar[W]/2 <= centerx and centery <= tar[Y]+tar[H]):
            ratio = (math.cos((data[gAngleC]-225)*0.0349)+1)*0.5
            data[gAngleL] = math.atan2(tar[X]+tar[W]*ratio-centerx, centery-tar[Y]-tar[H])*57.3
        elif (tar[Y]+tar[H] <= centery and tar[X] <= centerx) or (centerx <= tar[X]+tar[W]/2 and tar[Y]+tar[H]/2 <= centery and tar[X] <= centerx):
            ratio = (math.cos((data[gAngleC]-315)*0.0349)+1)*0.5
            data[gAngleL] = math.atan2(tar[X]-centerx, centery-tar[Y]-tar[H]*ratio)*57.3

        #AngleR
        if (tar[Y]+tar[H] <= centery and centerx <= tar[X]+tar[W]) or (tar[X]+tar[W]/2 <= centerx and tar[Y]+tar[H]/2 <= centery and centerx <= tar[X]+tar[W]):
            ratio = (math.cos((data[gAngleC]-45)*0.0349)+1)*0.5
            data[gAngleR] = math.atan2(tar[X]+tar[W]-centerx, centery-tar[Y]-tar[H]*ratio)*57.3
        elif (centerx <= tar[X] and centery <= tar[Y]+tar[H]) or (tar[Y]+tar[H]/2 <= centery and centerx <= tar[X]+tar[W]/2 and centery <= tar[Y]+tar[H]):
            ratio = (math.cos((data[gAngleC]-135)*0.0349+3.14)+1)*0.5
            data[gAngleR] = math.atan2(tar[X]+tar[W]*ratio-centerx, centery-tar[Y]-tar[H])*57.3
        elif (centery <= tar[Y] and tar[X] <= centerx) or (centerx <= tar[X]+tar[W]/2 and centery <= tar[Y]+tar[H]/2 and tar[X] <= centerx):
            ratio = (math.cos((data[gAngleC]-225)*0.0349+3.14)+1)*0.5
            data[gAngleR] = math.atan2(tar[X]-centerx, centery-tar[Y]-tar[H]*ratio)*57.3
        elif (tar[X]+tar[W] <= centerx and tar[Y] <= centery) or (centery <= tar[Y]+tar[H]/2 and tar[X]+tar[W]/2 <= centerx and tar[Y] <= centery):
            ratio = (math.cos((data[gAngleC]-315)*0.0349)+1)*0.5
            data[gAngleR] = math.atan2(tar[X]+tar[W]*ratio-centerx, centery-tar[Y])*57.3

        if usb_vcp.isconnected():
            img.draw_rectangle(tar, color = [255,255,0])
            img.draw_line([centerx, centery, centerx + int(180 * math.sin(data[gAngleR] * 3.14 / 180)), centery - int(180 * math.cos(data[gAngleR] * 3.14 / 180))], color = [0, 144, 0])
            img.draw_line([centerx, centery, centerx + int(data[gDistance] * math.sin(data[gAngleC] * 3.14 / 180)), centery - int(data[gDistance] * math.cos(data[gAngleC] * 3.14 / 180))], color = [255, 255, 0])
            img.draw_line([centerx, centery, centerx + int(180 * math.sin(data[gAngleL] * 3.14 / 180)), centery - int(180 * math.cos(data[gAngleL] * 3.14 / 180))], color = [0, 144, 0])

        data[gAngleC] = compress(data[gAngleC])
        data[gAngleL] = compress(data[gAngleL])
        data[gAngleR] = compress(data[gAngleR])

    data[FPS] = int(clock.fps())
    for i in range(8):
        uart.writechar(data[i])
    if uart.any() > 0:
        if uart.read(1) == b'b':
            thresholds=[Orange, Blue]
        else:
            thresholds=[Orange, Yellow]

    #print(clock.fps())

    if usb_vcp.isconnected():
        img.crop(roi=[centerx-visionR,centery-visionR,visionR*2,visionR*2],x_size=visionR*2, y_size=visionR*2)
        img.compress_for_ide(quality=80)
        #print(clock.fps())
        #print(data[bAngle])
        #print("%d %d %d %d %d %d" %(decompress(data[bAngle]), data[bDistance], data[gDistance], decompress(data[gAngleL]), decompress(data[gAngleC]), decompress(data[gAngleR])))
