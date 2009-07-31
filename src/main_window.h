/* VidRot is copyright Openismus GmbH, 2009
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
#include <gstreamermm/bin.h>
#include <gstreamermm/filesrc.h>
#include <gstreamermm/filesink.h>
#include <gstreamermm/message.h>
#include <gstreamermm/queue.h>
#include <gstreamermm/tee.h>
#include <gstreamermm/typefindelement.h>
#include <gstreamermm/videoscale.h>
#include <gstreamermm/ximagesink.h>
#include <iostream>
#include <cstdlib>
#include "vidrot_preview.h"
#include <config.h>

class MainWindow : public Gtk::Window
{
  public:
    explicit MainWindow(const Glib::RefPtr<Gst::Pipeline>& pipeline);
    virtual ~MainWindow();

    void set_file_uri(const Glib::ustring& file_uri);

  private:
    void create_elements();
    void link_elements();
    void setup_widgets();
    void respond_to_file_selection(const Glib::ustring& uri);
    void update_widget_sensitivity(bool processing, bool have_uri);
    void offer_finished_file(const Glib::ustring& file_uri);

    // Signal handlers.
    static void on_c_signal_file_selected(GtkFileChooserButton* button, void* user_data);
    void on_file_selected();
    void on_button_convert();
    void on_button_stop();
    void on_button_quit();
    bool on_bus_message(const Glib::RefPtr<Gst::Bus>& bus,
      const Glib::RefPtr<Gst::Message>& message);
    void on_decode_pad_added(const Glib::RefPtr<Gst::Pad>& new_pad);
    void on_no_more_pads();
    bool on_convert_timeout();

    // Widgets.
    Gtk::VBox m_vbox;
    Gtk::HButtonBox m_hbuttonbox;
    Gtk::FileChooserButton m_button_filechooser;
    VidRotPreview m_video_area;
    Gtk::RadioButtonGroup m_radiogroup;
    Gtk::RadioButton m_radio_clockwise;
    Gtk::RadioButton m_radio_anticlockwise;
    Gtk::ProgressBar m_progress_convert;
    Gtk::Button m_button_convert;
    Gtk::Button m_button_stop;
    Gtk::Button m_button_quit;

    // gstreamermm Variables.
    Glib::RefPtr<Gst::Pipeline> m_pipeline;
    Glib::RefPtr<Gst::Bin> m_bin_video;
    Glib::RefPtr<Gst::Bin> m_bin_audio;
    Glib::RefPtr<Gst::Queue> m_queue_video;
    Glib::RefPtr<Gst::Queue> m_queue_audio;
    Glib::RefPtr<Gst::Element> m_element_source;
    Glib::RefPtr<Gst::Element> m_element_colorspace;
    Glib::RefPtr<Gst::Element> m_element_audconvert;
    Glib::RefPtr<Gst::Element> m_element_filter;
    Glib::RefPtr<Gst::Element> m_element_vidrate;
    Glib::RefPtr<Gst::Element> m_element_audcomp;
    Glib::RefPtr<Gst::Element> m_element_vidcomp;
    Glib::RefPtr<Gst::Element> m_element_mux;
    Glib::RefPtr<Gst::FileSink> m_element_sink;
    guint m_watch_id;

    // Other internal state.
    sigc::connection m_timeout_connection;
    Glib::Timer time_remaining;
};

#endif /* _MAINWINDOW_H */
