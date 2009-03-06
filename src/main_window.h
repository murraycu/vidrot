/* VidRot is copyright David King, 2009
 *
 * This file is part of VidRot
 *
 * VidRot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VidRot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VidRot. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <gtkmm.h>
#include <gstreamermm.h>
#include <gstreamermm/filesrc.h>
#include <gstreamermm/filesink.h>
#include <iostream>
#include <config.h>

#define VIDROT_MAINWINDOW_UI "main_window_uimanager.ui"

class MainWindow : public Gtk::Window
{
  public:
    explicit MainWindow(const Glib::RefPtr<Gst::Pipeline>& pipeline);
    virtual ~MainWindow();

  private:
    // Signal handlers
    void on_file_selected();
    void on_button_convert();
    void on_button_quit();
    bool on_bus_message(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message);
    void on_decode_pad_added(const Glib::RefPtr<Gst::Pad>& new_pad);

    // Widgets
    Gtk::VBox m_vbox;
    Gtk::FileChooserButton m_button_filechooser;
    Gtk::RadioButtonGroup m_radiogroup;
    Gtk::RadioButton m_radio_clockwise;
    Gtk::RadioButton m_radio_anticlockwise;
    Gtk::Button m_button_convert;
    Gtk::Button m_button_quit;

    // Variables
    Glib::RefPtr<Gst::Pipeline> m_pipeline;
    Glib::RefPtr<Gst::FileSrc> m_element_source;
    Glib::RefPtr<Gst::Element> m_element_decode;
    Glib::RefPtr<Gst::Element> m_element_filter;
    Glib::RefPtr<Gst::FileSink> m_element_sink;
    guint m_watch_id;
};

#endif /* _MAINWINDOW_H */
