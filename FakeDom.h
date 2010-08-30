#ifndef FAKEDOM_H
#define FAKEDOM_H

#include <Wt/WContainerWidget>
#include <Wt/WEvent>

class FakeDom: public Wt::WContainerWidget
{
  public:
    FakeDom();
    Wt::EventSignal<Wt::WKeyEvent> &keyWentDown();
};

#endif
