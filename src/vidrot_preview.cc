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

#include <gdkmm.h>
#include <cstring>
#include "vidrot_preview.h"

VidRotPreview::VidRotPreview() :
  Glib::ObjectBase("vidrot_preview"),
  Gtk::Widget()
{
  set_flags(Gtk::NO_WINDOW);
}

VidRotPreview::~VidRotPreview()
{
}

void VidRotPreview::on_size_allocate(Gtk::Allocation& allocation)
{
  set_allocation(allocation);

  if(m_gdkwindow)
  {
    m_gdkwindow->move_resize(allocation.get_x(), allocation.get_y(), allocation.get_width(), allocation.get_height());
  }
}

void VidRotPreview::on_size_request(Gtk::Requisition* requisition)
{
  *requisition = Gtk::Requisition();

  requisition->height = -1;
  requisition->width = -1;
}

void VidRotPreview::on_realize()
{
  Gtk::Widget::on_realize();

  if(!m_gdkwindow)
  {
    GdkWindowAttr attributes;
    memset(&attributes, 0, sizeof(attributes));

    Gtk::Allocation allocation = get_allocation();

    attributes.x = allocation.get_x();
    attributes.y = allocation.get_y();
    attributes.width = allocation.get_width();
    attributes.height = allocation.get_height();
    attributes.event_mask = get_events() | Gdk::EXPOSURE_MASK;
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.wclass = GDK_INPUT_OUTPUT;

    m_gdkwindow = Gdk::Window::create(get_window(), &attributes, GDK_WA_X | GDK_WA_Y);
    unset_flags(Gtk::NO_WINDOW);
    set_window(m_gdkwindow);
    m_gdkwindow->set_user_data(gobj());
  }
}

void VidRotPreview::on_unrealize()
{
  m_gdkwindow->clear();
  Gtk::Widget::on_unrealize();
}

bool VidRotPreview::on_expose_event(GdkEventExpose* event)
{
  if(m_gdkwindow)
  {
    Cairo::RefPtr<Cairo::Context> context = m_gdkwindow->create_cairo_context();
    context->paint();
  }

  return true;
}
