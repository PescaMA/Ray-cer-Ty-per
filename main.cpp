#include <iostream>
#include <fstream>
#include <raylib.h>
#include <map>
namespace RayCerTyPer
{
    std::map<std::string,double> settings;
    int const fps = 60;
    Rectangle ScreenInfo; /// used to center

    class Loader
    {
        public:
        void load()
        {
            SetConfigFlags(FLAG_WINDOW_RESIZABLE);
            defaultSettings();
            InitWindow(ScreenInfo.width,ScreenInfo.height,"Ray-cer");
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
        void run()
        {
            while(!WindowShouldClose())
            {
                settings["ScreenWidth"] = GetScreenWidth();
                settings["ScreenHeight"] = GetScreenHeight();
                ScreenInfo = {0,0,(float)GetScreenWidth(),(float)GetScreenHeight()};
                ///logic soon to be crafted from the finest materials(raylib + C++) but by a terrible programmer(me)
                BeginDrawing();
                ClearBackground(RAYWHITE);
                EndDrawing();
            }
        }
        void unload()
        {
            CloseWindow();
        }
    };
}
int main()
{
    RayCerTyPer::Loader loader;
    loader.load();
    loader.run();
    loader.unload();
}
