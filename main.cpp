#include <iostream> /// for errors
#include <fstream>  /// for saving/loading data
#include <raylib.h> /// for GUI
#include <map>      /// for settings
#include <random>   /// for randomized colors
#include <chrono>   /// for time
#include <string.h> /// for char functions
#include "Headers/ExtraRaylib.h"

namespace RayJump
{
    std::map<std::string,double> settings;
    int const fps = 60;
    Rectangle ScreenInfo; /// used to center
    Texture2D CAR_ASSET_STRUCTURE;
    Texture2D CAR_ASSET_COLOR;
    Font myFont;

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
            myFont = LoadFontEx("liberation_mono.ttf", 18*4, NULL, 1000);
            SetExitKey(0);
            CAR_ASSET_STRUCTURE = LoadTexture("Images/carStructure.png");
            CAR_ASSET_COLOR = LoadTexture("Images/carColor.png");
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
            std::ifstream fin("Text_files/settings.txt");
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
            std::ofstream fout("Text_files/settings.txt");
            if(!fout)return;
            for(std::map<std::string,double>::iterator it=settings.begin();it!=settings.end();it++)
                fout<<it->first<<' '<<it->second<<'\n';

        }
        void unload()
        {
            changeSettings();
            UnloadTexture(CAR_ASSET_STRUCTURE);
            UnloadTexture(CAR_ASSET_COLOR);
            CloseWindow();
        }
    };


    class Car
    {
        public:
        static int const START = 40;
        static int const END = 300;
        static int const WIDTH = 120; /// sprite width
        static int const HEIGHT = 48; /// sprite height

        float xPr = 0;
        float const y;
        float *screenWidth =nullptr;
        float roadWidth;

        Color color = {0,222,0,255};
        Car():y(0){}
        Car(float y,float &screenWidth):y(y),screenWidth(&screenWidth){}
        void run()
        {
            roadWidth = *screenWidth * 0.75;
        }
        bool rightClicked()
        {
            return IsMouseButtonDown(1) && CheckCollisionPointRec(GetMousePosition(),{getPos(),y,WIDTH,HEIGHT});
        }
        void setXPr(float prc)
        {
            xPr = prc;
        }
        float getPos()
        {
            return START + (roadWidth-WIDTH) * xPr;
        }
        void draw()
        {
            DrawTexture(CAR_ASSET_COLOR,getPos(),y,color);
            DrawTexture(CAR_ASSET_STRUCTURE,getPos(),y,WHITE);
        }
    }; ///non const statics need to be declared
    class Road
    {
        int const Ystart = 50;
        int Ylength = Car::HEIGHT + 5;
        int nrOfRoads = 1;

        int const Xstart = 40;
        public:
        std::vector <Car> cars;
        Road(int nr_of_cars)
        {
            nrOfRoads = nr_of_cars;
            for(int i=0;i<nr_of_cars;i++)
                cars.emplace_back(Ystart + i*Ylength,ScreenInfo.width);
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
            DrawRectangle(Xstart,Ystart,cars[0].roadWidth,nrOfRoads*Ylength,GRAY);
            int sz = cars.size();
            for(int i=0; i<sz; i++)
                cars[i].draw();
        }
    };

    class FeedbackText : public ExtraRaylib::boxText
    {
        int linePos = 0; /// line position in separated text
        int charPos = 0;
        int wpm = 0, accuracy = 10000;
        int total = 0;
        int maxChar,currentChar=0;
        std::u16string gr,re,bl;
        bool stop = false;
        bool stop2 = false;
        long long startTime = 0;
        int secondPassed = 0;
        public:
        FeedbackText():FeedbackText({0,0,0,0},u""){}
        FeedbackText(Rectangle rect,std::u16string text):boxText(text,rect,3,18,&myFont){}
        void setText(std::u16string text)
        {
            this->text = text;
            setSepText();
            restart();
        }
        void restart()
        {
            bl = sepText[0];
            linePos = charPos = currentChar = total = 0;
            maxChar = text.size();
            stop = stop2 =false;
            gr = re = u"";
            startTime = secondPassed = 0;
            wpm = 0;
            accuracy = 10000;
        }
        void run()
        {
            if(maxChar == 0) maxChar = text.size();
            if(maxChar == 0)
                return;
            calculate();
            if(startTime == 0 && currentChar !=0)
                startTime = ExtraRaylib::getTimeMS();
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
        float getPrc()
        {
            return 1.0f * currentChar / maxChar;
        }
        int getTime()
        {
            if(startTime == 0)return 0;
            return std::max(ExtraRaylib::getTimeMS() - startTime,1LL * 1);
        }
        void calculate()
        {
            if ( startTime == 0) return;
            if(linePos == sepTextSize - 1 && bl.empty())
            {
                if(stop2)
                    return;
                stop2 = true;
            }
            if(secondPassed == getTime()/1000 && !stop2)
                return;
            secondPassed ++ ;
            wpm = 1.0f*currentChar/5*60/std::max((1.0f*getTime()/1000),(float)1e-10);
            accuracy = 1.0f * currentChar / total * 10000;
        }
        int getWPM() {return wpm;}
        int getAccuracy() {return accuracy;}
        void addChr()
        {
            char16_t c;
            while((c = GetCharPressed()) && !stop)
            {
                ///std::cout << c << ' ' << sepText[linePos][charPos] << '\n';
                if(currentChar == 0 && c != sepText[linePos][charPos])
                    continue;
                if(c == sepText[linePos][charPos] && re.empty())
                {
                    gr+=bl[0];
                    currentChar++;
                }
                else
                    re+=bl[0];
                total ++ ;
                bl.erase(0,1);
                charPos++;
                nextLine();
            }
        }
        void nextLine()
        {
            if(!bl.empty())
                return;

            stop = true;
            if(!re.empty())
                return; /// current line has to not have mistakes
            if(linePos+1 != sepTextSize)
            {
                linePos++;
                charPos = 0;
                re = gr = u"";
                stop = false;
            }
        }
        void eraseChr()
        {
            if(!IsKeyDown(KEY_BACKSPACE) || re.empty())
                return;

            /// why is temp needed? because c++ said so.
            std::u16string temp;
            temp =re.back();
            bl.insert(0,temp);
            re.pop_back();
            charPos--;
            stop = false;
        }
        void drawDominantLine()
        {
            if(bl==u"" && !stop && linePos<sepTextSize)
                bl = sepText[linePos];
            ExtraRaylib::drawtextUnicode(*font,gr,{rect.x,rect.y},font_size,1,GREEN);
            ExtraRaylib::drawtextUnicode(*font,re,{rect.x + ExtraRaylib::MeasureTextUnicode(*font,gr,font_size,1),rect.y},font_size,1,RED);
            ExtraRaylib::drawtextUnicode(*font,bl,{rect.x + ExtraRaylib::MeasureTextUnicode(*font,(gr + re),font_size,1),rect.y},font_size,1,BLACK);
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
            ExtraRaylib::drawtextUnicode(*font, sepText[i+linePos],{rect.x,rect.y + font_size*i},font_size,1,BLACK);
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
            ExtraRaylib::TxtAligned title;
            ExtraRaylib::TxtAligned start;
            public:
            void init()
            {
                title = ExtraRaylib::TxtAligned(&myFont,"RayJump",ScreenInfo,50,20,18*3,BLACK);
                start = ExtraRaylib::TxtAligned(&myFont,"- Press any button to start -",ScreenInfo,50,70,18,RED);
            }
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
            FeedbackText fText = FeedbackText({50,200,300,100},u"suferință. Mircea Eliade");
            void run()
            {
                player->setXPr(fText.getPrc());
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
                DrawTextEx(myFont,std::to_string(fText.getWPM()).c_str(),{0,0},18,1,BLACK);
                std::string chestie = std::to_string(fText.getAccuracy());
                chestie.insert(chestie.size()-2,".");
                DrawTextEx(myFont,chestie.c_str(),{100,0},18,1,BLACK);
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
                title.init();
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
