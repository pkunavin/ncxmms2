/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2012 Pavel Kunavin <tusk.kun@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#ifndef SIGNALS_H
#define SIGNALS_H

#include <boost/signals2.hpp>

namespace ncxmms2 {
namespace Signals = boost::signals2;
}

#define NCXMMS2_SIGNAL(NAME, ...) \
            protected: \
                ncxmms2::Signals::signal<void (__VA_ARGS__)> NAME; \
            public: \
                ncxmms2::Signals::connection NAME ## _Connect(const ncxmms2::Signals::signal<void (__VA_ARGS__)>::slot_type& slot) { \
                    return NAME.connect(slot);}

#endif // SIGNALS_H
