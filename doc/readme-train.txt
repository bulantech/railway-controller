1. Install flask web framework, ลง library ที่เกี่ยวข้อง
pip install Flask
pip install flask-cors

2. Copy railway-controller/ จาก pc ไปยัง pi /home/pi ด้วยโปรแกรม winSCP

3. Run, เปิด terminal
python /home/pi/railway-controller/train/app.py
# SEE IP ON CONSOLE, จะแสดง ip ที่จอ

4. Test output from pin input, press GPIO2 to ground, output GPIO17 is HIGH
จอวงจรทดสอบ สวิทช์ ต่อ GPIO2 กดต่อลงกราวด์, led ต่อ GPIO17

5. Test from web browser, ทดสอบยนเว็บบราวเซอร์

status
request:
	http://TRAIN_IP:8001/status
	
response: start or stop
	{
		"status": "stop"
	}
	
start
request:
	http://TRAIN_IP:8001/start
	
response: output high
	{
		"start": "OK"
	}
	
stop
request:
	http://TRAIN_IP:8001/stop
	
response: output low
	{
		"stop": "OK"
	}
