/**
 * TextureLoadingWorker.hpp - Worker thread to load TextureImages for a
 *                            TextureSelector.
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

#ifndef SE_APPWIN_TEXTURELOADINGWORKER_HPP
#define SE_APPWIN_TEXTURELOADINGWORKER_HPP

#include <editor/textures/TextureInfo.hpp>

#include <glibmm/dispatcher.h>

#include <memory>
#include <mutex>
#include <thread>
#include <vector>


namespace Sickle::TextureSelector
{
    /**
     * Worker to load all the textures from the TextureManager in RGB format.
     */
    class TextureLoadingWorker
    {
    public:
        using Result = std::pair<
            std::shared_ptr<Editor::Textures::TextureInfo>,
            std::shared_ptr<uint8_t[]>>;

        /**
         * The main work function. Loads all the textures from TextureManager
         * and pushes them onto the results queue, notifying dispatcher for
         * each.
         *
         * @param dispatcher Dispatcher to notify parent thread that there are
         *                   new results ready for processing. Cannot be null.
         */
        void do_work(Glib::Dispatcher *dispatcher);

        /**
         * Cancels the work and finishes as soon as possible.
         */
        void cancel();

        /**
         * Check if the work is finished or cancelled.
         *
         * @return True if the work is done/cancelled, else false.
         */
        bool is_done() const;

        /**
         * Get the results that have been processed so far and drain the
         * results queue.
         *
         * @return A collection of all the results processed so far.
         */
        std::vector<Result> get_results();

    private:
        mutable std::mutex _mutex{};
        bool _cancelled{false};
        bool _is_done{false};
        std::vector<Result> _results{};
    };
}

#endif
