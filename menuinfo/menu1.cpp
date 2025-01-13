// Example program
// Use SDL3 to create a window with properties

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_video.h>

int
main(int argc, char** argv)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return 0;
  }

  SDL_PropertiesID props = SDL_CreateProperties();
  if(props == 0) {
    SDL_Log("Unable to create properties: %s", SDL_GetError());
    return 0;
  }

  // Assume the following calls succeed
  SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "My Window");
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 640);
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 480);

  SDL_Window *window = SDL_CreateWindowWithProperties(props);
  if(window == NULL) {
    SDL_Log("Unable to create window: %s", SDL_GetError());
    return 0;
  }

  // A game loop goes here

  SDL_DestroyWindow(window);
  SDL_DestroyProperties(props);

  return 0;
}
