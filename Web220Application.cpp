#include "Web220Application.h"
#include "Web220Container.h"
#include "FakeDom.h"

Web220Application::Web220Application(const WEnvironment &env)
  : Wt::WApplication(env, new FakeDom())
{
  setTitle("Ajax Console");
  new Web220Container(root());
}

Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
  return new Web220Application(env);
}

int main(int argc, char **argv)
{
  return WRun(argc, argv, &createApplication);
}
