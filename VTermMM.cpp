#include "VTermMM.h"
#include "VTCell.h"

#include <algorithm>
#include <sstream>

VTermMM::VTermMM(int rows, int columns)
  : fd(0),
    foreground(VTERMMM_WHITE),
    background(VTERMMM_BLACK),
    cells(rows, vrow(columns)),
    reverse(false),
    invalid_region()
{
  for(int row = 0; row < rows; ++row)
  {
    for(int col = 0; col < columns; ++col)
    {
      cells[row][col].SetX(col);
      cells[row][col].SetY(row);
    }
  }
  cursor.row = 0;
  cursor.col = 0;
  invalidate(0, rows, 0, columns);
  _term = vterm_new(rows, columns);
  vterm_parser_set_utf8(_term, 1);
  VTermState *vts = vterm_obtain_state(_term);
  vterm_state_set_callbacks(vts, &vterm_callbacks, this);
}

void VTermMM::reset_invalid()
{
  invalid_region.clear();
}

InvalidRegionIter VTermMM::GetInvalidBegin()
{
  return invalid_region.begin();
}

InvalidRegionIter VTermMM::GetInvalidEnd()
{
  return invalid_region.end();
}

static std::string make_key(int x, int y)
{
  std::ostringstream s;
  s << x << "," << y;
  return s.str();
}

void VTermMM::invalidate(int sr, int er, int sc, int ec)
{
  for(int row = sr; row < er; row++)
  {
    for(int col = sc; col < ec; col++)
    {
      VTCell *c = &cells[row][col];
      std::string k = make_key(col, row);
      InvalidRegionPair v = InvalidRegionPair(k, c);
      if(invalid_region.find(k) == invalid_region.end())
        invalid_region.insert(v);
    }
  }
}

void VTermMM::invalidate(VTermRect r)
{
  invalidate(r.start_row, r.end_row, r.start_col, r.end_col);
}

void VTermMM::invalidate(VTermPos p)
{
  VTCell *c = &cells[p.row][p.col];
  std::string k = make_key(p.col, p.row);
  InvalidRegionPair v = InvalidRegionPair(k, c);
  if(invalid_region.find(k) == invalid_region.end())
    invalid_region.insert(v);
}

int VTermMM::putglyph(const uint32_t chars[], int width, VTermPos pos)
{
  invalidate(pos);
  std::string s((char *)chars);
  VTermColor f = foreground;
  VTermColor b = background;

  if(reverse)
  {
    f = background;
    b = foreground;
  }

  VTCell &c = cells[pos.row][pos.col];
  c.SetX(pos.col);
  c.SetY(pos.row);
  c.SetValue(s);
  c.SetForeground(f);
  c.SetBackground(b);
  c.SetBold(bold);
  c.SetItalic(italic);
  return 1;
}

int VTermMM::movecursor(VTermPos pos, VTermPos oldpos, int visible)
{
  cursor = pos;
  cursor_visible = visible;
  invalidate(oldpos);
  return 1;
}

int VTermMM::copyrect(VTermRect dst, VTermRect src)
{
  invalidate(dst);
  for(int row=src.start_row; row<src.end_row; ++row)
  {
    int dst_row = dst.start_row + row - src.start_row;
    for(int col=src.start_col; col<src.end_col; ++col)
    {
      int dst_col = dst.start_col + col - src.start_col;
      cells[dst_row][dst_col] = cells[row][col];
      VTCell &c = cells[dst_row][dst_col];
      c.SetX(dst_col);
      c.SetY(dst_row);
    }
  }
  return 1;
}

int VTermMM::copycell(VTermPos dest, VTermPos src)
{
  invalidate(dest);
  cells[dest.row][dest.col] = cells[src.row][src.col];
  VTCell &c = cells[dest.row][dest.col];
  c.SetX(dest.col);
  c.SetY(dest.row);
  return 1;
}

int VTermMM::erase(VTermRect rect)
{
  invalidate(rect);
  VTermColor f = foreground;
  VTermColor b = background;

  if(reverse)
  {
    f = background;
    b = foreground;
  }

  for(int row=rect.start_row; row<rect.end_row; ++row)
  {
    for(int col=rect.start_col; col<rect.end_col; ++col)
    {
      VTCell &c = cells[row][col];
      c.SetValue(" ");
      c.SetForeground(f);
      c.SetBackground(b);
      c.SetBold(false);
      c.SetItalic(false);
    }
  } 
  return 1;
}

int VTermMM::initpen()
{
  foreground = VTERMMM_WHITE;
  background = VTERMMM_BLACK;
  return 1;
}

int VTermMM::setpenattr(VTermAttr attr, VTermValue *val)
{
  int ret = 1;
  switch(attr)
  {
    case VTERM_ATTR_FOREGROUND:
      foreground = val->color;
      break;
    case VTERM_ATTR_BACKGROUND:
      background = val->color;
      break;
    case VTERM_ATTR_REVERSE:
      reverse = val->boolean;
      break;
    case VTERM_ATTR_BOLD:
      bold = val->boolean;
      break;
    case VTERM_ATTR_ITALIC:
      italic = val->boolean;
      break;
    default:
      ret = 0;
      break;
  }
  return ret;
}

int VTermMM::settermprop(VTermProp prop, VTermValue *val)
{
  int ret = 1;
  switch(prop)
  {
    case VTERM_PROP_ALTSCREEN:
      return 0;
      break;
    case VTERM_PROP_CURSORVISIBLE:
      cursor_visible = val->boolean;
      break;
    default:
      ret = 0;
      break;
  }
  return ret;
}

int VTermMM::setmousefunc(VTermMouseFunc func, void *data)
{
  return 0;
}

int VTermMM::bell()
{
  return 0;
}

int VTermMM::resize(int rows, int columns)
{
  return 0;
}

void VTermMM::process_in_out()
{
  size_t bufflen = vterm_output_bufferlen(_term);
  if(bufflen > 0)
  {
    char buffer[bufflen];
    bufflen = vterm_output_bufferread(_term, buffer, bufflen);
    write((int)fd, buffer, bufflen);
  }
}

void VTermMM::feed(const std::string &daturs, VTermModifier mod)
{
  vterm_input_push_str(_term, mod, daturs.c_str(), daturs.length());
  process_in_out();
}

void VTermMM::feed(VTermKey k, VTermModifier mod)
{
  vterm_input_push_key(_term, mod, k);
  process_in_out();
}

void VTermMM::feed(char c, VTermModifier mod)
{
  char key[] = { c, NULL };
  feed(key, mod);
}

bool VTermMM::process()
{
  char buffer[8192];
  size_t bytes = read((int)fd, buffer, sizeof buffer);
  if(bytes > 0)
    vterm_push_bytes(_term, buffer, bytes);
  return bytes > 0;
}
