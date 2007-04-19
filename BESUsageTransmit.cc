// BESUsageTransmit.cc

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
#include "BESUsageTransmit.h"
#include "DODSFilter.h"
#include "BESContainer.h"
#include "BESDataNames.h"
#include "cgi_util.h"
#include "BESUsage.h"
#include "usage.h"
#include "Error.h"
#include "util.h"
#include "BESTransmitException.h"

#include "BESLog.h"

using namespace dap_usage;

void
 BESUsageTransmit::send_basic_usage(BESResponseObject * obj,
                                    BESDataHandlerInterface & dhi)
{
    BESUsage *usage = dynamic_cast < BESUsage * >(obj);
    DAS *das = usage->get_das()->get_das();
    DDS *dds = usage->get_dds()->get_dds();

    dhi.first_container();

    string dataset_name = dhi.container->access();

    try {
        (*BESLog::TheLog()) << "writing usage/info" << endl;

        write_usage_response(stdout, *dds, *das, dataset_name, "", false);

        (*BESLog::TheLog()) << "done transmitting usage/info" << endl;
    }
    catch(Error & e) {
        string err =
            "Failed to write usage: " + e.get_error_message() + "(" +
            long_to_string(e.get_error_code()) + ")";
        throw BESTransmitException(err, __FILE__, __LINE__);
    }
    catch(...) {
        string err = "Failed to write usage: Unknown exception caught";
        throw BESTransmitException(err, __FILE__, __LINE__);
    }
}

void
BESUsageTransmit::send_http_usage( BESResponseObject *obj,
                                   BESDataHandlerInterface &dhi )
{
    set_mime_text( stdout, unknown_type ) ;
    BESUsageTransmit::send_basic_usage( obj, dhi ) ;
}

