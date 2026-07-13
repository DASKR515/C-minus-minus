#!/bin/bash

GOOS=windows GOARCH=amd64 go build -o gmm.exe mainAll.go
GOOS=linux GOARCH=amd64 go build -o gmm mainAll.go
GOOS=linux GOARCH=arm64 go build -o gmm-arm64 mainAll.go