# Source this file.
#
# Test the nph-nc script. This should work with the other Perl dispatch
# scrips as well.
# 
# The URL http://dcz.cvo.oneworld.com/cgi-bin/test-cgi.tcl/place/file.ext?barf
#
# SERVER_SOFTWARE
#      NCSA/1.5.2 
# SERVER_NAME
#      dcz.cvo.oneworld.com 
# GATEWAY_INTERFACE
#      CGI/1.1 
# SERVER_PROTOCOL
#      HTTP/1.0 
# SERVER_PORT
#      80 
# REQUEST_METHOD
#      GET 
# PATH_INFO
#      /place/file.ext 
# PATH_TRANSLATED
#      /usr/local/spool/http/place/file.ext 
# SCRIPT_NAME
#      /cgi-bin/test-cgi.tcl 
# QUERY_STRING
#      barf 
# REMOTE_HOST
#      dcz.cvo.oneworld.com 
# REMOTE_ADDR
#      143.227.0.193 
# HTTP_ACCEPT
#      image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, */* 
# HTTP_USER_AGENT
#      Mozilla/3.01 (X11; I; SunOS 4.1.3 sun4c) 

#PATH_INFO="/version"

PATH_INFO="/pathfinder.dds"
export PATH_INFO


SCRIPT_NAME="/etc/nph-jg"
export SCRIPT_NAME

QUERY_STRING=
export QUERY_STRING

#PATH_TRANSLATED="/usr/local/spool/http/version"
PATH_TRANSLATED="/usr/local/spool/http/pathfinder.dds"
export PATH_TRANSLATED

