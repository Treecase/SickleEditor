/**
 * DeferredExec.cpp - Object that holds a queue of commands to execute at a
 *                    later time.
 * Copyright (C) 2023-2024 Trevor Last
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

#include "DeferredExec.hpp"


sigc::signal<void()> DeferredExec::_sig_glcontext_ready{};
sigc::signal<void()> DeferredExec::_sig_glcontext_unready{};


void DeferredExec::context_ready()
{
    _sig_glcontext_ready.emit();
}

void DeferredExec::context_unready()
{
    _sig_glcontext_unready.emit();
}


DeferredExec::DeferredExec()
{
    _conn_ready = _sig_glcontext_ready.connect(
        sigc::mem_fun(*this, &DeferredExec::_on_ready));
    _conn_unready = _sig_glcontext_unready.connect(
        sigc::mem_fun(*this, &DeferredExec::_on_unready));
}


DeferredExec::~DeferredExec()
{
    _conn_ready.disconnect();
    _conn_unready.disconnect();
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


void DeferredExec::clear_queue()
{
    while (!_queue.empty())
        _queue.pop();
}


void DeferredExec::push_queue(QueuedFunc func)
{
    if (_is_ready)
        std::invoke(func);
    else
        _queue.push(func);
}



void DeferredExec::_on_ready()
{
    _is_ready = true;
    flush_queue();
}


void DeferredExec::_on_unready()
{
    _is_ready = false;
}
