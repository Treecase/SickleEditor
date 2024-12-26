/**
 * EditorObject.cpp - Interface for any object visible in the Outliner view.
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

#include "EditorObject.hpp"

#include <cassert>
#include <queue>
#include <stack>

using namespace Sickle::Editor;

EditorObject::EditorObject()
{
    signal_child_added().connect(
        sigc::mem_fun(*this, &EditorObject::on_child_added));
    signal_child_removed().connect(
        sigc::mem_fun(*this, &EditorObject::on_child_removed));
    signal_added().connect(sigc::mem_fun(*this, &EditorObject::on_added));
    signal_removed().connect(sigc::mem_fun(*this, &EditorObject::on_removed));
}

EditorObject::~EditorObject()
{
    if (parent() != nullptr)
    {
        puts("WARNING: object destroyed with non-null parent.");
    }
}

EditorObject *EditorObject::parent() const
{
    return _parent;
}

std::vector<EditorObjectRef> EditorObject::children_recursive() const
{
    std::stack<EditorObjectRef> stack{};
    for (auto const &child : children())
    {
        stack.push(child);
    }

    std::vector<EditorObjectRef> output{};
    while (!stack.empty())
    {
        auto const node = stack.top();
        stack.pop();
        for (auto const &n : node->children())
        {
            stack.push(n);
        }
        output.push_back(node);
    }
    return output;
}

std::vector<EditorObjectRef> EditorObject::children_recursive_breadth_first()
    const
{
    std::queue<EditorObjectRef> queue{};
    for (auto const &child : children())
    {
        queue.push(child);
    }

    std::vector<EditorObjectRef> output{};
    while (!queue.empty())
    {
        auto const node = queue.front();
        queue.pop();
        for (auto const &n : node->children())
        {
            queue.push(n);
        }
        output.push_back(node);
    }
    return output;
}

void EditorObject::foreach(SlotForEach func)
{
    for (auto &obj : children_recursive())
    {
        std::invoke(func, obj);
    }
}

void EditorObject::foreach_breadth_first(SlotForEach func)
{
    for (auto &obj : children_recursive_breadth_first())
    {
        std::invoke(func, obj);
    }
}

void EditorObject::foreach_direct(SlotForEach func)
{
    for (auto &obj : children())
    {
        std::invoke(func, obj);
    }
}

void EditorObject::on_child_added(EditorObjectRef const &child)
{
    if (child->parent())
    {
        throw std::logic_error{"node can only have one parent"};
    }
    child->_parent = this;
    child->signal_added().emit();
}

void EditorObject::on_child_removed(EditorObjectRef const &child)
{
    child->_parent = nullptr;
    child->signal_removed().emit();
}

void EditorObject::on_added()
{
    assert(parent() != nullptr);
}

void EditorObject::on_removed()
{
    select(false);
    assert(parent() == nullptr);
}
