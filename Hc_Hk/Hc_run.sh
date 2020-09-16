#!/bin/bash
if [ ! -d "./$1_Hc_Result" ];then
	mkdir "$1_Hc_Result"
fi

if [ "$2" == "2" ]; then
	root -l -q "Hc.C(\""$1"\", $2)"
else
	root -l -q "Hc.C(\""$1"\")"
fi
