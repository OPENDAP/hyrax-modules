
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

#include <DataDDS.h>

#include "AsciiOutput.h"
#include "name_map.h"

// These are likely relics from olders versions of dap_asciival. See ticket
// 515 about their removal.
name_map *names = 0;
bool translate = false;

/** Using the AsciiOutput::print_ascii(), write the data values to an 
    output file/stream as ASCII.
    
    @param dds A DataDDS loaded with data.
    @param dest Write ASCII here. */  
void
get_data_values_as_ascii(DataDDS *dds, FILE *dest)
{
    names = new name_map();
    
    fprintf(dest, "Dataset: %s\n", dds->get_dataset_name().c_str());

    DDS::Vars_iter i = dds->var_begin();
    while (i != dds->var_end()) {
        dynamic_cast<AsciiOutput &>(**i++).print_ascii(dest);
        fprintf(dest, "\n");
    }
    
    delete names;
}

