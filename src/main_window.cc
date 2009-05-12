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

#include "main_window.h"
#include <gtkmm.h>
#include <gstreamermm.h>
#include <gdk/gdkx.h>
#include <glibmm/i18n.h>
#include <iostream>
#include <config.h>

MainWindow::MainWindow(const Glib::RefPtr<Gst::Pipeline>& pipeline) :
  m_vbox(false, 6),
  m_hbuttonbox(Gtk::BUTTONBOX_END, 6),
  m_button_filechooser(_("Select a video to open"), Gtk::FILE_CHOOSER_ACTION_OPEN),
  m_radio_clockwise(m_radiogroup, _("Rotate 90° _clockwise"), true),
  m_radio_anticlockwise(m_radiogroup, _("Rotate 90° _anticlockwise"), true),
  // m_progress_convert has no arguments for default constructor.
  m_button_convert(Gtk::Stock::EXECUTE),
  m_button_quit(Gtk::Stock::QUIT),
  m_watch_id(0)
  // time_remaining has no arguments for default constructor.
{
  m_pipeline = pipeline;

  set_title(PACKAGE_STRING);
  set_border_width(12);

  // Call helper functions to setup Gstreamermm pipeline and Gtkmm widgets.
  create_elements();
  link_elements();
  setup_widgets();

  set_default_size(200, 300);
  show_all_children();
}

MainWindow::~MainWindow()
{
  m_pipeline->get_bus()->remove_watch(m_watch_id);
  m_pipeline->set_state(Gst::STATE_NULL);
}

// Create Gstreamermm elements and add to pipeline or bin.
void MainWindow::create_elements()
{
  // Attach watcher to message bus.
  Glib::RefPtr<Gst::Bus> bus = m_pipeline->get_bus();
  m_watch_id = bus->add_watch(sigc::mem_fun(*this, &MainWindow::on_bus_message));

  // Create Bins for audio and video filtering.
  m_bin_video = Gst::Bin::create("video-bin");
  g_assert(m_bin_video);
  m_pipeline->add(m_bin_video);
  m_bin_audio = Gst::Bin::create("audio-bin");
  g_assert(m_bin_audio);
  m_pipeline->add(m_bin_audio);

  // Create Queue elements.
  m_queue_video = Gst::Queue::create("video-queue");
  g_assert(m_queue_video);
  m_bin_video->add(m_queue_video);
  m_queue_audio = Gst::Queue::create("audio-queue");
  g_assert(m_queue_audio);
  m_bin_audio->add(m_queue_audio);

  // Create elements using ElementFactory.
  m_element_source = Gst::ElementFactory::create_element("uridecodebin", "uri-source");
  g_assert(m_element_source);
  m_pipeline->add(m_element_source);
  m_element_colorspace = Gst::ElementFactory::create_element("ffmpegcolorspace", "vid-colorspace");
  g_assert(m_element_colorspace);
  m_bin_video->add(m_element_colorspace);
  m_element_audconvert = Gst::ElementFactory::create_element("audioconvert", "aud-convert");
  g_assert(m_element_audconvert);
  m_bin_audio->add(m_element_audconvert);
  m_element_audcomp = Gst::ElementFactory::create_element("lame", "audcomp-element");
  g_assert(m_element_audcomp);
  m_bin_audio->add(m_element_audcomp);
  m_element_filter = Gst::ElementFactory::create_element("videoflip", "filter-element");
  g_assert(m_element_filter);
  m_bin_video->add(m_element_filter);
  m_element_vidrate = Gst::ElementFactory::create_element("videorate", "vidrate");
  g_assert(m_element_vidrate);
  m_bin_video->add(m_element_vidrate);
  m_element_vidcomp = Gst::ElementFactory::create_element("mpeg2enc", "vidcomp-element");
  g_assert(m_element_vidcomp);
  m_bin_video->add(m_element_vidcomp);
  m_element_mux = Gst::ElementFactory::create_element("avimux", "mux-element");
  g_assert(m_element_mux);
  m_pipeline->add(m_element_mux);
  m_element_sink = Gst::FileSink::create("file-sink");
  g_assert(m_element_sink);
  m_pipeline->add(m_element_sink);
}

