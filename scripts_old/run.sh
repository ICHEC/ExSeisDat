#!/bin/bash
#This script runs the test jobs#
bash gentasks.sh
export NODE_LIST="1 23 24"
bash genpbs.sh $1
