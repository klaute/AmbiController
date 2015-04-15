/**
 * Author: Kai Lauterbach
 * License: GNU Public License - GPL.txt or http://www.gnu.org/licenses/gpl-3.0.html
 * Creation date: 02/2015
 */

import java.util.Map;

boolean gLEDStripeConfigurationDone = false;

HashMap<String, Draggable> gLEDStripe = new HashMap<String, Draggable>();

//****************************************************************************//
// Build the LEDStripe HashMap
//****************************************************************************//
void genLEDStripe(float ca, float cb, float cc, float cd, float ce,
                  float tx, float ty, float tw, float th)
{
  // berechnung der LED-Abmessungen, des Bereiches in dem die LEDs gezeichnet werden sollen

  float LEDStripeLeftMargin  = tx;
  float LEDStripeTopMargin   = ty;
  float LEDStripeSpaceWidth  = tw;
  float LEDStripeSpaceHeight = th;
  
  float LEDStripeLEDWidth  = 0;
  float LEDStripeLEDHeight = 0;

  float LEDStripeLEDWH = 0;

  dbgPrintln("LEDStripeLeftMargin = "  +  LEDStripeLeftMargin);
  dbgPrintln("LEDStripeTopMargin = "   +   LEDStripeTopMargin);
  dbgPrintln("LEDStripeSpaceWidth = "  +  LEDStripeSpaceWidth);
  dbgPrintln("LEDStripeSpaceHeight = " + LEDStripeSpaceHeight);

  float LEDStripeSpaceBorder = (LEDStripeSpaceWidth * cLEDStripeSpaceBorderPercent) / 100;
  float LEDStripeLEDBorder   = (LEDStripeSpaceWidth *   cLEDStripeLEDBorderPercent) / 100;
  LEDStripeLEDWH = 10;

  dbgPrintln("LEDStripeSpaceBorder = " + LEDStripeSpaceBorder);
  dbgPrintln("LEDStripeLEDBorder = "   +   LEDStripeLEDBorder);

  // berechnen der breite und h�he der LED-Draggable-Objekte
  float tdiv;
  tdiv = ca + 2; // need 2 more elements (1 for each edge as a addetive border
  LEDStripeLEDWidth = (LEDStripeSpaceWidth  - 2 * LEDStripeSpaceBorder - (tdiv - 1) * LEDStripeLEDBorder) / tdiv;

  tdiv = ca + 2; // need 2 more elements (1 for each edge as a addetive border
  LEDStripeLEDHeight = (LEDStripeSpaceHeight - 2 * LEDStripeSpaceBorder - (tdiv - 1) * LEDStripeLEDBorder) / tdiv;

  // Channel A
  dbgPrintln("Generating channel A draggables");
  for (int i = 0; i < (int)ca; i++)
  {
    float x = LEDStripeLeftMargin + LEDStripeSpaceBorder;
    float y = LEDStripeTopMargin + LEDStripeSpaceHeight - LEDStripeSpaceBorder - (i + 2) * LEDStripeLEDHeight - (i + 1) * LEDStripeLEDBorder;
    float w = LEDStripeLEDWH;
    float h = LEDStripeLEDHeight;
    Draggable d = new Draggable(i, x, y, w, h);
    gLEDStripe.put(""+d, d);
    dbgPrintln("New draggable: " + d + " x=" + x + " y=" + y + " w=" + w + " h=" + h);
  }
  // Channel B
  tdiv = cb + 2; // need 2 more elements (1 for each edge as a addetive border
  LEDStripeLEDWidth = (LEDStripeSpaceWidth  - 2 * LEDStripeSpaceBorder - (tdiv - 1) * LEDStripeLEDBorder) / tdiv;
  dbgPrintln("Generating channel B draggables");
  for (int i = (int)ca; i < (int)(cb + ca); i++)
  {
    int ti = i - (int)ca;
    float x = LEDStripeLeftMargin + LEDStripeSpaceBorder + (ti + 1) * LEDStripeLEDWidth + (ti + 1) * LEDStripeLEDBorder;
    float y = LEDStripeTopMargin + LEDStripeSpaceBorder;
    float w = LEDStripeLEDWidth;
    float h = LEDStripeLEDWH;
    Draggable d = new Draggable(i, x, y, w, h);
    gLEDStripe.put(""+d, d);
    dbgPrintln("New draggable: " + d + " x=" + x + " y=" + y + " w=" + w + " h=" + h);
  }
  // Channel C
  tdiv = cc + 2;
  LEDStripeLEDHeight = (LEDStripeSpaceHeight - 2 * LEDStripeSpaceBorder - (tdiv - 1) * LEDStripeLEDBorder) / tdiv;
  dbgPrintln("Generating channel C draggables");
  for (int i = (int)cb + (int)ca; i < (int)cc + (int)cb + (int)ca; i++)
  {
    int ti = i - (int)(cb + ca);
    float x = LEDStripeLeftMargin + LEDStripeSpaceWidth - LEDStripeSpaceBorder - LEDStripeLEDWH;
    float y = LEDStripeTopMargin + LEDStripeSpaceBorder + (ti + 1) * LEDStripeLEDHeight + (ti + 1) * LEDStripeLEDBorder;
    float w = LEDStripeLEDWH;
    float h = LEDStripeLEDHeight;
    Draggable d = new Draggable(i, x, y, w, h);
    gLEDStripe.put(""+d, d);
    dbgPrintln("New draggable: " + d + " x=" + x + " y=" + y + " w=" + w + " h=" + h);
  }

  // Calculate the new LEDWidth
  tdiv = cd + ce + 2;
  LEDStripeLEDWidth = (LEDStripeSpaceWidth - 20 - 2 * LEDStripeSpaceBorder - (tdiv - 1) * LEDStripeLEDBorder) / tdiv;
  // Channel D
  dbgPrintln("Generating channel D draggables");
  for (int i = (int)(cc + cb + ca); i < (int)(cd + cc + cb + ca); i++)
  {
    int ti = i - (int)(cc + cb + ca);
    float x = LEDStripeLeftMargin + LEDStripeSpaceWidth - LEDStripeSpaceBorder - (ti + 2) * LEDStripeLEDWidth - (ti + 1) * LEDStripeLEDBorder;
    float y = LEDStripeTopMargin + LEDStripeSpaceHeight - LEDStripeSpaceBorder - LEDStripeLEDWH;
    float w = LEDStripeLEDWidth;
    float h = LEDStripeLEDWH;
    Draggable d = new Draggable(i, x, y, w, h);
    gLEDStripe.put(""+d, d);
    dbgPrintln("New draggable: " + d + " x=" + x + " y=" + y + " w=" + w + " h=" + h);
  }
  // Channel E
  dbgPrintln("Generating channel E draggables");
  for (int i = (int)(ce + cd + cc + cb + ca - 1); i >= (int)(cd + cc + cb + ca); i--)
  {
    int ti = (int)ce - (i - (int)(cd + cc + cb + ca));
    float x = LEDStripeLeftMargin + LEDStripeSpaceBorder + (ti) * LEDStripeLEDWidth + (ti) * LEDStripeLEDBorder;
    float y = LEDStripeTopMargin + LEDStripeSpaceHeight - LEDStripeSpaceBorder - LEDStripeLEDWH;
    float w = LEDStripeLEDWidth;
    float h = LEDStripeLEDWH;
    Draggable d = new Draggable(i, x, y, w, h);
    gLEDStripe.put(""+d, d);
    dbgPrintln("New draggable: " + d + " x=" + x + " y=" + y + " w=" + w + " h=" + h);
  }

  gLEDStripeConfigurationDone = true;
}

