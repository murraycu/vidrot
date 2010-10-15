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
#include <iostream>

VidRotPreview::VidRotPreview()
: m_video_width(0),
  m_video_height(0)
{
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

float VidRotPreview::get_aspect_ratio() const
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

  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
    window->move_resize(allocation.get_x(), allocation.get_y(), allocation.get_width(), allocation.get_height());
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

bool VidRotPreview::on_expose_event(GdkEventExpose* /* event */)
{
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(!window)
    return true;
    
  if(get_aspect_ratio() <= 0.0f)
    return true;

  if(!(m_video_width > 0 && m_video_height > 0))
    return true;
        
  Cairo::RefPtr<Cairo::Context> context = window->create_cairo_context();
  context->paint();

  const Gtk::Allocation allocation = get_allocation();
  float ratio = 1.0f;

  if(static_cast<float>(allocation.get_width()) / m_video_width > static_cast<float>(allocation.get_height()) / m_video_height)
  {
    ratio = static_cast<float>(allocation.get_height()) / m_video_height;
  }
  else
  {
    ratio = static_cast<float>(allocation.get_width()) / m_video_width;
  }

  const unsigned int draw_width = m_video_width * ratio;
  const unsigned int draw_height = m_video_height * ratio;
  //unsigned int x = (allocation.get_width() - draw_width) / 2;
  //unsigned int y = (allocation.get_height() - draw_height) / 2;

  // TODO: Layout preview corectly.
  //window->move_resize(x, y, draw_width, draw_height);
  window->move_resize(allocation.get_x(), allocation.get_y(), draw_width, draw_height);

  return true;
}
