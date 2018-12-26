#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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

void http_request_free(struct http_request *request) {
	free(request->method);
	free(request->path);
	free(request->protocol);

	for(size_t i = 0; i < request->headers_length; i++) {
		free(request->headers[i].key);
		free(request->headers[i].value);
	}

	free(request->headers);
	free(request->body);
}

int http_request_parse(struct http_request *request, char *buffer) {
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

	size_t content_length = 0;

	for(size_t i = 0; i < headers_length; i++) {
		key_start = value_end + 1;
		key_end = strchr(key_start, ':');
		key_length = key_end - key_start;

		key = malloc(key_length + 1);
		strncpy(key, key_start, key_length);
		key[key_length] = 0;

		for(size_t j = 0; j < key_length; j++) {
			key[j] = tolower(key[j]);
		}

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

		if(content_length == 0 && strcmp(key, "content-length") == 0) {
			sscanf(value, "%lu", &content_length);
		}
	}

	if(content_length) {
		char *body_start = strchr(value_end + 1, '\n') + 1;

		if(strlen(body_start) < content_length) {
			printf("length less than content length\n");
		}

		request->body = malloc(content_length + 1);
		strncpy(request->body, body_start, content_length);
		request->body[content_length] = 0;
	}

	return 0;
}

int main() {
	char *buffer = "POST /bin/login HTTP/1.1\nHost: 127.0.0.1:8000\nAccept: image/gif, image/jpeg, */*\nReferer: http://127.0.0.1:8000/login.html\nAccept-Language: en-us\nContent-Type: application/x-www-form-urlencoded\nAccept-Encoding: gzip, deflate\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\nContent-Length: 37\nConnection: Keep-Alive\nCache-Control: no-cache\n\nUser=Peter+Lee&pw=123456&action=login";

	struct http_request req;
	memset(&req, 0, sizeof(req));

	http_request_parse(&req, buffer);

	printf("request->method '%s'\n", req.method);
	printf("request->path '%s'\n", req.path);
	printf("request->protcol '%s'\n", req.protocol);

	printf("request->headers_length '%lu'\n", req.headers_length);

	for(size_t i = 0; i < req.headers_length; i++) {
		printf("request->header[%lu] '%s' '%s'\n", i, req.headers[i].key, req.headers[i].value);
	}

	printf("request->body '%s'\n", req.body);
}
