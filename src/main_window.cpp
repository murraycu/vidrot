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
#include <iostream>
#include <config.h>

MainWindow::MainWindow(const Glib::RefPtr<Gst::Pipeline>& pipeline) :
  m_vbox(false, 6),
  m_button_filechooser("Select a video to open", Gtk::FILE_CHOOSER_ACTION_OPEN),
  m_radio_clockwise(m_radiogroup, "Rotate 90° _clockwise", true),
  m_radio_anticlockwise(m_radiogroup, "Rotate 90° _anticlockwise", true),
  m_button_convert(Gtk::Stock::EXECUTE),
  m_button_quit(Gtk::Stock::QUIT),
  m_watch_id(0)
{
  set_title(PACKAGE_STRING);
  set_border_width(12);

  // Filter videos for FileChooserButton.
  Gtk::FileFilter filter_video;
  filter_video.set_name("Video files");
  filter_video.add_mime_type("video/*");
  m_button_filechooser.add_filter(filter_video);
  Gtk::FileFilter filter_any;
  filter_any.set_name(("All files"));
  filter_any.add_pattern("*");
  m_button_filechooser.add_filter(filter_any);

  // Attach signals to widgets.
  m_button_filechooser.signal_file_set().connect(sigc::mem_fun(*this, &MainWindow::on_file_selected));
  m_button_convert.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_convert));
  m_button_quit.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_quit));

  // Attach watcher to message bus.
  Glib::RefPtr<Gst::Bus> bus = pipeline->get_bus();
  m_watch_id = bus->add_watch(sigc::mem_fun(*this, &MainWindow::on_bus_message));

  // Create Bins for audio and video filtering.
  m_bin_video = Gst::Bin::create("video-bin");
  m_bin_audio = Gst::Bin::create("audio-bin");

  // Create Queue elements.
  m_queue_video = Gst::Queue::create("video-queue");
  m_queue_audio = Gst::Queue::create("audio-queue");

  // Create elements using ElementFactory.
  m_element_source = Gst::FileSrc::create("file-source");
  m_element_decode = Gst::ElementFactory::create_element("decodebin", "decode-element");
  m_element_filter = Gst::ElementFactory::create_element("videoflip", "filter-element");
  m_element_vidcomp = Gst::ElementFactory::create_element("mpeg2enc", "vidcomp-element");
  m_element_mux = Gst::ElementFactory::create_element("avimux", "mux-element");
  m_element_sink = Gst::FileSink::create("file-sink");

  // Add elements to pipeline (before linking together).
  pipeline->add(m_bin_video)->add(m_bin_audio)->add(m_element_source)->add(m_element_decode)->add(m_element_mux)->add(m_element_sink);
  m_bin_video->add(m_queue_video)->add(m_element_filter)->add(m_element_vidcomp);
  m_bin_audio->add(m_queue_audio);

  // Must link decode to filter after stream has been identified.
  // What happens if there is no audio stream?
  try
  {
    m_element_source->link(m_element_decode);
    m_element_decode->signal_pad_added().connect(sigc::mem_fun(*this, &MainWindow::on_decode_pad_added));
    m_queue_video->link(m_element_filter)->link(m_element_vidcomp);
    m_element_mux->link(m_element_sink);

    // Ghost pad setup for audio and video bins.
    Glib::RefPtr<Gst::Pad> bin_audio_sink = m_queue_audio->get_static_pad("sink");
    m_bin_audio->add_pad(Gst::GhostPad::create("audsink", bin_audio_sink));
    Glib::RefPtr<Gst::Pad> bin_audio_src = m_queue_audio->get_static_pad("src");
    m_bin_audio->add_pad(Gst::GhostPad::create("audsrc", bin_audio_src));
    Glib::RefPtr<Gst::Pad> bin_video_sink = m_queue_video->get_static_pad("sink");
    m_bin_video->add_pad(Gst::GhostPad::create("vidsink", bin_video_sink));
    Glib::RefPtr<Gst::Pad> bin_video_src = m_element_vidcomp->get_static_pad("src");
    m_bin_video->add_pad(Gst::GhostPad::create("vidsrc", bin_video_src));

    // Link bin src pads to AVI muxer.
    m_bin_video->link(m_element_mux);
    m_bin_audio->link(m_element_mux);
  }
  catch(const std::runtime_error& error)
  {
    std::cerr << "Exception while linking elements: " << error.what() << std::endl;
  }

  m_vbox.pack_start(m_button_filechooser);
  m_vbox.pack_start(m_radio_anticlockwise);
  m_vbox.pack_start(m_radio_clockwise);
  m_vbox.pack_start(m_button_convert);
  m_vbox.pack_start(m_button_quit);
  add(m_vbox);

  m_pipeline = pipeline;

  show_all_children();
}

MainWindow::~MainWindow()
{
  m_pipeline->get_bus()->remove_watch(m_watch_id);
}

void MainWindow::on_file_selected()
{
  // Set URI of filesrc and filesink elements.
  const std::string uri = m_button_filechooser.get_uri();
  m_element_source->set_uri(uri);
  // TODO: Write to same file.
  m_element_sink->set_uri(uri + ".new");
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

bool MainWindow::on_bus_message(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message)
{
  switch(message->get_message_type())
  {
    case Gst::MESSAGE_EOS:
      std::clog << "End of stream" << std::endl;
      m_pipeline->set_state(Gst::STATE_NULL);
      m_button_filechooser.set_sensitive();
      m_radio_clockwise.set_sensitive();
      m_radio_anticlockwise.set_sensitive();
      m_button_convert.set_sensitive();
      m_button_quit.set_sensitive();
      break;
    case Gst::MESSAGE_ERROR:
      {
        Glib::RefPtr<Gst::MessageError> message_error = Glib::RefPtr<Gst::MessageError>::cast_dynamic(message);
        if(message_error)
        {
          Glib::Error err = message_error->parse();
          std::cerr << "Error: " << err.what() << std::endl;
        }
        else
        {
          std::cerr << "Undefined error." << std::endl;
        }
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
            m_button_convert.set_sensitive(false);
            m_button_quit.set_sensitive(false);
          }
        }
        else
        {
          std::cerr << "Undefined state change." << std::endl;
        }
        break;
      }
    default:
      std::cerr << "Unhandled message on bus." << std::endl;
      break;
  }

  return true;
}

void MainWindow::on_decode_pad_added(const Glib::RefPtr<Gst::Pad>& new_pad)
{
  // Acquire sink pad from videoflip element.
  Glib::RefPtr<Gst::Pad> sink_pad = m_element_filter->get_static_pad("sink");

  // Link decodebin source to videoflip sink.
  try
  {
    new_pad->link(sink_pad);
  }
  catch(const std::runtime_error& err)
  {
    std::cerr << "Exception caught while linking added pad: " << err.what() << std::endl;
  }
}
