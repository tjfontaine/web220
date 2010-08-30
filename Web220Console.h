#ifndef WEB220CONSOLE_H
#define WEB220CONSOLE_H

#include <Wt/WApplication>
#include <Wt/WEvent>
#include <Wt/WPaintedWidget>

#include "VTermMM.h"

static int COL_SIZE = 8;
static int ROW_SIZE = 14;

class Web220Console : public Wt::WPaintedWidget
{
  private:
    VTermMM *term_;
    void process();

  public:
    Web220Console(int, int, Wt::WContainerWidget *parent = 0);
    void keyPressedEvent(const Wt::WKeyEvent &e);
    void keyWentDownEvent(const Wt::WKeyEvent &e);
    void paintEvent(Wt::WPaintDevice *);
    void process(Wt::WApplication *);
};

#endif
