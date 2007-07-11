// BESUsageModule.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.edu>
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

#include <iostream>

using std::endl ;

#include "BESUsageModule.h"

#include "BESUsageNames.h"
#include "BESResponseHandlerList.h"

#include "BESUsageRequestHandler.h"
#include "BESRequestHandlerList.h"

#include "BESUsageResponseHandler.h"

#include "BESUsageTransmit.h"
#include "BESTransmitter.h"
#include "BESReturnManager.h"
#include "BESTransmitterNames.h"

#include "BESDebug.h"


void
BESUsageModule::initialize( const string &modname )
{
    BESDEBUG( "Initializing OPeNDAP Usage module" << modname << endl )

    BESDEBUG( "    adding " << modname << " request handler" << endl )
    BESRequestHandler *handler = new BESUsageRequestHandler( modname ) ;
    BESRequestHandlerList::TheList()->add_handler( modname, handler ) ;

    BESDEBUG( "    adding " << Usage_RESPONSE << " response handler" << endl )
    BESResponseHandlerList::TheList()->add_handler( Usage_RESPONSE, BESUsageResponseHandler::UsageResponseBuilder ) ;

    BESTransmitter *t = BESReturnManager::TheManager()->find_transmitter( BASIC_TRANSMITTER ) ;
    if( t )
    {
	BESDEBUG( "    adding basic " << Usage_TRANSMITTER << " transmitter" << endl )
	t->add_method( Usage_TRANSMITTER, BESUsageTransmit::send_basic_usage ) ;
    }

    t = BESReturnManager::TheManager()->find_transmitter( HTTP_TRANSMITTER ) ;
    if( t )
    {
	BESDEBUG( "    adding http " << Usage_TRANSMITTER << " transmitter" << endl )
	t->add_method( Usage_TRANSMITTER, BESUsageTransmit::send_http_usage ) ;
    }

    BESDEBUG( "Done Initializing OPeNDAP Usage module" << modname << endl )
}

void
BESUsageModule::terminate( const string &modname )
{
    BESDEBUG( "Cleaning OPeNDAP usage module " << modname << endl )

    BESDEBUG( "    removing " << modname << " request handler " << endl )
    BESRequestHandler *rh = BESRequestHandlerList::TheList()->remove_handler( modname ) ;
    if( rh ) delete rh ;

    BESDEBUG( "    removing " << Usage_RESPONSE << " response handler " << endl )
    BESResponseHandlerList::TheList()->remove_handler( Usage_RESPONSE ) ;

    BESTransmitter *t = BESReturnManager::TheManager()->find_transmitter( BASIC_TRANSMITTER ) ;
    if( t )
    {
	BESDEBUG( "    removing basic " << Usage_TRANSMITTER << " transmitter" << endl )
	t->remove_method( Usage_TRANSMITTER ) ;
    }

    t = BESReturnManager::TheManager()->find_transmitter( HTTP_TRANSMITTER ) ;
    if( t )
    {
	BESDEBUG( "    removing http " << Usage_TRANSMITTER << " transmitter" << endl )
	t->remove_method( Usage_TRANSMITTER ) ;
    }

    BESDEBUG( "Done Cleaning OPeNDAP usage module " << modname << endl )
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance
 *
 * @param strm C++ i/o stream to dump the information to
 */
void
BESUsageModule::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "BESUsageModule::dump - ("
			     << (void *)this << ")" << endl ;
}

extern "C"
{
    BESAbstractModule *maker()
    {
	return new BESUsageModule ;
    }
}

