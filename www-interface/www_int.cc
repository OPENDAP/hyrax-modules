
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

/** 
    @author: jhrg */

// $Log: www_int.cc,v $
// Revision 1.2  1999/05/09 04:14:52  jimg
// String --> string
//
// Revision 1.1  1999/04/20 00:21:06  jimg
// First version
//

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: www_int.cc,v 1.2 1999/05/09 04:14:52 jimg Exp $"};

#include <stdio.h>
#include <assert.h>

#include <fstream>
#include <strstream>		// Should be sstream! 4/8/99 jhrg
#include <string>

#include <GetOpt.h>
#include <Pix.h>
#include <SLList.h>
#include <string>

#include "BaseType.h"
#include "Connect.h"
#include "WWWOutput.h"

#include "cgi_util.h"

#include "javascript.h"		// Try to hide this stuff...

const char *VERSION = WWW_INT_VER;
DAS global_das;
WWWOutput wo(cout);

static void
usage(string name)
{
    cerr << "Usage: " << name << endl
	 << "m: Print a MIME header. Use this with nph- style CGIs.\n"
	 << "v: Verbose output. Currently a null option.\n"
	 << "V: Print version information and exit.\n"
	 << "h|?: This meassage.\n";
}

/** Look for the override file by taking the dataset name and appending
    `.ovr' to it. If such a file exists, then read it in and store the
    contents in #doc#. Note that the file contents are not checked to see if
    they are valid HTML (which they must be). 

    @return True if the `override file' is present, false otherwise. In the
    later case #doc#'s contents are undefined.
*/

bool
found_override(string name, string &doc)
{
    ifstream ifs((name + ".ovr").c_str());
    if (!ifs)
	return false;

    char tmp[256];
    doc = "";
    while (!ifs.eof()) {
	ifs.getline(tmp, 255);
	strcat(tmp, "\n");
	doc += tmp;
    }

    return true;
}

/** Read the input stream #in# and discard the MIME header. The MIME header
    is separated from the body of the document by a single blank line. If no
    MIME header is found, then the input stream is `emptied' and will contain
    nothing.

    @memo Read and discard the MIME header of the stream #in#.
    @return True if a MIME header is found, false otherwise.
*/

bool
remove_mime_header(FILE *in)
{
    char tmp[256];
    while (!feof(in)) {
	fgets(tmp, 255, in);
	if (tmp[0] == '\n')
	    return true;
    }

    return false;
}    

/** Look in the CGI directory (given by #cgi#) for a per-cgi HTML* file. Also
    look for a dataset-specific HTML* document. Catenate the documents and
    return them in a single string variable.

    The #cgi# path must include the `API' prefix at the end of the path. For
    example, for the NetCDF server whose prefix is `nc' and resides in the
    DODS_ROOT/etc directory of my computer, #cgi# is
    `/home/dcz/jimg/src/DODS/etc/nc'. This function then looks for the file
    named #cgi#.html.

    Similarly, to locate the dataset-specific HTML* file it catenates `.html'
    to #name#, where #name# is the name of the dataset. If the filename part
    of #name# is of the form [A-Za-z]+[0-9]*.* then this function also looks
    for a file whose name is [A-Za-z].html For example, if #name# is
    .../data/fnoc1.nc this function first looks for .../data/fnoc1.nc.html.
    However, if that does not exist it will look for .../data/fnoc.html. This
    allows one `per-dataset' file to be used for a collection of files with
    the same root name.

    NB: An HTML* file contains HTML without the <html>, <head> or <body> tags
    (my own notation).

    @memo Look for the user supplied CGI- and dataset-specific HTML* documents.
    @return A string which contains these two documents catenated. Documents
    that don't exist are treated as `empty'.
*/

string
get_user_supplied_docs(string name, string cgi)
{
    char tmp[256];
    ostrstream oss;
    ifstream ifs((cgi + ".html").c_str());

    if (ifs) {
	while (!ifs.eof()) {
	    ifs.getline(tmp, 255);
	    oss << tmp << "\n";
	}
	ifs.close();
	
	oss << "<hr>";
    }

    ifs.open((name + ".html").c_str());

    // If name.html cannot be opened, look for basename.html
    if (!ifs) {
	unsigned int slash = name.find_last_of('/');
	string pathname = name.substr(0, slash);
	string filename = name.substr(slash+1);
	// filename = filename.at(RXalpha); // XXX can't do this with string
	string new_name = pathname + "/" + filename; // XXX + ".html"
	ifs.open(new_name.c_str());
    }

    if (ifs) {
	while (!ifs.eof()) {
	    ifs.getline(tmp, 255);
	    oss << tmp << "\n";
	}
	ifs.close();
    }

    oss << ends;
    string html = oss.str();
    oss.rdbuf()->freeze(0);

    return html;
}

