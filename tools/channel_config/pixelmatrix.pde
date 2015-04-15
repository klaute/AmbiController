/**
 * Author: Kai Lauterbach
 * License: GNU Public License - GPL.txt or http://www.gnu.org/licenses/gpl-3.0.html
 * Creation date: 02/2015
 */

import java.util.Map;

boolean gMatrixConfigurationDone = false;

HashMap<String, Dropable> gDropablePixel;

int[] cPixelMatrixExpertModePixels = new int[400];

//****************************************************************************//
// Build the PixelMatrix HashMap
//****************************************************************************//
void genPixelMatrix(int s, float tx, float ty, float tw, float th)
{
  gDropablePixel = new HashMap<String, Dropable>();

  // n% der breite des fensters entspricht dem rand des PixelMatrix-Bereichs
  float pixelMatrixBorder     = (tw  * cPixelMatrixBorderPercent) / 100;
  float pixelMatrixLeftMargin = tx;
  float pixelMatrixTopMargin  = ty;
  float pixelMatrixWidth      = tw;
  float pixelMatrixHeight     = th;

  dbgPrintln("PixelMatrixBorder=" + pixelMatrixBorder);
  dbgPrintln("DropablePixelMatrix position x=" + pixelMatrixLeftMargin +
             " y=" + pixelMatrixTopMargin);
  dbgPrintln("DropablePixelMatrix width=" + pixelMatrixWidth +
             " height=" + pixelMatrixHeight);

  // n% der breite des PixelMatrix-Bereichs entspricht dem Abstand zwischen den
  // Dropable-Pixeln
  float dropableBorder = (pixelMatrixWidth * cPixelBorderPercent) / 100;
  float dropableWidth  = (pixelMatrixWidth  - 2 * pixelMatrixBorder -
                         (s - 1) * dropableBorder) / s;
  float dropableHeight = (pixelMatrixHeight - 2 * pixelMatrixBorder -
                         (s - 1) * dropableBorder) / s;

  dbgPrintln("DropablePixelBorder=" + dropableBorder);
  dbgPrintln("DropablePixel width=" + dropableWidth +
             " height=" + dropableHeight);

  int i = 0;
  for (int y1 = 0; y1 < s; y1++)
  {
    for (int x1 = 0; x1 < s; x1++)
    {
      // berechnen von x/y f�r den aktuellen DroppablePixel auf den
      // Bereich im Fenster
      float tx1 = pixelMatrixLeftMargin + pixelMatrixBorder +
                  x1 * dropableBorder + x1 * dropableWidth;
      float ty1 = pixelMatrixTopMargin  + pixelMatrixBorder +
                  y1 * dropableBorder + y1 * dropableHeight;
      Dropable d = new Dropable(i, tx1, ty1, dropableWidth, dropableHeight);
      gDropablePixel.put(""+i, d);

      dbgPrintln("New dropable pixel: " + i + " (" + x1 + "," + y1 + ")=(" +
                 tx1 + "," + ty1 + ")");
      i++;
    }
  }

  gMatrixConfigurationDone = true;
}

//****************************************************************************//
// Rollover test and Display the pixel matrix
//****************************************************************************//
void rollDisplayPixelMatrix(int mx, int my,
                            float tx, float ty,
                            float tw, float th)
{
  if (false == gMatrixConfigurationDone)
  {
    return;
  }

  fill(cColorPixelMatrixFilling);

  stroke(cColorPixelMatrixBorder);
  strokeWeight(cPixelMatrixBorderStrokeWidth);

  rect(tx, ty, tw, th);

  for (Map.Entry me : gDropablePixel.entrySet())
  {
    Dropable d = (Dropable)me.getValue();
    d.rollover(mx, my);
    // alle dropablePixel zeichnen
    d.display();
  }
}

//****************************************************************************//
// Test it the left Mouse button was released over one pixel of the pixel matrix
//****************************************************************************//
Dropable dropOverPixelInMatrix(int mx, int my)
{
  if (false == gMatrixConfigurationDone)
  {
    return null;
  }

  for (Map.Entry me : gDropablePixel.entrySet())
  {
    // pr?fen wo und ob gedropt wurde
    Dropable d = (Dropable)me.getValue();
    Dropable ret = d.drop(mx, my);
    // pr?fen ob ein drop auf einem Dropable durchgef?hrt wurde
    if (ret != null)
    {
      // zur?ck geben des objektes auf das gedropt wurde
      return d;
    }
  }

  // es wurde kein Drop ausgef�hrt
  return null;
}

//----------------------------------------------------------------------------//
// Funktion zum markieren von Pixeln in der PixelMatrix. Diese Funktion geht
// von einer 20x20 Matrix aus. Alle mit "1" markierten Array-Elemente werden
// in den Dropables dazu verwendet um herauszuginden welcher Pixel nur im 
// export mode verwendet werden kann. Die so gekennzeichneten Pixel sind
// entweder defekt oder sollt nicht verwendet werden da die Farbberechnungs-
// Routine nicht darauf ausgelegt ist.
//----------------------------------------------------------------------------//
void assignExpertModePixel()
{
  dbgPrintln("Reset pixelMatrix - export mode pixels - list");

  for (int i = 0; i < 400; i++)
  {
    cPixelMatrixExpertModePixels[i] = 0;
  }

  for (int i = 19; i < 400; i += 20)
  {
    // mark the right column
    cPixelMatrixExpertModePixels[i] = 1;
    dbgPrintln("Set pixel " + i + " in pixelMatrix to expert only");
  }

  for (int i = 380; i < 400; i++)
  {
    // mark the bottom row
    cPixelMatrixExpertModePixels[(i - 20)] = 1;
    cPixelMatrixExpertModePixels[i] = 1;
    dbgPrintln("Set pixel " + i + " and " + (i - 20) + " in pixelMatrix to expert only");
  }

  for (int x = 1; x < 18; x++)
  {
    // the rightest row is disabled in normal mode,
    // but the row left to the rightest row is enabled
    for (int y = 1; y < 17; y++)
    {
      // the position in the pixel array
      int pos = x + y * 20;
      // all inner pixel are disabled in normal mode, by default
      cPixelMatrixExpertModePixels[pos] = 1;
      dbgPrintln("Set pixel " + pos + " in pixelMatrix to expert only");
    }
  }
  
  dbgPrintln("Resulting pixelMatrix export mode mask");
  for (int x = 0; x < 20; x++)
  {
    // the rightest row is disabled in normal mode,
    // but the row left to the rightest row is enabled
    for (int y = 0; y < 20; y++)
    {
      // the position in the pixel array
      int pos = x + y * 20;
      // all inner pixel are disabled in normal mode, by default

      dbgPrint(cPixelMatrixExpertModePixels[pos] + " ");      
    }
    dbgPrintln("");
  }
}

