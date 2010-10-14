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

#include <gdkmm.h>
#include <cstring>
#include "vidrot_preview.h"

VidRotPreview::VidRotPreview() :
  Gtk::Widget(),
  m_video_width(0),
  m_video_height(0)
{
  set_has_window(false);

#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  signal_realize().connect(
    sigc::mem_fun(*this, &VidRotPreview::on_realize) );
  signal_unrealize().connect(
    sigc::mem_fun(*this, &VidRotPreview::on_unrealize) );
#endif //GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
}

VidRotPreview::~VidRotPreview()
{
}

void VidRotPreview::set_aspect_ratio(unsigned int width, unsigned int height)
{
  m_video_width = width;
  m_video_height = height;
}

float VidRotPreview::get_aspect_ratio()
{
  if(m_video_height > 0)
  {
    return static_cast<float>(m_video_width) / m_video_height;
  }
  else
  {
    return 0.0f;
  }
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
  if(!requisition)
    return;

  *requisition = Gtk::Requisition();

  requisition->height = -1;
  requisition->width = -1;
}

void VidRotPreview::on_realize()
{
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  Gtk::Widget::on_realize();
#endif //GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED

  if(m_gdkwindow)
    return;

  GdkWindowAttr attributes;
  memset(&attributes, 0, sizeof(attributes));

  const Gtk::Allocation allocation = get_allocation();

  attributes.x = allocation.get_x();
  attributes.y = allocation.get_y();
  attributes.width = allocation.get_width();
  attributes.height = allocation.get_height();
  attributes.event_mask = get_events() | Gdk::EXPOSURE_MASK;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.wclass = GDK_INPUT_OUTPUT;

  m_gdkwindow = Gdk::Window::create(get_window(), &attributes, GDK_WA_X | GDK_WA_Y);
  set_has_window();
  set_window(m_gdkwindow);
  m_gdkwindow->set_user_data(gobj());
}

void VidRotPreview::on_unrealize()
{
  m_gdkwindow->clear();

#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  Gtk::Widget::on_unrealize();
#endif //GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
}

bool VidRotPreview::on_expose_event(GdkEventExpose* /* event */)
{
  if(m_gdkwindow)
  {
    Cairo::RefPtr<Cairo::Context> context = m_gdkwindow->create_cairo_context();
    context->paint();

    if(get_aspect_ratio() > 0.0f)
    {
      const Gtk::Allocation allocation = get_allocation();
      float ratio = 1.0f;

      if(m_video_width > 0 && m_video_height > 0)
      {
        if(static_cast<float>(allocation.get_width()) / m_video_width > static_cast<float>(allocation.get_height()) / m_video_height)
        {
          ratio = static_cast<float>(allocation.get_height()) / m_video_height;
        }
        else
        {
          ratio = static_cast<float>(allocation.get_width()) / m_video_width;
        }

        unsigned int draw_width = m_video_width * ratio;
        unsigned int draw_height = m_video_height * ratio;
        //unsigned int x = (allocation.get_width() - draw_width) / 2;
        //unsigned int y = (allocation.get_height() - draw_height) / 2;

        // TODO: Layout preview corectly.
        //m_gdkwindow->move_resize(x, y, draw_width, draw_height);
        m_gdkwindow->move_resize(allocation.get_x(), allocation.get_y(), draw_width, draw_height);

      }
      else
      {
        return true;
      }

    }
  }

  return true;
}
