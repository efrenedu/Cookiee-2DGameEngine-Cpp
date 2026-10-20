

#include "imgui.h"
#include<imgui_impl_sdl2.h>
#include<imgui_impl_sdlrenderer.h>
#include <SDL.h>
#include<iostream>


int main(int, char**)
{
      std::cout<<"init"<<std::endl;
       // Setup SDL
    bool res=false;
   if( SDL_Init(SDL_INIT_EVERYTHING)<0){
      std::cout<<"fail init"<<std::endl;
      return 0;
   }

   SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
   SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, window_flags);
   SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
   if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
   }
   // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

   bool done=false;
   SDL_Event event;
   while(!done){
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Test");
        ImGui::Text("test");
        ImGui::End();
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer,20,20,20,255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
      //  ImGui_ImplSDL2_NewFrame(window);
       // ImGui::NewFrame();
        //ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
        //ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)


   }

 return 0;
}
