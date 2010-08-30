#ifndef WEB220APPLICATION_H
#define WEB220APPLICATION_H

#include <Wt/WApplication>
#include <Wt/WContainerWidget>

#include "ajaxconsole.cpp"

class Web220Container : public Wt::WContainerWidget
{
  private:
    AjaxConsole *console;
    Wt::WApplication *app;
    boost::thread processor;
    //Wt::JSlot keyWentUp_;

  public:
    Web220Container(Wt::WContainerWidget *);
};

#endif
