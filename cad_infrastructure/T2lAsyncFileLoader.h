//
// Copyright (C) 2026 Petr Talla. [petr.talla@gmail.com]
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
#pragma once

#include <QString>
#include <QByteArray>
#include <functional>

namespace T2l
{

class AsyncFileLoader {
//=============================================================================
public:
//! @section Methods
    using Callback = std::function<void(QByteArray)>;
    using Provider = std::function<void(const QString&, Callback)>;

    static void providerSet(Provider p) { provider_() = p; }
    static bool available() { return (bool)provider_(); }
    static void request(const QString& fileName, Callback cb) {
        if (provider_()) provider_()(fileName, cb);
    }
//=============================================================================
private:
//! @section Data
    static Provider& provider_() { static Provider p; return p; }
};

} // namespace T2l
