/**
 * Author: Kai Lauterbach
 * License: GNU Public License - GPL.txt or http://www.gnu.org/licenses/gpl-3.0.html
 * Creation date: 02/2015
 */

import controlP5.*;
import java.text.*;

ControlP5 cp5;

boolean gLastExpertMode = false;

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
void setupConfigurationInputSystem()
{
  cp5 = new ControlP5(this);
  
  PFont p = createFont("Arial", 13);
  cp5.setControlFont(p);

  
  float configPanelElementYSpace = 60; // muss berechnet werden

  float configPanelMiddle = cConfigPanelWidth / 2;
  
  float numberboxWidth  = configPanelMiddle - 2 * cConfigPanelBorder;
  float numberboxHeight = height / 15;
  float toggleWidth     = configPanelMiddle - 2 * cConfigPanelBorder;
  float toggleHeight    = height / 15;

  dbgPrintln("Creating ControlP5 UI elements");
    
  float posy = cConfigPanelBorder;
  // input
  // -----
  // Checkbox f?r Expert-Mode
  cp5.addToggle(cToggleName_expertToggle, false,
                (int)cConfigPanelBorder, (int)posy,
                (int)toggleWidth, (int)toggleHeight);
  // Checkbox f?r Debug-Mode
  cp5.addToggle(cToggleName_debugToggle, gDebugMode,
                (int)cConfigPanelBorder + configPanelMiddle, (int)posy,
                (int)toggleWidth, (int)toggleHeight);
  
  // Eingabefeld f?r Matrix-Kantenl?nge (gesperrt)
  posy += configPanelElementYSpace;
  float tEdgeLenPosY = posy;
  cp5.addNumberbox(cNumberboxName_PixelMatrixEdgeLength, cPixelMatrixEdgeLengthMax,
                   (int)cConfigPanelBorder, (int)posy,
                   (int)numberboxWidth, (int)numberboxHeight)
     .setRange((float)cPixelMatrixEdgeLengthMin, (float)cPixelMatrixEdgeLengthMax);
  cp5.controller(cNumberboxName_PixelMatrixEdgeLength).hide();

  // Eingabefeld f?r die Anzahl der LEDs/Meter
  posy += configPanelElementYSpace;
  float tLEDPerMeterPosY = posy;
  cp5.addNumberbox(cNumberboxName_LEDPerMeter, cDefaultLEDStripeLEDPerMeter,
                   (int)cConfigPanelBorder, (int)posy,
                   (int)numberboxWidth, (int)numberboxHeight)
     .setRange((float)cLEDsPerMeterMinLEDs, (float)cLEDsPerMeterMaxLEDs);

  // Eingabefeld f?r LED-Anzahl
  posy += configPanelElementYSpace;
  float tLEDNumPosY = posy;
  cp5.addNumberbox(cNumberboxName_LEDNumber, cDefaultLEDStripeLEDs,
                   (int)cConfigPanelBorder, (int)posy,
                   (int)numberboxWidth, (int)numberboxHeight)
     .setRange((float)cLEDStripeMinLEDs, (float)cLEDStripeMaxLEDs).hide();

  // Eingabefelder f?r Channels (A-E)
  posy += configPanelElementYSpace;
  cp5.addNumberbox(cNumberboxName_ChannelA, cDefaultLEDStripeChannelALEDs,
                   (int)cConfigPanelBorder, (int)posy,
                   (int)numberboxWidth, (int)numberboxHeight)
     .setRange((float)cChannelMin, (float)cChannelACMax);
  posy += configPanelElementYSpace;
  cp5.addNumberbox(cNumberboxName_ChannelB, cDefaultLEDStripeChannelBLEDs,
                   (int)cConfigPanelBorder, (int)posy,
                   (int)numberboxWidth, (int)numberboxHeight)
     .setRange((float)cChannelMin,  (float)cChannelBMax);
  posy += configPanelElementYSpace;
  cp5.addNumberbox(cNumberboxName_ChannelC, cDefaultLEDStripeChannelCLEDs,
                   (int)cConfigPanelBorder, (int)posy,
                   (int)numberboxWidth, (int)numberboxHeight)
     .setRange((float)cChannelMin, (float)cChannelACMax);
  posy += configPanelElementYSpace;
  cp5.addNumberbox(cNumberboxName_ChannelD, cDefaultLEDStripeChannelDLEDs,
                   (int)cConfigPanelBorder, (int)posy,
                   (int)numberboxWidth, (int)numberboxHeight)
     .setRange((float)cChannelMin, (float)cChannelDEMax);
  posy += configPanelElementYSpace;
  cp5.addNumberbox(cNumberboxName_ChannelE, cDefaultLEDStripeChannelELEDs,
                   (int)cConfigPanelBorder, (int)posy,
                   (int)numberboxWidth, (int)numberboxHeight)
     .setRange((float)cChannelMin, (float)cChannelDEMax);

  // buttons
  cp5.addButton(cButtonName_Reset)
     .setPosition(100, 400)
     .setSize(80, 25);

  cp5.addButton(cButtonName_OpenFile)
     .setPosition(100, 430)
     .setSize(80, 25);

  cp5.addButton(cButtonName_SaveFile)
     .setPosition(100, 460)
     .setSize(80, 25);

  // view only elements
  // ------------------
  posy += configPanelElementYSpace;
  cp5.addTextlabel(cTextlabelName_LEDStripeLength,
                   cText_Length
                   + (cp5.controller(cNumberboxName_LEDNumber).value() /
                      cp5.controller(cNumberboxName_LEDPerMeter).value())
                   + cText_Meter,
                   (int)cConfigPanelBorder + (int)configPanelMiddle, (int)tLEDPerMeterPosY+15);
  cp5.addTextlabel(cTextlabelName_LEDNumber,
                   cText_LEDs + (int)cDefaultLEDStripeLEDs,
                   (int)cConfigPanelBorder, (int)tLEDNumPosY+15);
  cp5.addTextlabel(cTextlabelName_LEDStripeNonRelated,
                   cText_NonRelated + (int)cDefaultLEDStripeLEDs,
                   (int)cConfigPanelBorder + (int)configPanelMiddle, (int)tLEDNumPosY+15);
  cp5.addTextlabel(cTextlabelName_PixelMatrixEdgeLength, ""+(int)cPixelMatrixEdgeLengthMax,
                   (int)cConfigPanelBorder, (int)tEdgeLenPosY+15);
}

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
void updateConfigurationInfo()
{
  if (gLastExpertMode != gExpertMode)
  {
    gLastExpertMode = gExpertMode;
    if (true == gExpertMode)
    {
      // matrix size is allowed to change
      cp5.controller(cNumberboxName_PixelMatrixEdgeLength).show();
      cp5.controller(cTextlabelName_PixelMatrixEdgeLength).hide();
    } else {
      // matrix size is not allowed to change
      cp5.controller(cNumberboxName_PixelMatrixEdgeLength).hide();
      cp5.controller(cTextlabelName_PixelMatrixEdgeLength).show();
    }
  }

  int tmpLEDNumValue = (int)(cp5.controller(cNumberboxName_ChannelA).value() +
                             cp5.controller(cNumberboxName_ChannelB).value() +
                             cp5.controller(cNumberboxName_ChannelC).value() +
                             cp5.controller(cNumberboxName_ChannelD).value() +
                             cp5.controller(cNumberboxName_ChannelE).value());
  cp5.controller(cTextlabelName_LEDNumber)
     .setStringValue(cText_LEDs +
                     tmpLEDNumValue);
  cp5.controller(cNumberboxName_LEDNumber).setValue(tmpLEDNumValue);

  // Label f?r L?nge des Streifen
  DecimalFormat df = new DecimalFormat("#0.000");
  String tmpLen = df.format(cp5.controller(cNumberboxName_LEDNumber).value()
                        / cp5.controller(cNumberboxName_LEDPerMeter).value());
  cp5.controller(cTextlabelName_LEDStripeLength)
     .setStringValue(cText_Length
                     + tmpLen
                     + cText_Meter);

  // Label f?r unverbundene LEDs
  cp5.controller(cTextlabelName_LEDStripeNonRelated)
     .setStringValue(cText_NonRelated
                     + (int)(  cp5.controller(cNumberboxName_LEDNumber).value()
                             - gLEDRelations.size()));

  // TODO hier die plausibilität prüfen der eingaben und der anzeige
}

