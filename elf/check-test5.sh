#! /bin/bash
./elfloader test5-nonexistent 22 30
if [ $? -ne 2 ]
then
    echo "Wrong error code was returned"
    exit 1
else
    echo "==> Correct error code was returned"
    exit 0
fi
