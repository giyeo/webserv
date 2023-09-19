#include "Response.hpp"

Response::Response(
	std::string http_version,
	std::string status_code,
	std::string status_text,
	std::string content_type,
	std::string content_length,
	std::string date,
	std::string server,
	std::string cache_control,
	std::string set_cookie,
	std::string location,
	std::string connection,
	std::string response_body
) {
	res.http_version = http_version;
	res.status_code = status_code;
	res.status_text = status_text;
	res.content_type = content_type;
	res.content_length = content_length;
	res.date = date;
	res.server = server;
	res.cache_control = cache_control;
	res.set_cookie = set_cookie;
	res.location = location;
	res.connection = connection;
	res.response_body = response_body;
}

std::string Response::toString() {
	std::string response = "";
	//Status Line
	response.append("HTTP/")
	.append(res.http_version).append(" ")
	.append(res.status_code).append(" ")
	.append(res.status_text).append("\n")
	//Headers
	.append("Content-Type: ").append(res.content_type).append("\n")
	.append("Content-Length: ").append(res.content_length).append("\n")
	.append("Date: ").append(res.date).append("\n")
	.append("Server: ").append(res.server).append("\n")
	.append("Cache-Control: ").append(res.cache_control).append("\n")
	.append("Set-Cookie: ").append(res.set_cookie).append("\n")
	.append("Location: ").append(res.location).append("\n")
	.append("Connection: ").append(res.connection).append("\n")
	//Reponse Body (HTML, JSON, XML, text, binary data, or any other format.)
	.append("\n").append(res.response_body);
	return response;
}