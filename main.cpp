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
        Slider Sr,Sg,Sb;/// just changed diameter to radius . change here 2 future me.
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
        void run()
        {
            Sr.run();
            Sg.run();
            Sb.run();
        }
        Color getRGBColor()
        {
            return {static_cast<unsigned char>(255 * Sr.prc),
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
        int carPoz;
        static int const carStart = 40;
        static int const carEnd = 300;
        ExtraRaylib::Choose_RGB color = ExtraRaylib::Choose_RGB(30,200,60,60);
        public:
        static Texture2D ASSET_STRUCTURE;
        static Texture2D ASSET_COLOR;
        void run()
        {
            color.run();
        }
        void draw(int y=40)
        {
            DrawTexture(ASSET_COLOR,carStart,y,color.getRGBColor());
            DrawTexture(ASSET_STRUCTURE,carStart,y,WHITE);
            color.draw();
        }
    }; ///statics need to be declared
    Texture2D Car::ASSET_STRUCTURE;
    Texture2D Car::ASSET_COLOR;
    class Road
    {
        int const Ystart = 50;
        int Ylength = 55;
        int nrOfRoads = 1;

        int const Xstart = 40;
        int const width = 300;
        std::vector <Car> cars;
        public:
        Road()
        {
            cars.emplace_back();
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
    public:
        static void run()
        {
            using namespace ExtraRaylib;
            Slider test(10,10,100,10);
            Car player;
            Road roads;

            while(!WindowShouldClose())
            {

                test.run();
                player.run();
                settings["ScreenWidth"] = GetScreenWidth();
                settings["ScreenHeight"] = GetScreenHeight();
                ScreenInfo = {0,0,(float)GetScreenWidth(),(float)GetScreenHeight()};
                ///logic soon to be crafted from the finest materials(raylib + C++) but by a terrible programmer(me)
                BeginDrawing();
                ClearBackground(RAYWHITE);
                roads.draw();
                player.draw();
                test.draw();

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
