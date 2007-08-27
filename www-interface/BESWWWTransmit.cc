// BESWWWTransmit.cc

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
#include "BESWWWTransmit.h"
#include "DODSFilter.h"
#include "BESContainer.h"
#include "BESDataNames.h"
#include "BESWWWNames.h"
#include "cgi_util.h"
#include "BESWWW.h"
#include "Error.h"
#include "util.h"
#include "BESTransmitException.h"

#include "BESDebug.h"

#include "get_html_form.h"

using namespace dap_html_form;

void
 BESWWWTransmit::send_basic_form(BESResponseObject * obj,
                                 BESDataHandlerInterface & dhi)
{
#if 0
    BESWWW *usage = dynamic_cast < BESWWW * >(obj);
    DAS *das = usage->get_das();
    DDS *dds = usage->get_dds();
#endif

    dhi.first_container();
#if 0
    string dataset_name = dhi.container->access();
#endif
    try {
        BESDEBUG( "www", "converting dds to www dds" << endl )

	DDS *dds = dynamic_cast<BESWWW*>(obj)->get_dds()->get_dds() ;
        DDS *wwwdds = dds_to_www_dds( dds ) ;
	DAS *das = dynamic_cast<BESWWW*>(obj)->get_das()->get_das() ;
        wwwdds->transfer_attributes( das ) ;
        
        BESDEBUG( "www", "writing form" << endl )

        string url = dhi.data[WWW_URL];
#if 0
        write_html_form_interface(dhi.get_output_stream(), wwwdds, das, url, false);
#endif
        write_html_form_interface(dhi.get_output_stream(), wwwdds, url, false);

        BESDEBUG( "www", "done transmitting form" << endl )

	delete wwwdds ;
    }
    catch(Error & e) {
        string err =
            "Failed to write html form: " + e.get_error_message() + "(" +
            long_to_string(e.get_error_code()) + ")";
        throw BESTransmitException(err, __FILE__, __LINE__);
    }
    catch(...) {
        string err = "Failed to write html form: Unknown exception caught";
        throw BESTransmitException(err, __FILE__, __LINE__);
    }
}

void BESWWWTransmit::send_http_form(BESResponseObject * obj,
                                    BESDataHandlerInterface & dhi)
{
    set_mime_text(dhi.get_output_stream(), unknown_type);
    BESWWWTransmit::send_basic_form(obj, dhi);
}

