
// (c) COPYRIGHT URI/MIT 1998-2000
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

/** Asciival is a simple DODS client similar to geturl or writeval that reads
    a DODS data object (either by dereferencing a URL, reading from a file or
    reading from stdin) and writes comma separated ASCII representation of the
    data values in that object. 
    
    @author: jhrg */

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: ascii_val.cc,v 1.14 2001/09/28 23:46:06 jimg Exp $"};

#include <stdio.h>
#include <assert.h>
#include <string>

#include <GetOpt.h>

#include "BaseType.h"
#include "Connect.h"

#include "AsciiByte.h"
#include "AsciiInt32.h"
#include "AsciiUInt32.h"
#include "AsciiFloat64.h"
#include "AsciiStr.h"
#include "AsciiUrl.h"
#include "AsciiList.h"
#include "AsciiArray.h"
#include "AsciiStructure.h"
#include "AsciiSequence.h"
#include "AsciiGrid.h"

#include "name_map.h"
#include "cgi_util.h"
#include "debug.h"

name_map names;
bool translate = false;

#ifndef VERSION
const char *VERSION = "unknown";
#endif /* VERSION */

static void
usage(string name)
{
    cerr << "Usage: " << name 
	 << " [mngvVt] -- [<url> [-r <var>:<newvar> ...] ...]" << endl
	 << "       m: Output a MIME header." << endl
	 << "       n: Turn on name canonicalization." << endl
	 << "       g: Use the GUI to show progress" << endl
	 << "       v: Verbose output." << endl
	 << "       V: Print the version number and exit" << endl
	 << "       t: Trace network I/O (HTTP, ...). See geturl" << endl
	 << "       r: Per-URL name mappings; var becomes newvar." << endl
	 << endl
	 << "<url> may be a true URL, which asciival will dereference," << endl
	 << "it may be a local file or it may be standard input." << endl
	 << "In the later case use `-' for <url>."
	 << endl;
}

static void
process_per_url_options(int &i, int argc, char *argv[], bool verbose = false)
{
    names.delete_all();	// Clear the global name map for this URL.

    // Test for per-url option. Set variables accordingly.
    while (argv[i+1] && argv[i+1][0] == '-')
	switch (argv[++i][1]) {
	  case 'r':
	    ++i;	// Move past option to argument.
	    if (verbose)
		cerr << "  Renaming: " << argv[i] << endl;
	    // Note that NAMES is a global variable so that all the
	    // writeval() mfuncs can access it without having to pass
	    // it into each function call.
	    names.add(argv[i]);
	    break;
		
	  default:
	    cerr << "Unknown option `" << argv[i][1] 
		 << "' paired with URL has been ignored." << endl;
	    break;
	}

}

static void
process_trace_options(char *tcode) 
{
    while (*tcode){
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
	tcode++;
    }
}

static void
process_data(XDR *src, DDS *dds)
{
    cout << "Dataset: " << dds->get_dataset_name() << endl;

    for (Pix q = dds->first_var(); q; dds->next_var(q)) {
	dynamic_cast<AsciiOutput *>(dds->var(q))->print_ascii(cout);
	cout << endl;
    }
}

/** Write out the given error object. If the Error object #e# is empty, don't
    write anything out (since that will confuse loaddods).

    @author jhrg */

static void
output_error_object(const Error &e)
{
    if (e.OK())
	cout << "Error: " << e.get_error_message() << endl;
}

// Read a DODS data object. The object maybe specified by a URL (which will
// be dereferenceed using Connect, it maybe read from a file or it maybe read
// from stdin. Use `-' in the command line to indicate the next input should
// be read from standard input.

