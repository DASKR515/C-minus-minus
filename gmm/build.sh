#!/bin/bash

GOOS=windows GOARCH=amd64 go build -o gmm.exe
GOOS=linux GOARCH=amd64 go build -o gmm
GOOS=linux GOARCH=arm64 go build -o gmm-arm64
