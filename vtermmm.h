#ifndef _VTERMMM_H_DEFINED
#define _VTERMMM_H_DEFINED

extern "C" {
  #include <vterm.h>
  #include <vterm_input.h>
}

#include <iostream>

class VTCell
{
  public:
    VTCell(int row, int column, const std::string &v)
    {
      pos.row = row;
      pos.col = column;
      value = new std::string(v);
    }
    VTermColor fg_color;  
    VTermColor bg_color;
    VTermPos pos;
    std::string *value;
};

class VTermMM
{
  private:
    VTerm *_term;
    bool is_dirty;
    int fd;
    void process_in_out();

  public:
    VTermMM(int rows=25, int columns=80);
    void setFD(int filedesc) { fd = filedesc; }
    bool isDirty() { return is_dirty; }
    void feed(const std::string &, int mod = VTERM_MOD_NONE);
    void feed(VTermKey k, int mod = VTERM_MOD_NONE);
    bool process();
    int putglyph(const uint32_t[], int, VTermPos);
    int movecursor(VTermPos pos, VTermPos oldpos, int visible);
    int copyrect(VTermRect dest, VTermRect src);
    int copycell(VTermPos dest, VTermPos src);
    int erase(VTermRect rect);
    int initpen();
    int setpenattr(VTermAttr, VTermValue *);
    int settermprop(VTermProp prop, VTermValue *);
    int setmousefunc(VTermMouseFunc, void *);
    int bell();
    int resize(int, int);
    VTCell *cells[25][80];
};

int term_putglyph(const uint32_t chars[], int width, VTermPos pos, void *user);
int term_movecursor(VTermPos pos, VTermPos oldpos, int visible, void *user);
int term_copyrect(VTermRect dest, VTermRect src, void *user);
int term_copycell(VTermPos dest, VTermPos src, void *user);
int term_erase(VTermRect rect, void *user);
int term_initpen(void *user);
int term_setpenattr(VTermAttr attr, VTermValue *val, void *user);
int term_settermprop(VTermProp prop, VTermValue *val, void *user);
int term_setmousefunc(VTermMouseFunc func, void *data, void *user);
int term_bell(void *user);
int term_resize(int rows, int cols, void *user);

static VTermStateCallbacks vterm_callbacks = {
  &term_putglyph,
  &term_movecursor,
  &term_copyrect,
  &term_copycell,
  &term_erase,
  &term_initpen,
  &term_setpenattr,
  &term_settermprop,
  &term_setmousefunc,
  &term_bell,
  &term_resize,
};
#endif
