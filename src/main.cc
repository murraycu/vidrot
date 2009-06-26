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


class OptionGroup : public Glib::OptionGroup
{ 
public:
  OptionGroup();

  std::string m_arg_filename;
  bool m_arg_version;
};

OptionGroup::OptionGroup()
: Glib::OptionGroup("vidrot", _("Vidrot options"), _("Command-line options for vidrot")),
  m_arg_version(false)
{
  Glib::OptionEntry entry;
  entry.set_long_name("file");
  entry.set_short_name('f');
  entry.set_description(_("The Filename"));
  add_entry_filename(entry, m_arg_filename);

  Glib::OptionEntry entry_version;
  entry_version.set_long_name("version");
  entry_version.set_short_name('V');
  entry_version.set_description(_("The version of this application."));
  add_entry(entry_version, m_arg_version);
}

int main(int argc, char *argv[])
{
  // gettext initialisation.
  bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);

  //Initialize gtkmm and gstreamermm:
  Glib::OptionContext context;
  OptionGroup group;
  context.set_main_group(group);
  Gtk::Main kit(argc, argv, context);
  Gst::init(argc, argv);
  gst_pb_utils_init();


  //Process command-line options:
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try
#else
  std::auto_ptr<Glib::Error> error;
#endif // GLIBMM_EXCEPTIONS_ENABLED
  {
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    context.parse(argc, argv);
#else
    context.parse(argc, argv, error);
#endif // GLIBMM_EXCEPTIONS_ENABLED
  }
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  catch(const Glib::OptionError& ex)
#else
  if(error.get() != NULL)
#endif
  {
#ifndef GLIBMM_EXCEPTIONS_ENABLED
    const Glib::OptionError* exptr = dynamic_cast<Glib::OptionError*>(error.get());
    if(exptr)
    {
      const Glib::OptionError& ex = *exptr;
#endif // !GLIBMM_EXCEPTIONS_ENABLED
      std::cout << _("Error while parsing command-line options: ") << std::endl << ex.what() << std::endl;
      std::cout << _("Use --help to see a list of available command-line options.") << std::endl;
      return 0;
#ifndef GLIBMM_EXCEPTIONS_ENABLED
    }
    const Glib::Error& ex = *error.get();
#else
  }
  catch(const Glib::Error& ex)
  {
#endif
    std::cout << "Error: " << ex.what() << std::endl;
    return 0;
  }


  //The --version command-line option:
  if(group.m_arg_version)
  {
    std::cout << VERSION << std::endl;
    return 0;
  }


  //The --file parameter, if any:
  Glib::ustring input_uri = group.m_arg_filename;

  // The GOption documentation says that options without names will be returned to the application as "rest arguments".
  // I guess this means they will be left in the argv. murrayc
  if(input_uri.empty() && (argc > 1))
  {
    const char* pch = argv[1];
    if(pch)
      input_uri = pch; //Actually a filepath, not a URI.
  }

  if(!input_uri.empty())
  {
    //Get a URI (file://something) from the filepath:
    Glib::RefPtr<Gio::File> file = Gio::File::create_for_commandline_arg(input_uri);
    if(file)
      input_uri = file->get_uri(); 
    //std::cout << "URI = " << input_uri << std::endl;
  }


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
  main_window.set_file_uri(input_uri);
  Gtk::Main::run(main_window);

  // Clean up the pipeline.
  pipeline->set_state(Gst::STATE_NULL);

  return 0;
}
