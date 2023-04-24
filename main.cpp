#include <iostream> /// for errors and strings
#include <fstream>  /// for saving/loading data
#include "raylib.h" /// for brain damage
#include <map>      /// for settings
#include <random>   /// for randomizing
#include <chrono>   /// for time
#include <string.h> /// for char functions
#include <algorithm>/// for remove, erase
#include "Headers/ExtraRaylib.h"
#include "Headers/TexteHardCoded.h"
const extern CountryMap COUNTRY_TEXT;
const bool soTrue = true;
namespace RayJump
{
    CountryMap::const_iterator currentLanguage = COUNTRY_TEXT.begin();
    std::map<std::string,float> settings;
    int const fps = 60;
    Rectangle ScreenInfo; /// used to center
    Texture2D CAR_ASSET_STRUCTURE;
    Texture2D CAR_ASSET_COLOR;
    Font myFont;
    const char carNames[][30]={
        "<- Tu (joc curent)",
        "<- Jocul cel mai bun"
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
            SetWindowMinSize(500,375);
            BeginDrawing();
            ClearBackground(RAYWHITE);
            float width = std::min(ScreenInfo.width,ScreenInfo.height * 865 / 553);
            float height = std::min(ScreenInfo.height,ScreenInfo.width / 865 * 553);
            ExtraRaylib::drawTextureDest(
                LoadTexture("Images/title.png"),
                {0,0,865,553},
                {
                    ScreenInfo.width/2 - width/2,
                    ScreenInfo.height/2 - height/2,
                    width, height
                }
            );
            EndDrawing();
            myFont = LoadFontEx("liberation_mono.ttf", 18*4, NULL, 9000);
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
            settings["BestWPM"] = 10;
            settings["CopiedTextPos"] = -1;

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
            fin.close();
            ScreenInfo = {0,0,(float)settings["ScreenWidth"],(float)settings["ScreenHeight"]};
        }
        void changeSettings()
        {
            std::ofstream fout("Text_files/settings.txt");
            if(!fout)return;
            for(std::map<std::string,float>::iterator it=settings.begin();it!=settings.end();it++)
                fout<<(it->first)<<' '<<(it->second)<<'\n';
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
        int wpm = 15;
        int id;

        Color color = {0,222,0,255};
        Car():y(0){;}
        Car(float y,int id):y(y),id(id){;}
        void run()
        {true;}
        bool rightClicked()
        {
            return IsMouseButtonDown(1) && CheckCollisionPointRec(GetMousePosition(),{getPos(),y,WIDTH,HEIGHT});
        }
        float getPos()
        {
            int roadWidth = ScreenInfo.width - START*2;
            return START + std::max((roadWidth-WIDTH),0) * xPr;
        }
        void draw(bool isGameStarted = false)
        {
            DrawTexture(CAR_ASSET_COLOR,getPos(),y,color);
            DrawTexture(CAR_ASSET_STRUCTURE,getPos(),y,WHITE);
            if(!isGameStarted){
                DrawTextEx(myFont,carNames[id],{getPos() + WIDTH + 5,y + 15},18,1,BLACK);
            }
        }
    }; ///non const statics need to be declared
    class Road
    {
        int const Ystart = 50;
        int Ylength = Car::HEIGHT + 5;
        int const MAX_NR_OF_CARS = 2;
        int nrOfRoads = 1;
        int roadWidth;

        int const Xstart = 40;
        public:
        std::vector <Car> cars;
        Road(int nr_of_cars)
        {
            if(nr_of_cars > MAX_NR_OF_CARS)
                nr_of_cars = MAX_NR_OF_CARS;
            nrOfRoads = nr_of_cars;
            for(int i=0;i<nr_of_cars;i++)
                cars.emplace_back(Ystart + i*Ylength, i);
            if(nr_of_cars >= 2)cars[1].wpm = settings["BestWPM"];
        }
        void run()
        {
            roadWidth = ScreenInfo.width - Car::START*2;
            int sz = cars.size();
            for(int i=0; i<sz; i++)
                cars[i].run();
        }
        void setCompletionPrc(int timeMS, int characterCount)
        {
            int const timeCorrection = 12000; /// /1000 /60 * 5 = miliseconds to seconds to minutes; words to characters
            /// first car is the player and has variable wpm so we ignore it.
            for(int i=1;i<cars.size();i++)
            {
                float characters =  std::round(1.0f * cars[i].wpm * timeMS / timeCorrection);
                float prc = std::min(characters/characterCount, 1.0f); /// can't go more than 100%
                cars[i].xPr=prc;
            }
        }
        int getCarRightClicked()
        {
            int sz = cars.size();
            for(int i=0; i<sz; i++)
                if(cars[i].rightClicked())
                    return i;
            return -1;
        }
        void draw(bool isStarted = false)
        {
            DrawRectangle(Xstart,Ystart,roadWidth,nrOfRoads*Ylength,GRAY);
            int sz = cars.size();
            for(int i=0; i<sz; i++)
                cars[i].draw(isStarted);
        }
    };

