#include "VTermMM.h"

int term_putglyph(const uint32_t chars[], int width, VTermPos pos, void *user) 
{
  return ((VTermMM*)user)->putglyph(chars, width, pos);
}

int term_movecursor(VTermPos pos, VTermPos oldpos, int visible, void *user)
{
  return ((VTermMM*)user)->movecursor(pos, oldpos, visible);
}

int term_copyrect(VTermRect dest, VTermRect src, void *user)
{
  return ((VTermMM*)user)->copyrect(dest, src);
}

int term_copycell(VTermPos dest, VTermPos src, void *user)
{
  return ((VTermMM*)user)->copycell(dest, src);
}

int term_erase(VTermRect rect, void *user)
{
  return ((VTermMM*)user)->erase(rect);
}

int term_initpen(void *user)
{
  return ((VTermMM*)user)->initpen();
}

int term_setpenattr(VTermAttr attr, VTermValue *val, void *user)
{
  return ((VTermMM*)user)->setpenattr(attr, val);
}

int term_settermprop(VTermProp prop, VTermValue *val, void *user)
{
  return ((VTermMM*)user)->settermprop(prop, val);
}

int term_setmousefunc(VTermMouseFunc func, void *data, void *user)
{
  return ((VTermMM*)user)->setmousefunc(func, data);
}

int term_bell(void *user)
{
  return ((VTermMM*)user)->bell();
}

int term_resize(int rows, int cols, void *user)
{
  return ((VTermMM*)user)->resize(rows, cols);
}
