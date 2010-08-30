#include "Web220Application.h"
#include "ajaxconsole.cpp"

Web220Application::Web220Application(const WEnvironment &env)
  : Wt::WApplication(env, new FakeDom())
{
  setTitle("Ajax Console");
  new AjaxConsoleOuter(root());
}

Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
  return new Web220Application(env);
}

int main(int argc, char **argv)
{
  return WRun(argc, argv, &createApplication);
}
