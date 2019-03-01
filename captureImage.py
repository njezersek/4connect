import picamera

cam = picamera.PiCamera()
cam.rotation = 180
cam.resolution = (720, 480)
cam.capture("capture.png")
