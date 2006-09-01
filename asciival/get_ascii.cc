
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

using std::cerr ;
using std::endl ;

#include <DataDDS.h>

#include "get_ascii.h"
#include "AsciiOutput.h"
#include "name_map.h"

#include "AsciiByte.h"
#include "AsciiInt16.h"
#include "AsciiUInt16.h"
#include "AsciiInt32.h"
#include "AsciiUInt32.h"
#include "AsciiFloat32.h"
#include "AsciiFloat64.h"
#include "AsciiStr.h"
#include "AsciiUrl.h"
#include "AsciiArray.h"
#include "AsciiStructure.h"
#include "AsciiSequence.h"
#include "AsciiGrid.h"


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
    //cerr << "create name_map" << endl ;
    names = new name_map();
    //cerr << "done create name_map" << endl ;
    
    //cerr << "dataset name = " << dds->get_dataset_name() << endl ;
    fprintf(dest, "Dataset: %s\n", dds->get_dataset_name().c_str());

    //cerr << "iterating through the thing" << endl ;
    DDS::Vars_iter i = dds->var_begin();
    while (i != dds->var_end()) {
	//cerr << "getting " << (*i)->name() << " of type " << (*i)->type_name() << endl ;
        dynamic_cast<AsciiOutput &>(**i++).print_ascii(dest);
        fprintf(dest, "\n");
    }
    
    delete names;
}

DataDDS *
datadds_to_ascii_datadds( DataDDS *dds )
{
    DataDDS *asciidds = new DataDDS( dds->get_factory(),
    				     dds->get_dataset_name(),
                                     dds->get_version(),
				     dds->get_protocol() ) ;

    DDS::Vars_iter i = dds->var_begin();
    while( i != dds->var_end() )
    {
	//cerr << "converting " << (*i)->name() << " of type " << (*i)->type_name() << endl ;
	asciidds->add_var( basetype_to_asciitype( *i ) ) ;
	i++ ;
    }

    return asciidds ;
}

BaseType *
basetype_to_asciitype( BaseType *bt )
{
    switch( bt->type() )
    {
	case dods_byte_c:
	    {
		return new AsciiByte( dynamic_cast<Byte *>(bt) ) ;
	    }
	    break ;
	case dods_int16_c:
	    {
		return new AsciiInt16( dynamic_cast<Int16 *>(bt) ) ;
	    }
	    break ;
	case dods_uint16_c:
	    {
		return new AsciiUInt16( dynamic_cast<UInt16 *>(bt) ) ;
	    }
	    break ;
	case dods_int32_c:
	    {
		return new AsciiInt32( dynamic_cast<Int32 *>(bt) ) ;
	    }
	    break ;
	case dods_uint32_c:
	    {
		return new AsciiUInt32( dynamic_cast<UInt32 *>(bt) ) ;
	    }
	    break ;
	case dods_float32_c:
	    {
		return new AsciiFloat32( dynamic_cast<Float32 *>(bt) ) ;
	    }
	    break ;
	case dods_float64_c:
	    {
		return new AsciiFloat64( dynamic_cast<Float64 *>(bt) ) ;
	    }
	    break ;
	case dods_str_c:
	    {
		return new AsciiStr( dynamic_cast<Str *>(bt) ) ;
	    }
	    break ;
	case dods_url_c:
	    {
		return new AsciiUrl( dynamic_cast<Url *>(bt) ) ;
	    }
	    break ;
	case dods_array_c:
	    {
		return new AsciiArray( dynamic_cast<Array *>(bt) ) ;
	    }
	    break ;
	case dods_structure_c:
	    {
		return new AsciiStructure( dynamic_cast<Structure *>(bt) ) ;
	    }
	    break ;
	case dods_sequence_c:
	    {
		return new AsciiSequence( dynamic_cast<Sequence *>(bt) ) ;
	    }
	    break ;
	case dods_grid_c:
	    {
		return new AsciiGrid( dynamic_cast<Grid *>(bt) ) ;
	    }
	    break ;
    }
}

