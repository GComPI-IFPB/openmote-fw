#!/bin/bash

PROJECTS="projects"
SRC="src"
MAKE_COMMAND="make TARGET=cc2538"
CLEAN_COMMAND="make TARGET=cc2538 clean"

cd $PROJECTS
for PROJECT in *; do
    if [[ -d $PROJECT ]]; then
        cd $PROJECT/$SRC
        RESULT="Building $PROJECT..."
        OUTPUT=$(eval $MAKE_COMMAND 2>&1)
        if [[ $OUTPUT == *rror* ]]; then
            RESULT="$RESULT error!"
            echo $RESULT
            exit 1
        else
            RESULT="$RESULT ok!"
            echo $RESULT
        fi
        cd ../..
    fi
done

exit 0
