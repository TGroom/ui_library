#include "TemplateWS.h"

std::map<std::string, Button*> TemplateWS::Buttons;

TemplateWS::TemplateWS(Boundary* mContainer, ModelType model) : model(model), mContainer(mContainer) {
	TextRenderer.Load(G_ResourcePath + "/fonts/arial.ttf", 12);

	//Buttons["file"] = new Button(&TextRenderer, "File", "", SIMPLE, CENTER_ALIGN, ENABLED, 0, 0, 30);
	//Buttons["new"] = new Button(&TextRenderer, "New", "file_new", SIMPLE, LEFT_ALIGN, ENABLED, 0, 0, 60);

}

// Gets called before drawing and is used to define callbacks for mouse events
void TemplateWS::Register() {

}


// Gets called whenever a viewport workspace needs to be drawn with the pixel dimentions of the window as parameters
void TemplateWS::Draw(){
	mPrim.Rect(mContainer->x, mContainer->y, mContainer->width, mContainer->height, 0);
	mPrim.SetColour(BACKDROP_COLOUR);
	mPrim.Draw();
		
	mPrim.Rect(mContainer->x, mContainer->y, mContainer->width, HEADER_2_HEIGHT, 0);
	mPrim.SetColour(HEADER_2_COLOUR);
	mPrim.Draw();

	//Buttons["file"]->SetPos(mContainer->x + mContainer->width - 36, mContainer->y + 3);
	//Buttons["new"]->SetPos(mContainer->x + 36, mContainer->y + 3);
	//Buttons["file"]->Draw();
	//Buttons["new"]->Draw();
}