
# -*- coding: utf-8 -*-

print("Controller System Start..")

from gpiozero import LED, Button

led = LED(17) # for test
button = Button(3, pull_up=True) # for test

# set input pin	
sensor1 = Button(14, pull_up=True)
sensor2 = Button(15, pull_up=True)
sensor3 = Button(18, pull_up=True)
sensor4 = Button(23, pull_up=True)
sensor5 = Button(24, pull_up=True)

sensor6 = Button(25, pull_up=True)
sensor7 = Button(8, pull_up=True)
sensor8 = Button(7, pull_up=True)
sensor9 = Button(12, pull_up=True)
sensor10 = Button(16, pull_up=True)

sensor11 = Button(20, pull_up=True)
sensor12 = Button(21, pull_up=True)
sensor13 = Button(26, pull_up=True)
sensor14 = Button(19, pull_up=True)
sensor15 = Button(13, pull_up=True)
	
# input event on เมื่อสัญาณเข้าเป็น 0
button.when_pressed = lambda : sensorOn(1) # for test
button.when_released = lambda : sensorOff(1) # for test

sensor1.when_pressed = lambda : sensorOn(1)
sensor1.when_released = lambda : sensorOff(1)
sensor2.when_pressed = lambda : sensorOn(2)
sensor2.when_released = lambda : sensorOff(2)
sensor3.when_pressed = lambda : sensorOn(3)
sensor3.when_released = lambda : sensorOff(3)
sensor4.when_pressed = lambda : sensorOn(4)
sensor4.when_released = lambda : sensorOff(4)
sensor5.when_pressed = lambda : sensorOn(5)
sensor5.when_released = lambda : sensorOff(5)

sensor6.when_pressed = lambda : sensorOn(6)
sensor6.when_released = lambda : sensorOff(6)
sensor7.when_pressed = lambda : sensorOn(7)
sensor7.when_released = lambda : sensorOff(7)
sensor8.when_pressed = lambda : sensorOn(8)
sensor8.when_released = lambda : sensorOff(8)
sensor9.when_pressed = lambda : sensorOn(9)
sensor9.when_released = lambda : sensorOff(9)
sensor10.when_pressed = lambda : sensorOn(10)
sensor10.when_released = lambda : sensorOff(10)

sensor11.when_pressed = lambda : sensorOn(11)
sensor11.when_released = lambda : sensorOff(11)
sensor12.when_pressed = lambda : sensorOn(12)
sensor12.when_released = lambda : sensorOff(12)
sensor13.when_pressed = lambda : sensorOn(13)
sensor13.when_released = lambda : sensorOff(13)
sensor14.when_pressed = lambda : sensorOn(14)
sensor14.when_released = lambda : sensorOff(14)
sensor15.when_pressed = lambda : sensorOn(15)
sensor15.when_released = lambda : sensorOff(15)

# ตัวแปรเก็บค่าการเปลี่ยแปลงของ sensor
dataChange = { "event": "none", "channel": 0, "count": 0.0}
dataChangeBuf = { "event": "none", "channel": 0, "count": 0.0}
timeCount=[]
for i in range(15):
	timeCount.append(0)

import time

# ฟังชั่งเมื่เซนต์เซอร์เปลี่นสถานะ
def sensorOn(channel):
	# led.on()	
	count = 0
	dataChange["event"] = "sensorOn"
	dataChange["channel"] = channel
	dataChange["count"] = 0
	timeCount[channel-1] = time.time()
	print('sensorOn', str(channel), count)
	
def sensorOff(channel):
	# led.off()	
	count = 0
	count = time.time() - timeCount[channel-1]
	dataChange["event"] = "sensorOff"
	dataChange["channel"] = channel
	dataChange["count"] = count
	print('sensorOff', str(channel), count)

# web server and socketio	
import socketio
import eventlet
import eventlet.wsgi

# นำเข้า flask library เป็น framework สำหรับจัดการ webserver
from flask import Flask, render_template, send_from_directory

socketConnect = False

# get local ip address
# นำเข้า socket library เพื่ออ่าน ip
import socket
def get_ip():
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	try:
		# doesn't even have to be reachable
		s.connect(('10.255.255.255', 1))
		IP = s.getsockname()[0]
	except:
		IP = '127.0.0.1'
	finally:
		s.close()
	return IP

print("IP: "+get_ip())

#สร่าง socketio server
sio = socketio.Server()
app = Flask(__name__, static_url_path='')
thread = None

# ฟังชั่ส่งข้อมูลการเปลี่ยนแปลงของ sensor ไปหน้าเว็บ
def background_thread():
	while True:
		sio.sleep(0.1)
		if dataChange != dataChangeBuf :
			# print('dataChange != dataChangeBuf ' )
			sio.emit('sensor', dataChange, namespace='/chat')
			dataChangeBuf["event"] = dataChange["event"]
			dataChangeBuf["channel"] = dataChange["channel"]	
                 
# route
@app.route('/')
def index():
	global thread
	if thread is None:
		thread = sio.start_background_task(background_thread)
	return render_template('index.html')
    
@app.route('/js/<path:path>')
def send_js(path):
	print("-----path ", path)
	return send_from_directory('js', path)

@app.route('/images/<path:path>')
def send_images(path):
  print("-----path images", path)
  return send_from_directory('images', path)
  
# socketio @sio.on คือรับค่ามาจากหน้าเว็บ        
@sio.on('connect', namespace='/chat')
def connect(sid, environ):
	socketConnect = True
	print("----- connect ", sid)

@sio.on('chat message', namespace='/chat')
def message(sid, data):
	print("message ", data)
	sio.emit('my response', {'data': 'Server generated event'}, namespace='/chat')

@sio.on('disconnect', namespace='/chat')
def disconnect(sid):
	socketConnect = False
	print('disconnect ', sid)

# start flask web server
if __name__ == '__main__':
	# wrap Flask application with engineio's middleware
	app = socketio.Middleware(sio, app)
	
	# deploy as an eventlet WSGI server
	eventlet.wsgi.server(eventlet.listen(('', 8000)), app)
