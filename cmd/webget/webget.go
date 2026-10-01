package main

import (
	"fmt"
	"io"
	"net"
	"os"
	"time"
)

func getURL(host, path string) (string, error) {
	conn, err := net.DialTimeout("tcp", net.JoinHostPort(host, "80"), 10*time.Second)
	if err != nil {
		return "", fmt.Errorf("connect to %s: %w", host, err)
	}
	defer conn.Close()

	request := fmt.Sprintf(
		"GET %s HTTP/1.1\r\n"+
			"Host: %s\r\n"+
			"Connection: close\r\n"+
			"\r\n",
		path,
		host,
	)

	if _, err := io.WriteString(conn, request); err != nil {
		return "", fmt.Errorf("send HTTP request: %w", err)
	}

	response, err := io.ReadAll(conn)
	if err != nil {
		return "", fmt.Errorf("read HTTP response: %w", err)
	}

	return string(response), nil
}

func usage(program string) {
	fmt.Fprintf(os.Stderr, "Usage: %s HOST PATH\n", program)
	fmt.Fprintf(os.Stderr, "\tExample: %s stanford.edu /class/cs144\n", program)
}

func main() {
	if len(os.Args) != 3 {
		usage(os.Args[0])
		os.Exit(1)
	}

	response, err := getURL(os.Args[1], os.Args[2])
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	fmt.Print(response)
}
