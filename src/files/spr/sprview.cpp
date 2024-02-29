/**
 * sprview.cpp - .spr viewer.
 * Copyright (C) 2024 Trevor Last
 *
 * This is a little test program for .spr reading. It is completely separate
 * from the Sickle editor, besides being used to test the sprite loading code.
 * It loads sprite files passed on the command line, and displays them animated
 * at 10fps.
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

#include "spr.hpp"

#include <gtkmm.h>

#include <iostream>


// Convert sprite data to gtk pixbufs.
std::vector<Glib::RefPtr<Gdk::Pixbuf>>
convert_sprite(SPR::Sprite const &sprite);


class GioFileSpriteStream : public SPR::SpriteStream
{
public:
    GioFileSpriteStream(Glib::RefPtr<Gio::File> const &file)
    :   _stream{Gio::DataInputStream::create(file->read())}
    {
        _stream->set_byte_order(
            Gio::DataStreamByteOrder::DATA_STREAM_BYTE_ORDER_LITTLE_ENDIAN);
    }

    virtual uint8_t read_byte() override
    {
        return _stream->read_byte();
    }

    virtual uint16_t read_uint16() override
    {
        return _stream->read_uint16();
    }

    virtual int32_t read_int32() override
    {
        return _stream->read_int32();
    }

    virtual uint32_t read_uint32() override
    {
        return _stream->read_uint32();
    }

    virtual float read_float() override
    {
        float value;
        gsize total_bytes_read = 0;
        while (total_bytes_read < 4)
        {
            gsize bytes_read;
            _stream->read_all(&value + total_bytes_read, 4, bytes_read);
            total_bytes_read += bytes_read;
        }
        return value;
    }

    virtual uint8_t *read_bytes(size_t count) override
    {
        uint8_t *bytes = new uint8_t[count];
        gsize total_bytes_read = 0;
        while (total_bytes_read < count)
        {
            gsize bytes_read;
            _stream->read_all(bytes + total_bytes_read, 4, bytes_read);
            total_bytes_read += bytes_read;
        }
        return bytes;
    }

private:
    Glib::RefPtr<Gio::DataInputStream> _stream{nullptr};
};


class AppWin : public Gtk::ApplicationWindow
{
public:
    AppWin()
    :   Glib::ObjectBase{typeid(AppWin)}
    {
        set_title("sprview");
        add(_image);
        show_all_children();
    }
    virtual ~AppWin()=default;

    void open(Glib::RefPtr<Gio::File> const &file)
    {
        set_title("sprview - " + file->get_path());
        GioFileSpriteStream stream{file};
        auto const sprite = SPR::load_sprite(stream);
        _frames = convert_sprite(sprite);
        _frame_idx = 0;
        _image.set(_frames.at(_frame_idx));
        add_tick_callback(sigc::mem_fun(*this, &AppWin::tick_callback));
    }

    bool tick_callback(Glib::RefPtr<Gdk::FrameClock> const &clock)
    {
        static constexpr guint64 MAX_DELTA = (1.0 / 10.0) * 1000000;
        auto const t = clock->get_frame_time();
        if (t - _prev_time >= MAX_DELTA)
        {
            _frame_idx = (_frame_idx + 1) % _frames.size();
            _image.set(_frames.at(_frame_idx));
            set_icon(_frames.at(_frame_idx));
            _prev_time = t;
        }
        return true;
    }

private:
    Gtk::Image _image{};
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> _frames{};
    size_t _frame_idx{0};
    guint64 _prev_time{0};
};


class App : public Gtk::Application
{
public:
    static Glib::RefPtr<App> create()
    {
        return Glib::RefPtr{new App{}};
    }

    App()
    :   Glib::ObjectBase{typeid(App)}
    ,   Gtk::Application{
            "com.github.treecase.sickle.sprview",
            Gio::APPLICATION_HANDLES_OPEN}
    {
    }
    virtual ~App()=default;

    virtual void on_startup() override
    {
        Gtk::Application::on_startup();
    }

    virtual void on_activate() override
    {
        std::cout << "No files given.\n";
    }

    virtual void on_open(
        Gio::Application::type_vec_files const &files,
        Glib::ustring const &hint) override
    {
        for (auto const &file : files)
        {
            AppWin *appwin = _make_appwin();
            appwin->open(file);
            appwin->present();
        }
    }

private:
    AppWin *_make_appwin()
    {
        auto appwin = new AppWin{};
        add_window(*appwin);
        appwin->signal_hide().connect([appwin](){delete appwin;});
        return appwin;
    }
};


std::vector<Glib::RefPtr<Gdk::Pixbuf>> convert_sprite(SPR::Sprite const &sprite)
{
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> output{};
    for (auto const &frame : sprite.frames)
    {
        auto pixel_data = new guchar[frame.w * frame.h * 4];
        for (int i = 0, j = 0; i < frame.w * frame.h; ++i, j += 4)
        {
            auto const color = frame.data[i];
            auto const &pixel = sprite.palette.colors[color];
            pixel_data[j+0] = pixel.r;
            pixel_data[j+1] = pixel.g;
            pixel_data[j+2] = pixel.b;
            pixel_data[j+3] = pixel.a;
        }
        auto pixbuf = Gdk::Pixbuf::create_from_data(
            pixel_data,
            Gdk::Colorspace::COLORSPACE_RGB, true,
            8,
            frame.w, frame.h,
            frame.w * 4);
        output.push_back(pixbuf);
    }
    return output;
}


int main(int argc, char *argv[])
{
    auto app = App::create();
    return app->run(argc, argv);
}
