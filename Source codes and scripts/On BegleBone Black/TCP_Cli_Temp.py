#!/usr/bin/python3

import socket
import time
import cgitb
cgitb.enable()

client=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
ip="192.168.2.112"
port=1234
address=(ip,port)
client.connect(address)
data = 'led_on'
client.sendall(data.encode('utf-8'))
time.sleep(2)
temp = client.recv(1024)
data = 'led_off'
client.sendall(data.encode('utf-8'))
time.sleep(0.02)
client.close()


print("Content-type: text/html\n\n")

print("<html>\n<body>")
print("<div style=\"with:100%; font-size: 40px; font-weight bold; text-align: center:\">")
print("The LED was turned On and then turned Off again.")

print('<br />')
print("Temperature: ",temp.decode('utf-8'))
print("Celsius grads")

print("</div>\n</body>\n</html>")
