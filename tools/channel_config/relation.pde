/**
 * Author: Kai Lauterbach
 * License: GNU Public License - GPL.txt or http://www.gnu.org/licenses/gpl-3.0.html
 * Creation date: 02/2015
 */

import java.util.Map;

HashMap<String, Relation> gLEDRelations = new HashMap<String, Relation>();

//****************************************************************************//
// Remove one relation from global relation list/hash map
//****************************************************************************//
void removeRelation(int mx, int my)
{
  if (true == gLEDRelations.isEmpty())
  {
    return;
  }

  for (Map.Entry me : gLEDRelations.entrySet())
  {
    Relation r = (Relation)me.getValue();
    if (true == r.isMouseOver(mx, my))
    {
      Draggable d = r.getDraggable();
      d.setRelated(false);
      gLEDRelations.remove(""+d);
      dbgPrintln("Relation " + r + " removed");
      return;
    }
  }
}

//****************************************************************************//
// Create one relation
//****************************************************************************//
void createRelation(Draggable dr, Dropable dp)
{
  // draggable und droppable sollten bekannt und nicht "null" sein
  if (dr != null && dp != null && // pr�fen ob das drag'n'drop korrekt ausgef�hrt wurde
      null == gLEDRelations.get(""+dr))   // das Draggable darf nur jeweils eine Relation besitzen
  {
    // draggable objekt als verbunden markieren
    dr.setRelated(true);

    // relationsobjekt erzeugen
    Relation r = new Relation(dr, dp);

    // relationsobjekt in liste schreiben
    gLEDRelations.put(""+dr, r);
    dbgPrintln("New relation generated: " + r);
  }
}

//****************************************************************************//
// Rollover test and display the list of relations
//****************************************************************************//
void rollDisplayRelation(int mx, int my)
{
  if (true == gLEDRelations.isEmpty())
  {
    return;
  }

  for (Map.Entry me : gLEDRelations.entrySet())
  {
    // alle relationen zeichnen
    Relation r = (Relation)me.getValue();
    r.rollover(mx, my);
    r.display();
  }
}

class Relation
{
  boolean rollover = false;

  Draggable draggable;
  Dropable   dropable;

  float sx, sy, ex, ey = -1;

  Relation(Draggable tdrag, Dropable tdrop)
  {
    draggable = tdrag;
    dropable  = tdrop;

    sx = draggable.x + draggable.w / 2;
    sy = draggable.y + draggable.h / 2;

    ex = dropable.x  + dropable.w / 2;
    ey = dropable.y  + dropable.h / 2;
  }

  void display()
  {
    if (false == rollover)
    {
      // hier die farbe setzen wenn die maus nicht ?ber der relation ist
      strokeWeight(cNormalRelationStrokeWidth);
      stroke(cColorNormalRelation);
    } else {
      strokeWeight(cHighlightedRelationStrokeWidth);
      stroke(cColorHighlightedRelation);
      // hier die farbe und optik der Linie setzen
    }
    line(sx, sy, ex, ey);
  }

  void rollover(int mx, int my)
  {
    if (true == isMouseOver(mx, my))
    {
      rollover = true;
    } else {
      rollover = false;
    }
  }

  boolean isMouseOver(int mx, int my)
  {
    return pointInsideLine(new PVector(mx, my, 0),
                           new PVector(sx, sy, 0),
                           new PVector(ex, ey, 0),
                           cNormalRelationStrokeWidth);
  }

  String toString()
  {
    return draggable+" <-> "+dropable;
  }

  Draggable getDraggable()
  {
    return draggable;
  }
  
  Dropable getDropable()
  {
    return dropable;
  }

}
