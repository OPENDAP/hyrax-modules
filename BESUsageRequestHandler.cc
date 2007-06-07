// BESUsageRequestHandler.cc

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


#include "BESUsageRequestHandler.h"
#include "BESResponseHandler.h"
#include "BESResponseNames.h"
#include "BESVersionInfo.h"
#include "BESResponseException.h"
#include "BESDataNames.h"
#include "config.h"

BESUsageRequestHandler::BESUsageRequestHandler( const string &name )
    : BESRequestHandler( name )
{
    add_handler( HELP_RESPONSE, BESUsageRequestHandler::dap_build_help ) ;
    add_handler( VERS_RESPONSE, BESUsageRequestHandler::dap_build_version ) ;
}

BESUsageRequestHandler::~BESUsageRequestHandler()
{
}

bool
BESUsageRequestHandler::dap_build_help( BESDataHandlerInterface &dhi )
{
    BESInfo *info = (BESInfo *)dhi.response_handler->get_response_object() ;
    info->begin_tag( "dap-server" ) ;
    info->add_data_from_file( "DAP-SERVER.Help", "Dap-Server Help" ) ;
    info->end_tag( "dap-server" ) ;

    return true ;
}

bool
BESUsageRequestHandler::dap_build_version( BESDataHandlerInterface &dhi )
{
    BESVersionInfo *info = (BESVersionInfo *)dhi.response_handler->get_response_object() ;
    string name = (string)PACKAGE_NAME + "/usage" ;
    info->addHandlerVersion(name, PACKAGE_VERSION);
    return true ;
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance
 *
 * @param strm C++ i/o stream to dump the information to
 */
void
BESUsageRequestHandler::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "BESUsageRequestHandler::dump - ("
			     << (void *)this << ")" << endl ;
    BESIndent::Indent() ;
    BESRequestHandler::dump( strm ) ;
    BESIndent::UnIndent() ;
}

