#!/bin/bash

GOOS=windows GOARCH=amd64 go build -o gmm.exe main.go
GOOS=linux GOARCH=amd64 go build -o gmm main.go
GOOS=linux GOARCH=arm64 go build -o gmm-arm64 main.go
