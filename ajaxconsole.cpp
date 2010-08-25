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

#include "vtermmm.h"

#include <boost/thread.hpp>
#include <stdio.h>
#include <pty.h>

using namespace Wt;

class AjaxConsole : public WPaintedWidget
{
  private:
    VTermMM *term_;
    void process();

  public:
    AjaxConsole(WContainerWidget *parent = 0);
    void keyPressedEvent(const WKeyEvent &e);
    void keyWentUpEvent(const WKeyEvent &e);
    void keyWentDownEvent(const WKeyEvent &e);
    void paintEvent(WPaintDevice *);
    void process(WApplication *);
};

AjaxConsole::AjaxConsole(WContainerWidget *parent) : WPaintedWidget(parent)
{
  resize(800, 600);
  update();
  term_ = new VTermMM(25, 80);
  struct winsize size = {25, 80, 0, 0};
  int master;
  char *args[] = {NULL};

  pid_t kid = forkpty(&master, NULL, NULL, &size);
  if(kid == 0)
  {
    execvp("/bin/bash", args);
  }

  term_->setFD(master);
}

void AjaxConsole::keyPressedEvent(const WKeyEvent &e)
{
  int mod = VTERM_MOD_NONE;
  KeyboardModifier wtmod = e.modifiers();

  if((wtmod & ShiftModifier) != 0)
    mod |= VTERM_MOD_SHIFT;
  if((wtmod & ControlModifier) != 0)
    mod |= VTERM_MOD_CTRL;
  if((wtmod & AltModifier) != 0)
    mod |= VTERM_MOD_ALT;
  //if(wtmod & MetaModifier)
  //  mod |= VTERM_MOD_ALT;

  term_->feed(e.text().toUTF8(), (VTermModifier)mod);
  update();
}

void AjaxConsole::keyWentUpEvent(const WKeyEvent &e)
{
  int mod = VTERM_MOD_NONE;
  KeyboardModifier wtmod = e.modifiers();

  if((wtmod & ShiftModifier) != 0)
    mod |= VTERM_MOD_SHIFT;
  if((wtmod & ControlModifier) != 0)
    mod |= VTERM_MOD_CTRL;
  if((wtmod & AltModifier) != 0)
    mod |= VTERM_MOD_ALT;
  //if(wtmod & MetaModifier)
  //  mod |= VTERM_MOD_ALT;

  VTermKey k = VTERM_KEY_NONE;

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
  }

  if(k != VTERM_KEY_NONE)
  {
    term_->feed(k, (VTermModifier)mod);
    update();
  }
}

void AjaxConsole::keyWentDownEvent(const WKeyEvent &e)
{
}

void AjaxConsole::paintEvent(WPaintDevice *paintDevice)
{
  WPainter painter(paintDevice);
  WFont f;
  f.setFamily(WFont::Monospace);
  painter.setFont(f);
  for(int row=0; row<25; ++row)
  {
    for(int col=0; col<80; ++col)
    {
      VTCell *c = term_->cells[row][col];
      if(c != NULL && c->value != " ")
      {
        painter.drawText((8.0*col)+4, 12.0*row, 0, 0, AlignCenter, c->value);
      }
    }
  }
}

void AjaxConsole::process(WApplication *app)
{
  while(true)
  {
    if(term_->process())
    {
      Wt::WApplication::UpdateLock uiLock = app->getUpdateLock();
      update();
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
    Wt::JSlot keyWentUp_;

  public:
    AjaxConsoleOuter(WContainerWidget *root) : WContainerWidget(root),
      app(WApplication::instance())
    {
      console = new AjaxConsole(this);
      EventSignal<WKeyEvent> &s = app->globalKeyWentUp();
      //s.preventPropagation();
      s.connect(console, &AjaxConsole::keyWentUpEvent);

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

class keyWentDownStopBackspace : public EventSignal<WKeyEvent>
{
  public:
    keyWentDownStopBackspace(const char* name, WContainerWidget *wcw) : EventSignal<WKeyEvent>(name, wcw)
    {
    }
  
    const std::string javaScript() const
    {
      std::string result = EventSignal<WKeyEvent>::javaScript();
      return result + "var ignoreCodes=[8,9,33,34,35,36,37,38,39,40,45,46];if(ignoreCodes.indexOf(e.keyCode) > -1){ "+ WT_CLASS + ".cancelEvent(e);}";
    }
};

class FakeDom: public WContainerWidget
{
  public:
    FakeDom() : WContainerWidget()
    {
    }

    EventSignal<WKeyEvent>& keyWentDown()
    {
      EventSignalBase *b = getEventSignal("keydown");
      if(b)
			{
        return *static_cast<EventSignal<WKeyEvent> *>(b);
			}
      keyWentDownStopBackspace *result = new keyWentDownStopBackspace("keydown", this);
      addEventSignal(*result);
      return *result;
    }
};

class AjaxConsoleApp: public WApplication
{
  public:
    AjaxConsoleApp(const WEnvironment &env) : WApplication(env, new FakeDom())
    {
      setTitle("Ajax Console");
      new AjaxConsoleOuter(root());
    }
};

WApplication *createApplication(const WEnvironment& env)
{ 
  return new AjaxConsoleApp(env);
}

int main(int argc, char **argv)
{ 
  return WRun(argc, argv, &createApplication);
}
