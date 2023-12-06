#include "Response.hpp"

Response::Response(response_object &res): res(res) {}

void Response::sendResponse(int clientFd) {
	
	std::string responseString = toString();
	const char* buffer = responseString.c_str();
	int size = responseString.size();

	int totalSent = 0;
	while (totalSent < size) {
		int bytesSent = send(clientFd, buffer + totalSent, size - totalSent, 0);
		if (bytesSent < 0) {
			std::cerr << "Waiting to send Chunk" << std::endl;
		}
		sleep(1); //TODO that blocks the sending, we should try to add this guy to a
		//being sended list, storing the fd, responseString and bytes send
		totalSent += bytesSent;
	}
}

void Response::notFoundResponse(int fd, std::string serverName, std::string content) {
	response_object resp;

	resp.status_code = "302";
	resp.status_text = "Moved Temporarily";
	resp.date = __DATE__;
	resp.server = serverName;
	resp.content_type = "text/html";
	resp.location = content;

	Response httpRes(resp);
	httpRes.sendResponse(fd);
}

void Response::maxBodySizeResponse(int fd, std::string serverName, size_t size) {
	response_object resp;

	resp.status_code = "413";
	resp.status_text = "Request Entity Too Large: " + size;
	resp.date = __DATE__;
	resp.server = serverName;
	resp.content_type = "text/plain";

	Response httpRes(resp);
	httpRes.sendResponse(fd);
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
	.append((!res.filename.empty()) ? std::string("Content-Disposition: attachment; filename=").append(res.filename).append("\n") : "")
	//Reponse Body (HTML, JSON, XML, text, binary data, or any other format.)
	.append("\n")
	.append((!res.response_body.empty()) ? res.response_body.append("\n") : "");
	return response;
}