void controlEvent(ControlEvent theEvent)
{
  if (true == theEvent.isController())
  {
    // -----------------------
    // --- Export Checkbox ---
    if (theEvent.controller().getName() == cToggleName_expertToggle)
    {
      gExpertMode = theEvent.controller().getValue() == 0.0 ? false : true;
      dbgPrintln("Expert mode enabled = " + gExpertMode);
    }

    // ----------------------
    // --- Debug Checkbox ---
    if (theEvent.controller().getName() == cToggleName_debugToggle)
    {
      gDebugMode = theEvent.controller().getValue() == 0.0 ? false : true;
      dbgPrintln("Debug mode enabled = " + gDebugMode);
    }
    
    if (theEvent.controller().getName() == cNumberboxName_PixelMatrixEdgeLength)
    {
      gMatrixEdgeLength = (int)theEvent.controller().getValue();
    }
    
    if (theEvent.controller().getName() == cButtonName_Reset)
    {
      clearVisualizationData();
      initChannelConfigUIElements();
      dbgPrintln("Rebuild channel config elements");
    }
    
    if (theEvent.controller().getName() == cButtonName_OpenFile)
    {
        selectInput("Open a channel configuration file", "openChannelConfigFile");
    }

    if (theEvent.controller().getName() == cButtonName_SaveFile)
    {
        selectInput("Save a channel configuration file", "saveChannelConfigFile");
    }
  }
}

