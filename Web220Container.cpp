#include "Web220Container.h"

Web220Container::Web220Container(Wt::WContainerWidget *root)
  : Wt::WContainerWidget(root),
    app(Wt::WApplication::instance())
{
  console = new AjaxConsole(25, 80, this);
  //EventSignal<WKeyEvent> &s = app->globalKeyWentUp();
  //s.preventPropagation();
  //s.connect(console, &AjaxConsole::keyWentUpEvent);

  Wt::EventSignal<Wt::WKeyEvent> &s1 = app->globalKeyWentDown();
  //s1.preventPropagation();
  s1.connect(console, &AjaxConsole::keyWentDownEvent);

  /* Most of the time won't get fired because we suppress keydown */
  Wt::EventSignal<Wt::WKeyEvent> &s2 = app->globalKeyPressed();
  //s2.preventPropagation();
  s2.connect(console, &AjaxConsole::keyPressedEvent);

  app->enableUpdates();
  processor = boost::thread(boost::bind(&AjaxConsole::process, console, app));
}
