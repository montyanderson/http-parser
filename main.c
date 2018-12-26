#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct http_header {
	char *key;
	char *value;
};

struct http_request {
	char *method;
	char *path;
	char *protocol;
	size_t headers_length;
	struct http_header *headers;
	char *body;
};

struct http_request *http_request_parse(char *buffer) {
	// check if ready to parse

	struct http_request *request = calloc(1, sizeof(struct http_request));

	if(request == NULL)
		return NULL;

	// method

	char *method_start = buffer;
	char *method_end = strchr(method_start, ' ');
	size_t method_length = method_end - method_start;

	request->method = malloc(method_length + 1);
	strncpy(request->method, method_start, method_length);
	request->method[method_length] = 0;

	// path

	char *path_start = method_end + 1;
	char *path_end = strchr(path_start, ' ');
	size_t path_length = path_end - path_start;

	request->path = malloc(path_length + 1);
	strncpy(request->path, path_start, path_length);
	request->path[path_length] = 0;

	// protocol

	char *protocol_start = path_end + 1;
	char *protocol_end = strchr(protocol_start, '\n');
	size_t protocol_length = protocol_end - protocol_start;

	request->protocol = malloc(protocol_length + 1);
	strncpy(request->protocol, protocol_start, protocol_length);
	request->protocol[protocol_length] = 0;

	// headers

	char *headers_start = protocol_end + 1;
	ssize_t headers_length = 0;

	for(char *c = headers_start; *c != 0; c++) {
		if(*c == '\n') {
			headers_length++;
		}
	}

	request->headers_length = --headers_length;

	request->headers = calloc(headers_length, sizeof(struct http_header));

	char *key;
	char *key_start;
	char *key_end;
	size_t key_length;

	char *value;
	char *value_start;
	char *value_end = protocol_end;
	size_t value_length;

	for(size_t i = 0; i < headers_length; i++) {
		key_start = value_end + 1;
		key_end = strchr(key_start, ':');
		key_length = key_end - key_start;

		key = malloc(key_length + 1);
		strncpy(key, key_start, key_length);
		key[key_length] = 0;

		request->headers[i].key = key;

		value_start = key_end + 1;

		if(*value_start == ' ')
			value_start++;

		value_end = strchr(value_start, '\n');
		value_length = value_end - value_start;

		value = malloc(value_length + 1);
		strncpy(value, value_start, value_length);
		value[value_length] = 0;

		request->headers[i].value = value;
	}

	char *body_start = strchr(value_end + 1, '\n') + 1;
	size_t body_length = strlen(body_start);

	request->body = malloc(body_length + 1);
	strncpy(request->body, body_start, body_length);

	return request;
}

int main() {
	char *buffer = "POST /cgi-bin/process.cgi HTTP/1.1\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\nHost: www.tutorialspoint.com\nContent-Type: application/x-www-form-urlencoded\nContent-Length: length\nAccept-Language: en-us\nAccept-Encoding: gzip, deflate\nConnection: Keep-Alive\n \nlicenseID=string&content=string&/paramsXML=string";

	struct http_request *req = http_request_parse(buffer);

	printf("request->method '%s'\n", req->method);
	printf("request->path '%s'\n", req->path);
	printf("request->protcol '%s'\n", req->protocol);

	printf("request->headers_length '%lu'\n", req->headers_length);

	for(size_t i = 0; i < req->headers_length; i++) {
		printf("request->header[%lu] '%s' '%s'\n", i, req->headers[i].key, req->headers[i].value);
	}

	printf("request->body '%s'\n", req->body);
}
