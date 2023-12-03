/**
 * DeferredExec.hpp - Object that holds a queue of commands to execute at a
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

#ifndef SE_WORLD3D_DELAYEDEXEC_HPP
#define SE_WORLD3D_DELAYEDEXEC_HPP

#include <sigc++/signal.h>

#include <functional>
#include <queue>


/**
 * DeferredExec objects allow objects to delay function calls to a later time.
 * Currently that time is 'when an OpenGL context is ready'.
 */
class DeferredExec
{
public:
    using QueuedFunc = std::function<void()>;

    /** Signal to DeferredExec objects that the OpenGL context is ready. */
    static void context_ready();

    DeferredExec();

    /** Execute all operations in the queue. */
    void flush_queue();

protected:
    /** Push a new operation to the queue. */
    void push_queue(QueuedFunc func);

private:
    static sigc::signal<void()> _sig_glcontext_ready;

    std::queue<QueuedFunc> _queue{};
};

#endif
