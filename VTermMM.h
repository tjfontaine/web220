#ifndef VTERMMM_H
#define VTERMMM_H

extern "C" {
  #include <vterm.h>
  #include <vterm_input.h>
}

#include <iostream>
#include <map>
#include <vector>

#include "VTCell.h"

static VTermColor VTERMMM_WHITE = { 255, 255, 255 };
static VTermColor VTERMMM_BLACK = { 0, 0, 0 };

typedef std::map<std::string, VTCell*> InvalidRegion;
typedef InvalidRegion::iterator InvalidRegionIter;
typedef std::pair<std::string, VTCell*> InvalidRegionPair;

class VTermMM
{
  private:
    VTerm *_term;
    int fd;
    void process_in_out();
    void invalidate(int, int, int, int);
    void invalidate(VTermRect);
    void invalidate(VTermPos);
    VTermColor foreground;
    VTermColor background;
    bool reverse;
    bool bold;
    bool italic;
    InvalidRegion invalid_region;
    typedef std::vector<VTCell> vrow;
    std::vector<vrow> cells;

  public:
    VTermMM(int rows=25, int columns=80);
    void reset_invalid();
    InvalidRegionIter GetInvalidBegin();
    InvalidRegionIter GetInvalidEnd();
    void setFD(int filedesc) { fd = filedesc; }
    void feed(const std::string &, VTermModifier mod = VTERM_MOD_NONE);
    void feed(VTermKey k, VTermModifier mod = VTERM_MOD_NONE);
    void feed(char c, VTermModifier mod = VTERM_MOD_NONE);
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
    bool cursor_visible;
    VTermPos cursor;
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