//****************************************************************************//
// Rollover test and Display the LED Stripe data
//****************************************************************************//
void rollDragDisplayLEDStripe(int mx, int my, float tx, float ty, float tw, float th)
{
  if (false == gLEDStripeConfigurationDone)
  {
    return;
  }

  for (Map.Entry me : gLEDStripe.entrySet())
  {
    Draggable d = (Draggable)me.getValue();
    d.rollover(mx, my);
    d.drag(mx, my);
    d.display();
  }

  noFill();
  stroke(cColorLEDStripeBorder);
  strokeWeight(cLEDStripeBorderStrokeWeight);
  rect(tx, ty, tw, th);
}

//****************************************************************************//
// Test if the left mouse mutton has been released over a LED
//****************************************************************************//
Draggable stopDraggingLEDStripe(int mx, int my)
{
  if (false == gLEDStripeConfigurationDone)
  {
    return null;
  }

  for (Map.Entry me : gLEDStripe.entrySet())
  {
    Draggable d = (Draggable)me.getValue();
    if (null != d.stopDragging(mx, my))
    {
      return d;
    }
  }

  return null;
}

//****************************************************************************//
// Test if the left mouse mutton has been clicked over a LED
//****************************************************************************//
void clickedLEDStripe(int mx, int my)
{
  if (false == gLEDStripeConfigurationDone)
  {
    return;
  }

  for (Map.Entry me : gLEDStripe.entrySet())
  {
    Draggable d = (Draggable)me.getValue();
    d.clicked(mx, my);
  }

}

