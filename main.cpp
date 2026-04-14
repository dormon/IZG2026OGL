#include<iostream>

#include<SDL3/SDL.h>
#include<geGL/geGL.h>
#include<geGL/StaticCalls.h>

using namespace ge::gl;

int main(int argc,char*argv[]){
  std::cerr << "PAPOUCH ŽERE OŘECHY." << std::endl;
  auto window = SDL_CreateWindow("IZG2026",1024,768,SDL_WINDOW_OPENGL);

  auto context = SDL_GL_CreateContext(window);

  ge::gl::init();

  bool running = true;
  while(running){//main loop
    SDL_Event event;
    while(SDL_PollEvent(&event)){//event loop
      if(event.type == SDL_EVENT_QUIT)running = false;

    }
  


    glPointSize(10);
    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_POINTS,0,1);

    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DestroyContext(context);
  SDL_DestroyWindow(window);
  return 0;
}
