#ifndef _VTERMMM_H_DEFINED
#define _VTERMMM_H_DEFINED

extern "C" {
  #include <vterm.h>
  #include <vterm_input.h>
}

#include <iostream>
#include <vector>

static VTermColor VTERMMM_WHITE = { 255, 255, 255 };
static VTermColor VTERMMM_BLACK = { 0, 0, 0 };

class VTCell
{
  public:
    VTCell(const std::string &v = " ", VTermColor f = VTERMMM_WHITE, VTermColor b = VTERMMM_BLACK)
    {
      set(v, f, b);
    }

    void set(const std::string &v, VTermColor f, VTermColor b)
    {
      value = v;
      fg_color = f;
      bg_color = b;
    }

    void set(const VTCell &c)
    {
      set(c.value, c.fg_color, c.bg_color);
    }

    VTermColor fg_color;  
    VTermColor bg_color;
    std::string value;
};

class VTermMM
{
  private:
    VTerm *_term;
    int fd;
    void process_in_out();
    VTermRect invalid_region;
    void invalidate(int, int, int, int);
    void invalidate(VTermRect);
    void invalidate(VTermPos);
    VTermColor foreground;
    VTermColor background;

  public:
    VTermMM(int rows=25, int columns=80);
    void reset_invalid();
    VTermRect getInvalid() { return invalid_region; }
    void setFD(int filedesc) { fd = filedesc; }
    bool isDirty();
    void feed(const std::string &, VTermModifier mod = VTERM_MOD_NONE);
    void feed(VTermKey k, VTermModifier mod = VTERM_MOD_NONE);
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
    typedef std::vector<VTCell> vrow; 
    std::vector<vrow> cells;
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
