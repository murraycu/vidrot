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

#ifndef _VIDROT_PREVIEW_H
#define _VIDROT_PREVIEW_H

#include <gtkmm.h>

class VidRotPreview : public Gtk::Widget
{
  public:
    VidRotPreview();
    virtual ~VidRotPreview();

    void set_aspect_ratio(unsigned int width, unsigned int height);
    float get_aspect_ratio();

  private:
    virtual void on_size_allocate(Gtk::Allocation& allocation);
    virtual void on_size_request(Gtk::Requisition* requisition);
    virtual void on_realize();
    virtual void on_unrealize();
    virtual bool on_expose_event(GdkEventExpose* event);

    Glib::RefPtr<Gdk::Window> m_gdkwindow;

    unsigned int m_video_width;
    unsigned int m_video_height;
};

#endif /* _VIDROT_PREVIEW_H */
