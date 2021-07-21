#include "Windows.h"


/** Assigns the current window
 *
 */
void Windows::setCurrentWindow (Window* newCurrentWindow)
{
	currentWindow = newCurrentWindow;
}

/** Returns pointer to current window
 *
 */
Window* Windows::getCurrentWindow (void)
{
	Window* window = nullptr;

	if (currentWindow != nullptr)
	{
    return (currentWindow);
	}

	return (window);
}

/** Repaint currently active window
 *
 */
void Windows::repaintCurrent (void)
{
	if (getCurrentWindow ())
	{
	  getCurrentWindow ()->repaint ();
	}
}