    class FeedbackText : public ExtraRaylib::boxText
    {public:
        int linePos = 0; /// line position in separated text
        int charPos = 0;
        float wpm = 0;
        int accuracy = 10000;
        int total = 0;
        std::u16string gr,re,bl;
        bool stop = false;
        bool stop2 = false;
        int secondPassed = 0;
        long long startTime = 0;
        int maxChar,currentChar=0;
        bool finished = false;
        FeedbackText():FeedbackText({0,0,0,0},u""){}
        FeedbackText(Rectangle rect,std::u16string text):boxText(text,rect,3,18,&myFont){}
        void setText(std::u16string text)
        {
            this->text = text;
            sepText.clear();
            setSepText();
            restart();
        }
        void restart()
        {
            bl = sepText[0];
            linePos = charPos = currentChar = total = 0;
            maxChar = text.size();
            stop = stop2 = finished = false;
            gr = re = u"";
            startTime = secondPassed = 0;
            wpm = 0;
            accuracy = 10000;
        }
        void run()
        {
            if(finished) return;
            if(maxChar == 0) maxChar = text.size();
            if(maxChar == 0)
                return;
            calculate();
            eraseChr();
            if(startTime == 0 && currentChar !=0)
                startTime = ExtraRaylib::getTimeMS();
            if(linePos == sepTextSize - 1 && bl.empty() && re.empty())
            {
                if(IsKeyDown(KEY_ENTER))
                    finished = true;
                return;
            }
            handleKeyPress();
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
        bool isAtEnd(){
            return currentChar == maxChar;
        }
        void calculate()
        {
            if (!startTime || stop2) return;
            if(linePos == sepTextSize - 1 && bl.empty() && re.empty())
                stop2 = true; /// last calculation.
            if(secondPassed == getTime()/1000 && !stop2)
                return;
            secondPassed ++ ;
            wpm = 1.0f*currentChar/5*60/std::max((1.0f*getTime()/1000),(float)1e-10);
            accuracy = 1.0f * currentChar / total * 10000;
        }
        const char* getWPM()
        {
            static std::string result;
            result = (std::to_string((int)std::round(wpm)) + " wpm");
            return result.c_str();
        }
        const char* getAccuracy()
        {
            static std::string str;
            str = std::to_string(accuracy);
            str.insert(str.size()-2,".");
            str += "% acc";
            return str.c_str();
        }
        void handleKeyPress()
        {
            char16_t c;
            while((c = GetCharPressed()) && !stop)
            {
                c = flattenString(c);
                if(currentChar == 0 && c != flattenString(sepText[linePos][charPos]))
                    continue;
                if(c == flattenString(sepText[linePos][charPos]) && re.empty())
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
            Color LIGHT_GREEN = {115, 246, 155,255};
            Color LIGT_RED = {250, 181, 181, 255};
            ExtraRaylib::drawtextUnicode(*font,gr,{rect.x,rect.y},font_size,1,GREEN,BLANK);
            ExtraRaylib::drawtextUnicode(*font,re,{rect.x + ExtraRaylib::MeasureTextUnicode(*font,gr,font_size,1),rect.y},font_size,1,BLACK,LIGT_RED);
            ExtraRaylib::drawtextUnicode(*font,bl,{rect.x + ExtraRaylib::MeasureTextUnicode(*font,(gr + re),font_size,1),rect.y},font_size,1,BLACK,BLANK);
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
            ExtraRaylib::drawtextUnicode(*font, sepText[i+linePos],{rect.x,rect.y + font_size*i},font_size,1,BLACK,BLANK);
        }
    };
    class savedText
    {
    public:
        static std::u16string getText()
        {
            if(settings["CopiedTextPos"] < 1)
                return getRandomText();
            std::ifstream fin("Text_files/CopiedTxt.txt");
            if(!fin)
                return u"Eroare citire text copiat";
            int lines = getLineCount(fin);
            if(settings["CopiedTextPos"] > lines)
            {
                settings["CopiedTextPos"] = -1;
                fin.close();
                return getRandomText();
            }
            std::string copiedText;
            ignoreLines(fin,settings["CopiedTextPos"]-1);
            getline(fin,copiedText);
            settings["CopiedTextPos"]++;
            if(copiedText.empty())
                return u"Empty line.";
            return utf8_to_u16(copiedText);
        }
        static std::u16string getRandomText()
        {
            std::string randomTxt = (currentLanguage->second)[rand()%((currentLanguage->second).size())];
            return (utf8_to_u16(randomTxt));
        }
        static void saveClipboardText()
        {
            std::string copiedText = GetClipboardText();
            _formatClipboard(copiedText);
            settings["CopiedTextPos"] = 1;
            std::ofstream fout("Text_files/CopiedTxt.txt");
            fout << copiedText;
            fout.close();
        }
        static void _formatClipboard(std::string &text)
        {
            text.erase(remove(text.begin(), text.end(), '\r'), text.end());
            /// windows problem : adds carriage return (ascii 13 or \r).
            /// note: remove just moves the text to the left when finding the character

            char tok[text.size() + 5]; /// char array for strtok
            strcpy(tok,text.c_str());
            text.clear();
            char *p = strtok(tok,"\n");
            bool firstLine = true;
            while(p)
            {
                int n = strlen(p);
                bool goodFormat = false;
                int i;
                for(i=0;i<n;i++)
                    if(p[i] != ' ')
                    {
                        goodFormat = true; /// if line isn't filled with just spaces
                        break;
                    }
                if(goodFormat)
                {
                    if(firstLine)
                        firstLine = false;
                    else
                        text+='\n';
                    text+=(p+i);
                }
                p = strtok(NULL,"\n");
            }
        }
    };

    namespace ScreenManaging
    {
        Car *player;
        class ScreenStuff
        {
            public:
            enum screens{Sgame,SRGBpick,Stitle,Shelp} now = Sgame;
            void setScreen(screens screen)
            {
                now = screen;
            }
            ExtraRaylib::ScreenWrapper* getScreen();
        }currentScreen;
        class veryUsefulAndProfessionalTitleScreenWithJustTheName : public ExtraRaylib::ScreenWrapper
        {
            Texture2D img;
            const Rectangle imageRect = {0,0,865,553};
            const float aspectRatio = 1.0f * imageRect.width / imageRect.height;
            public:
            void init()
            {
                img  = LoadTexture("Images/title.png");
            }
            void run()
            {
                if(GetKeyPressed() || ExtraRaylib::getSpecialKeyDown()!=-1)
                    currentScreen.setScreen(ScreenStuff::screens::Sgame);
            }
            void draw()
            {
                ClearBackground(RAYWHITE);
                float width = std::min(ScreenInfo.width,ScreenInfo.height * aspectRatio);
                float height = std::min(ScreenInfo.height,ScreenInfo.width / aspectRatio);
                ExtraRaylib::drawTextureDest(
                    img,
                    imageRect,
                    {
                        ScreenInfo.width/2 - width/2,
                        ScreenInfo.height/2 - height/2,
                        width,
                        height
                    }
                );
            }
        }title;
        class NormalGame : public ExtraRaylib::ScreenWrapper
        {
            public:
            Road roads = Road(2);
            FeedbackText fText = FeedbackText({50,200,300,100},u"default text.");
            ExtraRaylib::Button langButton = ExtraRaylib::Button(myFont,currentLanguage->first,300,0,24,BLACK,YELLOW);
            ExtraRaylib::Button help = ExtraRaylib::Button(myFont,"Help",300,0,24,BLACK,YELLOW);
            NormalGame(){
                fText.setText(savedText::getText());
            }
            void run()
            {
                runButtons();
                if(soTrue)
                    if(ExtraRaylib::isControlDown() && IsKeyPressed(KEY_R))
                        fText.restart(); /// delete dis ltr (debug only)
                if(fText.finished)
                {
                    if(fText.wpm > settings["BestWPM"] && fText.wpm < 250)
                        settings["BestWPM"] = fText.wpm;
                    roads.cars[1].wpm = settings["BestWPM"];
                    if((ExtraRaylib::isShiftDown() || ExtraRaylib::isControlDown()) && IsKeyPressed(KEY_ENTER))
                    {
                        fText.setText(savedText::getText());
                        fText.restart();
                    }
                    return;
                }
                if(ExtraRaylib::isControlDown() && IsKeyPressed(KEY_V))
                {
                    savedText::saveClipboardText();
                    fText.setText(savedText::getText());
                }
                if(!fText.isAtEnd())
                    roads.setCompletionPrc(fText.getTime(),fText.maxChar);
                player->xPr=fText.getPrc();
                fText.run();
                roads.run();
                int nr = roads.getCarRightClicked();
                if(nr == 0) /// we can only custimize the first car (our own)
                {
                    currentScreen.setScreen(ScreenStuff::screens::SRGBpick);
                }
            }
            void runButtons()
            {
                langButton.align(100,0,ScreenInfo);
                if(langButton.Lclicked())
                {
                    currentLanguage++;
                    if(currentLanguage == COUNTRY_TEXT.end())
                        currentLanguage = COUNTRY_TEXT.begin();
                    langButton.text = currentLanguage->first;
                    langButton.re_measure();

                    fText.setText(savedText::getRandomText());
                }
                help.align(50,100,ScreenInfo);
                if(help.Lclicked())
                    currentScreen.setScreen(ScreenStuff::screens::Shelp);

            }
            void draw()
            {
                if(!fText.finished)
                    drawGame();
                else
                    drawFinish();
            }
            void drawGame()
            {
                roads.draw(fText.startTime);
                DrawTextEx(myFont,fText.getWPM(),{0,0},18,1,BLACK);
                DrawTextEx(myFont,fText.getAccuracy(),{100,0},18,1,BLACK);
                langButton.draw();
                help.draw();
                fText.draw();
            }
            void drawFinish()
            {
                Rectangle rect = {ScreenInfo.x + ScreenInfo.width*0.1f,ScreenInfo.y + ScreenInfo.height*0.1f,ScreenInfo.width*0.8f,ScreenInfo.height*0.8f};
                DrawRectangleRec(rect,LIGHTGRAY);
                Rectangle Header = {rect.x,rect.y,rect.width,rect.height*0.2f};
                DrawRectangleRec(Header,YELLOW);
                using ExtraRaylib::TxtAligned;
                TxtAligned winMessage(&myFont,"You can type. Grats!",rect,50,10,36,GREEN,BLANK);
                TxtAligned accuracy(&myFont,fText.getWPM(),rect,20,30,20,BLACK,BLANK);
                TxtAligned wpm(&myFont,fText.getAccuracy(),rect,80,30,20,BLACK,BLANK);
                winMessage.draw(true);
                accuracy.draw(true);
                wpm.draw(true);
            }
        } *game = new NormalGame();

        class Help : public ExtraRaylib::ScreenWrapper
        {
            void run()
            {
                if(IsKeyDown(KEY_ESCAPE))
                    currentScreen.setScreen(ScreenStuff::screens::Sgame);
            }
            void draw()
            {
                DrawText("noob.",0,0,80,BLACK);
            }
        }helpScreen;
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
            if(now == Sgame) screen = game;
            if(now == SRGBpick) screen = &colorPicker;
            if(now == Stitle) screen = &title;
            if(now == Shelp) screen = &helpScreen;

            return screen;
        }
        class MainLoop
        {
        public:
            static void run()
            {
                loadSettings();
                title.init();

                currentScreen.setScreen(ScreenStuff::screens::Stitle);
                colorPicker.setColor(player->color);
                while(!WindowShouldClose())
                {
                    ExtraRaylib::mouseAction = MOUSE_CURSOR_DEFAULT;
                    updateScreenVariables();
                    currentScreen.getScreen()->run();
                    BeginDrawing();
                        ClearBackground(RAYWHITE);
                        currentScreen.getScreen()->draw();
                    EndDrawing();
                    if(ExtraRaylib::isKeyHeldFor(KEY_ESCAPE,1500))
                        break;
                    SetMouseCursor(ExtraRaylib::mouseAction);
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
                 game = new NormalGame();
                 player = &(game->roads.cars[0]);
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
