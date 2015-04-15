/**
 * Author: Kai Lauterbach
 * License: GNU Public License - GPL.txt or http://www.gnu.org/licenses/gpl-3.0.html
 * Creation date: 02/2015
 */

// Based on Daniel Shiffman D'n'D implementation from
// http://www.learningprocessing.com/examples/chapter-5/draggable-shape/

class Draggable
{
  boolean dragging = false; // Is the object being dragged?
  boolean rollover = false; // Is the mouse over the shape/image?
  boolean related  = false;

  int id = -1;

  float x, y, w, h; // Location and size

  Draggable(int tempid, float tempX, float tempY, float tempW, float tempH)
  {
    id = tempid;
    x  =  tempX;
    y  =  tempY;
    w  =  tempW;
    h  =  tempH;

  }

  // Method to display
  void display()
  {
    if (related)
    {
      fill(cColorDraggableRelated);
      noStroke();
      strokeWeight(1);
      rect(x, y, w, h);
      //image(imgRelated, x, y, w, h);
    } else if (dragging)
    {
      stroke(cColorDraggableRelation);
      strokeWeight(cColorDraggableRelationWidth);

      line(x + w / 2, y + h / 2, mouseX, mouseY);

      fill(cColorDraggableDragging);
      noStroke();
      strokeWeight(1);
      rect(x, y, w, h);
      //image(imgDrag, x, y, w, h);
    } else if (rollover)
    {
      fill(cColorDraggableRollover);
      noStroke();
      strokeWeight(1);
      rect(x, y, w, h);
      //image(imgDrag, x, y, w, h);
    } else {
      fill(cColorDraggableNormal);
      noStroke();
      strokeWeight(1);
      rect(x, y, w, h);
      //image(img, x, y, w, h);
    }
  }

  // Is a point inside the rectangle (for click)?
  void clicked(int mx, int my)
  {
    if (true == isMouseOver(mx, my))
    {
      dragging = true;
    }
  }

  // Is a point inside the rectangle (for rollover)
  void rollover(int mx, int my)
  {
    if (true == isMouseOver(mx, my))
    {
      rollover = true;
    } else {
      rollover = false;
    }
  }

  // Stop dragging
  Draggable stopDragging(int mx, int my)
  {
    boolean tdrag = dragging;
    dragging = false;
    if (tdrag)
    {
      return this;
    } else {
      return null;
    }
  }

  // Drag the "relation"
  Draggable drag(int mx, int my)
  {
    if (dragging)
    {
      line(x + w/2, y + h/2, mx, my);
      return this;
    } else {
      return null;
    }
  }

  void setRelated(boolean r)
  {
    related = r;
  }

  boolean isRelated()
  {
    return related;
  }

  private boolean isMouseOver(int mx, int my)
  {
    if (mx > x && mx < x + w && my > y && my < y + h)
    {
      return true;
    }
    return false;
  }

  int getID()
  {
    return id;
  }

  String toString()
  {
    return ""+id;
  }

}