int
main(int argc, char * argv[])
{
    GetOpt getopt (argc, argv, "ngmvVh?t:");
    int option_char;
    bool verbose = false;
    bool trace = false;
    bool translate = false;
    bool gui = false;
    bool mime_header = false;
    string expr = "";
    char *tcode = NULL;
    int topts = 0;

    putenv("_POSIX_OPTION_ORDER=1"); // Suppress GetOpt's argv[] permutation.

    while ((option_char = getopt()) != EOF)
	switch (option_char) {
	  case 'n': translate = true; break;
	  case 'g': gui = true; break;
	  case 'm': mime_header = true; break;
	  case 'v': verbose = true; break;
	  case 'V': {cerr << "asciival: " << VERSION << endl; exit(0);}
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

    for (int i = getopt.optind; i < argc; ++i) {
	try {
	    if (url)
		delete url;
	
	    if (strcmp(argv[i], "-") == 0){
		url = new Connect("stdin", trace, false);
		DBG(cerr << "Instantiated Connect object using stdin." \
		    << endl);

	    }
	    else{
		url = new Connect(argv[i], trace);
		DBG(cerr << endl << "Instantiated Connect object using " \
		    << argv[i] << endl << endl);
	    }

	    DBG2(cerr << "argv[" << i << "] (of " << argc << "): " 
		 << argv[i] << endl);

	    if (verbose) {
		string source_name;

		DBG(cerr << "URL->is_local(): " << url->is_local() << endl);
		DBG(cerr << "argv[" << i << "]: \"" << argv[i] << "\"" 
		    << endl);

		if (url->is_local() && (strcmp(argv[i], "-") == 0))
		    source_name = "standard input";
		else if (url->is_local())
		    source_name = argv[i];
		else
		    source_name = url->URL(false);

		cerr << endl << "Reading: " << source_name << endl;
	    }

	    process_per_url_options(i, argc, argv, verbose);

	    DDS *dds;

	    if (url->is_local() && (strcmp(argv[i], "-") == 0))
		dds = url->read_data(stdin, gui, false);
	    else if (url->is_local())
		dds = url->read_data(fopen(argv[i], "r"), gui, false);
	    else
		dds = url->request_data(expr, gui, false);

	    if (dds) {
		if (mime_header)
		    set_mime_text(cout, dods_data);
		process_data(url->source(), dds);
	    }
	    else {
		output_error_object(url->error());
	    }
	}
	catch (Error &e) {
	    output_error_object(e);
	}
    }

    cout.flush();
    cerr.flush();

    return 0;
}

// $Log: ascii_val.cc,v $
// Revision 1.14  2001/09/28 23:46:06  jimg
// merged with 3.2.3.
//
// Revision 1.13.4.2  2001/09/27 03:10:36  jimg
// Minor fix to a DBG call.
//
// Revision 1.13.4.1  2001/09/18 23:29:26  jimg
// Massive changes to use the new AsciiOutput class. Output more or less
// conforms to the DAP Spec. draft.
//
// Revision 1.13  2000/10/02 20:09:52  jimg
// Moved Log entries to the end of the files
//
// Revision 1.12  1999/07/28 23:00:54  jimg
// Separated from the writeval directory, moved to tools
//
// Revision 1.11  1999/07/24 00:10:28  jimg
// Merged the release-3-0-2 branch
//
// Revision 1.10  1999/05/25 18:47:11  jimg
// Merged Nathan's fixes for the -t option and some debugging instrumentation.
//
// Revision 1.9  1999/05/18 20:59:32  jimg
// Removed name_from_url(...) since int is never used.
//
// Revision 1.8  1999/04/30 17:06:56  jimg
// Merged with no-gnu and release-2-24
//
// Revision 1.7  1999/03/24 06:23:43  brent
// convert String.h to std lib <string>, convert to packages regex -- B^2
//
// Revision 1.6  1998/09/16 23:30:26  jimg
// Change process_data() so that it calls print_all_vals() for Structure.
//
// Revision 1.5  1998/08/01 01:31:36  jimg
// Fixed a bug in process_data() where deserialize() was not called before the
// call to print_all_vals().
//
// Revision 1.4  1998/07/30 19:05:54  jimg
// Fixed the call to usage; passing a char * invoked cgi-util.cc:usage which
// was not what we wanted. Also added help about the -m option.
//
// Revision 1.3  1998/03/19 23:26:03  jimg
// Added code to write out error messages read from the server.
//
// Revision 1.2  1998/03/16 19:45:09  jimg
// Added mime header output. See -m.
//
// Revision 1.1  1998/03/16 18:30:19  jimg
// Added
