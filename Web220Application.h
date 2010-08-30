#ifndef WAPPLICATION_H
#define WAPPLICATION_H

#include <Wt/WApplication>
#include <Wt/WEnvironment>

class Web220Application : public Wt::WApplication
{
  public:
    Web220Application(const Wt::WEnvironment &env);
};

#endif
