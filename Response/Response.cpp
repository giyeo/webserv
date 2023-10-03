#include "Response.hpp"

Response::Response(response_object &res): res(res) {}

void Response::sendResponse(int clientFd) {
	const char *buffer = toString().c_str();
	int size = toString().size();

	int totalSent = 0;
	while(totalSent < size) {
		int bytesSent = send(clientFd, buffer + totalSent, size - totalSent, 0);
		if (bytesSent < 0) {
			std::cerr << "Error sending response to client" << std::endl;
			break;
		}
		totalSent += bytesSent;
	}
	close(clientFd);
}

std::string Response::toString() {
	std::string response = "";
	//Status Line
	response.append("HTTP/")
	.append(res.http_version).append(" ")
	.append(res.status_code).append(" ")
	.append(res.status_text).append("\n")
	//Headers
	.append((!res.content_type.empty()) ? std::string("Content-Type: ").append(res.content_type).append("\n") : "")
	.append((!res.content_length.empty()) ? std::string("Content-Length: ").append(res.content_length).append("\n") : "")
	.append((!res.date.empty()) ? std::string("Date: ").append(res.date).append("\n") : "")
	.append((!res.server.empty()) ? std::string("Server: ").append(res.server).append("\n") : "")
	.append((!res.cache_control.empty()) ? std::string("Cache-Control: ").append(res.cache_control).append("\n") : "")
	.append((!res.set_cookie.empty()) ? std::string("Set-Cookie: ").append(res.set_cookie).append("\n") : "")
	.append((!res.location.empty()) ? std::string("Location: ").append(res.location).append("\n") : "")
	.append((!res.connection.empty()) ? std::string("Connection: ").append(res.connection).append("\n") : "")
	//Reponse Body (HTML, JSON, XML, text, binary data, or any other format.)
	.append("\n")
	.append((!res.response_body.empty()) ? res.response_body.append("\n") : "");
	return response;
}