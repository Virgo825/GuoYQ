#!/bin/bash
if [ ! -d "./$1_Hc_Result" ];then
	mkdir "$1_Hc_Result"
fi
root -l -q "Hc.C(\""$1"\")"
