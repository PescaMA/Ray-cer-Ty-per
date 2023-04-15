#ifndef EXTRA_RAYLIB_H
#define EXTRA_RAYLIB_H

#include <raylib.h>
#include <iostream>
#include <vector>
namespace ExtraRaylib
{
    /// TO do: recode raylib.
    /************************************
              DIVERSE FUNCTIONS
    ************************************/
    int getSpecialKeyDown();
    void drawtextUnicode(Font font, std::u16string text,Vector2 position, int font_size,int spacing,Color color);
    float MeasureTextUnicode(Font font,std::u16string text,int font_size,float spacing);
    long long getTimeMS();
    long long getTimeMCS();
    bool isKeyHeldFor(int key,int durationMS);
    void align(float &coord,int startCoord,int length,float percent,int textSize);

    /************************************
                TEXT CLASSES
    ************************************/

    struct Txt
    {
        Font *font; /// pointer so we can load it later
        float x,y,fontSize;
        char text[105];
        Color color;
        Txt(){}
        Txt(Font *font,char const text[105],int x,int y,int fontSize,Color color);
        void draw(bool shouldUnderline=false);
        void underline();
    };
    struct TxtAligned : public Txt
    {
        float xPercent,yPercent;
        Rectangle *container;
        TxtAligned(){}
        TxtAligned(Font *font,char const text[105],Rectangle &container,int xPercent,int yPercent,int fontSize,Color color);
        void align();
    };
    class boxText
    {
        protected:
        std::u16string text;
        std::vector<std::u16string> sepText;
        Rectangle rect;
        int font_size;
        int const MAX_LINES;
        Font *font;
        int sepTextSize = 0;
        public:
        boxText():MAX_LINES(-1){}
        boxText(std::u16string text, Rectangle box, int max_nr_lines,int font_size,Font *font);
        void setSepText();
        void draw(int linePos = 0);
    };
    void drawTextureDest(Texture2D asset, Rectangle drawnPart, Rectangle destination);
    double distSquare(int x1,int y1, int x2,int y2);

    /************************************
              DIVERSE CLASSES
    ************************************/

    struct Slider
    {
        int len = 100;
        int x,y,radius;
        double prc = 0;
        bool isSel = false;
        Slider(int x,int y,int length,int radius);
        void run();
        void draw();
    };
    struct Choose_RGB
    {
        int x,y,width,height;
        Color *directColor = nullptr;
        Slider Sr,Sg,Sb;
        Choose_RGB(int x,int y,int width,int H);
        void draw();
        void setColor(Color &color);
        void run();
        void setRGBColor();
    };
    struct ScreenWrapper
    {
        virtual void run(){}
        virtual void draw(){}
    };
}
#endif // EXTRA_RAYLIB_H
