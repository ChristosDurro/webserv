# Webserv

Webserv is a web server implemented in C++98 that handles HTTP GET, POST, and DELETE requests, executes CGIs supporting GET and POST requests, and serves static and error pages. The server is configured using a configuration file similar to the nginx format.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [HTTP Requests Supported](#http-requests-supported)
- [Error Pages](#error-pages)

## Introduction

Webserv is designed to be a lightweight and efficient web server that can handle multiple types of HTTP requests. It uses a configuration file to define server settings, including ports, server locations, and error pages.

## Features

- **HTTP Methods**: Supports GET, POST, and DELETE requests.
- **CGI Execution**: Executes CGI scripts that handle GET and POST requests.
- **Configuration File**: Uses a configuration file similar to nginx format for server setup.
- **Static Page Serving**: Serves static pages based on server locations defined in the configuration.
- **Custom Error Pages**: Supports custom error pages for various HTTP error codes.

## Installation

To compile the Webserv program, follow these steps:

1. Clone this repository and navigate to the project directory.
   ```bash
   git clone https://github.com/ChristosDurro/webserv.git
   cd webserv
   ```

2. Compile the program using the provided Makefile:

   ```bash
   make
   ```

## Usage

To run the Webserv program, provide the configuration file as an argument:

   ```bash
   ./webserv path/to/configuration/file
  ```

## HTTP Requests Supported

- GET: Retrieves static files or runs CGI scripts and returns the output.
- POST: Sends data to the server, typically used with forms or CGI scripts.
- DELETE: Removes the specified resource from the server.

## Error Pages

Webserv supports custom error pages for the following HTTP error codes:

- 404: Not Found
- 405: Method Not Allowed
- 408: Request Timeout
- 413: Payload Too Large
- 500: Internal Server Error
- 505: HTTP Version Not Supported
