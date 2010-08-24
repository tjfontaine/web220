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
  char *args[1];
  args[0] = "";

  pid_t kid = forkpty(&master, NULL, NULL, &size);
  if(kid == 0)
  {
    execvp("/bin/bash", args);
  }

  term_->setFD(master);
}

void AjaxConsole::keyPressedEvent(const WKeyEvent &e)
{
  term_->feed(e.text().toUTF8());
  update();
}

void AjaxConsole::paintEvent(WPaintDevice *paintDevice)
{
  WPainter painter(paintDevice);
  for(int row=0; row<25; ++row)
  {
    for(int col=0; col<80; ++col)
    {
      if(term_->cells[row][col])
      {
        WString s(term_->cells[row][col]->value->c_str());
        painter.drawText(1.0*col*8, 1.0*12*row, 10.0, 10.0, AlignCenter, s);
      }
    }
  }
}

void AjaxConsole::process(WApplication *app)
{
  std::cerr << "Starting AjaxConsole process" << std::endl;
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

  public:
    AjaxConsoleOuter(WContainerWidget *root) : WContainerWidget(root),
      app(WApplication::instance())
    {
      console = new AjaxConsole(this);
      app->globalKeyPressed().connect(console, &AjaxConsole::keyPressedEvent);
      app->enableUpdates();
      processor = boost::thread(boost::bind(&AjaxConsole::process, console, app));
    }
};

class AjaxConsoleApp: public WApplication
{
  public:
    AjaxConsoleApp(const WEnvironment &env): WApplication(env)
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

