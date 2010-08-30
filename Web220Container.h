#ifndef WEB220APPLICATION_H
#define WEB220APPLICATION_H

#include <boost/thread.hpp>

#include <Wt/WApplication>
#include <Wt/WContainerWidget>

#include "Web220Console.h"

class Web220Container : public Wt::WContainerWidget
{
  private:
    Web220Console *console;
    Wt::WApplication *app;
    boost::thread processor;
    //Wt::JSlot keyWentUp_;

  public:
    Web220Container(Wt::WContainerWidget *);
};

#endif
