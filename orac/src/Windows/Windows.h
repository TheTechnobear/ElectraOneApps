#pragma once

#include "PageWindow.h"

class Windows
{
  public:
    Windows () :
			currentWindow (nullptr)
    {
    }

		void setCurrentWindow (Window* newCurrentWindow);
		Window* getCurrentWindow (void);
    void repaintCurrent (void);


	private:
		Window *currentWindow;
};
