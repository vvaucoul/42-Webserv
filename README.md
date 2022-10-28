# Webserv


### Project

The objective of the project was to recreate a complete HTTP server according to RFC standards with a nginx model.<br>
We therefore had to:

- Manage multiple client requests
- Parse these requests
- Send an adequate response

## Usage

- Clone Repository
```bash
git clone https://github.com/vvaucoul/Webserv && cd Webserv
```

- Compilation
```bash
make
```

- Start Web Server
```bash
./webserv [Configuration File]
```

Default Port: **8081**<br>
Default ConfigFile:<br>

```bash
server {
	host 127.0.0.1;
	port 8081;
	server_name webserv;

	root ./webSite;

	autoindex on;
	allow_methods GET;
}
```

> Use **Config_Simple.wscfg** to get full website

```bash
./webserv tests/config_simple.wscfg
```

Then, go to your webbrowser and enter this IP: **127.0.0.1:8081**

## Content

- HTTP Methods (Get, Post, Delete)
- Multiple HTTP code.
- Exceptions and errors handled.
- Nginx Locations.
- Python and PHP CGI.
- Upload large files.
- Upload, texts, images, movies and other things...
- Send php forms.

## Server Configuration Files

Requirements fields:
  - host
  - port
  - server_name
  - root
  - allow_methods (at least, GET)
  - autoindex
 
Default: [**config_default.wscfg**](https://github.com/vvaucoul/Webserv/blob/main/tests/config_default.wscfg)<br>
Simple-Linux: [**config_simple_linux.wscfg**](https://github.com/vvaucoul/Webserv/blob/main/tests/config_simple_linux.wscfg)<br>
Locations: [**config_locations.wscfg**](https://github.com/vvaucoul/Webserv/blob/main/tests/config_locations.wscfg)<br>

## Example

![Animation](https://user-images.githubusercontent.com/66129673/198697847-f7959b88-5603-4799-9ff4-c83d71112697.gif)

## Ressources

- [HTTP MDN](https://developer.mozilla.org/fr/docs/Web/HTTP)
- [Multiplexing](https://fr.wikipedia.org/wiki/Multiplexage#:~:text=Le%20multiplexage%20est%20une%20technique,temporelle)
- [NGINX Documentation](https://nginx.org/en/docs/)
