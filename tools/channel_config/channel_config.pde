/**
 * Author: Kai Lauterbach
 * License: GNU Public License - GPL.txt or http://www.gnu.org/licenses/gpl-3.0.html
 * Creation date: 02/2015
 */

//-----------------
// Window specific
//-----------------
final int cWindowWidth  = 800;
final int cWindowHeight = 600;

//------------------
// Relation specific
//------------------
final color cColorNormalRelation      = color(100, 100, 200);
final color cColorHighlightedRelation = color(150, 150, 250);

final int cNormalRelationStrokeWidth      = 3;
final int cHighlightedRelationStrokeWidth = 5;

//-------------------
// Draggable specific
//-------------------
final color cColorDraggableRelation = color(100, 100, 128);
final color cColorDraggableRelated  = color(100, 100, 128);
final color cColorDraggableDragging = color(155, 149, 146);
final color cColorDraggableRollover = color(150, 150, 250);
final color cColorDraggableNormal   = color(175, 175, 175);

final int   cColorDraggableRelationWidth = 3;

//------------------
// Dropable specific
//------------------
final color cColorDropableInactive         = color( 50, 50, 50);
final color cColorDropableRollover         = color(150, 150, 250);
final color cColorDropable                 = color(175, 175, 175);
final color cColorExpertModeDropable       = color( 78, 78, 78);
final color cColorExpertModeDropableStroke = color(120, 40, 40);
final color cColorDropableStroke           = color(165, 165, 165);
final color cColorDropableRolloverStroke   = cColorDropableRollover;

final int cDropableStrokeWeight                   = 1;
final int cExpertModeDropableStrokeWeightRollover = 3;
final int cExpertModeDropableStrokeWeight         = 1;

//---------------------
// PixelMatrix specific
//---------------------
final int cPixelMatrixEdgeLengthMin =  8;
final int cPixelMatrixEdgeLengthMax = 20; // 20x20 pixel maximum is fixed

final color cColorPixelMatrixFilling = color( 50, 50, 100);
final color cColorPixelMatrixBorder  = color(110, 110, 210);

final int cPixelMatrixBorderStrokeWidth = 1;

final float cPixelMatrixBorderPercent = 2; // %
final float cPixelBorderPercent       = 0.75; // %

final float cPixelMatrixX = 255;
final float cPixelMatrixY =  75;
final float cPixelMatrixW = 470;
final float cPixelMatrixH = 420;

//-------------------
// LEDStripe specific
//-------------------
final String[] cChannelNames = {
  "A", "B", "C", "D", "E"
};

final float cLEDStripeX = 220;
final float cLEDStripeY =  35;
final float cLEDStripeW = 540;
final float cLEDStripeH = 500;

final int cLEDsPerMeterMinLEDs =   1;
final int cLEDsPerMeterMaxLEDs = 144;

final int cChannelMin    =  0;
final int cChannelACMax  = 64;
final int cChannelBMax   = 96;
final int cChannelDEMax  = 48;

final int cDefaultLEDStripeLEDPerMeter = 60;

final int cLEDStripeMinLEDs = 1;
final int cLEDStripeMaxLEDs = 2 * cChannelACMax + cChannelBMax + 2 * cChannelDEMax;

final int cDefaultLEDStripeLEDs         = 120;
final int cDefaultLEDStripeChannelALEDs =  24;
final int cDefaultLEDStripeChannelBLEDs =  40;
final int cDefaultLEDStripeChannelCLEDs =  24;
final int cDefaultLEDStripeChannelDLEDs =  16;
final int cDefaultLEDStripeChannelELEDs =  16;

final float cLEDStripeSpaceBorderPercent = 0.5;
final float cLEDStripeLEDBorderPercent   = 0.5;

final color cColorLEDStripeBorder = color(110, 110, 210);

final int cLEDStripeBorderStrokeWeight = 1;

//-----------------------------
// Configuration panel specific
//-----------------------------
final float cConfigPanelBorder     = 10;
final float cConfigPanelInnerSpace = 20;
final float cConfigPanelWidth      = (cWindowWidth * 20) / 100;

final String cText_NonRelated = "Unconnected: ";
final String cText_LEDs       =       "LED's: ";
final String cText_Length     =      "Length: ";
final String cText_Meter      =            " m";

final String cNumberboxName_PixelMatrixEdgeLength = "PixelMatrixEdgeLength";
final String cNumberboxName_LEDPerMeter           = "LEDspermeter";
final String cNumberboxName_LEDNumber             = "LEDNumber";
final String cNumberboxName_ChannelA              = "ChannelA";
final String cNumberboxName_ChannelB              = "ChannelB";
final String cNumberboxName_ChannelC              = "ChannelC";
final String cNumberboxName_ChannelD              = "ChannelD";
final String cNumberboxName_ChannelE              = "ChannelE";

