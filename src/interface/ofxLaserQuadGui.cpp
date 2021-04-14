//
//  QuadGui.cpp
//
//  Created by Seb Lee-Delisle on 17/08/2015.
//
//

#include "ofxLaserQuadGui.h"
using namespace ofxLaser;

QuadGui::QuadGui() {
    initListeners();
    
    visible = true;
    isDirty=true;
    selected = false;
    set(0,0,60,60);
    initialised = true;
    lineColour.set(255); 
    
}
QuadGui::~QuadGui() {
    if(initialised) removeListeners();
    
}

void QuadGui::setName (string displaylabel) {
    displayLabel = displaylabel;
}


void QuadGui::set (const ofRectangle& rect) {
    set(rect.x, rect.y, rect.getWidth(), rect.getHeight());
}

void QuadGui::set(float x, float y, float w, float h) {
    
    allHandles.clear();
    
    for(int i = 0; i<4; i++) {
        float xpos = ((float)(i%2)/1.0f*w)+x;
        float ypos = (floor((float)(i/2))/1.0f*h)+y;
        
        handles[i].set(xpos, ypos);
        allHandles.push_back(&handles[i]);
        
    }
    
    centreHandle.set(x + (w/2.0f), y+(h/2.0f));
    allHandles.push_back(&centreHandle);
    for(DragHandle* handle : allHandles) {
        handle->overCol = ofColor::magenta;
        handle->col = ofColor(255,0,255,128);
    }
    
}

void QuadGui::setConstrained(const ofRectangle &rect) {
    constrainRect = rect;
    constrained = true;
}

void QuadGui :: initListeners() {
    
    ofAddListener(ofEvents().mousePressed, this, &QuadGui::mousePressed, OF_EVENT_ORDER_BEFORE_APP);
    ofAddListener(ofEvents().mouseMoved, this, &QuadGui::mouseMoved, OF_EVENT_ORDER_BEFORE_APP);
    ofAddListener(ofEvents().mouseReleased, this, &QuadGui::mouseReleased, OF_EVENT_ORDER_BEFORE_APP);
    ofAddListener(ofEvents().mouseDragged, this, &QuadGui::mouseDragged, OF_EVENT_ORDER_BEFORE_APP);
    
}

void QuadGui :: removeListeners() {
    
    ofRemoveListener(ofEvents().mousePressed, this, &QuadGui::mousePressed, OF_EVENT_ORDER_BEFORE_APP);
    ofRemoveListener(ofEvents().mouseMoved, this, &QuadGui::mouseMoved, OF_EVENT_ORDER_BEFORE_APP);
    ofRemoveListener(ofEvents().mouseReleased, this, &QuadGui::mouseReleased, OF_EVENT_ORDER_BEFORE_APP);
    ofRemoveListener(ofEvents().mouseDragged, this, &QuadGui::mouseDragged, OF_EVENT_ORDER_BEFORE_APP);
    
}

void QuadGui :: draw() {
	
    if(!visible) {
        isDirty = false;
        return;
    }
    ofPushMatrix();
    ofTranslate(offset);
    ofScale(scale, scale);
    
	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(1);
	/*
	if(isDirty) {
		//updateHomography();
		ofSetColor(ofColor::red);
	}*/
    
    ofSetColor(lineColour);
    
	glm::vec3 p1 = glm::mix((glm::vec3)handles[0],(glm::vec3)handles[1], 0.1);
	glm::vec3 p2 = glm::mix((glm::vec3)handles[0],(glm::vec3)handles[2], 0.1);
    
    //if(selected) {
		
        drawDashedLine(handles[1], handles[3]);
        drawDashedLine(handles[3], handles[2]);
        drawDashedLine(handles[0], handles[1]);
        drawDashedLine(handles[2], handles[0]);
        
        
     //   drawDashedLine(p1,p2);
    //}
    
    
    if(!displayLabel.empty()) {
        float textwidth = displayLabel.size()*8;

        ofFill();
        ofPushMatrix();
        ofTranslate(handles[1]-ofPoint(textwidth+24,0));
        ofSetColor(0,150);
        
        ofDrawRectangle(0,0,textwidth+24,24);
        ofSetColor(255,0,255);

        ofDrawBitmapString(displayLabel, 6, 17);

        ofPopMatrix();
        ofPopStyle();
    }
    if(selected) {
        for(int i = 0; i<numHandles; i++) {
           // handles[i].scale = scale;
           
            handles[i].draw(mousePos, scale);
        }
        centreHandle.draw(mousePos, scale);
    }
    
  
	
	isDirty = false;
    ofPopMatrix();
}
void QuadGui::drawDashedLine(ofPoint p1, ofPoint p2) {
	UI::drawDashedLine(p1,p2);
}


bool QuadGui::checkDirty() {
	if(isDirty) {
		return true;
	} else {
		return false;
	}
}


