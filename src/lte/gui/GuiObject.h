/*
 * GuiObject.h
 *
 *  Created on: Sep 28, 2015
 *      Author: marian
 */

#ifndef GUIOBJECT_H_
#define GUIOBJECT_H_

namespace ns3 {

class GuiObject {
public:
	GuiObject();
	virtual ~GuiObject();
	virtual void draw()=0;
	virtual void draw(float r,float g,float b)=0;
};

} /* namespace ns3 */
#endif /* GUIOBJECT_H_ */
