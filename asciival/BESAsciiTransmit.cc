// BESAsciiTransmit.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.edu> and Jose Garcia <jgarcia@ucar.edu>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301

// (c) COPYRIGHT University Corporation for Atmostpheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

#include "BESDapTransmit.h"
#include "DODSFilter.h"
#include "BESAsciiTransmit.h"
#include "DODSFilter.h"
#include "BESContainer.h"
#include "BESDataNames.h"
#include "cgi_util.h"
#include "BESDataDDSResponse.h"
#include "BaseType.h"
#include "Sequence.h"
#include "ConstraintEvaluator.h"
#include "get_ascii.h"
#include "InternalErr.h"
#include "util.h"
#include "BESDapError.h"
#include "BESInternalFatalError.h"

#include "BESDebug.h"

using namespace dap_asciival;

void
BESAsciiTransmit::send_basic_ascii( BESResponseObject * obj,
                                    BESDataHandlerInterface & dhi )
{
    BESDataDDSResponse *bdds = dynamic_cast < BESDataDDSResponse * >(obj);
    DataDDS *dds = bdds->get_dds();
    ConstraintEvaluator & ce = bdds->get_ce();

    dhi.first_container();

    string constraint = dhi.data[POST_CONSTRAINT];
    try
    {
        ce.parse_constraint( constraint, *dds ) ;
    }
    catch( InternalErr &e )
    {
        string err = "Failed to parse the constraint expression: "
            + e.get_error_message() ;
        throw BESDapError( err, true, e.get_error_code(), __FILE__, __LINE__ ) ;
    }
    catch( Error &e )
    {
        string err = "Failed to parse the constraint expression: "
            + e.get_error_message() ;
        throw BESDapError( err, false, e.get_error_code(), __FILE__, __LINE__ );
    }
    catch(...)
    {
        string err = (string) "Failed to parse the constraint expression: "
            + "Unknown exception caught";
        throw BESInternalFatalError( err, __FILE__, __LINE__ ) ;
    }

    dds->tag_nested_sequences();        // Tag Sequences as Parent or Leaf node.

    string dataset_name = dhi.container->access();

    try {
        // Handle *functional* constraint expressions specially 
        if (ce.functional_expression()) {
            // This returns a new BaseType, not a pointer to one in the DataDDS
            // So once the data has been read using this var create a new
            // DataDDS and add this new var to the it.
            BaseType *var = ce.eval_function(*dds, dataset_name);
            if (!var)
                throw Error(unknown_error,
                            "Error calling the CE function.");

            var->read(dataset_name);

            // FIXME: Do I need to delete the other DataDDS? Do I need it
            // anymore. I've got what I need doing the eval_function call
            // and I'm going to create a new DataDDS with it. So I don't
            // think I need the old one.
            //
            // delete dds ;
            dds = new DataDDS(NULL, "virtual");
            dds->add_var(var);
        } else {
            // Iterate through the variables in the DataDDS and read in the data
            // if the variable has the send flag set. 
            // Note the special case for Sequence. The transfer_data() method
            // uses the same logic as serialize() to read values but transfers
            // them to the d_values field instead of writing them to a XDR sink
            // pointer. jhrg 9/13/06
            for (DDS::Vars_iter i = dds->var_begin(); i != dds->var_end(); i++) {
                BESDEBUG( "ascii", "processing var: " << (*i)->name() << endl )
                if ((*i)->send_p()) {
                    BESDEBUG( "ascii", "reading some data for: " << (*i)->name() << endl )
                    (**i).intern_data(dataset_name, ce, *dds);
#if 0
                    switch ((*i)->type()) {
                    case dods_sequence_c:
                        dynamic_cast <
                            Sequence & >(**i).transfer_data(dataset_name, ce, *dds);
                        break;

                    case dods_structure_c:
                        dynamic_cast <
                            Structure & >(**i).transfer_data(dataset_name, ce, *dds);
                        break;

                    default:
                        (*i)->read(dataset_name);
                        break;
                    }
#endif
                }
            }
        }
    }

    catch( InternalErr &e )
    {
        string err = "Failed to read data: " + e.get_error_message() ;
	throw BESDapError( err, true, e.get_error_code(), __FILE__, __LINE__ ) ;
    }
    catch(Error & e)
    {
        string err = "Failed to read data: " + e.get_error_message() ;
        throw BESDapError( err, false, e.get_error_code(), __FILE__, __LINE__ );
    }
    catch(...)
    {
        string err = "Failed to read data: Unknown exception caught";
        throw BESInternalFatalError( err, __FILE__, __LINE__ ) ;
    }

    try {
        // Now that we have constrained the DataDDS and read in the data,
        // send it as ascii
        BESDEBUG( "ascii", "converting to ascii datadds" << endl )
        DataDDS *ascii_dds = datadds_to_ascii_datadds(dds);
        BESDEBUG( "ascii", "getting ascii values" << endl )
        get_data_values_as_ascii(ascii_dds, dhi.get_output_stream());
        BESDEBUG( "ascii", "got the ascii values" << endl )
	dhi.get_output_stream() << flush ;
        delete ascii_dds;

        BESDEBUG( "ascii", "done transmitting ascii" << endl )
    }
    catch( InternalErr &e )
    {
        string err =
            "Failed to get values as ascii: " + e.get_error_message() ;
        throw BESDapError( err, true, e.get_error_code(), __FILE__, __LINE__ ) ;
    }
    catch( Error &e )
    {
        string err =
            "Failed to get values as ascii: " + e.get_error_message() ;
        throw BESDapError( err, false, e.get_error_code(), __FILE__, __LINE__ );
    }
    catch(...) {
        string err =
            "Failed to get values as ascii: Unknown exception caught";
        throw BESInternalFatalError( err, __FILE__, __LINE__ ) ;
    }
}

void
BESAsciiTransmit::send_http_ascii(BESResponseObject * obj,
				  BESDataHandlerInterface & dhi)
{
    set_mime_text(dhi.get_output_stream(), dods_data);
    BESAsciiTransmit::send_basic_ascii(obj, dhi);
}

