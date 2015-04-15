/**
 * Author: Kai Lauterbach
 * License: GNU Public License - GPL.txt or http://www.gnu.org/licenses/gpl-3.0.html
 * Creation date: 02/2015
 */

// Based on Daniel Shiffman D'n'D implementation from:
// http://www.learningprocessing.com/examples/chapter-5/draggable-shape/

class Dropable
{
  boolean rollover = false; // used to visualize that the mouse is over the shape

  int id = -1;

  float x, y, w, h; // Location and size

  Dropable(int tempid, float tempX, float tempY, float tempW, float tempH)
  {
    id = tempid;

    x  = tempX;
    y  = tempY;
    w  = tempW;
    h  = tempH;
  }

  //----------------------------------------------------------------------------
  // This function should be used in the main draw function to show the dropable
  //----------------------------------------------------------------------------
  void display()
  { 
    strokeWeight(cDropableStrokeWeight);
    

    if (rollover)
    {
      stroke(cColorDropableRolloverStroke);
      
      if (false == gExpertMode)
      {
        if (1 == cPixelMatrixExpertModePixels[id])
        {
          // it's a "limited for expertMode" pixel
          strokeWeight(cExpertModeDropableStrokeWeightRollover);
          stroke(cColorExpertModeDropableStroke);
        }
      }
      fill(cColorDropableRollover);
    } else {
      stroke(cColorDropableStroke);
      fill(cColorDropable);

      if (false == gExpertMode)
      { 
        if (1 == cPixelMatrixExpertModePixels[id])
        {
          // it's a "limited for expertMode" pixel
          strokeWeight(cExpertModeDropableStrokeWeight);
          stroke(cColorExpertModeDropableStroke);
          
          fill(cColorExpertModeDropable);
        }
      }
    }

    rect(x, y, w, h);
  }

  //-----------------------------------------------
  // Is a point inside the rectangle (for rollover)
  //-----------------------------------------------
  void rollover(int mx, int my)
  {
    if (true == isMouseOver(mx, my))
    {
      rollover = true;
    } else {
      rollover = false;
    }
  }

  //---------------------------------------------------------------------
  // The function which should be called it the mouse button was released
  //---------------------------------------------------------------------
  Dropable drop(int mx, int my)
  {
    // berechnen ob mx, my auf geraden
    if (true == isMouseOver(mx, my))
    {
      if (false == gExpertMode)
      {
        if (1 == cPixelMatrixExpertModePixels[id])
        {
          // expert pixel found, drop is not alowed if we are not in
          // expert mode
          //dbgPrintln("Droped over Expert pixel " + id);
          return null;
        } else {
          return this;
        }
      } else {
        // we are not in expert mode, drop over everything is alowed
        return this;
      }
    }

    return null;
  }

  int getID()
  {
    return id;
  }

  String toString()
  {
    return ""+id;
  }
  
  private boolean isMouseOver(int mx, int my)
  {
    if (mx > x && mx < x + w && my > y && my < y + h)
    {
      return true;
    }

    return false;
  }
}

