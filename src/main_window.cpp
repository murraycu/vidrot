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
#include "main_window.h"
#include "config.h"

MainWindow::MainWindow() :
  m_vbox(false, 6),
  m_button_filechooser("Select a video to open", Gtk::FILE_CHOOSER_ACTION_OPEN),
  m_button_quit(Gtk::Stock::QUIT)
{
  set_title(PACKAGE_STRING);
  set_border_width(12);

  m_button_quit.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_quit));
  m_button_filechooser.signal_file_set().connect(sigc::mem_fun(*this, &MainWindow::on_file_selected));

  m_vbox.pack_start(m_button_filechooser);
  m_vbox.pack_start(m_button_quit);
  add(m_vbox);
  show_all_children();
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_button_quit()
{
  std::cout << "Quit button clicked" << std::endl;

  this->hide();

  return;
}

void MainWindow::on_file_selected()
{
  std::cout << "File " << m_button_filechooser.get_filename() << " selected" <<std::endl;
}
