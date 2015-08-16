#!/bin/bash
home=$(echo ~)
mydir=$home/jsbak/
for js in `find ~ -name "*.js"`;do  
    if [ ! -d $mydir ];then
        mkdir $mydir
    fi
    echo "cpone"
    echo $js
    cp -v $js $mydir
    echo $mydir
done

for jsfile in `find $mydir -name "*.js"`;do
    mv $jsfile `echo $jsfile | sed 's/\.js/\.js\.bak/'`
done

