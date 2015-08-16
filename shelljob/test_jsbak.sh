#!/bin/bash
result="/tmp/jsbak.tmp"
find ~ -name "*.js">$result
if [ -e $result -a ! -s $result ];then
    echo "empty"
fi

#题目究竟是说用户根目录还是根目录，真是日了狗了
#如果是用户根目录
mkdir ~/jsbak
find ~ -name "*.js" -print0 | xargs -i -0 cp {} ~/jsbak
for js in `find ~/jsbak -name "*.js"`;do  
    mv $js `echo $js | sed 's/\.js/\.js\.bak/'`
done
