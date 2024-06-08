main: src/main.cpp src/renderutils.cpp src/ui.cpp
	emcc src/main.cpp src/renderutils.cpp src/ui.cpp -sUSE_SDL=2 -sUSE_SDL_TTF=2 -lwebsocket.js --embed-file assets -o client.js