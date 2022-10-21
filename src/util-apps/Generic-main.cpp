/**
 * Generic-main.cpp - Base for all utility programs.
 * Copyright (C) 2022 Trevor Last
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

// This file sets up everything needed for a generic app. To use it, just
// #include it at the bottom of your app's .cpp. To interface with this file,
// you must define the following (you can leave them blank if you don't use
// them):
//
// Functions
//  void APP_print_usage(char const *name)
//      Called when --help is passed. (Just print the "Usage:" line and your
//      own defined options, don't worry about --help and --version, nor the
//      trailing info.)
//  APP_Config APP_handle_args(int argc, char *argv[])
//      Called before the app runs, to handle command-line args.
//  int APP_run(APP_Config config)
//      The main body of your app.
//
// Variables
//  [std::string|char *] APP_canon_name
//      App's canonical name, used for --version.
//  [std::string|char *] APP_version
//      App's version number, used for --version.
//
// Types
//  APP_Config
//      Data to be passed to APP_run, returned by APP_handle_args.
//
// Any of the above can also be #define'd instead.

#include "version.hpp"

#include <iostream>
#include <stdexcept>

#include <cstring>


/** Set app config from command-line arguments. */
APP_Config handle_args(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--version") == 0)
        {
            std::cout <<
                APP_canon_name
                << " (" << SE_CANON_NAME << " " << SE_VERSION << ") "
                << APP_version << "\n" <<
                "Copyright (C) 2022 Trevor Last\n"
                "License GPLv3+: GNU GPL version 3 or later "
                    "<https://gnu.org/licenses/gpl.html>\n"
                "This is free software: you are free to change and "
                    "redistribute it.\n"
                "There is NO WARRANTY, to the extent permitted by law.\n";
            exit(0);
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            APP_print_usage(argv[0]);
            std::cout <<
                "  --help\tdisplay this help and exit\n"
                "  --version\toutput version information and exit\n"
                "\n"
                "Report bugs to: "
                    "https://github.com/Treecase/SickleEditor/issues\n"
                "pkg home page: https://github.com/Treecase/SickleEditor\n";
            exit(0);
        }
    }
    return APP_handle_args(argc, argv);
}


/** Program entry point. */
int main(int argc, char *argv[])
{
    int r = EXIT_FAILURE;

    // Run the program.
#ifdef NDEBUG
    try
    {
        r = APP_run(handle_args(argc, argv));
    }
    catch (std::exception const &e)
    {
        std::cerr << "FATAL: " << e.what() << std::endl;
    }
#else
    r = APP_run(handle_args(argc, argv));
#endif

    return r;
}
