// CSVModule.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004-2009 University Corporation for Atmospheric Research
// Author: Stephan Zednik <zednik@ucar.edu> and Patrick West <pwest@ucar.edu>
// and Jose Garcia <jgarcia@ucar.edu>
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301
 
// (c) COPYRIGHT University Corporation for Atmospheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//	zednik      Stephan Zednik <zednik@ucar.edu>
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

#include <iostream>

using std::endl ;

#include "CSVModule.h"
#include "BESRequestHandlerList.h"
#include "CSVRequestHandler.h"
#include "BESLog.h"
#include "BESResponseHandlerList.h"
#include "BESResponseNames.h"
#include "CSVResponseNames.h"

#include "BESContainerStorageList.h"
#include "BESContainerStorageCatalog.h"
#include "BESCatalogDirectory.h"
#include "BESCatalogList.h"
#include <BESDapService.h>

#include "BESDebug.h"

void
CSVModule::initialize( const string &modname )
{
    BESDEBUG( "csv", "Initializing CSV Module " << modname << endl ) ;

    BESDEBUG( "csv", "    adding " << modname << " request handler" << endl ) ;
    BESRequestHandlerList::TheList()->
	add_handler( modname, new CSVRequestHandler( modname ) ) ;

    BESDEBUG( "csv", modname << " handles dap services" << endl ) ;
    BESDapService::handle_dap_service( modname ) ;

    BESDEBUG( "csv", "    adding " << CSV_CATALOG << " catalog" << endl ) ;
    if( !BESCatalogList::TheCatalogList()->ref_catalog( CSV_CATALOG ) )
    {
	BESCatalogList::TheCatalogList()->
	    add_catalog(new BESCatalogDirectory( CSV_CATALOG ) ) ;
    }
    else
    {
	BESDEBUG( "csv", "    catalog already exists, skipping" << endl ) ;
    }

    BESDEBUG( "csv", "    adding Catalog Container Storage" << endl ) ;
    if( !BESContainerStorageList::TheList()->ref_persistence( CSV_CATALOG ) )
    {
	BESContainerStorageList::TheList()->
	    add_persistence( new BESContainerStorageCatalog( CSV_CATALOG ) ) ;
    }
    else
    {
	BESDEBUG( "csv", "    storage already exists, skipping" << endl ) ;
    }

    BESDEBUG( "csv", "    adding csv debug context" << endl ) ;
    BESDebug::Register( "csv" ) ;

    BESDEBUG( "csv", "Done Initializing CSV Handler " << modname << endl ) ;
}

void
CSVModule::terminate( const string &modname )
{
    BESDEBUG( "csv", "Cleaning CSV Module" << modname << endl ) ;

    BESDEBUG( "csv", "    removing " << modname << " request handler" << endl );
    BESRequestHandler *rh = BESRequestHandlerList::TheList()->remove_handler( modname ) ;
    if( rh ) delete rh ;
    
    BESDEBUG( "csv", "    removing catalog container storage"
		     << CSV_CATALOG << endl ) ;
    BESContainerStorageList::TheList()->deref_persistence( CSV_CATALOG ) ;

    BESDEBUG( "csv", "    removing " << CSV_CATALOG << " catalog" << endl ) ;
    BESCatalogList::TheCatalogList()->deref_catalog( CSV_CATALOG ) ;

    BESDEBUG( "csv", "Done Cleaning CSV Module" << modname << endl ) ;
}

extern "C"
{
    BESAbstractModule *maker()
    {
	return new CSVModule ;
    }
}

void
CSVModule::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "CSVModule::dump - ("
			     << (void *)this << ")" << endl ;
}

