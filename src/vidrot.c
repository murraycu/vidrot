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

#include <gst/gst.h>
#include "config.h"

#define VIDROT_MAINWINDOW_UI "main_window_uimanager.ui"

int main (int argc, char *argv[])
{
  GMainLoop *main_loop;

  gst_init (&argc, &argv);
  main_loop = g_main_loop_new (NULL, FALSE);

  if (argc !=2)
  {
    g_print ("Incorrect number of arguments\n");
    return 1;
  }

  g_main_loop_run (main_loop);

  return 0;
}
