
#include "testApp.h"
#include "ofxOscSender.h"

//--------------------------------------------------------------
void testApp::setup() {
    ofBackground(0,0,0);
    
    w = 320;
    h = 240;
    
    movie.initGrabber(w, h, true);
    
    //reserve memory for cv images
    rgb.allocate(w, h);
    hsb.allocate(w, h);
    hue.allocate(w, h);
    sat.allocate(w, h);
    bri.allocate(w, h);
    filtered.allocate(w, h);
    
    sender.setup("127.0.0.1", 7400);
}

//--------------------------------------------------------------
void testApp::update(){
    
    movie.update();
    
    if (movie.isFrameNew()) {
        
        //copy webcam pixels to rgb image
        rgb.setFromPixels(movie.getPixels(), w, h);
        
        //mirror horizontal
        rgb.mirror(false, true);
        
        //duplicate rgb
        hsb = rgb;
        
        //convert to hsb
        hsb.convertRgbToHsv();
        
        //store the three channels as grayscale images
        hsb.convertToGrayscalePlanarImages(hue, sat, bri);
        
        //filter image based on the hue value were looking for
        for (int i=0; i<w*h; i++) {
            filtered.getPixels()[i] = ofInRange(hue.getPixels()[i],findHue-5,findHue+5) ? 255 : 0;
        }
        filtered.flagImageChanged();
        
        //run the contour finder on the filtered image to find blobs with a certain hue
        contours.findContours(filtered, 50, w*h/2, 1, false);
        //tracker.update(filtered, -1, 50, w*h/2, 1 );
        
    }
   
    //VELOCITY
    posDiffX = (v1.x - pos.x)/10;
    if((posDiffX > 1) || (posDiffX < 0)){
    cout << posDiffX << endl;

        //SEND VELOCITY
        ofxOscMessage x;
		x.setAddress("/1/fader1");
		x.addFloatArg(posDiffX);
		sender.sendMessage(x);
        
    }
    
    v1.set(pos.x);
    

    
}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(255,255,255);
    
    //draw all cv images
    rgb.draw(0,0);
    hsb.draw(640,0);
    hue.draw(0,240);
    sat.draw(320,240);
    bri.draw(640,240);
    filtered.draw(0,480);
    contours.draw(0,480);
    
    ofSetColor(255, 0, 0);
    ofFill();
    
    
    //draw red circles for found blobs
    for (int i=0; i<contours.nBlobs; i++) {
        
        ofCircle(contours.blobs[i].centroid.x, contours.blobs[i].centroid.y, 20);
        pos.x = contours.blobs[i].centroid.x;
        pos.y = contours.blobs[i].centroid.y;
        
        posSpeed = newPos.x - pos.x;
        
        //draw xPos
        string xposition = "xPos: "+ofToString(pos.x, 15);
        ofDrawBitmapString(xposition, 100, 100);
        
        //draw yPos
        string yposition = "yPos: "+ofToString(pos.y, 15);
        ofDrawBitmapString(yposition, 100, 125);

    
        
    }
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
    
    //calculate local mouse x,y in image
    int mx = x % w;
    int my = y % h;
    
    //get hue value on mouse position
    findHue = hue.getPixels()[my*w+mx];
}


