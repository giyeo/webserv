#!/usr/bin/env python3

import cgi
import sys
import os
from datetime import datetime
# Read data from standard input (file descriptor 0)
data = sys.stdin.buffer.read().decode('utf-8')

path = os.environ.get("PATH_INFO")
request_method = os.environ.get("REQUEST_METHOD")
# Process the data as needed
result = f"Received data: {data}"

def send_http_response(fd, content, error):
	# Get the current time for the Date header
	current_time = datetime.utcnow().strftime('%a, %d %b %Y %H:%M:%S GMT')

	errors = {200: "OK", 404: "Not Found",403: "Forbidden",500: "Internal server error",201: "Created"}

	# Construct the HTTP response headers
	response_headers = (
		"HTTP/1.1 {error} {error_text}\r\n"
		"Date: {date}\r\n"
		"Server: ft_webserver\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: {length}\r\n"
		"Connection: close\r\n"
		"\r\n"
	).format(date=current_time, length=len(content), error=error, error_text=errors[error])

	# Convert headers and content to bytes
	response_bytes = response_headers.encode('utf-8') + content.encode('utf-8')

	# Send the HTTP response to the file descriptor
	os.write(fd, response_bytes)
	os.close(fd)


clientFd = int(os.environ.get("CLIENT_FD"))
file_path = path + os.environ.get("UPLOADNAME")

if request_method == "GET":
    try:
        with open(file_path, "r") as file:
            content = file.read()
            send_http_response(clientFd, content, 200)
    except FileNotFoundError:
        send_http_response(clientFd, "File not found", 404)
    except PermissionError:
        send_http_response(clientFd, "Permission error", 403)
    except IOError as e:
        send_http_response(clientFd, f"Error reading the file: {e}", 500)

if(request_method == "POST"):
	with open(file_path, "w") as file:
		double_crlf_pos = data.find("\r\n\r\n")
		extracted_value = data[double_crlf_pos + 4:]
		file.write(extracted_value)
		res = (f"The file {file_path} has been created.")
		send_http_response(clientFd, res, 201)

if(request_method == "DELETE"):
	res = ""
	if os.path.exists(file_path):
		os.remove(file_path)
		res = (f"The file {file_path} has been deleted."), 200
	else:
		res = (f"The file {file_path} does not exist."), 404
	send_http_response(clientFd, *res)
