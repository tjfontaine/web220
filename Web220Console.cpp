#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WPainter>
#include <Wt/WPoint>
#include <Wt/WText>
#include <Wt/WString>
#include <Wt/WRectArea>
#include <Wt/WSignal>
#include <Wt/WFont>

#include <iostream>

#include <stdio.h>
#include <pty.h>

#include "Web220Console.h"

Web220Console::Web220Console(int rows, int cols, Wt::WContainerWidget *parent)
  : Wt::WPaintedWidget(parent)
{
  resize((cols+1)*COL_SIZE, (rows+1)*ROW_SIZE);
  update(Wt::PaintUpdate);
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

VTermModifier toVTermModifier(Wt::KeyboardModifier wtmod)
{
  int mod = VTERM_MOD_NONE;

  if((wtmod & Wt::ShiftModifier) != 0)
  {
    mod |= VTERM_MOD_SHIFT;
  }

  if((wtmod & Wt::ControlModifier) != 0)
  {
    mod |= VTERM_MOD_CTRL;
  }

  if((wtmod & Wt::AltModifier) != 0)
  {
    mod |= VTERM_MOD_ALT;
  }

  //if(wtmod & MetaModifier)
  //  mod |= VTERM_MOD_ALT;

  return (VTermModifier)mod;
}

void Web220Console::keyPressedEvent(const Wt::WKeyEvent &e)
{
  VTermModifier mod = toVTermModifier(e.modifiers());
  term_->feed(e.text().toUTF8(), (VTermModifier)mod);
  update(Wt::PaintUpdate);
}

void Web220Console::keyWentDownEvent(const Wt::WKeyEvent &e)
{
  VTermModifier mod = toVTermModifier(e.modifiers());
  VTermKey k = VTERM_KEY_NONE;
  char c = '\0';

  switch(e.key())
  {
    case Wt::Key_Backspace:
      k = VTERM_KEY_BACKSPACE;
      break;
    case Wt::Key_Tab:
      k = VTERM_KEY_TAB;
      break;
    case Wt::Key_Escape:
      k = VTERM_KEY_ESCAPE;
      break;
    case Wt::Key_Up:
      k = VTERM_KEY_UP;
      break;
    case Wt::Key_Down:
      k = VTERM_KEY_DOWN;
      break;
    case Wt::Key_Left:
      k = VTERM_KEY_LEFT;
      break;
    case Wt::Key_Right:
      k = VTERM_KEY_RIGHT;
      break;
    case Wt::Key_Home:
      k = VTERM_KEY_DOWN;
      break;
    case Wt::Key_End:
      k = VTERM_KEY_END;
      break;
    case Wt::Key_PageUp:
      k = VTERM_KEY_PAGEUP;
      break;
    case Wt::Key_PageDown:
      k = VTERM_KEY_PAGEDOWN;
      break;
    case Wt::Key_Insert:
      k = VTERM_KEY_INS;
      break;
    case Wt::Key_Delete:
      k = VTERM_KEY_DEL;
      break;
    case Wt::Key_A:
    case Wt::Key_B:
    case Wt::Key_C:
    case Wt::Key_D:
    case Wt::Key_E:
    case Wt::Key_F:
    case Wt::Key_G:
    case Wt::Key_H:
    case Wt::Key_I:
    case Wt::Key_J:
    case Wt::Key_K:
    case Wt::Key_L:
    case Wt::Key_M:
    case Wt::Key_N:
    case Wt::Key_O:
    case Wt::Key_P:
    case Wt::Key_Q:
    case Wt::Key_R:
    case Wt::Key_S:
    case Wt::Key_T:
    case Wt::Key_U:
    case Wt::Key_V:
    case Wt::Key_W:
    case Wt::Key_X:
    case Wt::Key_Y:
    case Wt::Key_Z:
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
    update(Wt::PaintUpdate);
  }
  else if(c != '\0')
  {
    std::cerr << "Sending " << int(c) << " Mod: " << mod << std::endl;
    term_->feed(c, (VTermModifier)mod);
    update(Wt::PaintUpdate);
  }
}

static const Wt::WColor toWColor(VTermColor c)
{
  return Wt::WColor(c.red, c.green, c.blue);
}

void Web220Console::paintEvent(Wt::WPaintDevice *paintDevice)
{
  Wt::WPainter painter(paintDevice);
  Wt::WFont f;
  f.setFamily(Wt::WFont::Monospace);

  InvalidRegionIter it = term_->GetInvalidBegin();
  InvalidRegionIter end = term_->GetInvalidEnd();

  for(; it != end; ++it)
  {
    VTCell *c = (*it).second;
    Wt::WRectF r(COL_SIZE*c->GetX(), ROW_SIZE*c->GetY(), COL_SIZE, ROW_SIZE);
    painter.fillRect(r, Wt::WBrush(toWColor(c->GetBackground())));
    std::string value = c->GetValue();
    //std::cerr << *c << std::endl;
    if(value != " ")
    {
      painter.setPen(Wt::WPen(toWColor(c->GetForeground())));

      if(c->GetBold())
      {
        f.setWeight(Wt::WFont::Bolder);
      }
      else
      {
        f.setWeight(Wt::WFont::NormalWeight);
      }

      if(c->GetItalic())
      {
        f.setStyle(Wt::WFont::Italic);
      }
      else
      {
        f.setStyle(Wt::WFont::NormalStyle);
      }

      painter.setFont(f);
      painter.drawText(COL_SIZE*(c->GetX()+1), ROW_SIZE*(c->GetY()+1), 0, 0, Wt::AlignRight|Wt::AlignBottom, value);
    }
  }

  if(term_->cursor_visible)
  {
    Wt::WRectF r(COL_SIZE*term_->cursor.col, ROW_SIZE*term_->cursor.row, COL_SIZE, ROW_SIZE);
    painter.setPen(Wt::WPen(Wt::WColor("white")));
    painter.fillRect(r, Wt::WBrush(Wt::WColor("white")));
  }

  term_->reset_invalid();
}

void Web220Console::process(Wt::WApplication *app)
{
  while(true)
  {
    if(term_->process())
    {
      Wt::WApplication::UpdateLock uiLock = app->getUpdateLock();
      update(Wt::PaintUpdate);
      app->triggerUpdate();
    }
  }
}
