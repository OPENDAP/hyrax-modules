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
<<<<<<< test-dispatch.sh
PATH_INFO="/cat_avhrr.dds"
=======
PATH_INFO="/data/penny.mat.das"
>>>>>>> 1.2
export PATH_INFO

<<<<<<< test-dispatch.sh
SCRIPT_NAME="/etc/nph-jg"
=======
SCRIPT_NAME="/etc/nph-mat"
>>>>>>> 1.2
export SCRIPT_NAME

QUERY_STRING=
export QUERY_STRING

#PATH_TRANSLATED="/usr/local/spool/http/version"
<<<<<<< test-dispatch.sh
PATH_TRANSLATED="/usr/local/spool/http/cat_avhrr.dds"
=======
PATH_TRANSLATED="/usr/local/spool/http/data/penny.mat.das"
>>>>>>> 1.2
export PATH_TRANSLATED

