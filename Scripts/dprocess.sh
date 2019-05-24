#!/bin/sh
# debug output processor

filename=$(basename "$1");
filename="${filename%.*}"

sed -i '' -e 's/parser.c:17:execute_asm(): //g' $1

# status
egrep -o 'Y:[A-Fa-f0-9]{2} P:[A-Za-z0-9]{2} ' $1 | egrep -o 'P:[A-Za-z0-9]{2} ' >"${filename}_status.log"

# sp
egrep -o 'SP:[A-Fa-f0-9]{2} ' $1 >"${filename}_sp.log"

# addr
egrep -o '^[A-Fa-f0-9]{4} ' $1 >"${filename}_addr.log"
