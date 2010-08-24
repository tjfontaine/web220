#include "vtermmm.h"

VTermMM::VTermMM(int rows, int columns) : is_dirty(false), fd(0)
{
  for(int y=0; y < rows; ++y)
  {
    for(int x=0; x < columns; ++x)
    {
      cells[y][x] = new VTCell(y, x, " ");
    }
  }


  _term = vterm_new(rows, columns);
  vterm_parser_set_utf8(_term, 1);
  VTermState *vts = vterm_obtain_state(_term);
  vterm_state_set_callbacks(vts, &vterm_callbacks, this);
}

int VTermMM::putglyph(const uint32_t chars[], int width, VTermPos pos)
{
  std::string s((char *)chars);
  delete cells[pos.row][pos.col];
  cells[pos.row][pos.col] = new VTCell(pos.row, pos.col, s);
  return 1;
}

int VTermMM::movecursor(VTermPos pos, VTermPos oldpos, int visible)
{
  return 1;
}

int VTermMM::copyrect(VTermRect dest, VTermRect src)
{
  int dest_rows = dest.end_row - dest.start_row;
  int dest_cols = dest.end_col - dest.start_col;
  return 1;
}

int VTermMM::copycell(VTermPos dest, VTermPos src)
{
  delete cells[dest.row][dest.col];
  VTCell *c = cells[src.row][src.col];
  cells[dest.row][dest.col] = new VTCell(c->pos.row, c->pos.col, c->value->c_str());
  return 1;
}

int VTermMM::erase(VTermRect rect)
{
  for(int row=rect.start_row; row<rect.end_row; ++row)
  {
    for(int col=rect.start_col; col<rect.end_col; ++col)
    {
      delete cells[row][col];
      cells[row][col] = new VTCell(row, col, " ");
    }
  } 
  return 1;
}

int VTermMM::initpen()
{
  return 1;
}

int VTermMM::setpenattr(VTermAttr attr, VTermValue *val)
{
  return 1;
}

int VTermMM::settermprop(VTermProp prop, VTermValue *val)
{
  return 1;
}

int VTermMM::setmousefunc(VTermMouseFunc func, void *data)
{
  return 1;
}

int VTermMM::bell()
{
  return 1;
}

int VTermMM::resize(int rows, int columns)
{
  return 1;
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

void VTermMM::feed(const std::string &daturs, int mod)
{
  vterm_input_push_str(_term, (VTermModifier)mod, daturs.c_str(), daturs.length());
  process_in_out();
}

void VTermMM::feed(VTermKey k, int mod)
{
  vterm_input_push_key(_term, (VTermModifier)mod, k);
  process_in_out();
}

bool VTermMM::process()
{
  char buffer[8192];
  size_t bytes = read((int)fd, buffer, sizeof buffer);
  if(bytes > 0)
    vterm_push_bytes(_term, buffer, bytes);
  return bytes > 0;
}
