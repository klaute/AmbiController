/**
 * Author: Kai Lauterbach
 * License: GNU Public License - GPL.txt or http://www.gnu.org/licenses/gpl-3.0.html
 * Creation date: 02/2015
 */
// Copied from internet, don't know the URL/author

/**
  * PVector thePoint 
  * the point we will check if it is close to our line.
  *
  * PVector theLineEndPoint1 
  * one end of the line.
  *
  * PVector theLineEndPoint2
  * the second end of the line.
  *
  * int theTolerance 
  * how close thePoint must be to our line to be recogized.
  */
boolean pointInsideLine(PVector thePoint,
                        PVector theLineEndPoint1, 
                        PVector theLineEndPoint2, 
                        int theTolerance) {
                          
  PVector dir = new PVector(theLineEndPoint2.x,
                            theLineEndPoint2.y,
                            theLineEndPoint2.z);
  dir.sub(theLineEndPoint1);
  PVector diff = new PVector(thePoint.x, thePoint.y, 0);
  diff.sub(theLineEndPoint1);

  // inside distance determines the weighting 
  // between linePoint1 and linePoint2 
  float insideDistance = diff.dot(dir) / dir.dot(dir);

  if(insideDistance>0 && insideDistance<1) {
    // thePoint is inside/close to 
    // the line if insideDistance>0 or <1
    /*println( ((insideDistance<0.5) ? 
            "closer to p1":"closer to p2" ) + 
            "\t p1:"+nf((1-insideDistance),1,2)+
            " / p2:"+nf(insideDistance,1,2) );*/
            
    PVector closest = new PVector(theLineEndPoint1.x,
                                  theLineEndPoint1.y,
                                  theLineEndPoint1.z);
    dir.mult(insideDistance);
    closest.add(dir);
    PVector d = new PVector(thePoint.x, thePoint.y, 0);
    d.sub(closest);
    // println((insideDistance>0.5) ? "b":"a");
    float distsqr = d.dot(d);
    
    // check the distance of thePoint to the line against our tolerance. 
    return (distsqr < pow(theTolerance,2)); 
  }
  return false;
}
