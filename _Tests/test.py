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
