//
// Copyright (C) 2020 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lKohonenDataSample.h"

using namespace T2l;

//=============================================================================
KohonenDataSample::KohonenDataSample( int columnSize, const char* id,
                const char* sourceDir, double price,
                const char* logo, const char* html ) :
    column_(columnSize),
    sourceDir_(sourceDir),
    id_(id),
    price_(price),
    logo_(logo),
    html_(html)
{
}

//=============================================================================
