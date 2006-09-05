
// -*- mode: c++; c-basic-offset:4 -*-

// Copyright (c) 2006 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
// 
// This is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
// more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// This file holds the interface for the 'get data as ascii' function of the
// OPeNDAP/HAO data server. This function is called by the BES when it loads
// this as a module. The functions in the file ascii_val.cc also use this, so
// the same basic processing software can be used both by Server4 and tie older
// Server3.

#include <stdio.h>

#include <iostream>
#include <sstream>

#include <DataDDS.h>

#include "get_html_form.h"
#include "WWWOutput.h"
#include "WWWOutputFactory.h"
#include "javascript.h"         // Try to hide this stuff...

#include "WWWByte.h"
#include "WWWInt16.h"
#include "WWWUInt16.h"
#include "WWWInt32.h"
#include "WWWUInt32.h"
#include "WWWFloat32.h"
#include "WWWFloat64.h"
#include "WWWStr.h"
#include "WWWUrl.h"
#include "WWWArray.h"
#include "WWWStructure.h"
#include "WWWSequence.h"
#include "WWWGrid.h"

namespace dap_html_form {
        
// A better way to do this would have been to make WWWStructure, ..., inherit
// from both Structure and WWWOutput. But I didn't... jhrg 8/29/05
WWWOutput *wo = 0;

/** Given a BaseType varaible, return a pointer to a new variable that has the
    same parent types (Byte, et c.) but is now one of the WWW specializations.
    
    @param bt A BaseType such as NCArray, HDFArray, ...
    @return A BaseType that uses the WWW specialization (e.g., WWWArray). */
BaseType *
basetype_to_wwwtype( BaseType *bt )
{
    switch( bt->type() )
    {
        case dods_byte_c:
            {
                return new WWWByte( dynamic_cast<Byte *>(bt) ) ;
            }
            break ;
        case dods_int16_c:
            {
                return new WWWInt16( dynamic_cast<Int16 *>(bt) ) ;
            }
            break ;
        case dods_uint16_c:
            {
                return new WWWUInt16( dynamic_cast<UInt16 *>(bt) ) ;
            }
            break ;
        case dods_int32_c:
            {
                return new WWWInt32( dynamic_cast<Int32 *>(bt) ) ;
            }
            break ;
        case dods_uint32_c:
            {
                return new WWWUInt32( dynamic_cast<UInt32 *>(bt) ) ;
            }
            break ;
        case dods_float32_c:
            {
                return new WWWFloat32( dynamic_cast<Float32 *>(bt) ) ;
            }
            break ;
        case dods_float64_c:
            {
                return new WWWFloat64( dynamic_cast<Float64 *>(bt) ) ;
            }
            break ;
        case dods_str_c:
            {
                return new WWWStr( dynamic_cast<Str *>(bt) ) ;
            }
            break ;
        case dods_url_c:
            {
                return new WWWUrl( dynamic_cast<Url *>(bt) ) ;
            }
            break ;
        case dods_array_c:
            {
                return new WWWArray( dynamic_cast<Array *>(bt) ) ;
            }
            break ;
        case dods_structure_c:
            {
                return new WWWStructure( dynamic_cast<Structure *>(bt) ) ;
            }
            break ;
        case dods_sequence_c:
            {
                return new WWWSequence( dynamic_cast<Sequence *>(bt) ) ;
            }
            break ;
        case dods_grid_c:
            {
                return new WWWGrid( dynamic_cast<Grid *>(bt) ) ;
            }
            break ;
    }
}

/** Given a DDS filled with variables that are one specialization of BaseType,
    build a second DDS which contains variables made using the WWW 
    specialization.
    
    @param dds A DDS
    @return A DDS where each variable in \e dds is now a WWW* variable. */
DataDDS *
datadds_to_www_datadds( DataDDS *dds )
{
    // Should the following use WWWOutputFactory instead of the source DDS'
    // factory class? It doesn't matter for the following since the function
    // basetype_to_asciitype() doesn't use the factory. So long as no other
    // code uses the DDS' factory, this is fine. jhrg 9/5/06 
    DataDDS *wwwdds = new DataDDS( dds->get_factory(),
                                   dds->get_dataset_name(),
                                   dds->get_version(),
                                   dds->get_protocol() ) ;

    DDS::Vars_iter i = dds->var_begin();
    while( i != dds->var_end() )
    {
        wwwdds->add_var( basetype_to_wwwtype( *i ) ) ;
        i++ ;
    }

    return wwwdds ;
}


/** Using the stuff in WWWOutput and the hacked (specialized) print_val() 
    methods in the WWW* classes, write out the HTML form interface.
    
    @todo Modify this code to expect a DDS whose BaseTypes are loaded with
    Attribute tables.
    
    @param dest Write HTML here.
    @param dds A DataDDS loaded with data; the BaseTypes in this must be
    WWW* instances,
    @param url The URL that should be initially displayed in the form. The
    form's javescript code will update this incrementally as the user builds
    a constraint.
    @param das The DAS for these variables. Once this is modified as per the
    todo entry, ignored.
    @param html_header Print a HTML header/footer for the page. True by default.
    @param admin_name "support@unidata.ucar.edu" by default; use this as the 
    address for support printed on the form.
    @param help_location "http://www.opendap.org/online_help_files/opendap_form_help.html" 
    by default; otherwise, this is locataion where an HTML document that 
    explains the page can be found. 
    */
void
write_html_form_interface(FILE *dest, DDS *dds, DAS *das, const string &url,
                          bool html_header,
                          const string &admin_name, const string &help_location)
{
        WWWOutput *wo = new WWWOutput(dest);
        wo->set_das(das);

        if (html_header)
            wo->write_html_header();
            
        ostringstream oss;
        oss << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n"
             << "\"http://www.w3.org/TR/REC-html40/loose.dtd\">\n"
             << "<html><head><title>OPeNDAP Server Dataset Query Form</title>\n"
             << "<base href=\"" << help_location << "\">\n"
             << "<script type=\"text/javascript\">\n"
             << "<!--\n"
            // Javascript code here
             << java_code << "\n"
             << "DODS_URL = new dods_url(\"" << url << "\");\n"
             << "// -->\n"
             << "</script>\n"
             << "</head>\n" 
             << "<body>\n"
             << "<p><h2 align='center'>OPeNDAP Server Dataset Access Form</h2>\n"
             << "<hr>\n"
             << "<form action=\"\">\n"
             << "<table>\n";
        fprintf(stdout, "%s", oss.str().c_str());
        
        wo->write_disposition(url);
        
        fprintf(stdout, "<tr><td><td><hr>\n\n");
        
        wo->write_global_attributes(*wo->get_das());
        
        fprintf(stdout, "<tr><td><td><hr>\n\n");

        wo->write_variable_entries(*wo->get_das(), *dds);

        oss.str("");
        oss << "</table></form>\n\n" << "<hr>\n\n";
        oss << "<address>Send questions or comments to: <a href=\"mailto:"
                << admin_name << "\">" << admin_name << "</a></address>"
                << "<p>\n\
                    <a href=\"http://validator.w3.org/check?uri=referer\"><img\n\
                        src=\"http://www.w3.org/Icons/valid-html40\"\n\
                        alt=\"Valid HTML 4.0 Transitional\" height=\"31\" width=\"88\">\n\
                    </a></p>\n"
                 << "</body></html>\n";
        
        fprintf(stdout, "%s", oss.str().c_str());
        
}

} // namespace dap_html_form
