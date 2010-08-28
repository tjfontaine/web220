#include "vtermmm.h"
#include <algorithm>

VTermMM::VTermMM(int rows, int columns)
  : fd(0),
    foreground(VTERMMM_WHITE),
    background(VTERMMM_BLACK),
    cells(rows, vrow(columns)),
    reverse(false)
{
  reset_invalid();
  invalidate(0, rows, 0, columns);
  _term = vterm_new(rows, columns);
  vterm_parser_set_utf8(_term, 1);
  VTermState *vts = vterm_obtain_state(_term);
  vterm_state_set_callbacks(vts, &vterm_callbacks, this);
}

void VTermMM::reset_invalid()
{
  invalid_region.start_row = -1;
  invalid_region.end_row = -1;
  invalid_region.start_col = -1;
  invalid_region.end_col = -1;
}

VTermRect VTermMM::getInvalid()
{
  VTermRect ir = invalid_region;
  int max_row = 0;
  int max_col = 0;
  vterm_get_size(_term, &max_row, &max_col);
  ir.start_row = std::max(ir.start_row, 0);
  ir.start_col = std::max(ir.start_col, 0);
  ir.end_row = std::min(ir.end_row, max_row);
  ir.end_col = std::min(ir.end_col, max_col);
  return ir;
}

void VTermMM::invalidate(int sr, int er, int sc, int ec)
{
  sr = std::max(sr, 0);
  er = std::max(er, 0);
  sc = std::max(sc, 0);
  ec = std::max(ec, 0);

  if(invalid_region.start_row == -1)
  {
    invalid_region.start_row = sr;
  }
  else
  {
    invalid_region.start_row = std::min(sr, invalid_region.start_row);
  }

  if(invalid_region.end_row == -1)
  {
    invalid_region.end_row = er;
  }
  else
  {
    invalid_region.end_row = std::max(er, invalid_region.end_row);
  }

  if(invalid_region.start_col == -1)
  {
    invalid_region.start_col = sc;
  }
  else
  {
    invalid_region.start_col = std::min(sc, invalid_region.start_col);
  }

  if(invalid_region.end_col == -1)
  {
    invalid_region.end_col = ec;
  }
  else
  {
    invalid_region.end_col = std::max(ec, invalid_region.end_col);
  }
}

void VTermMM::invalidate(VTermRect r)
{
  invalidate(r.start_row, r.end_row, r.start_col, r.end_col);
}

void VTermMM::invalidate(VTermPos p)
{
  invalidate(p.row, p.row+1, p.col, p.col+1);
}

bool VTermMM::isDirty()
{
  return invalid_region.end_row != -1 && invalid_region.start_row != -1 && invalid_region.end_col != -1 && invalid_region.start_col != -1;
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

  cells[pos.row][pos.col].set(s, f, b, bold, italic);
  return 1;
}

int VTermMM::movecursor(VTermPos pos, VTermPos oldpos, int visible)
{
  cursor = pos;
  cursor_visible = visible;
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
    }
  }
  return 1;
}

int VTermMM::copycell(VTermPos dest, VTermPos src)
{
  invalidate(dest);
  cells[dest.row][dest.col] = cells[src.row][src.col];
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
      cells[row][col].set(" ", f, b);
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
