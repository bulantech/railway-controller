from gpiozero import LED, Button
from time import sleep

led = LED(17) # comment
button = Button(2, pull_up=True)
status = "stop"

print("test.py Start..")


""" 
while True:
	# led on off
	print("led.on()")
	led.on()
	sleep(1)
	print("led.off()")
	led.off()
	sleep(1)
	
	# input button
	if button.is_pressed:
		led.on()		
	else:
		led.off()	
"""	

from flask import Flask, jsonify

app = Flask(__name__)

@app.route('/')
@app.route('/status')
def index():   
	return jsonify({"status": status})
    
@app.route('/run')
def run():   
	led.on()	
	return jsonify({"run": "OK"})
    
    
@app.route('/stop')
def stop():   
	led.off()	
	return jsonify({"stop": "OK"})

if __name__ == '__main__':
	# deploy as an eventlet WSGI server
	#eventlet.wsgi.server(eventlet.listen(('', 8001)), app)
	app.run(host="127.0.0.1", port=5001)
