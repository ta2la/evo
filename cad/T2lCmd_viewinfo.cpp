//
// Copyright (C) 2015 Petr Talla. [petr.talla@gmail.com]
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//		      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=============================================================================
#include "T2lCmd_viewinfo.h"

#include "T2lUpdateLock.h"
#include "T2lScene.h"
#include "T2lDisplay.h"
#include "T2lStyle.h"
#include "T2lFilter.h"

#include "Protocol.h"

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_viewinfo::Cmd_viewinfo(void) :
    Cmd("measure")
{
}

//===================================================================
Cmd_viewinfo::~Cmd_viewinfo(void)
{
}

//===================================================================
void Cmd_viewinfo::enterPoint( const Point2F& pt, Display& view )
{
    Filter* filter = view.entityPack()->scene()->filter();

    cout << "VIEW: " << &view << endl;

    stringstream ss;
    ss << "filter info: " << filter->print().c_str() << endl;

    Protocol::protocolCmdGet().append(ss.str().c_str());
}

//===================================================================
void Cmd_viewinfo::enterMove( const Point2F& pt, Display& view )
{   
}

//===================================================================
