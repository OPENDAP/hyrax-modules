
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

#ifndef _www_output_h
#define _www_output_h

#include <string>
#include <iostream>

#include "BaseType.h"
#include "DDS.h"
#include "DAS.h"

/** Write various parts of the HTML form for a dataset.
    
    @author jhrg. */

// NB: I tried to add a private default ctor to prevent anypne from calling
// it. However, g++ complains about such a ctor because it does not
// initialize the _os reference member. 4/13/99 jhrg

class WWWOutput {
 protected:
    ostream &_os;
    int _attr_rows;
    int _attr_cols;

    void write_attributes(AttrTable *attr);

 public:
    /** Create a WWWOutput. 

	@param os The output stream to which HTML should be sent.
	@param rows The number of rows to show in the attribute textbox
	(default 5).
	@param cols The number of columns to show in the attribute textbox
	(default 70). */
    WWWOutput(ostream &os, int rows = 5, int cols = 70);

    /** Write out the header for the HTML document. */
    void write_html_header(bool nph_header);

    /** Write the disposition section of the HTML page. This section shows
	the URL and provides buttons with which the user can choose the type
	of output.

	@param url The initial URL to display. */
    void write_disposition(string url);

    /** Given the DAS, write the HTML which contains all the global
	attributes for this dataset. A global attribute is defined as
	any attribute for which name_in_kill_file() is false and
	name_is_global() is true.

	NB: This mfunc used to scan the DDS and look for attributes which
	matched no variable's name. Such an attribute was considered global.
	However, this proved to be unreliable because some servers create
	attributes which match no variable names exactly and because
	attribute aliases can introduce new attrbute containers which also
	match no variables' names.

	@param das The DAS for the dataset. */
    void write_global_attributes(DAS &das);

    /** Write the dataset variable list. This is a scrolling select box.

	@deprecated
	@param dds The dataset's DDS. */
    void write_variable_list(DDS &dds);

    /** Given the DAS and the DDS, write an HTML table which describes each
	one of the variables by listing its name, datatype and all of its
	attributes.

	@memo Write the variable entries.
	@param das The dataset's DAS.
	@param dds The dataset's DDS. */
    void write_variable_entries(DAS &das, DDS &dds);

    /** Write a variable's attribtute information. 

	@param das The dataset's DAS.
	@param btp A pointer to the variable. */
    void write_variable_attributes(BaseType *btp, DAS &das);
};

/** Write out the HTML for simple type variables. This is used not only for
    single instances of simple types, but also for those types when they
    appear within Structure and Sequence types.

    @param name The name of the varaible. */
void write_simple_variable(ostream &os, const string &name, 
			   const string &type);

/** Return a string which describes the datatype of a DODS variable.

    @param v The variable.
    @return A string describing the variable's type. */
string fancy_typename(BaseType *v);

// $Log: WWWOutput.h,v $
// Revision 1.2  2000/10/03 20:07:21  jimg
// Moved Logs to the end of each file.
//

#endif // __www_output_h
