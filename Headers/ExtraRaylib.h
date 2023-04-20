#include <iostream> /// for errors
#include <fstream>  /// for saving/loading data
#include <raylib.h> /// for GUI
#include <map>      /// for settings
#include <random>   /// for randomized colors
#include <chrono>   /// for time
#include <string.h> /// for char functions
#include <climits>
#include <locale>
#include <codecvt>
int getLineCount(std::ifstream &fin)
{
    int lines = 0;
    while(fin.ignore(LONG_MAX, '\n'))
        lines++ ;
    return lines;
}
int getLineCount(std::string filePath)
{
    std::ifstream fin(filePath);
    return getLineCount(fin);
}
std::u32string utf8_to_u32(std::string const& utf8) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cnv;
    std::u32string u32s = cnv.from_bytes(utf8);
    return u32s;
}
std::u16string utf8_to_u16(std::string const& utf8) {
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cnv;
    std::u16string u16s = cnv.from_bytes(utf8);
    return u16s;
}
std::string u16_to_utf8(const std::u16string& u16str) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string utf8str = convert.to_bytes(u16str);
    return utf8str;
}
void ignoreLines(std::ifstream &fin,int lineNr)
{
    while(lineNr--)
        fin.ignore(LONG_MAX,'\n');
}
namespace ExtraRaylib
{
    bool isShiftDown()
    {
        return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    }
    bool isControlDown()
    {
        return IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    }
    /// TO do: recode raylib.
    /************************************
              DIVERSE FUNCTIONS
    ************************************/
    int getSpecialKeyDown()
    {
        /// warning: slow function probably
        /// DO NOT QUESTION MASTER RAYLIB
        for(int i=256;i<=301;i++)
            if(IsKeyDown(i))
                return i;
        for(int i=340;i<=347;i++)
            if(IsKeyDown(i))
                return i;
        return -1;
    }
    void drawtextUnicode(Font font, std::u16string text,Vector2 position, int font_size,int spacing,Color color)
    {
        while(!text.empty())
        {
            DrawTextCodepoint(font,text[0],position,font_size,color);
            std::string a;
            a+=text[0];
            position.x += MeasureTextEx(font,a.c_str(),font_size,spacing).x;
            text.erase(0,1);
        }
    }
    float MeasureTextUnicode(Font font,std::u16string text,int font_size,float spacing)
    { /// Font must be monospaced!
        int length = text.size();
        if(length == 0) return 0.0f;
        return length * MeasureTextEx(font,"a",font_size,spacing).x;
    }
    long long getTimeMS()
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    }
    long long getTimeMCS()
    {
        using namespace std::chrono;
        return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
    }
    bool isKeyHeldFor(int key,int durationMS)
    {
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
    void align(float &coord,int startCoord,int length,float percent,int textSize)
    {
        coord=startCoord + length*percent -textSize/2;
        if(coord + textSize > startCoord + length)
            coord = startCoord + length - textSize;
        if(coord<startCoord)
            coord=startCoord;
    }

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
        Txt(Font *font,char const text[105],int x,int y,int fontSize,Color color)
        :font(font),x(x),y(y),fontSize(fontSize),color(color)
        {
            strcpy(this->text,text);
        }
        void draw(bool shouldUnderline=false)
        {
            DrawTextEx(*font,text,{x,y},fontSize,1,color);
            if(shouldUnderline)
                underline();
        }
        void underline()
        {
            int len=MeasureTextEx(*font,text,fontSize,1).x;
            Vector2 init= {x,y+fontSize};
            Vector2 endp= {x+len,y+fontSize};
            DrawLineEx(init,endp,fontSize/20+1,color);
        }
    };
    struct TxtAligned : public Txt
    {
        float xPercent,yPercent;
        Rectangle *container;
        TxtAligned(){}
        TxtAligned(Font *font,char const text[105],Rectangle &container,int xPercent,int yPercent,int fontSize,Color color)
        :Txt(font,text,container.x,container.y,fontSize,color),xPercent(xPercent/100.0),yPercent(yPercent/100.0)
        {
            this->container = &container;
            this->align();
        }
        void align()
        {
            ///void align(float &coord,int startCoord,int length,int percent,int textSize)
            ExtraRaylib::align(x,container->x,container->width,xPercent,MeasureTextEx(*font,text,fontSize,1).x);
            ExtraRaylib::align(y,container->y,container->height,yPercent,fontSize);
        }
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
        boxText(std::u16string text, Rectangle box, int max_nr_lines,int font_size,Font *font)
        :text(text), rect(box), font_size(std::max(font_size,8)), MAX_LINES(max_nr_lines), font(font){
            rect.height = MAX_LINES * font_size;
        }
        void setSepText()
        {
            /// probably overcomplicated
            int sz = text.size();
            if(!sz) return;
            int pos = 0;
            int nr = 0;
            sepText.emplace_back();
            while(pos < sz)
            {
                int extra = 0;
                for(extra = 0;extra + pos < sz && text[extra+pos]!=' ';extra++);
                std::u16string addedWord = text.substr(pos,extra+1);
                if(MeasureTextUnicode(*font,sepText[nr]+addedWord,font_size,1) > rect.width)
                    nr++,sepText.emplace_back();
                sepText[nr]+=addedWord;
                pos += extra + 1;
            }
            sepTextSize = sepText.size();
        }
        void draw(int linePos = 0)
        {
            if(sepTextSize == 0)
                setSepText();
            DrawRectangleRec(rect,WHITE);
            for(int i=0;i<MAX_LINES && i+linePos<sepTextSize;i++)
                ExtraRaylib::drawtextUnicode(*font,sepText[i+linePos].c_str(),{rect.x,rect.y+i*font_size},font_size,1,BLACK);
        }
    };
    void drawTextureDest(Texture2D asset, Rectangle drawnPart, Rectangle destination)
    {
        /// not entirely sure what all the parameters mean but seems to work.
        NPatchInfo ninePatchInfo = { drawnPart, 0, 0, 0, 0, NPATCH_NINE_PATCH };
        DrawTextureNPatch(asset, ninePatchInfo, destination, {0,0}, 0.0f, WHITE);
    }
    double distSquare(int x1,int y1, int x2,int y2)
    {
        return (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2);
    }

    /************************************
              DIVERSE CLASSES
    ************************************/

    struct Slider
    {
        int len = 100;
        int x,y,radius;
        double prc = 0;
        bool isSel = false;
        Slider(int x,int y,int length,int radius):len(length),x(x),y(y),radius(radius)
        {
            if(radius < 6) radius = 6;
            if(radius > 30) radius = 30;
        }
        void run()
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
            int sliderX = mouseX;
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
        Color *directColor = nullptr;
        Slider Sr,Sg,Sb;
        Choose_RGB(int x,int y,int width,int H):x(x),y(y),width(width),height(H),
        Sr(x+10,(int)(y+0.25*height),width * 0.7,height * 0.1),
        Sg(x+10,(int)(y+0.50*height),width * 0.7,height * 0.1),
        Sb(x+10,(int)(y+0.75*height),width * 0.7,height * 0.1){}
        void draw()
        {
            DrawRectangle(x,y,width,height,GRAY);
            Sr.draw();
            Sg.draw();
            Sb.draw();
        }
        void setColor(Color &color)
        {
            directColor = &color;
            Sr.prc = (double)color.r/255;
            Sg.prc = (double)color.g/255;
            Sb.prc = (double)color.b/255;
        }
        void run()
        {
            Sr.run();
            Sg.run();
            Sb.run();
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
}
