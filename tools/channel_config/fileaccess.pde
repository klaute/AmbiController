/**
 * Author: Kai Lauterbach
 * License: GNU Public License - GPL.txt or http://www.gnu.org/licenses/gpl-3.0.html
 * Creation date: 02/2015
 */

import java.util.Map;

int[][] gTempChannelData = new int[5][2 * cChannelACMax + cChannelBMax + 2 * cChannelDEMax];

/*

 selectInput("Save a channel configuration file", "saveChannelConfigFile");
 
 selectInput("Open a channel configuration file", "openChannelConfigFile");
 
 */

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
boolean saveChannelConfigFile(File selection)
{
  return saveChannelConfigFile(selection, false);
}

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
boolean saveChannelConfigFile(File selection, boolean mirror)
{
  // 1. testen ob eine Datei selektiert wurde
  if (null == selection)
  {
    return false;
  }

  // 2. Anzahl der LEDs pro Channels herausfinden
  int ledNum  = (int)cp5.controller(cNumberboxName_LEDNumber).getValue();
  int ledNumA = (int)cp5.controller(cNumberboxName_ChannelA).getValue();
  int ledNumB = (int)cp5.controller(cNumberboxName_ChannelB).getValue();
  int ledNumC = (int)cp5.controller(cNumberboxName_ChannelC).getValue();
  int ledNumD = (int)cp5.controller(cNumberboxName_ChannelD).getValue();
  int ledNumE = (int)cp5.controller(cNumberboxName_ChannelE).getValue();

  int ledNumAB = ledNumA + ledNumB;
  int ledNumABC = ledNumAB + ledNumC;
  int ledNumABCD = ledNumABC + ledNumD;
  int ledNumABCDE = ledNumABCD + ledNumE;

  // 3. pr�fen ob die Summe der LEDs in den Channels gleich der Anzahl der LEDs ist
  if (ledNum <= 0 || ledNum != ledNumA + ledNumB + ledNumC + ledNumD + ledNumE)
  {
    // TODO hier Fehlermeldung ausgeben an Benutzer
    dbgPrintln("Invalid LED parameter, aborting save action");
    return false;
  }

  // 4. pr�fen ob die anzahl der relationen (LED<->Pixel) gleich der Anzahl der
  //    verf�gbaren LEDs ist
  if (null == gLEDRelations || ledNum != gLEDRelations.size())
  {
    // TODO hier Fehlermeldung ausgeben an Benutzer
    dbgPrintln("No LED to pixel relation given or incomplete relation set given, aborting save action");
    return false;
  }

  // 5. datei ��ffnen
  String fname = selection.getAbsolutePath();

  // 6. Relationen in Datei speichern (alles immer �berschreiben)
  //    Schleife �ber die relationen, sortiert nach der ID des in einer relation
  //    enthaltenen draggable objects mit aufsteigender nummerierung.
  String[] lines = new String[5];

  dbgPrintln("Generating channel config file data");

  // die hashMap zu einem sortiertem array umwandeln
  for (Map.Entry me : gLEDRelations.entrySet ())
  {
    Relation r = (Relation)me.getValue();
    // 6.1. hier die logik welche die einzelnen Strings der Channels erzeugt

    // liefert die LED-Nummer der Relation
    int dragID = r.getDraggable().getID();
    // liefert die Pixel-Nummer der Relation
    int dropID = r.getDropable().getID();

    int chan   = 0;
    int offset = 0;
    if (dragID < ledNumA)
    { // Channel A
      chan   = 0;
      offset = 0;
    } else if (dragID >= ledNumA && dragID < ledNumAB)
    { // Channel B
      chan   = 1;
      offset = ledNumA;
    } else if (dragID >= ledNumAB && dragID < ledNumABC)
    { // Channel C
      chan   = 2;
      offset = ledNumAB;
    } else if (dragID >= ledNumABC && dragID < ledNumABCD)
    { // Channel D
      chan   = 3;
      offset = ledNumABC;
    } else if (dragID >= ledNumABCD && dragID < ledNumABCDE)
    { // Channel E
      chan   = 4;
      offset = ledNumABCD;
    }
    // save the relation in the channel specific container
    gTempChannelData[chan][dragID - offset] = dropID;
    dbgPrintln("LED " + dragID + " => LED in channel " + (dragID - offset) +
               " in channel " + chan + " related to pixel " + dropID +
               " with offset " + offset);
  }

  // save the relation array data to the file strings array lines[]
  // Channel A
  if (true == mirror)
  {
    lines[0] = genChannelLineStr(0, ledNumC, true);
  } else {
    lines[0] = genChannelLineStr(0, ledNumA, false);
  }
  // Channel B
  lines[1] = genChannelLineStr(1, ledNumB, mirror);
  // Channel C
  if (true == mirror)
  {
    lines[2] = genChannelLineStr(2, ledNumA, true);
  } else {
    lines[2] = genChannelLineStr(2, ledNumC, false);
  }
  if (true == mirror)
  {
    // Channel D
    lines[3] = genChannelLineStr(3, ledNumE, true);
    // Channel E
    lines[4] = genChannelLineStr(4, ledNumD, true);
  } else {
    // Channel D
    lines[3] = genChannelLineStr(3, ledNumD, false);
    // Channel E
    lines[4] = genChannelLineStr(4, ledNumE, false);
  }

  // 7. daten speichern
  saveStrings(fname, lines);

  dbgPrintln("File " + fname + " written successfully");
  
  return true;
}

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
boolean openChannelConfigFile(File selection)
{
  // 1. return if no selection is done
  if (null == selection)
  {
    return false;
  }

  // drop the old/current data
  clearVisualizationData();
  
  int chan    = 0; // 0=A; 1=B; 2=C; 3=D; 4=E
  int ledNum  = 0;
  // 5 channels with 96 LEDs maximum, LEDs per channel maximum has to be
  // checked by software during read process
  int[][] channelData = new int[5][cChannelBMax];
  int[] channelLEDNum = {
    0, 0, 0, 0, 0
  };

  // 2. clear all previous generated objects
  dbgPrintln("Discarding current configuration and channel config data");

  // 3. open and read file
  String fname = selection.getAbsolutePath();
  dbgPrintln("Reading channel config file " + fname);
  String lines[] = loadStrings(fname);

  // 4. process channels each by each
  dbgPrintln("Processing channel config file data");

  // process the read lines
  for (String s1 : lines)
  {
    // remove the channel name from line
    String t1[] = split(s1, ":");
    if (t1[0].equals(cChannelNames[chan]))
    {
    }
    // split the channel data into separated numbers, separated by comma signs
    String t2[] = split(t1[1], ",");

    dbgPrint(cChannelNames[chan] + ":");

    for (String s2 : t2)
    {
      // hier pr�fen ob der zielwert korrekt ist: er muss kleiner sein als die
      // pixel matrix kantenl�nge quadriert

      // 4.1 hier pr�fen ob die maximall�ngen der LEDs/Channel nicht �berschritten werden
      if ((((chan == 0 || chan == 2) && channelLEDNum[chan] < cChannelACMax)  ||
           ((chan == 1)              && channelLEDNum[chan] <  cChannelBMax)  ||
           ((chan == 3 || chan == 4) && channelLEDNum[chan] < cChannelDEMax)) &&
          ledNum < cLEDStripeMaxLEDs)
      {
        dbgPrintln(">>"+s2+"<<");
        channelData[chan][channelLEDNum[chan]] = int(s2);

        channelLEDNum[chan]++;
        ledNum++;
        dbgPrint(channelData[chan][channelLEDNum[chan]-1] + " ");
      } // else ignore further LED information // TODO hier einmalig eine meldung ausgeben das das einlesen abrebrochen wird
    }
    dbgPrintln("");
    // all numbers in line proceeded, go on with the next line/channel
    chan++;
  }
  
  dbgPrintln("Setting the new configuration parameter tu the UI");
  // 5. set number of LEDs
  cp5.controller(cNumberboxName_LEDNumber).setValue(ledNum);
  
  // 6. set the LED numbers per channel
  cp5.controller(cNumberboxName_ChannelA).setValue(channelLEDNum[0]);
  cp5.controller(cNumberboxName_ChannelB).setValue(channelLEDNum[1]);
  cp5.controller(cNumberboxName_ChannelC).setValue(channelLEDNum[2]);
  cp5.controller(cNumberboxName_ChannelD).setValue(channelLEDNum[3]);
  cp5.controller(cNumberboxName_ChannelE).setValue(channelLEDNum[4]);
  
  // 7. use 20x20 pixel matrix and 60 LEDs per channel by default
  //cp5.controller(cNumberboxName_PixelMatrixEdgeLength)
  //   .setValue(cPixelMatrixEdgeLengthMax);

  dbgPrintln("Rebuild the graphical UI and datastructure components");
  // 8. generate the pixel matrix, the LEDStripe and the relations between all
  //    the draggables and the dropables
  initChannelConfigUIElements();

  int tLEDNum = 0;
  for (int i = 0; i < 5; i++)
  {
      for (int j = 0; j < channelLEDNum[i]; j++)
    {
      int drID = tLEDNum;
      int dpID = channelData[i][j];
      
      Draggable dr = gLEDStripe.get(""+drID);
      Dropable  dp = gDropablePixel.get(""+dpID);
      
      createRelation(dr, dp);
      tLEDNum++;
    }
  }

  return true;
}

//----------------------------------------------------------------------------//
// Generate one line which could be used to put into a channel config file
//----------------------------------------------------------------------------//
String genChannelLineStr(int chan, int ledNumMax)
{
  return genChannelLineStr(chan, ledNumMax, false);
}

//----------------------------------------------------------------------------//
// Generate one line which could be used to put into a channel config file
//----------------------------------------------------------------------------//
String genChannelLineStr(int chan, int ledNumMax, boolean mirror)
{
  String line;
  boolean first = true;
  line = cChannelNames[chan] + ":";

  if (true == mirror)
  {
    // invert the LED number order
    for (int i = ledNumMax-1; i == 0; i--)
    {
      if (true == first)
      {
        first = false;
      } else {
        line += ",";
      }
      line += "" + gTempChannelData[chan][i];
    }
  } else {
    for (int i = 0; i < ledNumMax; i++)
    {
      if (true == first)
      {
        first = false;
      } else {
        line += ",";
      }
      line += "" + gTempChannelData[chan][i];
    }
  }

  dbgPrintln(line);
  return line;
}

