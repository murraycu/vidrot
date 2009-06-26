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
#include <gst/pbutils/pbutils.h>
#include <iostream>
#include <glibmm/i18n.h>
#include <config.h>

int main(int argc, char *argv[])
{
  // gettext initialisation.
  bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);

  Gtk::Main kit(argc, argv);
  Gst::init(argc, argv);
  gst_pb_utils_init();

  // Pipeline setup. Abort if pipeline could not be created.
  Glib::RefPtr<Gst::Pipeline> pipeline = Gst::Pipeline::create("flippipe");

  if(!pipeline)
  {
    std::cerr << _("Pipeline could not be created.") << std::endl;
    return 1;
  }
  else
  {
    pipeline->set_state(Gst::STATE_PAUSED);
  }

  MainWindow main_window(pipeline);

  Gtk::Main::run(main_window);

  // Clean up the pipeline.
  pipeline->set_state(Gst::STATE_NULL);

  return 0;
}