final String cButtonName_Reset    = "reset";
final String cButtonName_OpenFile = "openfile";
final String cButtonName_SaveFile = "savefile";

final String cTextlabelName_PixelMatrixEdgeLength = "PixelMatrixEdgeLengthLabel";
final String cTextlabelName_LEDNumber             = "LEDNumberLabel";
final String cTextlabelName_LEDStripeLength       = "LEDStripeLength";
final String cTextlabelName_LEDStripeNonRelated   = "LEDStripeNonRelated";

final String cToggleName_expertToggle = "expert";
final String cToggleName_debugToggle  = "debug";

//--------
// globals
//--------
boolean gExpertMode = false;
boolean gDebugMode  = false;

int gMatrixEdgeLength = cPixelMatrixEdgeLengthMax;

//****************************************************************************//
// Global setup method
//****************************************************************************//
void setup()
{
  colorMode(RGB, 255);

  size(cWindowWidth, cWindowHeight);  // size always goes first!

  if (frame != null)
  {
    frame.setResizable(false);
  }

  smooth();

  assignExpertModePixel();

  setupConfigurationInputSystem();

  initChannelConfigUIElements();
}

//****************************************************************************//
// Global draw method
//****************************************************************************//
void draw()
{
  background(50, 50, 50); // hintergrund l�schen

  /*stroke(0,255,0);
   strokeWeight(1);
   rect(100,100,12,16);*/

  // update the shown information in the left configuration/info section
  updateConfigurationInfo();

  // rollover testing and displaying the pixel matrix
  rollDisplayPixelMatrix(mouseX, mouseY, 
  cPixelMatrixX, // x
  cPixelMatrixY, // y
  cPixelMatrixW, // w
  cPixelMatrixH); // h

  // rollover testing and displaying the relations between draggables and dropables
  rollDisplayRelation(mouseX, mouseY);

  // rollover and drag testing and displaying the LED stripe data
  rollDragDisplayLEDStripe(mouseX, mouseY, 
  cLEDStripeX, // x
  cLEDStripeY, // y
  cLEDStripeW, // w
  cLEDStripeH); // h
  
  //dbgPrintln(""+frameRate);
}

//****************************************************************************//
// Global mouse pressed method
//****************************************************************************//
void mousePressed()
{
  if (mouseButton == LEFT)
  {
    clickedLEDStripe(mouseX, mouseY);
  }

  if (mouseButton == RIGHT && mouseEvent.getClickCount() == 2)
  {
    // double click with the right mouse button
    removeRelation(mouseX, mouseY);
  }
}

void mouseReleased()
{
  Draggable draggable = stopDraggingLEDStripe(mouseX, mouseY);

  Dropable dropable = dropOverPixelInMatrix(mouseX, mouseY);

  createRelation(draggable, dropable);
}

//****************************************************************************//
//
//****************************************************************************//
void initChannelConfigUIElements()
{
  genPixelMatrix(gMatrixEdgeLength, 
                 cPixelMatrixX, // x
                 cPixelMatrixY, // y
                 cPixelMatrixW, // w
                 cPixelMatrixH); // h

  genLEDStripe(cp5.controller(cNumberboxName_ChannelA).value(), 
               cp5.controller(cNumberboxName_ChannelB).value(), 
               cp5.controller(cNumberboxName_ChannelC).value(), 
               cp5.controller(cNumberboxName_ChannelD).value(), 
               cp5.controller(cNumberboxName_ChannelE).value(), 
               cLEDStripeX, // x
               cLEDStripeY, // y
               cLEDStripeW, // w
               cLEDStripeH); // h
}

//****************************************************************************//
//
//****************************************************************************//
void clearVisualizationData()
{
  // vllt auslagern in die drei dateien welche f�r die koordination zust�ndig sind
  gLEDStripe     = new HashMap<String, Draggable>();
  gLEDRelations  = new HashMap<String, Relation>();
  gDropablePixel = new HashMap<String, Dropable>();

  gLEDStripeConfigurationDone = false;
  gMatrixConfigurationDone    = false;
}

//****************************************************************************//
// println encapsulation to prevent stdout printing without having debug mode enabled
//****************************************************************************//
void dbgPrintln(String s)
{
  if (true == gDebugMode)
  {
    println(s);
  }
}

void dbgPrint(String s)
{
  if (true == gDebugMode)
  {
    print(s);
  }
}

