#!/bin/bash
if [ ! -d "./$1_Hk_Result" ];then
	mkdir "$1_Hk_Result"
fi
root -l -q "Hk.C(\""$1"\")"