// Link pipeline elements together.
void MainWindow::link_elements()
{
  // Dynamically link uridecodebin to audio and video processing bins.
  m_element_source->signal_pad_added().connect(sigc::mem_fun(*this, &MainWindow::on_decode_pad_added));
  m_element_source->signal_no_more_pads().connect(sigc::mem_fun(*this, &MainWindow::on_no_more_pads));

  // Must link decode to filter after stream has been identified.
  // What happens if there is no audio stream?
  m_element_colorspace->link(m_element_filter)->link(m_element_vidrate)->link(m_element_vidcomp)->link(m_queue_video);
  m_element_audconvert->link(m_element_audcomp)->link(m_queue_audio);

  // Ghost pad setup for audio and video bins.
  Glib::RefPtr<Gst::Pad> bin_audio_sink = m_element_audconvert->get_static_pad("sink");
  m_bin_audio->add_pad(Gst::GhostPad::create("audsink", bin_audio_sink));
  Glib::RefPtr<Gst::Pad> bin_audio_src = m_queue_audio->get_static_pad("src");
  m_bin_audio->add_pad(Gst::GhostPad::create("audsrc", bin_audio_src));
  Glib::RefPtr<Gst::Pad> bin_video_sink = m_element_colorspace->get_static_pad("sink");
  m_bin_video->add_pad(Gst::GhostPad::create("vidsink", bin_video_sink));
  Glib::RefPtr<Gst::Pad> bin_video_src = m_queue_video->get_static_pad("src");
  m_bin_video->add_pad(Gst::GhostPad::create("vidsrc", bin_video_src));

  // Link bin src pads to AVI muxer.
  m_bin_video->link(m_element_mux);
  m_bin_audio->link(m_element_mux);
  m_element_mux->link(m_element_sink);
}

void MainWindow::setup_widgets()
{
  // Cannot convert if a file is not selected.
  m_button_convert.set_sensitive(false);

  // Filter videos for FileChooserButton.
  Gtk::FileFilter filter_video;
  filter_video.set_name(_("Video files"));
  filter_video.add_mime_type("video/*");
  m_button_filechooser.add_filter(filter_video);
  Gtk::FileFilter filter_any;
  filter_any.set_name(_("All files"));
  filter_any.add_pattern("*");
  m_button_filechooser.add_filter(filter_any);
  m_button_filechooser.set_current_folder(Glib::get_user_special_dir(G_USER_DIRECTORY_VIDEOS));

  // Attach signals to widgets.
  m_button_filechooser.signal_file_set().connect(sigc::mem_fun(*this, &MainWindow::on_file_selected));
  m_button_convert.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_convert));
  m_button_quit.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_quit));

  // Set tooltips.
  m_button_filechooser.set_tooltip_text(_("Select a video to rotate"));
  m_radio_anticlockwise.set_tooltip_text(_("Rotate the video anticlockwise by 90°"));
  m_radio_clockwise.set_tooltip_text(_("Rotate the video clockwise by 90°"));
  m_progress_convert.set_tooltip_text(_("Progress of conversion"));
  m_button_convert.set_tooltip_text(_("Begin conversion"));
  m_button_quit.set_tooltip_text("Quit " PACKAGE_NAME);

  // Pack widgets into vbox.
  m_vbox.pack_start(m_button_filechooser, Gtk::PACK_SHRINK);
  m_vbox.pack_start(m_video_area, Gtk::PACK_EXPAND_WIDGET);
  m_vbox.pack_start(m_radio_anticlockwise, Gtk::PACK_SHRINK);
  m_vbox.pack_start(m_radio_clockwise, Gtk::PACK_SHRINK);
  m_vbox.pack_start(m_progress_convert, Gtk::PACK_SHRINK);
  m_hbuttonbox.pack_start(m_button_convert);
  m_hbuttonbox.pack_start(m_button_quit);
  m_vbox.pack_start(m_hbuttonbox, Gtk::PACK_SHRINK);

  add(m_vbox);
}

void MainWindow::on_file_selected()
{
  const std::string uri = m_button_filechooser.get_uri();

  // Set URI of uridecoder and filesink elements.
  m_element_source->set_state(Gst::STATE_NULL);
  m_element_source->set_property("uri", uri);
  m_element_source->set_state(Gst::STATE_PAUSED);

  // TODO: Write to same file.
  m_element_sink->set_uri(uri + ".new");
  m_button_convert.set_sensitive();
  m_progress_convert.set_fraction(0.0);

  // Set preview size to 0, add probe so that first buffer sets preview size.
  m_video_area.set_size_request(0, 0);
}

void MainWindow::on_button_convert()
{
  // Set videoflip method based on radio button state.
  // TODO: Use GstVideoFlipMethod enumeration.
  m_element_filter->set_property("method", m_radio_clockwise.get_active() ? 1 : 3);

  // Begin conversion process (play stream).
  m_pipeline->set_state(Gst::STATE_PLAYING);
}

void MainWindow::on_button_quit()
{
  this->hide();

  return;
}

