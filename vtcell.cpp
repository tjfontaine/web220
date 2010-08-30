#include "vtcell.h"
#include "VTermMM.h"

VTCell::VTCell() :
  m_x(0),
  m_y(0),
  m_bold(false),
  m_italic(false),
  m_value(" ")
{
  m_foreground = VTERMMM_WHITE;
  m_background = VTERMMM_BLACK;
}

VTCell::VTCell(const VTCell& c)
{
  m_foreground = c.m_foreground;
  m_background = c.m_background;
  m_value = c.m_value;
  m_bold = c.m_bold;
  m_italic = c.m_italic;
  m_x = c.m_x;
  m_y = c.m_y;
}

int VTCell::GetX()
{
  return m_x;
}

void VTCell::SetX(int x)
{
  m_x = x;
}

int VTCell::GetY()
{
  return m_y;
}

void VTCell::SetY(int y)
{
  m_y = y;
}

const std::string& VTCell::GetValue()
{
  return m_value;
}

void VTCell::SetValue(const std::string& value)
{
  m_value = value;
}

bool VTCell::GetBold()
{
  return m_bold;
}

void VTCell::SetBold(bool bold)
{
  m_bold = bold;
}

bool VTCell::GetItalic()
{
  return m_italic;
}

void VTCell::SetItalic(bool italic)
{
  m_italic = italic;
}

const VTermColor& VTCell::GetForeground()
{
  return m_foreground;
}

void VTCell::SetForeground(const VTermColor& foreground)
{
  m_foreground = foreground;
}

const VTermColor& VTCell::GetBackground()
{
  return m_background;
}

void VTCell::SetBackground(const VTermColor& background)
{
  m_background = background;
}

std::ostream& operator<< (std::ostream& out, const VTCell& c)
{
  out << "[" << c.m_x << "," << c.m_y << "] ";
  out << "(" << c.m_value << ") ";
  out << "B:" << c.m_bold << " ";
  out << "I:" << c.m_italic << " ";
  return out;
}