static void
process_trace_options(char *tcode) 
{
    while (tcode++)
	switch (*tcode) {
	  case 'a': WWWTRACE |= SHOW_ANCHOR_TRACE; break;
	  case 'A': WWWTRACE |= SHOW_APP_TRACE; break;
	  case 'b': WWWTRACE |= SHOW_BIND_TRACE; break;
	  case 'c': WWWTRACE |= SHOW_CACHE_TRACE; break;
	  case 'h': WWWTRACE |= SHOW_AUTH_TRACE; break;
	  case 'i': WWWTRACE |= SHOW_PICS_TRACE; break;
	  case 'k': WWWTRACE |= SHOW_CORE_TRACE; break;
	  case 'l': WWWTRACE |= SHOW_SGML_TRACE; break;
	  case 'm': WWWTRACE |= SHOW_MEM_TRACE; break;
	  case 'p': WWWTRACE |= SHOW_PROTOCOL_TRACE; break;
	  case 's': WWWTRACE |= SHOW_STREAM_TRACE; break;
	  case 't': WWWTRACE |= SHOW_THREAD_TRACE; break;
	  case 'u': WWWTRACE |= SHOW_URI_TRACE; break;
	  case 'U': WWWTRACE |= SHOW_UTIL_TRACE; break;
	  case 'x': WWWTRACE |= SHOW_MUX_TRACE; break;
	  case 'z': WWWTRACE = SHOW_ALL_TRACE; break;
	  default:
	    cerr << "Unrecognized trace option: `" << *tcode << "'" 
		 << endl;
	    break;
	}
}

/** Write out the given error object. If the Error object #e# is empty, don't
    write anything out.

    @author jhrg */

static void
output_error_object(Error e)
{
    if (e.OK())
	cout << "Error: " << e.error_message() << endl;
}

/** 
    Build an HTML page which both describes the data and provides a way for
    users to query the dataset. The page prompts for the input of query
    parameters and builds a URL which matches those parameters. 

    Since this HTML page should display both DDS and DAS information it makes
    little sense to have it read from standard input. Thus the only way to
    get output is with the URL of a dataset passed as a command line
    argument. Note that the software will accept only one argument and ignore
    others (printing a warning). At some point in the future this program
    might accept several URLs and create an page whcih reads from several
    datasets at once.

    @author jhrg */

int
main(int argc, char * argv[])
{
    GetOpt getopt (argc, argv, "vVt:hm");
    int option_char;
    bool trace = false;
    bool verbose = false;
    bool regular_header = false;
    bool nph_header = false;
    char *tcode = NULL;
    int topts = 0;

    putenv("_POSIX_OPTION_ORDER=1"); // Suppress GetOpt's argv[] permutation.

    while ((option_char = getopt()) != EOF)
	switch (option_char) {
	  case 'm': regular_header = true; break;
	  case 'n': nph_header = true; break;
	  case 'v': verbose = true; break;
	  case 'V': {cerr << "WWW Interface version: " << VERSION << endl
			  << "DAP version: " << DVR << endl; exit(0);}
	  case 't':
	    trace = true;
	    topts = strlen(getopt.optarg);
	    if (topts) {
		tcode = new char[topts + 1];
		strcpy(tcode, getopt.optarg); 
		process_trace_options(tcode);
		delete tcode;
	    }
	    break;
	  case 'h':
	  case '?':
	  default:
	    usage((string)argv[0]); exit(1); break;
	}

    Connect *url = 0;

    // This program only works with one URL; I'm using a processing loop
    // because I ripped this code from asciival. Ignore i > getopt.optind and
    // print a warning. Maybe in the future this will be able to do something
    // sensible (build a combined document?) for multiple URLs. 4/7/99 jhrg
    for (int i = getopt.optind; i < argc; ++i) {
	DBG(cerr << "argv[" << i << "] (of " << argc << "): " << argv[i] \
	     << endl);

	if (i > getopt.optind) {
	    cerr << "Warning: URL `" << argv[i] << "' ignored" << endl;
	    continue;
	}
	    
	if (url)
	    delete url;
	
	url = new Connect(argv[i], trace);

	if (url->is_local()) {
	    cerr << "Error: URL `" << argv[i] << "' is local." << endl;
	    continue;
	}

	if (!url->request_das() || !url->request_dds()) {
	    cerr << "Error: URL `" << argv[i] << "' could not be accessed."
		 << endl;
	    continue;
	}
	
	global_das = url->das();
	DDS dds = url->dds();

	if (regular_header || nph_header)
	    wo.write_html_header(nph_header);

	cout << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n"
	     << "\"http://www.w3.org/TR/REC-html40/loose.dtd\">\n"
	     << "<html><head><title>DODS Dataset Query Form</title>\n"
	     << "<script type=\"text/javascript\">\n"
	     << "<!--\n"
	    // Javascript code here
	     << java_code << "\n"
	     << "DODS_URL = new dods_url(\"" << argv[i] << "\");\n"
	     << "// -->\n"
	     << "</script>\n"
	     << "</head>\n" 
	     << "<body>\n"
	     << "<center><h2>DODS Dataset Query Form</h2></center>\n"
	     << "<hr>\n"
	     << "<form action=\"\">\n"
	     << "<table>\n";
	wo.write_disposition(argv[i]);
	cout << "<tr><td><td><hr>\n\n";
	wo.write_global_attributes(global_das);
	cout << "<tr><td><td><hr>\n\n";
	wo.write_variable_entries(global_das, dds);
	cout << "</table></form>\n\n"
	     << "<hr>\n\n"
	     << "<address>Send questions or comments to: <a href=\"mailto:support@unidata.ucar.edu\">support@unidata.ucar.edu</a></address>"
	     << "</body></html>\n";
    }

    cout.flush();
    cerr.flush();

    return 0;
}
