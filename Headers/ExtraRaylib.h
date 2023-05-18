#include <iostream> /// for strings and errors
#include <fstream>  /// for saving/loading data
#include <raylib.h> /// for GUI
#include <map>      /// for maps
#include <chrono>   /// for time
#include <string.h> /// for char functions
#include <climits>  /// for LONG_MAX
#include <locale>   /// for transformation between string and u16string
#include <codecvt>  /// for code conversion
/// Conversion functions:
int* u16_to_codepoint(std::u16string u16Str){
    int *result= new int(u16Str.size());
    int n = u16Str.size();
    for(int i=0;i<n;i++)
        result[i]=u16Str[i];
    return result;
}
std::u32string utf8_to_u32(std::string const& utf8Str) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cnv;
    std::u32string u32s = cnv.from_bytes(utf8Str);
    return u32s;
}
std::u16string utf8_to_u16(std::string const& utf8Str) {
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cnv;
    std::u16string u16s = cnv.from_bytes(utf8Str);
    return u16s;
}
std::string u16_to_utf8(const std::u16string& u16Str) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string utf8str = convert.to_bytes(u16Str);
    return utf8str;
}
char16_t flattenUTF16Char(char16_t c){
    /// function converts some diacritics to ASCII
    if(c < 190) return c; /// first 190 unicode characters don't have diacritics
    static const std::vector<std::u16string> diacritics = {u"aâäàáåãāăą",
    u"AÂÄÀÁÅÃĀĂĄ",
    u"sßšșş",
    u"SẞŠȘŞ",
    u"tţțť",
    u"TŢȚŤ",
    u"iîïìíīĩī",
    u"IÎÏÌÍĪĨ",
    u"\"„”"};
    int n = diacritics.size();
    for(int i=0;i<n;i++)
    {
        int m = diacritics[i].size();
        for(int j=0;j<m;j++)
            if(diacritics[i][j] == c)
                return diacritics[i][0]; ///first one is ascii.
    }
    return c;
}
/// Utility functions:
int getFileLineCount(std::ifstream &fin){
    int initPos = fin.tellg();
    fin.seekg(0);
    int lines = 0;
    while(fin.ignore(LONG_MAX, '\n'))
        lines++;
    fin.clear();
    fin.seekg(initPos);
    return lines;
}
int getFileLineCount(std::string filePath){
    std::ifstream fin(filePath);
    int result = getFileLineCount(fin);
    fin.close();
    return result;
}
void ignoreLines(std::ifstream &fin,int lineNr){
    while((lineNr--) > 0 )
        fin.ignore(LONG_MAX,'\n');
}
double distSquare(double x1,double y1,double x2,double y2){
    return (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2);
}
void align(float &coord,int startCoord,int length,float percent,int textSize){
    coord=startCoord + length*percent -textSize/2;
    if(coord + textSize > startCoord + length)
        coord = startCoord + length - textSize;
    if(coord<startCoord)
        coord=startCoord;
}
long long getTimeMS(){
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}
long long getTimeMCS(){
    using namespace std::chrono;
    return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

namespace ExtraRaylib
{
    /************************************
              DIVERSE FUNCTIONS
    ************************************/
    int cursorType;
    bool isShiftDown(){
        return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    }
    bool isControlDown(){
        return IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    }
    int getSpecialKeyDown(){
        for(int i=256;i<=301;i++)
            if(IsKeyDown(i))
                return i;
        ///these keys include: tab, enter, arrows
        for(int i=340;i<=347;i++)
            if(IsKeyDown(i))
                return i;
        ///these keys include: ALT, SHIFT, CTRL
        return 0;
    }
    bool isKeyHeldFor(int key,int durationMS){
        long long currentTime = getTimeMS();
        static std::map<int,long long> timeMap;
        if(IsKeyPressed(key))
            timeMap[key] = currentTime;
        if(!IsKeyDown(key))
        {
            timeMap[key] = -1;
            return false;
        }
        return (currentTime - timeMap[key] >= durationMS);
    }
    void drawTextureDest(Texture2D asset, Rectangle drawnPart, Rectangle destination,Color color){
        /// only way to scale an image seems to be a "nine patch", which can be considered
        /// overkill since I had to ignore some functionality of it that I don't fully understand.
        NPatchInfo ninePatchInfo = { drawnPart, 0, 0, 0, 0, NPATCH_NINE_PATCH };
        DrawTextureNPatch(asset, ninePatchInfo, destination, {0,0}, 0.0f, color);
    }
    float MeasureTextUnicode(Font font,std::u16string text,int font_size,float spacing){ /// Font must be monospaced!
        int length = text.size();
        if(length == 0) return 0.0f;
        return length * MeasureTextEx(font,"a",font_size,spacing).x;
    }
    void drawtextUnicode(Font monoFont, std::u16string text,Vector2 position, int font_size,
                         int spacing,Color color,Color backgroundColor){
        if(text.size() == 0) return;
        float charSize = MeasureTextEx(monoFont,"a",font_size,spacing).x;
        DrawRectangle(position.x,position.y,text.size() * charSize ,font_size,backgroundColor);
        while(!text.empty())
        {
            DrawTextCodepoint(monoFont,text[0],position,font_size,color);
            text.erase(0,1);
            position.x += charSize;
        }
    }

    /************************************
                TEXT CLASSES
    ************************************/
    struct Txt{
        Font *font; /// pointer so we can load it later
        float x,y,fontSize;
        char text[105];
        Color color,backgroundColor;
        Txt(){}
        Txt(Font *font,char const text[105],int x,int y,int fontSize,Color color,Color backgroundColor)
        :font(font),x(x),y(y),fontSize(fontSize),color(color),backgroundColor(backgroundColor){
            strcpy(this->text,text);
        }
        void draw(bool shouldUnderline=false)
        {
            Rectangle background = {x,y,MeasureTextEx(*font,text,fontSize,1).x,MeasureTextEx(*font,text,fontSize,1).y};
            DrawRectangleRec(background,backgroundColor);
            DrawTextEx(*font,text,{x,y},fontSize,1,color);

            if(shouldUnderline)
                underline();
        }
        void underline()
        {
            int len=MeasureTextEx(*font,text,fontSize,1).x;
            Vector2 init= {x,y+fontSize};
            Vector2 endp= {x+len,y+fontSize};
            DrawLineEx(init,endp,fontSize/10+1,color);
        }
    };
    struct TxtAligned : public Txt{
        float xPercent,yPercent;
        Rectangle *container;
        TxtAligned(){}
        TxtAligned(Font *font,char const text[105],Rectangle &container,int xPercent,int yPercent,int fontSize,Color color,Color backgroundColor)
        :Txt(font,text,container.x,container.y,fontSize,color,backgroundColor),
        xPercent(xPercent/100.0),yPercent(yPercent/100.0)
        {
            this->container = &container;
            this->align();
        }
        void align()
        {
            ::align(x,container->x,container->width,xPercent,MeasureTextEx(*font,text,fontSize,1).x);
            ::align(y,container->y,container->height,yPercent,fontSize);
        }
    };
    class BoxText
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
        std::string getText(){
            return u16_to_utf8(text);
        }
        BoxText():MAX_LINES(-1){}
        BoxText(std::u16string text, Rectangle box, int max_nr_lines,int font_size,Font *font)
        :text(text), rect(box), font_size(std::max(font_size,8)), MAX_LINES(max_nr_lines), font(font){
            rect.height = MAX_LINES * font_size;
        }
        void setSepText()
        {
            int sz = text.size();
            if(!sz) return;
            sepText.clear();
            sepText.emplace_back();
            int pos = 0;
            float lineSize = 0;
            while(pos < sz)
            {
                int wordLength;
                for(wordLength = 0;wordLength + pos < sz && text[wordLength+pos]!=' ';wordLength++);
                std::u16string addedWord = text.substr(pos,wordLength+1);
                float wordSize = MeasureTextUnicode(*font,addedWord,font_size,1);
                pos += wordLength + 1;
                if(wordSize > rect.width){/// words that can't fit in a line
                    bigWord(addedWord,lineSize);
                    continue;
                }
                lineSize+=wordSize;
                if(lineSize > rect.width)
                {
                    sepText.emplace_back();
                    lineSize = wordSize;
                }

                sepText.back()+=addedWord;
            }
            sepTextSize = sepText.size();
        }
        void bigWord(std::u16string addedWord,float &lineSize)
        {
            float letterSize = MeasureTextUnicode(*font,u"a",font_size,1);
            unsigned int letters = (rect.width - lineSize) / letterSize;
            sepText.back() += addedWord.substr(0,letters);
            addedWord.erase(0,letters);
            while(!addedWord.empty())
            {
                sepText.emplace_back();
                letters = rect.width / letterSize;
                if(letters > addedWord.size())
                {
                    letters = addedWord.size();
                    lineSize = letters * letterSize;
                }
                sepText.back() += addedWord.substr(0,letters);
                addedWord.erase(0,letters);
            }
        }
        void draw(int linePos = 0)
        {
            if(sepTextSize == 0)
                setSepText();
            DrawRectangleRec(rect,WHITE);
            for(int i=0;i<MAX_LINES && i+linePos<sepTextSize;i++)
                ExtraRaylib::drawtextUnicode(*font,sepText[i+linePos].c_str(),{rect.x,rect.y+i*font_size},font_size,1,BLACK,BLANK);
        }
    };


    /************************************
              DIVERSE CLASSES
    ************************************/

    struct Slider
    {
        float len = 100;
        float x,y,radius;
        Rectangle defaultRect;
        double prc = 0;
        bool isSel = false;
        Slider(float x,float y,float length,float radius):len(length),x(x),y(y),
        radius(radius)
        {defaultRect = {x,y,len,this->radius};}
        void run(float pixel)
        {
            x = defaultRect.x * pixel;
            y = defaultRect.y * pixel;
            len = defaultRect.width * pixel;
            radius = defaultRect.height * pixel;
            drag();
        }
        void drag()
        {
            int mouseX = GetMouseX();
            int mouseY = GetMouseY();
            if(IsMouseButtonPressed(0))
            {
                int r2 = radius * radius;
                int xCoord = x + prc*len;
                if(distSquare(mouseX,mouseY,xCoord,y) <= r2)
                    isSel = true;
                else
                    isSel = false;
            }
            if(!IsMouseButtonDown(0))
                isSel = false;
            if(!isSel)
                return;
            float sliderX = mouseX;
            sliderX = std::min(sliderX, x+len);
            sliderX = std::max(sliderX, x);
            prc = (sliderX - x)*1.0 / len;
        }
        void draw()
        {
            DrawRectangle(x-1,y-1,len+2,3,BLACK);
            DrawLine(x,y,x+len,y,WHITE);
            DrawCircle(x + prc*len, y, radius,WHITE);
            DrawCircle(x + prc*len, y, radius-1,BLACK);
        }
    };
    struct Choose_RGB
    {
        int x,y,width,height;
        Rectangle defaultRect;
        Color *directColor = nullptr;
        Slider Sr,Sg,Sb;
        Font *myFont;
        Choose_RGB(Font &font,float x,float y,float width,float height):
            x(x),y(y),width(width),height(height),
            Sr(x+10,(int)(y+0.25*height),width * 0.5,height * 0.1),
            Sg(x+10,(int)(y+0.50*height),width * 0.5,height * 0.1),
            Sb(x+10,(int)(y+0.75*height),width * 0.5,height * 0.1),
            myFont(&font)
            {defaultRect = {x,y,width,height};}
        void draw()
        {
            DrawRectangle(x,y,width,height,LIGHTGRAY);
            float left = Sr.x + Sr.len + Sr.radius;
            float right = x + width;
            float center = (left + right)/2;
            float radius = Sr.radius;
            Sr.draw();
            DrawTextEx(*myFont,"Red",{center - MeasureText("Red",radius)/2,Sr.y - radius * 0.7f},radius * 2,1,RED);
            Sg.draw();
            DrawTextEx(*myFont,"Green",{center - MeasureText("Green",radius)/2,Sg.y - radius * 0.7f},radius * 2,1,DARKGREEN);
            Sb.draw();
            DrawTextEx(*myFont,"Blue",{center - MeasureText("Blue",radius)/2,Sb.y - radius * 0.7f},radius * 2,1,DARKBLUE);
        }
        void setColor(Color &color)
        {
            directColor = &color;
            Sr.prc = (double)color.r/255;
            Sg.prc = (double)color.g/255;
            Sb.prc = (double)color.b/255;
        }
        void run(float pixel)
        {
            x = defaultRect.x * pixel;
            y = defaultRect.y * pixel;
            width = defaultRect.width * pixel;
            height = defaultRect.height * pixel;
            Sr.run(pixel);
            Sg.run(pixel);
            Sb.run(pixel);
            setRGBColor();
        }
        void setRGBColor()
        {
            if(directColor == nullptr)
                directColor = new Color;
            (*directColor) = {static_cast<unsigned char>(255 * Sr.prc),
                    static_cast<unsigned char>(255 * Sg.prc),
                    static_cast<unsigned char>(255 * Sb.prc),255};
        }
    };
    struct ScreenWrapper
    {
        virtual void run(){}
        virtual void draw(){}
    };


    struct Button
    {
        Font *font;
        std::string text;
        Rectangle rect;
        Rectangle padding = {0,0,0,0};
        struct ColorPair
        {
            Color text,background;
        }normalColor,hoverColor;
        int fontSize;
        float thickness;
        bool isHovering;
        bool forceHover;
        Button(){}
        Button(Font &font,std::string text,int startX,int startY,int fontSize,Color color,Color hoverColor)
        :font(&font),text(text),fontSize(fontSize)
        {
            isHovering=false;
            forceHover=false;
            thickness=std::max(1,fontSize/10);
            rect= {(float)startX,(float)startY, ///x,y
            (float)MeasureTextEx(*(this->font),text.c_str(),fontSize,1).x+10+thickness*2, ///width
            fontSize+10+thickness*2}; /// height

            (this->normalColor).text=color;
            (this->hoverColor).text=hoverColor;
            normalColor.background=(this->hoverColor).background=BLANK;
        }
        ColorPair getCurrentColor(int transparency)
        {
            ColorPair color;
            if(isHovering || forceHover)
                color=hoverColor;
            else
                color=normalColor;
            color.text.a=std::min(transparency,(int)color.text.a);
            color.background.a=std::min(transparency,(int)color.background.a);
            return color;
        }
        void draw(int transparency=255)
        {
            ColorPair color=getCurrentColor(transparency);
            DrawRectangleRec(rect,color.background);
            DrawRectangleLinesEx(rect,thickness,color.text);
            Vector2 position = {rect.x + 5 + thickness, rect.y + 5 + thickness};
            DrawTextEx(*font,text.c_str(),position,fontSize,1,color.text);
        }
        bool Lclicked()
        {
            return clicked(0);
        }
        bool Rclicked()
        {
            return clicked(1);
        }
        void align(int xPrc,int yPrc,Rectangle container)
        {
            ::align(rect.x,container.x,container.width,xPrc/100.0f,rect.width);
            ::align(rect.y,container.y,container.height,yPrc/100.0f,rect.height);
        }
        void re_measure()
        {
            rect.width=MeasureTextEx(*font,text.c_str(),rect.height-10 - thickness*2,1).x + 10 + thickness*2;
        }
        protected:
        bool clicked(int nr)
        {
            Vector2 mouse=GetMousePosition();
            if(forceHover && IsKeyPressed(KEY_ENTER))
            {
                forceHover=false;
                return true;
            }

            if (CheckCollisionPointRec(mouse, {
            rect.x-padding.x,
            rect.y-padding.y,
            rect.width+padding.width+padding.x,
            rect.height+padding.height+padding.y}))
            {
                isHovering=true;
                cursorType=MOUSE_CURSOR_POINTING_HAND;
                SetMouseCursor(cursorType);
                if(IsMouseButtonPressed(nr))
                    return true;
            }
            else
                isHovering=false;
            return false;
        }
    };
}
