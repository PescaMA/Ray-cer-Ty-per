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
extern const CountryMap HardcodeRayJump::COUNTRY_TEXT;
using HardcodeRayJump::COUNTRY_TEXT;
namespace RayJump
{
    CountryMap::const_iterator currentLanguage = COUNTRY_TEXT.begin();
    std::map<std::string,float> settings;
    int const fps = 60;
    Rectangle ScreenInfo; /// used to center
    Texture2D CAR_ASSET_STRUCTURE;
    Texture2D CAR_ASSET_COLOR;
    Font myFont;
    float minWindowWidth = 720;
    float minWindowHeight = 480;
    float pixel = 1.0f;

    const std::vector<std::string> carNamesRO={
        "<- Tu (joc curent)",
        "<- Jocul cel mai bun",
        "<- Media jocurilor"
    };
    const std::vector<std::string> carNamesEN={
        "<- You (current game)",
        "<- Best game",
        "<- Average game"
    };

    class Loader
    {
        public:
        void load()
        {
            srand(getTimeMS()); /// setting the random seed to current time
            SetConfigFlags(FLAG_WINDOW_RESIZABLE);

            defaultSettings();
            readSettings();
            InitWindow(ScreenInfo.width,ScreenInfo.height,"Rayjump");
            SetWindowMinSize(720,480);

            if(settings["IsWindowMaximized"] == 1.0f)
                MaximizeWindow();
            SetWindowIcon(LoadImage("Images/icon.png"));

            ///While the inefficient font loading takes place, we draw the start screen
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
                },WHITE
            );
            EndDrawing();

