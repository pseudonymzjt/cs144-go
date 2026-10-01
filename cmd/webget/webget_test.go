package main

import (
	"bufio"
	"io"
	"strings"
	"testing"
)

func TestWebGetHasher(t *testing.T) {
	const (
		host         = "cs144.keithw.org"
		path         = "/nph-hasher/xyzzy"
		expectedHash = "7SmXqWkrLKzVBCEalbSPqBcvs11Pw263K7x4Wv3JckI"
	)

	response, err := getURL(host, path)
	if err != nil {
		t.Fatalf("webget failed: %v", err)
	}

	t.Log(response)

	lastLine := lastNonEmptyLine(response)
	if lastLine != expectedHash {
		t.Fatalf("webget returned %q; want %q", lastLine, expectedHash)
	}
}

func lastNonEmptyLine(s string) string {
	scanner := bufio.NewScanner(strings.NewReader(s))
	last := ""

	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line != "" {
			last = line
		}
	}

	return last
}

var _ = io.EOF
