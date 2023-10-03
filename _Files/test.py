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

html_response = """<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Fancy Page</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f2f2f2;
        }
        .container {
            max-width: 600px;
            margin: 0 auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.2);
        }
        h1 {
            color: #333;
            text-align: center;
        }
        p {
            color: #666;
            font-size: 18px;
            line-height: 1.6;
        }
        .button {
            display: inline-block;
            padding: 10px 20px;
            background-color: #007bff;
            color: #fff;
            text-decoration: none;
            border-radius: 5px;
            transition: background-color 0.3s ease;
        }
        .button:hover {
            background-color: #0056b3;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Welcome to CGI Script Page</h1>
        <p>This is a simple HTML response with some fancy CSS styling.</p>
    </div>
</body>
</html>"""


if request_method == "GET":
    print(html_response)


elif request_method == "POST":
    print("<html><head><title>POST Request</title></head><body>")
    print("<h1>POST Request Received</h1>")
    print("<p>Input Data: {}</p>".format(input_data))
    print("</body></html>")