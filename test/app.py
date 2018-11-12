

import eventlet
import eventlet.wsgi


import socketio
from flask import Flask, render_template, send_from_directory

sio = socketio.Server()
app = Flask(__name__, static_url_path='')

@app.route('/')
def index():
    """Serve the client-side application."""
    return render_template('index.html')

@app.route('/js/<path:path>')
def send_js(path):
	return send_from_directory('js', path)

@sio.on('connect', namespace='/train')
def connect(sid, environ):
    print("-------- connect ", sid)
    
        
@sio.on('connect', namespace='/chat')
def connect(sid, environ):
    print("connect ", sid)

@sio.on('chat message', namespace='/chat')
def message(sid, data):
    print("message ", data)
    sio.emit('reply', room=sid)

@sio.on('disconnect', namespace='/chat')
def disconnect(sid):
    print('disconnect ', sid)

if __name__ == '__main__':
	# wrap Flask application with engineio's middleware
	app = socketio.Middleware(sio, app)
	
	# deploy as an eventlet WSGI server
	eventlet.wsgi.server(eventlet.listen(('', 8000)), app)
	
