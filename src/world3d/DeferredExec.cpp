/**
 * DeferredExec.cpp - Object that holds a queue of commands to execute at a
 *                    later time.
 * Copyright (C) 2023 Trevor Last
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "world3d/DeferredExec.hpp"


sigc::signal<void()> DeferredExec::_sig_glcontext_ready{};


void DeferredExec::context_ready()
{
    _sig_glcontext_ready.emit();
}


DeferredExec::DeferredExec()
{
    _conn = _sig_glcontext_ready.connect(
        sigc::mem_fun(*this, &DeferredExec::flush_queue));
}


DeferredExec::~DeferredExec()
{
    _conn.disconnect();
}


void DeferredExec::flush_queue()
{
    while (!_queue.empty())
    {
        auto const func = _queue.front();
        _queue.pop();
        std::invoke(func);
    }
}


void DeferredExec::push_queue(QueuedFunc func)
{
    _queue.push(func);
}
