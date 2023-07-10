#!/bin/bash
ps aux | grep "main\.solver" | grep -v grep | awk '{ print $2 }' | xargs kill -9
