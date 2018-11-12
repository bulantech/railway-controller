# -*- coding: utf-8 -*-

print("Train System Start..")

from gpiozero import LED, Button # นำเข้า class LED, Button จาก gpiozero library
from time import sleep

led = LED(17) # กำนด output ที่ GPIO17
button = Button(2, pull_up=True) # กำนด input แบบ pullup ที่ GPIO2
status = "stop" # ตัวแปรสถานะ output 

# ปุ่มสำหรับทดสอบ
button.when_pressed = led.on # เมื่กดปุ่ output เป็น 1
button.when_released = led.off # เมื่กดปุ่ output เป็น 0

import socket # นำเข้า socket library เพื่ออ่าน ip
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

from flask import Flask, jsonify # นำเข้า flask library เป็น framework สำหรับจัดการ webserver
from flask_cors import CORS # นำเข้า flask_cors library เพื่อจัดการเกี่ยวกับการเรียกข้อมลเว็บข้ามโดเมน

app = Flask(__name__)
CORS(app)

@app.route('/') # เมื่อเข้าเว็บหน้า / หรือ /status
@app.route('/status')
def index():   
	return jsonify({"status": status}) #ตอบกลับข้อมูลแบบ json
    
@app.route('/start')
def start():   
	led.on() # output high	
	status = "start" # set status to 'start'
	return jsonify({"start": "OK"}) #ตอบกลับข้อมูลแบบ json
      
@app.route('/stop')
def stop():   
	led.off() # output low	
	status = "stop" # set status to 'stop'
	return jsonify({"stop": "OK"}) #ตอบกลับข้อมูลแบบ json

if __name__ == '__main__':
	app.run(host=get_ip(), port=8001) # run webserver
	#app.run()
	
