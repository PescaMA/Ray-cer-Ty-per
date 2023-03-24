#include <iostream>
#include <fstream>
#include <vector>
#include <raylib.h>
#include <map>
namespace ExtraRaylib
{
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
            {
                std::cout << "No color set for RGB!\n";
                exit(1);
            }
            (*directColor) = {static_cast<unsigned char>(255 * Sr.prc),
                    static_cast<unsigned char>(255 * Sg.prc),
                    static_cast<unsigned char>(255 * Sb.prc),255};
        }
    };
}
namespace RayCerTyPer
{
    std::map<std::string,double> settings;
    int const fps = 60;
    Rectangle ScreenInfo; /// used to center
    class Car
    {
        static int const START = 40;
        static int const END = 300;
        static int const S_WIDTH = 120; /// sprite width
        static int const S_HEIGHT = 48; /// sprite height

        float xPoz = START;
        float const y;

        public:
        Color color;
        static Texture2D ASSET_STRUCTURE;
        static Texture2D ASSET_COLOR;
        Car():y(0){}
        Car(float y):y(y){}
        void run()
        {
        }
        bool rightClicked()
        {
            return CheckCollisionPointRec(GetMousePosition(),{xPoz,y,S_WIDTH,S_HEIGHT});
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
        int Ylength = 55;
        int nrOfRoads = 1;

        int const Xstart = 40;
        int const width = 300;
        public:
        std::vector <Car> cars;
        Road(int nr_of_cars)
        {
            for(int i=0;i<nr_of_cars;i++)
                cars.emplace_back();
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
        }
    };
    class Loader
    {
        public:
        void load()
        {
            SetConfigFlags(FLAG_WINDOW_RESIZABLE);
            defaultSettings();
            InitWindow(ScreenInfo.width,ScreenInfo.height,"Ray-cer");
            Car::ASSET_STRUCTURE = LoadTexture("Images/carStructure.png");
            Car::ASSET_COLOR = LoadTexture("Images/carColor.png");
            SetTargetFPS(fps);
        }
        void defaultSettings()
        {
            settings["ScreenWidth"] = 960;
            settings["ScreenHeight"] = 540;
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
        }
        void changeSettings()
        {
            std::ofstream fout("settings.txt");
            for(std::map<std::string,double>::iterator it;it!=settings.end();it++)
                fout<<it->first<<' '<<it->second<<'\n';
        }
        void unload()
        {
            UnloadTexture(Car::ASSET_STRUCTURE);
            UnloadTexture(Car::ASSET_COLOR);
            CloseWindow();
        }
    };
    class MainLoop
    {
        enum Screens
        {
            _NormalGame,
            _ColorPicker
        }doing;
        /// for obv reasons namespaces are illegal.(in classes)
        namespace NormalGame
        {
            Road roads;
            void run()
            {
                roads.run();
                int nr = roads.getCarRightClicked();
                if(nr >= 0)
                {
                    doing = _NormalGame;
                    colorPicker->setColor(roads.cars[nr].color);
                }
            }
            void draw()
            {
                roads.draw();
            }
        }
        namespace ColorPicker
        {
            ExtraRaylib::Choose_RGB RGBcolor = ExtraRaylib::Choose_RGB(30,200,60,60);
            void setColor(Color& color)
            {
                RGBcolor.setColor(color);
            }
            void run()
            {
                RGBcolor.run();
            }
            void draw()
            {
                RGBcolor.draw();
            }
        }

    public:
        static void run()
        {

            while(!WindowShouldClose())
            {
                settings["ScreenWidth"] = GetScreenWidth();
                settings["ScreenHeight"] = GetScreenHeight();
                ScreenInfo = {0,0,(float)GetScreenWidth(),(float)GetScreenHeight()};
                if(doing == _NormalGame) NormalGame::run();
                if(doing == _ColorPicker) ColorPicker::run();

                ///logic soon to be crafted from the finest materials(raylib + C++) but by a terrible programmer(me)
                BeginDrawing();
                    ClearBackground(RAYWHITE);
                    if(doing == _NormalGame) NormalGame::run();
                    if(doing == _ColorPicker) ColorPicker::run();
                EndDrawing();
            }
        }

    };
}
int main()
{
    RayCerTyPer::Loader loader;
    loader.load();
    RayCerTyPer::MainLoop::run();
    loader.unload();
}
