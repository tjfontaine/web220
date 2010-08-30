#ifndef VTCELL_H
#define VTCELL_H

#include <iostream>

extern "C" {
  #include <vterm.h>
  #include <vterm_input.h>
}

class VTCell
{
  private:
    VTermColor m_foreground;
    VTermColor m_background;
    std::string m_value;
    bool m_bold;
    bool m_italic;
    int m_x;
    int m_y;

  public:
    VTCell();
    VTCell(const VTCell& c);
    int GetX();
    void SetX(int);
    int GetY();
    void SetY(int);
    const std::string& GetValue();
    void SetValue(const std::string&);
    void SetBold(bool);
    bool GetBold();
    void SetItalic(bool);
    bool GetItalic();
    void SetForeground(const VTermColor&);
    const VTermColor& GetForeground();
    void SetBackground(const VTermColor&);
    const VTermColor& GetBackground();

  friend std::ostream& operator<< (std::ostream&, const VTCell&);
};

#endif
