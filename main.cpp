#include <iostream>
#include <fstream>
#include <raylib.h>
#include <map>
namespace RayCerTyPer
{
    std::map<std::string,double> settings;
    int const fps = 60;

    Rectangle ScreenInfo; /// used to center
    class Car
    {
        int const carStart = 40;
        int const carEnd = 300;
        public:
        static Texture2D ASSET_CAR;
        void draw()
        {
            DrawTexture(ASSET_CAR,carStart,100,WHITE);
        }
    };
    class Road
    {
        int const Ystart = 50;
        int Ylength = 100;
        int nrOfRoads = 1;

        int const Xstart = 40;
        int const width = 300;
        public:
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
            Car::ASSET_CAR = LoadTexture("car.png");
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
            UnloadTexture(Car::ASSET_CAR);
            CloseWindow();
        }
    };
    class MainLoop
    {
    public:
        static void run()
        {
            while(!WindowShouldClose())
            {
                Car player;
                Road roads;
                settings["ScreenWidth"] = GetScreenWidth();
                settings["ScreenHeight"] = GetScreenHeight();
                ScreenInfo = {0,0,(float)GetScreenWidth(),(float)GetScreenHeight()};
                ///logic soon to be crafted from the finest materials(raylib + C++) but by a terrible programmer(me)
                BeginDrawing();
                roads.draw();
                player.draw();
                ClearBackground(RAYWHITE);
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
