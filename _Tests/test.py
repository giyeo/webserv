import json
import requests
import unittest

class MyTests(unittest.TestCase):
	def test1GetIndex(self):
		response = requests.get("http://localhost:8081/")
		headers = dict(response.headers)
		
		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"example.com")
		self.assertTrue("test1index" in response.text)
		
	def test1GetErrorTest(self):
		response = requests.get("http://localhost:8081/nonexist")
		headers = dict(response.headers)
		
		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"example.com")
		self.assertTrue("test1error" in response.text)

	def test2GetIndex(self):
		response = requests.get("http://localhost:8082/")
		headers = dict(response.headers)
		
		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"example.com")
		self.assertTrue("test2index" in response.text)
		
	def test2GetError(self):
		response = requests.get("http://localhost:8082/nonexist")
		headers = dict(response.headers)
		
		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"example.com")
		self.assertTrue("test2error" in response.text)

	def test2GetLocationIndex(self):
		response = requests.get("http://localhost:8082/location/")
		headers = dict(response.headers)
		
		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"example.com")
		self.assertTrue("locationtest2index" in response.text)

		
	def test2GetLocationError(self):
		response = requests.get("http://localhost:8082/location/nonexist")
		headers = dict(response.headers)
		
		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"example.com")
		self.assertTrue("locationtest2error" in response.text)
		
unittest.main()

# try:
# 	# Make a GET request
# 	response = httpx.get(url)

# 	# Check the response status code
# 	if response.status_code == 200:
# 		print(f"Request was successful (Status Code: {response.status_code})")
# 		# Process the response content if needed
# 		print(response.text)
# 	else:
# 		print(f"Request failed with Status Code: {response.status_code}")
# 		print(response.text)

# except httpx.RequestError as e:
# 	print(f"Request error: {e}")
# except httpx.HTTPError as e:
# 	print(f"HTTP error: {e}")