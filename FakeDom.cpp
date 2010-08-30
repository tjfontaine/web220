#include "FakeDom.h"
#include "ajaxconsole.cpp"

FakeDom::FakeDom() : Wt::WContainerWidget()
{
}

Wt::EventSignal<Wt::WKeyEvent>& FakeDom::keyWentDown()
{
  Wt::EventSignalBase *b = getEventSignal("keydown");
  if(b)
  {
    return *static_cast<Wt::EventSignal<Wt::WKeyEvent> *>(b);
  }

  keyWentDownStopBackspace *result = new keyWentDownStopBackspace("keydown", this);
  addEventSignal(*result);
  return *result;
}
