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

#include <gtkmm.h>
#include <gstreamermm.h>
#include <iostream>
#include <config.h>
#include "main_window.h"

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

  // Create elements using ElementFactory.
  m_element_source = Gst::FileSrc::create("file-source");
  m_element_filter = Gst::VideoScale::create("filter-element");
  m_element_sink = Gst::FileSink::create("file-sink");

  // Add elements to pipeline (before linking together).
  pipeline->add(m_element_source)->add(m_element_filter)->add(m_element_sink);

  // Link elements together.
  try
  {
    m_element_source->link(m_element_filter)->link(m_element_sink);
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
  const std::string uri = m_button_filechooser.get_uri();
  std::cout << "URI " << uri << " selected" <<std::endl;
  m_element_source->set_uri(uri);
  m_element_sink->set_uri(uri);
}

void MainWindow::on_button_convert()
{
  std::cout << std::boolalpha << "Convert clicked with rotation clockwise " << m_radio_clockwise.get_active() << std::endl;
}

void MainWindow::on_button_quit()
{
  std::cout << "Quit button clicked" << std::endl;

  this->hide();

  return;
}

bool MainWindow::on_bus_message(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message)
{
  switch(message->get_message_type())
  {
    case Gst::MESSAGE_EOS:
      std::cout << "End of stream" << std::endl;
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
    default:
      std::cout << "Unhandled message on bus." << std::endl;
      break;
  }

  return true;
}
