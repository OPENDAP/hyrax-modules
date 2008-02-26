

	$Id:README 15847 2007-02-14 18:16:29Z jimg $

This software is used by two projects: The original OPeNDAP Data Server, whichused CGI 1.1 in conjunction with a web server such as Apache, and the new data server (Hyrax) which uses Java Servlets in combination with the OPeNDAPBack-End Server (BES). This package contains general purpose handlers foruse with either of the two servers. These are the Usage, ASCII and HTML formhandlers. Each takes input from a 'data handler' and returns a HTML or plaintext response --- something other than a DAP response object.

This software contains the 'dispatch' software for the original OPeNDAPdata server, comprised of several Perl modules and the Perl program 'nph-dods'.All of this software is included when you get the source code distribution.However, when you get the RPM packages or OS/X disk images (.dmg files), the CGI server software is separated in a package named 'dap-server-cgi'. The binary programs and libraries for both servers are in the 'dap-server'. In afuture version we will move the binaries used by the cgi-based server to thecgi package. For more information on this cgi-server installation, pleaserefer to README.cgi-server.

For Hyrax this package includes the library modules that can be dynamicallyloaded into the OPeNDAP Back-End Server (BES) as well as help files for themodules. To load these modules into the BES simply edit the BESconfiguration file, its default location in /etc/bes/bes.conf. Directions forediting the configuration file follow, however, you can run the bes-dap-data.sh script to edit the script automatically and install sample data files.

To edit the bes.conf script by hand:

Set the BES user, group and admin email (see the comments in the bes.conffile for instructions).

Change the parameter BES.modules to include the three dap-server modulesascii, usage and www as follows (example assumes you have installed thenetcdf_handler module):

BES.modules=dap,cmds,ascii,usage,www,nc


And add the following three lines below this:

BES.module.ascii=/usr/lib/bes/libascii_module.so
BES.module.usage=/usr/lib/bes/libusage_module.so
BES.module.www=/usr/lib/bes/libwww_module.so


Also add the following near where the bes_help files are specified.

DAP-SERVER.Help.TXT=/usr/share/bes/dap-server_help.txt
DAP-SERVER.Help.HTML=/usr/share/bes/dap-server_help.html
DAP-SERVER.Help.XML=/usr/share/bes/dap-server_help.html


The next time the BES is started these modules will be loaded and the ascii,info_page, and html_form responses will be supported as well as the helpfiles for dap-server specifying the syntax of these commands.

