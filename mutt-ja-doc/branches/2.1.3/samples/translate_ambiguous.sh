#! /bin/sh
# generate east asian ambiguous character width
#
awk 'BEGIN { FLAG =0 } /^% Character/    { FLAG=1 } /^.*/ { if(FLAG == 1) print }' |sed -e  "s/>       1/>       2/" <UTF-8.cent62.patch | sed -e "s/>                 1/>                 2/"

