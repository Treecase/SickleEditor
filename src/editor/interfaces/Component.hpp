/**
 * Component.hpp - Component interface.
 * Copyright (C) 2024 Trevor Last
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

#ifndef SE_EDITOR_INTERFACE_COMPONENT_HPP
#define SE_EDITOR_INTERFACE_COMPONENT_HPP

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace Sickle
{
    class Component;
    class Componentable;

    /**
     * A component.
     */
    class Component
    {
    public:
        friend class Componentable;

        virtual ~Component() = default;

        /** Run whatever functionality this component supplies. */
        virtual void execute() = 0;

    protected:
        /** Called when the component is attached to an object. */
        virtual void on_attach(Componentable &) = 0;
        /** Called when the component is detached from an object. */
        virtual void on_detach(Componentable &) = 0;
    };

    /**
     * An object which can have components.
     */
    class Componentable
    {
    public:
        virtual ~Componentable() = default;

        /**
         * Get all the attached components.
         *
         * @return A collection of the attached components.
         */
        std::vector<std::shared_ptr<Component>> get_components() const;

        /**
         * Get attached components matching the predicate.
         *
         * @return A collection of matching attached components.
         */
        std::vector<std::shared_ptr<Component>> get_components_matching(
            std::function<bool(std::shared_ptr<Component>)> predicate) const;

        /**
         * Attach a component to the object.
         *
         * If the component's on_attach method throws, the component will not
         * be added, and the exception will be re-thrown.
         *
         * @param component The component to add.
         */
        void add_component(std::shared_ptr<Component> const &component);

        /**
         * Detach a component from the object.
         *
         * @param component The component to remove.
         */
        void remove_component(std::shared_ptr<Component> const &component);

    protected:
        std::vector<std::shared_ptr<Component>> components{};
    };
} // namespace Sickle

#endif
