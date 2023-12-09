#include "Response.hpp"


std::string errorHtml(std::string error_code, std::string error_text) {
    std::ostringstream errorPage;

    // Append the HTML template with dynamic values
    errorPage << "<!DOCTYPE html>\n"
              << "<html lang=\"en\">\n"
              << "<head>\n"
              << "    <meta charset=\"UTF-8\">\n"
              << "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n"
              << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
              << "    <title>Error Page</title>\n"
              << "    <style>\n"
              << "        body {\n"
              << "            font-family: 'Arial', sans-serif;\n"
              << "            background-color: #f4f4f4;\n"
              << "            color: #333;\n"
              << "            text-align: center;\n"
              << "            margin: 0;\n"
              << "            padding: 0;\n"
              << "            display: flex;\n"
              << "            align-items: center;\n"
              << "            justify-content: center;\n"
              << "            height: 100vh;\n"
              << "        }\n"
              << "\n"
              << "        .error-container {\n"
              << "            background-color: #fff;\n"
              << "            border-radius: 8px;\n"
              << "            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\n"
              << "            padding: 20px;\n"
              << "            max-width: 400px;\n"
              << "            width: 100%;\n"
              << "        }\n"
              << "\n"
              << "        h1 {\n"
              << "            color: #e44d26;\n"
              << "        }\n"
              << "\n"
              << "        p {\n"
              << "            margin-top: 10px;\n"
              << "            font-size: 18px;\n"
              << "        }\n"
              << "    </style>\n"
              << "</head>\n"
              << "<body>\n"
              << "    <div class=\"error-container\">\n"
              << "        <h1>Error " << error_code << "</h1>\n"
              << "        <p>" << error_text << "</p>\n"
              << "    </div>\n"
              << "</body>\n"
              << "</html>\n";

    // Get the concatenated HTML string
    return errorPage.str();
}


void sendToClientOrService(Config &config, std::string errorString) {
	int clientFd = config.clientFd;
	t_event &event = config.events[clientFd];
	
	std::string responseString = (errorString.empty()) ? event.buffer : errorString;
	const char* buffer = responseString.c_str();

	int bytesSent = 0;
	while (event.totalSent < event.bytes) {
		ssize_t maxChunkSize = std::min((ssize_t)16384, event.bytes - event.totalSent);
		if(event.type == SERVICE)
			bytesSent = fwrite(buffer + event.totalSent, 1, maxChunkSize, config.events[clientFd].fp);
		else
			bytesSent = send(clientFd, buffer + event.totalSent, maxChunkSize, 0);
		if (bytesSent < 0) {
			log(__FILE__, __LINE__, "Waiting to write", LOG);
			break;
		}
		event.totalSent += bytesSent;
		log(__FILE__, __LINE__, concat(4,"bytesSent: ", intToString(event.totalSent).c_str(), "/", intToString(event.bytes).c_str()), LOG);
		if (event.totalSent == event.bytes) {
			if(event.type == SERVICE) {
				log(__FILE__, __LINE__, "Success on Sending to the service, Erasing event, removing from epoll", LOG);
				pclose(config.events[clientFd].fp);
			}
			if(event.type != SERVICE) {
				log(__FILE__, __LINE__, "Success on Sending to the client, Closing FD, Erasing event, removing from epoll", LOG);
				close(clientFd);
			}
			epoll_ctl(config.epollFd, EPOLL_CTL_DEL, clientFd, NULL);
			config.events.erase(clientFd);
			break;
		}
		if (event.totalSent >= 16000) {
			log(__FILE__, __LINE__, "Exiting the function, re-enter epoll to keep non-blocking", LOG);
			break;
		}
	}
	std::cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n\n\n";
}

void notFoundResponse(Config &config, std::string content) {
	response_object resp;

	resp.status_code = "302";
	resp.status_text = "Moved Temporarily";
	resp.date = __DATE__;
	resp.server = config.serverName;
	resp.location = content;

	std::string errorResponseString = resToString(resp);
	config.events[config.clientFd].buffer = errorResponseString;
	config.events[config.clientFd].bytes = errorResponseString.size();

	struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.fd = config.clientFd;
	epoll_ctl(config.epollFd, EPOLL_CTL_MOD, config.clientFd, &ev);
}

void redirectPage(Config &config, std::string code, std::string path) {
	response_object resp;

	resp.status_code = code;
	resp.status_text = "Returni";
	resp.date = __DATE__;
	resp.server = config.serverName;
	resp.location = "http://" + path;

	std::string errorResponseString = resToString(resp);
	config.events[config.clientFd].buffer = errorResponseString;
	config.events[config.clientFd].bytes = errorResponseString.size();

	struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.fd = config.clientFd;
	epoll_ctl(config.epollFd, EPOLL_CTL_MOD, config.clientFd, &ev);
} 

void errorPage(Config &config, std::string code, std::string text) {
	response_object resp;

	resp.status_code = code;
	resp.status_text = text;
	resp.date = __DATE__;
	resp.server = config.serverName;
	resp.content_type = "text/html";
	resp.response_body = errorHtml(code, text);

	log(__FILE__, __LINE__, concat(3, code.c_str(), ": ", text.c_str()), FAILED);
	std::string errorResponseString = resToString(resp);
	config.events[config.clientFd].buffer = errorResponseString;
	config.events[config.clientFd].bytes = errorResponseString.size();
	struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.fd = config.clientFd;
	epoll_ctl(config.epollFd, EPOLL_CTL_MOD, config.clientFd, &ev);
}

void directoryListing(Config &config, std::string content) {
	response_object resp;

	resp.status_code = 200;
	resp.status_text = "OK";
	resp.date = __DATE__;
	resp.server = config.serverName;
	resp.content_type = "text/plain";
	resp.response_body = content;

	log(__FILE__, __LINE__, "Sending directory Listing", FAILED);
	std::string errorResponseString = resToString(resp);
	config.events[config.clientFd].buffer = errorResponseString;
	config.events[config.clientFd].bytes = errorResponseString.size();
	struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.fd = config.clientFd;
	epoll_ctl(config.epollFd, EPOLL_CTL_MOD, config.clientFd, &ev);
}

std::string resToString(response_object &res) {
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
	.append(std::string("Server: ").append("ft_webserver/0.1").append("\n"))
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