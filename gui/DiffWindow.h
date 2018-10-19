#ifndef DIFF_WINDOW_H
#define DIFF_WINDOW_H

#include "DiffDocument.h"
#include "TextWindow.h"


class DiffWindow : public TextWindow
{
public:
  DiffWindow(AppScreen* p_pAppScreen);
  virtual ~DiffWindow();

  /**
   * Used to define if this window is the left or the right diff window
   */
  enum DW_TYPE
  {
    LEFT,
    RIGHT,
  };

  /**
   * Opens the window.
   *
   * @param dwType
   * Regarding to this type the window will be opened on the left or on
   * the right side of the screen
   *
   * @returns
   * false if opening fails
   */
  bool Open(DW_TYPE p_DwType);
  
  /**
   * Open a text file
   *
   * Extended method from base class DiffDocument
   *
   * @param p_FileName
   * Full file name with path for to be opened file. If empty a ASL
   * request will be opened asking the user for the file name.
   */
  virtual bool SetContent(DiffDocument* p_pDiffDocument);
};


#endif
