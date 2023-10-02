#!/usr/bin/env python
import cgi
import cgitb
import sys
import os
# Enable detailed error messages for debugging (remove in production)
cgitb.enable()

# Print the Content-type header
# print("Content-type: text/html\n")

# Get the request method (GET or POST)
request_method = os.environ.get("REQUEST_METHOD")

# Parse the query parameters for GET requests
if request_method == "GET":
    form = cgi.FieldStorage()
    input_data = form.getvalue("input_data", "")

# Parse the form data for POST requests
elif request_method == "POST":
    form = cgi.FieldStorage()
    input_data = form.getvalue("input_data", "")

# Handle the request based on the method
if request_method == "GET":
    print("<html><head><title>GET Request</title></head><body>")
    print("<h1>GET Request Received</h1>")
    print("<p>Input Data: {}</p>".format(input_data))
    print("</body></html>")

elif request_method == "POST":
    print("<html><head><title>POST Request</title></head><body>")
    print("<h1>POST Request Received</h1>")
    print("<p>Input Data: {}</p>".format(input_data))
    print("</body></html>")