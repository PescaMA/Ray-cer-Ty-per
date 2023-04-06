#include <iostream> /// for errors
#include <fstream>  /// for saving/loading data
#include <raylib.h> /// for GUI
#include <map>      /// for settings
#include <random>   /// for randomized colors
#include <chrono>   /// for time
#include <string.h> /// for char functions
namespace ExtraRaylib
{
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
        std::string text;
        std::vector<std::string> sepText;
        Rectangle rect;
        int font_size;
        int const MAX_LINES;
        Font *font;
        int sepTextSize = 0;
        public:
        boxText():MAX_LINES(-1){}
        boxText(std::string text, Rectangle box, int max_nr_lines,int font_size,Font *font)
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
                std::string addedWord = text.substr(pos,extra+1);
                if(MeasureTextEx(*font,(sepText[nr]+addedWord).c_str(),font_size,1).x > rect.width)
                    nr++,sepText.emplace_back();
                sepText[nr]+=addedWord;
                pos += extra + 1;
            }
            sepTextSize = sepText.size();
        }
        void draw(int linePos = 0)
        {
            /*if(sepTextSize == 0)
                setSepText();
            DrawRectangleRec(rect,WHITE);
            for(int i=0;i<MAX_LINES && i+linePos<sepTextSize;i++)
                DrawText(sepText[i+linePos].c_str(),rect.x,rect.y+i*font_size,font_size,BLACK);*/
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
namespace RayJump
{
    std::map<std::string,double> settings;
    int const fps = 60;
    Rectangle ScreenInfo; /// used to center
    class Car
    {
        public:
        static int const START = 40;
        static int const END = 300;
        static int const WIDTH = 120; /// sprite width
        static int const HEIGHT = 48; /// sprite height

        float xPoz = START;
        float const y;

        Color color = {0,222,0,255};
        static Texture2D ASSET_STRUCTURE;
        static Texture2D ASSET_COLOR;
        Car():y(0){}
        Car(float y):y(y){}
        void run()
        {
        }
        bool rightClicked()
        {
            return IsMouseButtonDown(1) && CheckCollisionPointRec(GetMousePosition(),{xPoz,y,WIDTH,HEIGHT});
        }
        void draw()
        {
            DrawTexture(ASSET_COLOR,START,y,color);
            DrawTexture(ASSET_STRUCTURE,START,y,WHITE);
        }
    }; ///non const statics need to be declared
    Texture2D Car::ASSET_STRUCTURE;
    Texture2D Car::ASSET_COLOR;
    class Road
    {
        int const Ystart = 50;
        int Ylength = Car::HEIGHT + 5;
        int nrOfRoads = 1;

        int const Xstart = 40;
        int const width = 300;
        public:
        std::vector <Car> cars;
        Road(int nr_of_cars)
        {
            nrOfRoads = nr_of_cars;
            for(int i=0;i<nr_of_cars;i++)
                cars.emplace_back(Ystart + i*Ylength);
        }
        void run()
        {
            int sz = cars.size();
            for(int i=0; i<sz; i++)
                cars[i].run();
        }
        int getCarRightClicked()
        {
            int sz = cars.size();
            for(int i=0; i<sz; i++)
                if(cars[i].rightClicked())
                    return i;
            return -1;
        }
        void draw()
        {
            DrawRectangle(Xstart,Ystart,width,nrOfRoads*Ylength,GRAY);
            int sz = cars.size();
            for(int i=0; i<sz; i++)
                cars[i].draw();
        }
    };
    Font myFont;
    class FeedbackText : public ExtraRaylib::boxText
    {
        int linePos = 0;
        int charPos = 0;
        std::string gr,re,bl;
        bool stop = false;
        public:
        FeedbackText():FeedbackText({0,0,0,0},""){}
        FeedbackText(Rectangle rect,std::string text):boxText(text,rect,3,18,&myFont){}
        void setText(std::string text)
        {
            this->text = text;
            setSepText();
            restart();
        }
        void restart()
        {
            bl = sepText[0];
            linePos = 0;
            charPos = 0;
            stop = false;
            gr = re = "";
        }
        void run()
        {
            if(text.size() == 0)
                return;
            eraseChr();
            if(linePos == sepTextSize - 1 && bl.empty())
            {
                if(IsKeyDown(KEY_ENTER))
                    restart();
                return;
            }
            if(stop)
                return;
            addChr();
        }
        void addChr()
        {
            char c;
            while((c = GetCharPressed()))
            {
                if(c != sepText[linePos][charPos] || !re.empty())
                    re+=bl[0];
                else
                    gr+=bl[0];
                bl.erase(0,1);
                charPos++;
                if(bl.empty())
                {
                    if(!re.empty())
                    {
                        stop = true;
                        break;
                    }


                    linePos++;
                    if(linePos == sepTextSize)
                        linePos--,stop = true;
                        else
                            re=gr="";
                    charPos=0;

                }
            }
        }
        void eraseChr()
        {
            if(!IsKeyDown(KEY_BACKSPACE) || re.empty())
                return;

            /// why is temp needed? because c++ said so.
            std::string temp;
            temp =re.back();
            bl.insert(0,temp);
            re.pop_back();
            charPos--;
            stop = false;
        }
        void drawDominantLine()
        {
            if(bl=="" && !stop && linePos<sepTextSize)
                bl = sepText[linePos];
            DrawTextEx(*font,gr.c_str(),{rect.x,rect.y},font_size,1,GREEN);
            DrawTextEx(*font,re.c_str(),{rect.x + MeasureTextEx(*font,gr.c_str(),font_size,1).x,rect.y},font_size,1,RED);
            DrawTextEx(*font,bl.c_str(),{rect.x + MeasureTextEx(*font,(gr + re).c_str(),font_size,1).x,rect.y},font_size,1,BLACK);
        }
        void draw()
        {
            if(text.size() == 0)
                return;
            DrawRectangleRec(rect,YELLOW);
            if(sepTextSize == 0)
                setSepText();
            drawDominantLine();
            for(int i=1;i<MAX_LINES && i+linePos < sepTextSize;i++)
            DrawTextEx(*font, sepText[i+linePos].c_str(),{rect.x,rect.y + font_size*i},font_size,1,BLACK);
        }
    };

    class Loader
    {
        public:
        void load()
        {
            srand(ExtraRaylib::getTimeMS()); /// setting the random seed to current time

            SetConfigFlags(FLAG_WINDOW_RESIZABLE);
            defaultSettings();
            readSettings();
            InitWindow(ScreenInfo.width,ScreenInfo.height,"Rayjump");
            myFont = LoadFontEx("liberation_mono.ttf", 18*4, NULL, -1);
            SetExitKey(0);
            Car::ASSET_STRUCTURE = LoadTexture("Images/carStructure.png");
            Car::ASSET_COLOR = LoadTexture("Images/carColor.png");
            SetTargetFPS(fps);
        }
        void defaultSettings()
        {
            settings["ScreenWidth"] = 960;
            settings["ScreenHeight"] = 540;
            settings["PlayerR"] = 0;
            settings["PlayerG"] = 0;
            settings["PlayerB"] = 200;

            ScreenInfo = {0,0,(float)settings["ScreenWidth"],(float)settings["ScreenHeight"]};
        }
        void readSettings()
        {
            std::ifstream fin("settings.txt");
            if(!fin)
                return;
            std::string name;
            double val;
            while(fin>>name>>val)
                settings[name]=val;
            ScreenInfo = {0,0,(float)settings["ScreenWidth"],(float)settings["ScreenHeight"]};
        }
        void changeSettings()
        {
            std::ofstream fout("settings.txt");
            if(!fout)return;
            for(std::map<std::string,double>::iterator it=settings.begin();it!=settings.end();it++)
                fout<<it->first<<' '<<it->second<<'\n';

        }
        void unload()
        {
            changeSettings();
            UnloadTexture(Car::ASSET_STRUCTURE);
            UnloadTexture(Car::ASSET_COLOR);
            CloseWindow();
        }
    };
    namespace ScreenManaging
    {
        Car *player;
        class ScreenStuff
        {
            public:
            enum screens{Sgame,SRGBpick,Stitle} now = Sgame;
            void setScreen(screens screen)
            {
                now = screen;
            }
            ExtraRaylib::ScreenWrapper* getScreen();
        }currentScreen;
        class veryUsefulAndProfessionalTitleScreenWithJustTheName : public ExtraRaylib::ScreenWrapper
        {
            ExtraRaylib::TxtAligned title = ExtraRaylib::TxtAligned(&myFont,"RayJump",ScreenInfo,50,20,18*3,BLACK);
            ExtraRaylib::TxtAligned start = ExtraRaylib::TxtAligned(&myFont,"- Press any button to start -",ScreenInfo,50,70,18,RED);
            void run()
            {
                title.align();
                start.align();
                if(GetKeyPressed() || ExtraRaylib::getSpecialKeyDown()!=-1)
                    currentScreen.setScreen(ScreenStuff::screens::Sgame);
            }
            void draw()
            {
                ClearBackground(RAYWHITE);
                title.draw();
                start.draw();

            }
        }title;
        class NormalGame : public ExtraRaylib::ScreenWrapper
        {
            public:
            Road roads = Road(2);
            FeedbackText fText = FeedbackText({50,200,300,100},"true that's the problem.....");
            void run()
            {
                fText.run();
                roads.run();
                int nr = roads.getCarRightClicked();
                if(nr == 0) /// we can only custimize the first car (our own)
                {
                    currentScreen.setScreen(ScreenStuff::screens::SRGBpick);
                }
            }
            void draw()
            {
                roads.draw();
                fText.draw();
            }
        } game;
        class ColorPicker : public ExtraRaylib::ScreenWrapper
        {
            public:
            ExtraRaylib::Choose_RGB RGBcolor = ExtraRaylib::Choose_RGB(30,200,60,60);
            void setColor(Color& color)
            {
                RGBcolor.setColor(color);
            }
            void run()
            {
                RGBcolor.run();
                if(IsKeyDown(KEY_ESCAPE))
                    currentScreen.setScreen(ScreenStuff::screens::Sgame);

            }
            void draw()
            {
                player->draw();
                RGBcolor.draw();
            }
        } colorPicker;
        ExtraRaylib::ScreenWrapper* ScreenStuff::getScreen()
        {
            ExtraRaylib::ScreenWrapper *screen;
            if(now == Sgame) screen = &game;
            if(now == SRGBpick) screen = &colorPicker;
            if(now == Stitle) screen = &title;

            return screen;
        }
        class MainLoop
        {
        public:
            static void run()
            {
                currentScreen.setScreen(ScreenStuff::screens::Stitle);
                player = &game.roads.cars[0];
                loadSettings();
                colorPicker.setColor(player->color);
                while(!WindowShouldClose())
                {
                    updateScreenVariables();
                    currentScreen.getScreen()->run();
                    BeginDrawing();
                        ClearBackground(RAYWHITE);
                        currentScreen.getScreen()->draw();
                    EndDrawing();
                    if(ExtraRaylib::isKeyHeldFor(KEY_ESCAPE,1500))
                        break;
                }
                saveSettings();
            }
            static void updateScreenVariables()
            {
                settings["ScreenWidth"] = GetScreenWidth();
                settings["ScreenHeight"] = GetScreenHeight();
                ScreenInfo = {0,0,(float)GetScreenWidth(),(float)GetScreenHeight()};
            }
            static void saveSettings()
            {
                settings["PlayerR"] = player->color.r;
                settings["PlayerG"] = player->color.g;
                settings["PlayerB"] = player->color.b;
            }
            static void loadSettings()
            {
                 player->color.r = settings["PlayerR"];
                 player->color.g = settings["PlayerG"];
                 player->color.b = settings["PlayerB"];
            }
        };
    }
}
int main()
{
    RayJump::Loader loader;
    loader.load();
    RayJump::ScreenManaging::MainLoop::run();
    loader.unload();
}