            myFont = LoadFontEx("liberation_mono.ttf", 24, NULL, 9000);
            SetTextureFilter(myFont.texture, TEXTURE_FILTER_BILINEAR);
            SetExitKey(0);
            CAR_ASSET_STRUCTURE = LoadTexture("Images/carStructure.png");
            CAR_ASSET_COLOR = LoadTexture("Images/carColor.png");
            SetTargetFPS(fps);
        }
        static void defaultSettings()
        {
            settings["ScreenWidth"] = 960;
            settings["ScreenHeight"] = 540;
            settings["BestWPM"] = 11;
            settings["GamesPlayed"] = 0;
            settings["AverageWPM"] = 10;
            settings["CopiedTextPos"] = -1;
            settings["IsWindowMaximized"]=false;
            settings["PlayerR"] = 250;
            settings["PlayerG"] = 0;
            settings["PlayerB"] = 0;
            settings["Car1R"] = 0;
            settings["Car1G"] = 250;
            settings["Car1B"] = 0;
            settings["Car2R"] = 0;
            settings["Car2G"] = 0;
            settings["Car2B"] = 250;

            ScreenInfo = {0,0,(float)settings["ScreenWidth"],(float)settings["ScreenHeight"]};
        }
        static void updateSettings(float wpm,int textLength)
        {
            settings["GamesPlayed"]++;
            if(wpm > 250 || textLength <= 15)
                return; /// is not valid
            if(wpm > settings["BestWPM"])
                settings["BestWPM"] = wpm;
            settings["AverageWPM"] = (wpm + settings["AverageWPM"] * (settings["GamesPlayed"]-1)) / settings["GamesPlayed"];

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
        static void changeSettings()
        {
            if(!IsWindowMaximized())
            {
                settings["ScreenWidth"] = GetScreenWidth();
                settings["ScreenHeight"] = GetScreenHeight();
            }
            if(IsWindowMaximized())
                settings["IsWindowMaximized"]=1.0f;
            else
                settings["IsWindowMaximized"]=0.0f;

            std::ofstream fout("Text_files/settings.txt");
            if(!fout)return;
            for(std::map<std::string,float>::iterator it=settings.begin();it!=settings.end();it++)
                fout<<(it->first)<<' '<<(it->second)<<'\n';
            fout.close();
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
        Rectangle SPRITE = {0,0,WIDTH,HEIGHT};

        float xPr = 0;
        float const y;
        int wpm = 15;
        int id;
        const std::string *name;

        Color color = {0,222,0,255};
        Car():y(0),id(999){changeName();}
        Car(float y,int id):y(y),id(id){changeName();}
        void run()
        {}
        void changeName()
        {
            if(id < (int)carNamesEN.size())
                name = currentLanguage==COUNTRY_TEXT.begin()?&(carNamesEN[id]):&(carNamesRO[id]);
            else
                name = new std::string("Random car");
        }
        bool rightClicked()
        {
            return IsMouseButtonDown(1) && CheckCollisionPointRec(GetMousePosition(),{getPos(),y * pixel,WIDTH * pixel,HEIGHT * pixel});
        }
        float getPos()
        {
            int roadWidth = ScreenInfo.width - START*4;
            return START + std::max((int)(roadWidth-WIDTH * pixel),0) * xPr;
        }
        void draw(bool isGameStarted = false)
        {
            changeName();
            Rectangle destinationCar = SPRITE;
            destinationCar.x = getPos();
            destinationCar.y = y*pixel;
            destinationCar.width *=pixel;
            destinationCar.height *=pixel;
            ExtraRaylib::drawTextureDest(CAR_ASSET_COLOR,SPRITE,destinationCar,color);
            ExtraRaylib::drawTextureDest(CAR_ASSET_STRUCTURE,SPRITE,destinationCar,WHITE);
            if(!isGameStarted){
                DrawTextEx(myFont,(*name).c_str(),{destinationCar.x + destinationCar.width + 5 * pixel,(y + 15)*pixel},18 * pixel,1,BLACK);
            }
        }
    }; ///non const statics need to be declared
    class Road
    {
        Font *font;
        int const Ystart = 50;
        int Ylength = Car::HEIGHT + 5;
        int const MAX_NR_OF_CARS = 4;
        int nrOfRoads = 1;
        int roadWidth;

        int const Xstart = 40;
        public:
        std::vector <Car> cars;
        Road(Font &font,int nr_of_cars)
        :font(&font)
        {
            if(nr_of_cars > MAX_NR_OF_CARS)
                nr_of_cars = MAX_NR_OF_CARS;
            nrOfRoads = nr_of_cars;
            for(int i=0;i<nr_of_cars;i++)
                cars.emplace_back(Ystart + i*Ylength, i);
            updateCarWPM();
        }
        void updateCarWPM()
        {
            if(cars.size() > 1)cars[1].wpm = settings["BestWPM"];
            if(cars.size() > 2)cars[2].wpm = settings["AverageWPM"];
        }
        void loadColors()
        {
            cars[0].color.r = settings["PlayerR"];
            cars[0].color.g= settings["PlayerG"];
            cars[0].color.b= settings["PlayerB"];
            if(cars.size() <= 1) return;
            cars[1].color.r = settings["Car1R"];
            cars[1].color.g = settings["Car1G"];
            cars[1].color.b = settings["Car1B"];
            if(cars.size() <= 2) return;
            cars[2].color.r = settings["Car2R"];
            cars[2].color.g = settings["Car2G"];
            cars[2].color.b = settings["Car2B"];

        }
        void saveColors()
        {
            settings["PlayerR"] = cars[0].color.r;
            settings["PlayerG"] = cars[0].color.g;
            settings["PlayerB"] = cars[0].color.b;
            if(cars.size() <= 1) return;
            settings["Car1R"] = cars[1].color.r;
            settings["Car1G"] = cars[1].color.g;
            settings["Car1B"] = cars[1].color.b;
            if(cars.size() <= 2) return;
            settings["Car2R"] = cars[2].color.r;
            settings["Car2G"] = cars[2].color.g;
            settings["Car2B"] = cars[2].color.b;
        }
        void run()
        {
            roadWidth = ScreenInfo.width - Car::START*4;
            int sz = cars.size();
            for(int i=0; i<sz; i++)
                cars[i].run();
        }
        void setCompletionPrc(int timeMS, int characterCount)
        {
            int const timeCorrection = 12000; /// /1000 /60 * 5 = miliseconds to seconds to minutes; words to characters
            /// first car is the player and has variable wpm so we ignore it.
            for(int i=1;i<(int)cars.size();i++)
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
            DrawRectangle(Xstart,Ystart*pixel,roadWidth,nrOfRoads*Ylength*pixel,GRAY);
            int sz = cars.size();
            for(int i=0; i<sz; i++)
            {
                cars[i].draw(isStarted);
                Vector2 position = {Xstart + roadWidth + 10.0f,(cars[i].y + 14.0f)*pixel};
                DrawTextEx(*font,TextFormat("%i wpm",cars[i].wpm),position,20,1,BLACK);
            }

        }
    };

    class FeedbackText : public ExtraRaylib::boxText
    {public:
        int linePos = 0; /// line position in separated text
        int charPos = 0; /// column position in separted text
        float wpm = 0;   /// words per minute
        int accuracy = 10000;/// acurracy * 100 (for 2 decimals)
        int unsigned total = 0,correctCount=0; /// nr of characters written by player in current text
        std::u16string gr,re,bl; /// green, red, black substrings
        bool stop = false;
        bool stop2 = false;
        int secondPassed = 0;
        long long startTime = 0;
        bool finished = false;
        /*************************************
        *           Simple functions
        **************************************/
        FeedbackText():FeedbackText({0,0,0,0},u""){}
        FeedbackText(Rectangle rect,std::u16string text):boxText(text,rect,3,18,&myFont){}
        void setText(std::u16string text){
            this->text = text;
            setSepText();
            restart();
        }
        int getTextSize(){
            return text.size();
        }
        float getPrc(){
            return 1.0f * correctCount / text.size();
        }
        int getTime(){
            if(startTime == 0)return 0;
            return std::max(getTimeMS() - startTime,1LL * 1);
        }
        bool isAtEnd(){
            return correctCount == text.size();
        }
        const char* getAccuracy(){
            static std::string str;
            str = std::to_string(accuracy);
            str.insert(str.size()-2,".");
            str += "% acc";
            return str.c_str();
        }
        /*************************************
        *           Running function
        **************************************/
        void restart()
        {
            bl = sepText[0];
            linePos = charPos = correctCount = total = 0;
            stop = stop2 = finished = false;
            gr = re = u"";
            startTime = secondPassed = 0;
            wpm = 0;
            accuracy = 10000;
        }
        void run()
        {
            if(finished || text.size()==0) return;
            calculate();
            eraseChr();
            if(startTime == 0 && correctCount !=0)
                startTime = getTimeMS();
            if(isAtEnd())
            {
                if(IsKeyDown(KEY_ENTER))
                    finished = true;
                return;
            }
            handleKeyPress();
        }
        void calculate()
        {
            if (!startTime || stop2) return;
            if(isAtEnd())
                stop2 = true; /// last calculation.
            if(secondPassed == getTime()/1000 && !stop2)
                return;
            secondPassed ++ ;
            wpm = 1.0f*correctCount/5*60/std::max((1.0f*getTime()/1000),(float)1e-10);
            accuracy = 1.0f * correctCount / total * 10000;
        }
        void handleKeyPress()
        {
            char16_t c;
            while((c = GetCharPressed()) && !stop)
            {
                c = flattenUTF16Char(c);
                if(correctCount == 0 && c != flattenUTF16Char(sepText[linePos][charPos]))
                    continue;
                if(c == flattenUTF16Char(sepText[linePos][charPos]) && re.empty())
                {
                    gr+=bl[0];
                    correctCount++;
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
        /*************************************
        *           Draw functions
        **************************************/
        void drawDominantLine()
        {
            if(bl==u"" && !stop && linePos<sepTextSize)
                bl = sepText[linePos];
            Color LIGT_RED = {250, 181, 181, 255};
            ExtraRaylib::drawtextUnicode(*font,gr,{rect.x,rect.y},font_size,1,GREEN,BLANK);
            ExtraRaylib::drawtextUnicode(*font,re,{rect.x + ExtraRaylib::MeasureTextUnicode(*font,gr,font_size,1),rect.y},font_size,1,BLACK,LIGT_RED);
            ExtraRaylib::drawtextUnicode(*font,bl,{rect.x + ExtraRaylib::MeasureTextUnicode(*font,(gr + re),font_size,1),rect.y},font_size,1,BLACK,BLANK);
        }
        void draw()
        {
            if(text.size() == 0 || sepTextSize==0) return;
            int oldFontSize = font_size;
            Rectangle oldRect = rect;
            font_size*=pixel;
            rect.y *= pixel;
            rect.width *= pixel;
            rect.height *= pixel;

            DrawRectangleRec(rect,YELLOW);
            drawDominantLine();
            for(int i=1;i<MAX_LINES && i+linePos < sepTextSize;i++)
            ExtraRaylib::drawtextUnicode(*font, sepText[i+linePos],{rect.x,rect.y + font_size*i},font_size,1,BLACK,BLANK);
            font_size = oldFontSize;
            rect =oldRect;
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
            std::string copiedText;
            if(GetClipboardText() == NULL)
                copiedText = "Empty clipboard.";
            else
                copiedText = GetClipboardText();
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
        Car *selectedCar;
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
                if(GetKeyPressed() || ExtraRaylib::getSpecialKeyDown())
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
                    },WHITE
                );
            }
        }title;
        class NormalGame : public ExtraRaylib::ScreenWrapper
        {
            public:
            Road roads = Road(myFont,3);
            FeedbackText fText;
            ExtraRaylib::Button langButton = ExtraRaylib::Button(myFont,currentLanguage->first,300,0,24,BLACK,YELLOW);
            ExtraRaylib::Button help = ExtraRaylib::Button(myFont,"Help",300,0,24,BLACK,YELLOW);
            ExtraRaylib::Button deleteData = ExtraRaylib::Button(myFont,"Erase all data",300,0,24,BLACK,RED);
            NormalGame():fText({40,230,600,10},u"default text."){
                fText.setText(savedText::getText());
            }
            void run()
            {
                runButtons();
                if(ExtraRaylib::isControlDown() && IsKeyPressed(KEY_C))
                    SetClipboardText(fText.getText().c_str());
                if(ExtraRaylib::isControlDown() && IsKeyPressed(KEY_R))
                    fText.restart();
                if(fText.finished && (IsKeyPressed(KEY_ESCAPE) ||
                ((ExtraRaylib::isShiftDown() || ExtraRaylib::isControlDown()) && IsKeyPressed(KEY_ENTER))))
                {/// checks for exit command: 1)shift/ctrl + ENTER or 2)ESC
                    Loader::updateSettings(fText.wpm,fText.getTextSize());
                    std::cout << (savedText::getText() == utf8_to_u16(fText.getText())) << " A ";
                    roads.updateCarWPM();
                    if(settings["CopiedTextPos"] > -1 && savedText::getText() == utf8_to_u16(fText.getText()))
                    {
                        std::cout << "STUFF BE DONE!\n";
                        settings["CopiedTextPos"]++;
                    }

                    fText.setText(savedText::getText());
                    fText.restart();
                }
                if(fText.finished)
                    return;
                if(ExtraRaylib::isControlDown() && IsKeyPressed(KEY_V))
                {
                    savedText::saveClipboardText();
                    fText.setText(savedText::getText());
                }
                if(!fText.isAtEnd())
                    roads.setCompletionPrc(fText.getTime(),fText.getTextSize());
                player->xPr=fText.getPrc();
                player->wpm = fText.wpm;
                fText.run();
                roads.run();
                int nr = roads.getCarRightClicked();
                if(nr != -1 && fText.startTime==0)
                { /// if we selected a car and the race hasn't started we can change their color.
                    selectedCar = &roads.cars[nr];
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
                    help.text = currentLanguage->first == "english" ?"Help":"Ajutor";
                    deleteData.text = currentLanguage==COUNTRY_TEXT.begin()?"Erase all data":"Șterge toate datele";
                    langButton.re_measure();
                    help.re_measure();
                    deleteData.re_measure();

                    fText.setText(savedText::getRandomText());
                }
                help.align(100,100,ScreenInfo);
                if(help.Lclicked() || IsKeyPressed(KEY_F1))
                    currentScreen.setScreen(ScreenStuff::screens::Shelp);
                deleteData.align(0,100,ScreenInfo);
                if(deleteData.Lclicked())
                {
                    Rectangle window = ScreenInfo;
                    Loader::defaultSettings();
                    ScreenInfo = window;
                    settings["ScreenWidth"] = ScreenInfo.width;
                    settings["ScreenHeight"] = ScreenInfo.height;
                    Loader::changeSettings();
                    fText.restart();
                    roads.updateCarWPM();
                    roads.loadColors();
                }
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
                fText.draw();
                langButton.draw();
                help.draw();
                deleteData.draw();
            }
            void drawFinish()
            {
                bool en= (currentLanguage->first == "english");
                Rectangle rect = {ScreenInfo.x + ScreenInfo.width*0.1f,ScreenInfo.y + ScreenInfo.height*0.1f,ScreenInfo.width*0.8f,ScreenInfo.height*0.8f};
                DrawRectangleRec(rect,LIGHTGRAY);
                Rectangle Header = {rect.x,rect.y,rect.width,rect.height*0.2f};
                DrawRectangleRec(Header,YELLOW);
                using ExtraRaylib::TxtAligned;
                std::string personalizedWinMessage;
                if(fText.wpm > settings["BestWPM"] && fText.wpm < 250 && fText.getTextSize() > 15)
                    personalizedWinMessage = en?"A new record!":"Un nou record!";
                else
                    personalizedWinMessage = en? "You finished a new race. Good job!" : "Ați terminat o nouă cursă. Felicitări!";
                TxtAligned winMessage(&myFont,personalizedWinMessage.c_str(),rect,50,10,28*pixel,GREEN,BLANK);
                TxtAligned characters(&myFont,TextFormat(en?"You just typed %i characters with: ":"Tocmai ați tastat %i de caractere cu:",fText.getTextSize()),rect,50,30,20*pixel,BLACK,BLANK);
                TxtAligned wpm(&myFont,TextFormat(en?"%i WPM":"%i CuvPeMin",(int)fText.wpm),rect,20,50,24*pixel,BLACK,BLANK);
                TxtAligned bestWpm(&myFont,TextFormat(en?"%i words per minute in best race":"%i cuvinte pe minute în cea mai bună cursă" ,(int)settings["BestWPM"]),rect,20,60,20*pixel,BLACK,BLANK);
                TxtAligned accuracy(&myFont,fText.getAccuracy(),rect,80,50,24*pixel,BLACK,BLANK);
                TxtAligned gamesPlayed(&myFont,TextFormat(en?"%i games played previously":"%i jocuri jucate înainte",(int)settings["GamesPlayed"]),rect,0,80,20*pixel,BLACK,BLANK);
                TxtAligned avgWpm(&myFont,TextFormat(en?"%i average WordsPerMin previously":"%i = media cuvintelor pe minut înainte" ,(int)settings["AverageWPM"]),rect,0,90,20*pixel,BLACK,BLANK);
                winMessage.draw(true);
                accuracy.draw(true);
                wpm.draw(true);
                bestWpm.draw();
                characters.draw();
                gamesPlayed.draw();
                avgWpm.draw();
            }
        } *game = new NormalGame();

        class Help : public ExtraRaylib::ScreenWrapper
        {
            void run()
            {
                if(IsKeyDown(KEY_ESCAPE) || IsKeyPressed(KEY_F1))
                    currentScreen.setScreen(ScreenStuff::screens::Sgame);
            }
            void draw()
            {
                Vector2 pos = {0,0};
                float pixel = currentLanguage==COUNTRY_TEXT.begin()
                    ?std::min(ScreenInfo.width/700,ScreenInfo.height/400)
                    :std::min(ScreenInfo.width/750,ScreenInfo.height/400);
                const std::string *str = currentLanguage==COUNTRY_TEXT.begin()? &HardcodeRayJump::HELP_MESSEAGE : &HardcodeRayJump::HELP_MESSEAGE_RO;
                DrawTextEx(myFont,(*str).c_str(),pos,13.0f* pixel,1,BLACK);
            }
        }helpScreen;
        class ColorPicker : public ExtraRaylib::ScreenWrapper
        {
            public:
            ExtraRaylib::Choose_RGB RGBcolor = ExtraRaylib::Choose_RGB(myFont,30,250,200,60);
            void run()
            {
                RGBcolor.setColor(selectedCar->color);
                RGBcolor.run(pixel);
                if(IsKeyDown(KEY_ESCAPE))
                    currentScreen.setScreen(ScreenStuff::screens::Sgame);
            }
            void draw()
            {
                selectedCar->draw();
                RGBcolor.draw();
            }
        } colorPicker;
        ExtraRaylib::ScreenWrapper* ScreenStuff::getScreen()
        {
            ExtraRaylib::ScreenWrapper *screen = &title;
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
                while(!WindowShouldClose())
                {
                    ExtraRaylib::cursorType = MOUSE_CURSOR_DEFAULT;
                    updateScreenVariables();
                    currentScreen.getScreen()->run();
                    BeginDrawing();
                        ClearBackground(RAYWHITE);
                        currentScreen.getScreen()->draw();
                    EndDrawing();
                    if(ExtraRaylib::isKeyHeldFor(KEY_ESCAPE,1500))
                        break;
                    SetMouseCursor(ExtraRaylib::cursorType);
                }
                game->roads.saveColors();
            }
            static void updateScreenVariables()
            {
                if(!IsWindowMaximized())
                {
                    settings["ScreenWidth"] = GetScreenWidth();
                    settings["ScreenHeight"] = GetScreenHeight();
                }
                ScreenInfo = {0,0,(float)GetScreenWidth(),(float)GetScreenHeight()};
                pixel = std::min(ScreenInfo.width/minWindowWidth,ScreenInfo.height/minWindowHeight);
            }
            static void loadSettings()
            {
                game = new NormalGame();
                player = &(game->roads.cars[0]);
                game->roads.loadColors();
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
