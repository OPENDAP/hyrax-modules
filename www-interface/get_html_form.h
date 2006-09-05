
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

#include <BaseType.h>
#include <DataDDS.h>
#include "WWWOutput.h"

namespace dap_html_form {

extern WWWOutput *wo;

BaseType *basetype_to_wwwtype( BaseType *bt );
DataDDS *datadds_to_www_datadds( DataDDS *dds );
void write_html_form_interface(FILE *dest, DDS *dds, DAS *das, const string &url,
                          bool html_header = true,
                          const string &admin_name = "support@unidata.ecar.edu", 
                          const string &help_location = "http://www.opendap.org/online_help_files/opendap_form_help.html");
}
