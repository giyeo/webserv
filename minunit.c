#include "minunit.h"
#include "./Parser/Utils.hpp"
#include "./Parser/Server.hpp"
#include "./Parser/Location.hpp"

MU_TEST(test_tokenizer_function) {
	std::vector<std::string> expected_result;
	expected_result.push_back("example.com");
	expected_result.push_back("127.0.0.1");
	std::vector<std::string> result = tokenizer("example.com 127.0.0.1", ' ');

	mu_check(expected_result == result);
}

MU_TEST(test_parse_server_name_with_two_correct_values) {
	std::vector<std::string> expected_result;
	expected_result.push_back("example.com");
	expected_result.push_back("www.example.com");

	Server server;
	server.parseServerName("example.com www.example.com");

	mu_check(expected_result == server.serverName);
}

MU_TEST(test_parse_listen_with_correct_port_value) {
	std::string expected_result = "80";

	Server server;
	server.parseListen("80");

	mu_check(expected_result == server.listen);
}

MU_TEST(test_parse_listen_with_correct_ip_and_port_value) {
	std::string expected_result = "127.0.0.1:80";

	Server server;
	server.parseListen("127.0.0.1:80");

	mu_check(expected_result == server.listen);
}

MU_TEST(test_parse_listen_with_incorrect_ip_and_port_value) {
	std::string expected_result = "Invalid listen value";
	std::string result;
	Server server;
	try {
		server.parseListen("127.0.0.1:80 8080");
	} catch (std::invalid_argument& e) {
		result = e.what();
	}
	mu_check(expected_result == result);
}

MU_TEST(test_parse_root_with_correct_value) {
	std::string expected_result = "/var/www/html";

	Server server;
	server.parseRoot("/var/www/html");

	mu_check(expected_result == server.root);
}

MU_TEST(test_parse_root_with_inexistent_path) {
	std::string expected_result = "Invalid number of arguments in a \"root\" directive";
	std::string result;
	Server server;
	try {
		server.parseRoot("/var/www/html /var/www/invalid");
	} catch (std::invalid_argument& e) {
		result = e.what();
	}
	mu_check(expected_result == result);	
}

MU_TEST(test_parse_index_with_correct_value) {
	std::string expected_result = "/var/www/html/index.html";

	Server server;
	server.parseIndex("/var/www/html/index.html");

	mu_check(expected_result == server.index);
}

MU_TEST(test_parse_error_page_with_correct_value) {
	std::string expected_result = "/var/www/html/error.html";

	Server server;
	server.parseErrorPage("404 /var/www/html/error.html");

	mu_check(expected_result == server.errorPage);
}

MU_TEST(test_client_max_body_size_with_correct_value) {
	std::string expected_result = "100";
	Server server;
	try {
		server.parseClientMaxBodySize("100");
	} catch (std::invalid_argument& e) {}

	mu_check(expected_result == server.clientMaxBodySize);
}

MU_TEST(test_client_max_body_size_with_incorrect_value) {
	std::string expected_result = "Invalid client_max_body_size value";
	std::string result;
	Server server;
	try {
		server.parseClientMaxBodySize("100kb");
	} catch (std::invalid_argument& e) {
		result = e.what();
	}

	mu_check(expected_result == result);
}

MU_TEST(test_client_max_body_size_with_incorrect_value_2) {
	std::string expected_result = "Invalid client_max_body_size value";
	std::string result;
	Server server;
	try {
		server.parseClientMaxBodySize("42m");
	} catch (std::invalid_argument& e) {
		result = e.what();
	}

	mu_check(expected_result == result);
}

MU_TEST(test_location_with_correct_value) {

	Server server;
	server.parseLocation("/images" , "root /var/www/html/images \n index index.html \n proxy_pass http://backend \n error_page 404 /var/www/html/error.html");


	mu_check(server.locations[0].path == "/images");
	mu_check(server.locations[0].root == "/var/www/html/images");
	mu_check(server.locations[0].index == "index.html");
	mu_check(server.locations[0].proxyPass == "http://backend");
	mu_check(server.locations[0].errorPage == "404 /var/www/html/error.html");
}

MU_TEST_SUITE(parse_config_file) {
	MU_RUN_TEST(test_tokenizer_function);
	MU_RUN_TEST(test_parse_server_name_with_two_correct_values);
	MU_RUN_TEST(test_parse_listen_with_correct_port_value);
	MU_RUN_TEST(test_parse_listen_with_correct_ip_and_port_value);
	MU_RUN_TEST(test_parse_listen_with_incorrect_ip_and_port_value);
	MU_RUN_TEST(test_parse_root_with_correct_value);
	MU_RUN_TEST(test_parse_root_with_inexistent_path);
	MU_RUN_TEST(test_parse_index_with_correct_value);
	MU_RUN_TEST(test_parse_error_page_with_correct_value);
	MU_RUN_TEST(test_client_max_body_size_with_correct_value);
	MU_RUN_TEST(test_client_max_body_size_with_incorrect_value);
	MU_RUN_TEST(test_client_max_body_size_with_incorrect_value_2);
	MU_RUN_TEST(test_location_with_correct_value);
}

int main(int argc, char *argv[]) {
	(void)argv;
	(void)argc;
	MU_RUN_SUITE(parse_config_file);
	MU_REPORT();
	return MU_EXIT_CODE;
}

