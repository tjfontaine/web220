#include "FakeDom.h"
#include "keyWentDownPrevent.h"
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

  keyWentDownPrevent *result = new keyWentDownPrevent("keydown", this);
  addEventSignal(*result);
  return *result;
}