void QuadGui :: startDragging(int handleIndex, glm::vec3 clickPos) {
	
	handles[handleIndex].startDrag(clickPos);
    
    int x = ((handleIndex%2)+1)%2;
    int y = handleIndex/2;
    
    int xhandleindex = x+(y*2);
    
    x = handleIndex%2;
    y = ((handleIndex/2)+1)%2;
    int yhandleindex = x+(y*2);
    
    handles[xhandleindex].startDrag(clickPos, false, true, !lockPerpendicular);
    handles[yhandleindex].startDrag(clickPos, true, false, !lockPerpendicular);
    
	
}
bool QuadGui :: hitTest(ofPoint mousePoint) {
    
    mousePoint-=offset;
    mousePoint/=scale;
    
    ofPolyline poly;

	poly.addVertex(handles[0]);
    poly.addVertex(handles[1]);
    poly.addVertex(handles[3]);
    poly.addVertex(handles[2]);

    poly.close();
    return(poly.inside(mousePoint));

}


bool QuadGui :: mousePressed(ofMouseEventArgs &e){
	
        
	if(!visible) return false;

    bool hit = hitTest(e);
    if((hit) &&(!selected)) {
        selected = true;
        return false;
    }
    
        
    if(!selected) {
        return false;
    }
    ofPoint mousePoint = e;
    mousePoint-=offset;
    mousePoint/=scale;
    
   
    
	bool handleHit = false;
	
    
	if(centreHandle.hitTest(mousePoint)) {
		
		centreHandle.startDrag(mousePoint);
		handleHit = true;
		for(int i = 0; i<numHandles; i++) {
			handles[i].startDrag(mousePoint);
		}
			
    
    } else {

		for(int i = 0; i<numHandles; i++) {
			if(handles[i].hitTest(mousePoint)) {
				startDragging(i, mousePoint);
				handleHit = true;
			}
		
		}
	}
	
    if(!handleHit && !hit) {
        selected = false;
    }
	return handleHit;
	
}
bool QuadGui :: mouseMoved(ofMouseEventArgs &e){
    mousePos = e;
    mousePos-=offset;
    mousePos/=scale;
    
}
bool QuadGui :: mouseDragged(ofMouseEventArgs &e){

	if(!visible) return false;
	if(!selected) return false;

    mousePos = e;
    mousePos-=offset;
    mousePos/=scale;
    
	ofRectangle bounds(centreHandle, 0, 0);
	bool dragging = false;
	for(int i = 0; i<numHandles; i++) {
        DragHandle& handle = handles[i];
        if(handle.updateDrag(mousePos)){
            dragging = true;
            if(constrained && (!constrainRect.inside(handle))) {
                handle.x = ofClamp(handle.x, constrainRect.getLeft(), constrainRect.getRight());
                handle.y = ofClamp(handle.y, constrainRect.getTop(), constrainRect.getBottom());
            }

            bounds.growToInclude(handle);
        }
	}
 	if(!dragging) {
		dragging = centreHandle.updateDrag(mousePos);
	} else {
		updateCentreHandle();
		
	}
	
	isDirty |= dragging;
	
	
	return dragging;
	
	
}


bool QuadGui :: mouseReleased(ofMouseEventArgs &e){
	
	if(!visible) return false;
	if(!selected) return false;

	
	bool wasDragging = false;
	
	for(int i = 0; i<allHandles.size(); i++) {
		if(allHandles[i]->stopDrag()) wasDragging = true;
	}
    if(wasDragging) {
//        if(!reversable) {
//            // TODO FIGURE OUT HOW TO UNWARP RECT!
//
//        }
        //saveSettings();
    }
	return wasDragging;
	
}



void QuadGui::updateCentreHandle() {
	centreHandle.set(0,0);
	for(int i = 0; i<4; i++) {
		centreHandle+=(glm::vec3)handles[i];
	}
	centreHandle/=4;
	//	centreHandle.set(bounds.getCenter());
}



void QuadGui::serialize(ofJson&json) {
	//ofSerialize(json,params);
	ofJson& handlesjson = json["quadgui"];
	for(int i = 0; i<4; i++) {
		DragHandle& pos = handles[i];
		handlesjson.push_back({pos.x, pos.y});
	}
	//cout << json.dump(3) << endl;
	//deserialize(json);
}

bool QuadGui::deserialize(ofJson& jsonGroup) {
	
	ofJson& handlejson = jsonGroup["quadgui"];
	
	for(int i = 0; i<4; i++) {
		ofJson& point = handlejson[i];
		handles[i].x = point[0];
		handles[i].y = point[1];
		handles[i].z = 0;
		
	}
    updateCentreHandle();
	return true; 
}

void QuadGui::setVisible(bool warpvisible) {
	
	visible = warpvisible;
	
}