// Process asynchronous bus messages.
bool MainWindow::on_bus_message(const Glib::RefPtr<Gst::Bus>& /* bus */, const Glib::RefPtr<Gst::Message>& message)
{
  switch(message->get_message_type())
  {
    case Gst::MESSAGE_EOS:
      std::clog << "End of stream" << std::endl;
      m_pipeline->set_state(Gst::STATE_NULL);
      m_button_filechooser.set_sensitive();
      m_radio_clockwise.set_sensitive();
      m_radio_anticlockwise.set_sensitive();
      m_progress_convert.set_fraction(1.0);
      m_progress_convert.set_text(_("Conversion complete!"));
      m_button_convert.set_sensitive();
      m_button_quit.set_sensitive();
      m_timeout_connection.disconnect();
      time_remaining.stop();
      break;
    case Gst::MESSAGE_ERROR:
      {
        Glib::RefPtr<Gst::MessageError> message_error = Glib::RefPtr<Gst::MessageError>::cast_dynamic(message);
        if(message_error)
        {
          Glib::Error err = message_error->parse();
          std::cerr << _("Error: ") << err.what() << std::endl;
        }
        else
        {
          std::cerr << _("Undefined error.") << std::endl;
        }
        m_progress_convert.set_text(_("Conversion error"));
        break;
      }
    case Gst::MESSAGE_STATE_CHANGED:
      {
        // Set UI to be insensitive during conversion.
        Glib::RefPtr<Gst::MessageStateChanged> message_statechange = Glib::RefPtr<Gst::MessageStateChanged>::cast_dynamic(message);
        if(message_statechange)
        {
          if(message_statechange->parse() == Gst::STATE_PLAYING)
          {
            m_button_filechooser.set_sensitive(false);
            m_radio_clockwise.set_sensitive(false);
            m_radio_anticlockwise.set_sensitive(false);
            m_progress_convert.set_text(_("Conversion progress"));
            m_button_convert.set_sensitive(false);
            m_button_quit.set_sensitive(false);
            m_timeout_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &MainWindow::on_convert_timeout), 200);

            // Start timer to estimate remaining conversion time.
            time_remaining.start();
          }
        }
        else
        {
          std::cerr << _("Unexpected state change: ") << message_statechange->parse() << std::endl;
        }
        break;
      }
    case Gst::MESSAGE_WARNING:
      {
        // For instance: "No decoder available for type ..."
        // TODO: Show with a dialog.
        // TODO: Eventually use the encoder/decoder installer helper thingy used by totem and others.
        Glib::RefPtr<Gst::MessageWarning> message_warning = Glib::RefPtr<Gst::MessageWarning>::cast_dynamic(message);
        if(message_warning)
        {
          const Glib::Error error = message_warning->parse();
          std::cout << _("Gstreamer Warning: ") << error.what() << std::endl;
          return false; //These warnings seem to be errors.
        }
        break;
      }
    default:
      std::cerr << _("Unhandled message on bus: ") << Gst::Enums::get_name(message->get_message_type()) << std::endl;
      break;
  }

  return true;
}

void MainWindow::on_decode_pad_added(const Glib::RefPtr<Gst::Pad>& new_pad)
{
  // Check whether dynamic pad has audio or video caps.
  Glib::RefPtr<const Gst::Caps> new_caps = new_pad->get_caps();
  const Glib::ustring caps_string = new_caps->to_string();
  const Glib::ustring::size_type caps_audio = caps_string.find("audio/");
  const Glib::ustring::size_type caps_video = caps_string.find("video/");
  if(caps_audio == Glib::ustring::npos && caps_video == Glib::ustring::npos)
  {
    // No video or audio caps on pad.
    std::cerr << _("Not able to link dynamic non-audio or video pad.") << std::endl;
  }
  else if(caps_audio != Glib::ustring::npos && caps_video == Glib::ustring::npos)
  {
    // Audio caps found.
    try
    {
      Glib::RefPtr<Gst::Pad> sink_pad = m_bin_audio->get_static_pad("audsink");
      new_pad->link(sink_pad);
      m_bin_audio->set_state(Gst::STATE_PAUSED);
    }
    catch(const std::runtime_error& err)
    {
      std::cerr << _("Exception caught while linking added pad: ") << err.what() << std::endl;
    }
  }
  else if(caps_audio == Glib::ustring::npos && caps_video != Glib::ustring::npos)
  {
    // Video caps found.
    try
    {
      // Link decodebin source to videoflip sink.
      // Acquire sink pad from videoflip element.
      Glib::RefPtr<Gst::Pad> sink_pad = m_bin_video->get_static_pad("vidsink");
      new_pad->link(sink_pad);
      m_bin_video->set_state(Gst::STATE_PAUSED);
    }
    catch(const std::runtime_error& err)
    {
      std::cerr << _("Exception caught while linking added pad: ") << err.what() << std::endl;
    }
  }
  else
  {
    // Audio and video caps found?
    std::cerr << _("Invalid caps on dynamic pad") << std::endl;
  }
}

// Move elements to PAUSED state as soon as possible.
void MainWindow::on_no_more_pads()
{
  m_element_sink->set_state(Gst::STATE_PAUSED);
  m_element_mux->set_state(Gst::STATE_PAUSED);
}

// Update progress bar every 200 ms.
bool MainWindow::on_convert_timeout()
{
  Gst::Format format = Gst::FORMAT_TIME;
  gint64 position = 0;
  gint64 duration = 0;

  if(m_pipeline->query_position(format, position) && m_pipeline->query_duration(format, duration))
  {
    double fraction = static_cast<double>(position) / duration;
    m_progress_convert.set_fraction(fraction);
    int seconds_remaining = time_remaining.elapsed() / fraction;
    Glib::ustring conversion_status = Glib::ustring::compose("Time remaining: %1:%2", seconds_remaining / 60, seconds_remaining % 60);
    m_progress_convert.set_text(conversion_status);
  }

  return true;
}
