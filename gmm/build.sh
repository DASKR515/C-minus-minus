#!/bin/bash

GOOS=windows GOARCH=amd64 go build main.go -o gmm.exe
GOOS=linux GOARCH=amd64 go build main.go -o gmm
GOOS=linux GOARCH=arm64 go build main.go -o gmm-arm64
