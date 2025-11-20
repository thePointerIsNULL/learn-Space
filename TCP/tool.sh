#!/bin/bash

./build/tool "$1" $(cat params | xargs)

