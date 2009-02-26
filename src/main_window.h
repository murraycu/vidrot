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
#include <iostream>
#include "config.h"

#define VIDROT_MAINWINDOW_UI "main_window_uimanager.ui"

class MainWindow : public Gtk::Window
{
  public:
    MainWindow();
    virtual ~MainWindow();

  protected:
    void on_button_quit();
    void on_file_selected();

    Gtk::VBox m_vbox;
    Gtk::FileChooserButton m_button_filechooser;
    Gtk::Button m_button_quit;
};

#endif /* _MAINWINDOW_H */
