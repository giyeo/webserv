# TODO
PARSING CONF FILE
- key values to some class
Open a socket based on the CONF FILE
Handle Req and Res accordingly to RFC.

# TODO TONIGHT
- REQ, RES, CONF Struct
# TODO NEXT
- Deal with GET and Path (how to deal with resources)
- Stream data mechanism to deliver data to the client (Text, documents, video, etc).

# webserv execution order
Configuration -> Communication ->
start -> Request -> Resource Processing ->
Response -> Repeat start;

# how fcntl is usefull

Client Request Header Example:
GET / HTTP/1.1
Host: localhost:8080
Connection: keep-alive
Cache-Control: max-age=0
sec-ch-ua: "Microsoft Edge";v="117", "Not;A=Brand";v="8", "Chromium";v="117"
sec-ch-ua-mobile: ?0
sec-ch-ua-platform: "Windows"
DNT: 1
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36 Edg/117.0.2045.31
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br
Accept-Language: pt-BR,pt;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6

parsing conf files, there are 2 options, ints and string.
each configuration will be one of those, in case of int, the parse to int should be a success.
there will be an array "configuration that are integers" {"port", "nqueue", ...}, those should
parse the conf file with success.