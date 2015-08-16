#!/bin/bash
#统计行数
conf_path="."
find ${conf_path} -name "*.conf" | xargs cat | grep ^$ | wc -l
#替换空行为###
find ${conf_path} -name "*.conf" | xargs sed -i 's/^$/###/g' 
