#include "Web220Container.h"

Web220Container::Web220Container(Wt::WContainerWidget *root)
  : Wt::WContainerWidget(root),
    app(Wt::WApplication::instance())
{
  console = new Web220Console(25, 80, this);
  //EventSignal<WKeyEvent> &s = app->globalKeyWentUp();
  //s.preventPropagation();
  //s.connect(console, &Web220Console::keyWentUpEvent);

  Wt::EventSignal<Wt::WKeyEvent> &s1 = app->globalKeyWentDown();
  //s1.preventPropagation();
  s1.connect(console, &Web220Console::keyWentDownEvent);

  /* Most of the time won't get fired because we suppress keydown */
  Wt::EventSignal<Wt::WKeyEvent> &s2 = app->globalKeyPressed();
  //s2.preventPropagation();
  s2.connect(console, &Web220Console::keyPressedEvent);

  app->enableUpdates();
  processor = boost::thread(boost::bind(&Web220Console::process, console, app));
}
