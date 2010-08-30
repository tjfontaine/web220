#include <sstream>

#include "keyWentDownPrevent.h"

keyWentDownPrevent::keyWentDownPrevent(const char *name, Wt::WContainerWidget *wcw)
  : Wt::EventSignal<Wt::WKeyEvent>(name, wcw)
{
}

const std::string keyWentDownPrevent::javaScript() const
{
  std::ostringstream result(Wt::EventSignal<Wt::WKeyEvent>::javaScript());
  result << "var ignoreCodes=[8,9,33,34,35,36,37,38,39,40,45,46];";
  result << "if(ignoreCodes.indexOf(e.keyCode) > -1){ ";
  result << WT_CLASS << ".cancelEvent(e);}";
  return result.str();;
}
