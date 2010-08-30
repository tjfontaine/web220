#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WPainter>
#include <Wt/WPaintedWidget>
#include <Wt/WPoint>
#include <Wt/WText>
#include <Wt/WEvent>
#include <Wt/WString>
#include <Wt/WRectArea>
#include <Wt/WSignal>
#include <Wt/WFont>

#include <iostream>

#include "VTermMM.h"

#include <boost/thread.hpp>
#include <stdio.h>
#include <pty.h>

using namespace Wt;

static int COL_SIZE = 8;
static int ROW_SIZE = 14;

#define WEB220_DEBUG_DRAW 0

class AjaxConsole : public WPaintedWidget
{
  private:
    VTermMM *term_;
    void process();

  public:
    AjaxConsole(int, int,WContainerWidget *parent = 0);
    void keyPressedEvent(const WKeyEvent &e);
    void keyWentDownEvent(const WKeyEvent &e);
    void paintEvent(WPaintDevice *);
    void process(WApplication *);
};

AjaxConsole::AjaxConsole(int rows, int cols, WContainerWidget *parent)
  : WPaintedWidget(parent)
{
  resize((cols+1)*COL_SIZE, (rows+1)*ROW_SIZE);
  update(PaintUpdate);
  term_ = new VTermMM(rows, cols);
  struct winsize size = {rows, cols, 0, 0};
  int master;
  char *args[] = {NULL};

  pid_t kid = forkpty(&master, NULL, NULL, &size);
  if(kid == 0)
  {
    execvp("/bin/bash", args);
  }

  term_->setFD(master);
}

VTermModifier toVTermModifier(KeyboardModifier wtmod)
{
  int mod = VTERM_MOD_NONE;

  if((wtmod & ShiftModifier) != 0)
  {
    mod |= VTERM_MOD_SHIFT;
  }

  if((wtmod & ControlModifier) != 0)
  {
    mod |= VTERM_MOD_CTRL;
  }

  if((wtmod & AltModifier) != 0)
  {
    mod |= VTERM_MOD_ALT;
  }

  //if(wtmod & MetaModifier)
  //  mod |= VTERM_MOD_ALT;

  return (VTermModifier)mod;
}

void AjaxConsole::keyPressedEvent(const WKeyEvent &e)
{
  VTermModifier mod = toVTermModifier(e.modifiers());
  term_->feed(e.text().toUTF8(), (VTermModifier)mod);
  update(PaintUpdate);
}

void AjaxConsole::keyWentDownEvent(const WKeyEvent &e)
{
  VTermModifier mod = toVTermModifier(e.modifiers());
  VTermKey k = VTERM_KEY_NONE;
  char c = '\0';

  switch(e.key())
  {
    case Key_Backspace:
      k = VTERM_KEY_BACKSPACE;
      break;
    case Key_Tab:
      k = VTERM_KEY_TAB;
      break;
    case Key_Escape:
      k = VTERM_KEY_ESCAPE;
      break;
    case Key_Up:
      k = VTERM_KEY_UP;
      break;
    case Key_Down:
      k = VTERM_KEY_DOWN;
      break;
    case Key_Left:
      k = VTERM_KEY_LEFT;
      break;
    case Key_Right:
      k = VTERM_KEY_RIGHT;
      break;
    case Key_Home:
      k = VTERM_KEY_DOWN;
      break;
    case Key_End:
      k = VTERM_KEY_END;
      break;
    case Key_PageUp:
      k = VTERM_KEY_PAGEUP;
      break;
    case Key_PageDown:
      k = VTERM_KEY_PAGEDOWN;
      break;
    case Key_Insert:
      k = VTERM_KEY_INS;
      break;
    case Key_Delete:
      k = VTERM_KEY_DEL;
      break;
    case Key_A:
    case Key_B:
    case Key_C:
    case Key_D:
    case Key_E:
    case Key_F:
    case Key_G:
    case Key_H:
    case Key_I:
    case Key_J:
    case Key_K:
    case Key_L:
    case Key_M:
    case Key_N:
    case Key_O:
    case Key_P:
    case Key_Q:
    case Key_R:
    case Key_S:
    case Key_T:
    case Key_U:
    case Key_V:
    case Key_W:
    case Key_X:
    case Key_Y:
    case Key_Z:
      if(mod == VTERM_MOD_CTRL)
      {
        c = (char)e.key()-64;
        mod = VTERM_MOD_NONE;
      }
      break;
  }

  if(k != VTERM_KEY_NONE)
  {
    term_->feed(k, (VTermModifier)mod);
    update(PaintUpdate);
  }
  else if(c != '\0')
  {
    std::cerr << "Sending " << int(c) << " Mod: " << mod << std::endl;
    term_->feed(c, (VTermModifier)mod);
    update(PaintUpdate);
  }
}

