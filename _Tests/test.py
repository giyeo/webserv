import requests
import unittest

class MyTests(unittest.TestCase):
	def test1GetIndex(self):
		response = requests.get("http://www.testando.com:8081/")
		headers = dict(response.headers)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"ft_webserver/0.1")
		self.assertTrue("test1index" in response.text)
		
	def test1GetErrorTest(self):
		response = requests.get("http://testando:8081/nonexe")
		headers = dict(response.headers)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"ft_webserver/0.1")
		print(response.text)
		self.assertTrue("test1error" in response.text)

	def test2GetIndex(self):
		response = requests.get("http://rafinha.com:8082/")
		headers = dict(response.headers)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"ft_webserver/0.1")
		self.assertTrue("test2index" in response.text)
		
	def test2GetError(self):
		response = requests.get("http://rafinha.org:8082/nonexist")
		headers = dict(response.headers)
		
		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"ft_webserver/0.1")
		self.assertTrue("test2error" in response.text)

	def test2GetLocationIndex(self):
		response = requests.get("http://rafinha.com:8082/location/")
		headers = dict(response.headers)
		# 
		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"ft_webserver/0.1")
		self.assertTrue("locationtest2index" in response.text)

	def test2GetLocationError(self):
		response = requests.get("http://rafinha.com:8082/location/nonexist")
		headers = dict(response.headers)
		
		self.assertEqual(response.status_code, 200)
		self.assertEqual(headers.get("Server"),"ft_webserver/0.1")
		self.assertTrue("locationtest2error" in response.text)
	
	def test3GetDefaultErrorPage(self):
		res = "404"
		response = requests.get("http://edu.com:8083/")
		headers = dict(response.headers)
		self.assertEqual(headers.get("Server"),"ft_webserver/0.1")
		self.assertTrue(res in response.text)
		response = requests.get("http://edu.com:8083/asd")
		self.assertTrue(res in response.text)
		response = requests.get("http://edu.com:8083/asd/")
		self.assertTrue(res in response.text)
		response = requests.get("http://edu.com:8083/location/")
		self.assertTrue(res in response.text)
		response = requests.get("http://edu.com:8083/location")
		self.assertTrue(res in response.text)
		response = requests.get("http://edu.com:8083/location/asd")
		self.assertTrue(res in response.text)

unittest.main()
