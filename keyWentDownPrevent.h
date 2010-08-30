#ifndef KEYWENTDOWNPREVENT_H
#define KEYWENTDOWNPREVENT_H

#include <Wt/WContainerWidget>
#include <Wt/WEvent>

class keyWentDownPrevent : public Wt::EventSignal<Wt::WKeyEvent>
{
  public:
    keyWentDownPrevent(const char *, Wt::WContainerWidget*);
    const std::string javaScript() const;
};

#endif
