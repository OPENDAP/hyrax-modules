// BESUsage.h

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

#ifndef I_BESUsage_h
#define I_BESUsage_h 1

#include "DODSResponseObject.h"
#include "DAS.h"
#include "DDS.h"

/** @brief container for a DAS and DDS needed to write out the usage
 * information for a dataset.
 *
 * This is a container for the usage response information, which needs a DAS
 * and a DDS. An instances of BESUsage takes ownership of the das and dds
 * passed to it and deletes it in the destructor.
 *
 * @see DODSResponseObject
 * @see DAS
 * @see DDS
 */
class BESUsage : public DODSResponseObject
{
private:
    DAS *			_das ;
    DDS *			_dds ;

				BESUsage() {}
public:
    				BESUsage( DAS *das, DDS *dds )
				    : _das( das ), _dds( dds ) {}
    virtual			~BESUsage()
				{
				    if( _das ) delete _das ;
				    if( _dds ) delete _dds ;
				}

    DAS *			get_das() { return _das ; }
    DDS *			get_dds() { return _dds ; }
} ;

#endif // I_BESUsage_h