static const WColor toWColor(VTermColor c)
{
  return WColor(c.red, c.green, c.blue);
}

void AjaxConsole::paintEvent(WPaintDevice *paintDevice)
{
  WPainter painter(paintDevice);
  WFont f;
  f.setFamily(WFont::Monospace);

  InvalidRegionIter it = term_->GetInvalidBegin();
  InvalidRegionIter end = term_->GetInvalidEnd();

  for(; it != end; ++it)
  {
    VTCell *c = (*it).second;
    WRectF r(COL_SIZE*c->GetX(), ROW_SIZE*c->GetY(), COL_SIZE, ROW_SIZE);
    painter.fillRect(r, WBrush(toWColor(c->GetBackground())));
    std::string value = c->GetValue();
    //std::cerr << *c << std::endl;
    if(value != " ")
    {
      painter.setPen(WPen(toWColor(c->GetForeground())));

      if(c->GetBold())
      {
        f.setWeight(WFont::Bolder);
      }
      else
      {
        f.setWeight(WFont::NormalWeight);
      }

      if(c->GetItalic())
      {
        f.setStyle(WFont::Italic);
      }
      else
      {
        f.setStyle(WFont::NormalStyle);
      }

      painter.setFont(f);
      painter.drawText(COL_SIZE*(c->GetX()+1), ROW_SIZE*(c->GetY()+1), 0, 0, AlignRight|AlignBottom, value);
    }
  }

  if(term_->cursor_visible)
  {
    WRectF r(COL_SIZE*term_->cursor.col, ROW_SIZE*term_->cursor.row, COL_SIZE, ROW_SIZE);
    painter.setPen(WPen(WColor("white")));
    painter.fillRect(r, WBrush(WColor("white")));
  }

  term_->reset_invalid();
}

void AjaxConsole::process(WApplication *app)
{
  while(true)
  {
    if(term_->process())
    {
      Wt::WApplication::UpdateLock uiLock = app->getUpdateLock();
      update(PaintUpdate);
      app->triggerUpdate();
    }
  }
}

class AjaxConsoleOuter : public WContainerWidget
{
  private:
    AjaxConsole *console;
    WApplication *app;
    boost::thread processor;
    //Wt::JSlot keyWentUp_;

  public:
    AjaxConsoleOuter(WContainerWidget *root) : WContainerWidget(root),
      app(WApplication::instance())
    {
      console = new AjaxConsole(25, 80, this);
      //EventSignal<WKeyEvent> &s = app->globalKeyWentUp();
      //s.preventPropagation();
      //s.connect(console, &AjaxConsole::keyWentUpEvent);

      EventSignal<WKeyEvent> &s1 = app->globalKeyWentDown();
      //s1.preventPropagation();
      s1.connect(console, &AjaxConsole::keyWentDownEvent);

      /* Most of the time won't get fired because we suppress keydown */
      EventSignal<WKeyEvent> &s2 = app->globalKeyPressed();
      //s2.preventPropagation();
      s2.connect(console, &AjaxConsole::keyPressedEvent);

      app->enableUpdates();
      processor = boost::thread(boost::bind(&AjaxConsole::process, console, app));
    }
};
