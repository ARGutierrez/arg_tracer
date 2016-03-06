#include <SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 640

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_RED   0xFFFF0000
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE  0xFF0000FF

typedef uint8_t   u8;
typedef uint32_t u32;

SDL_Window* window;
SDL_Surface* screen;
SDL_Surface* render_surface;

struct vec3
{
	float x, y, z;

	vec3(vec3& in)
	{
		x = in.x;
		y = in.y;
		z = in.z;
	}

	vec3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

struct vec4
{
	float x, y, z, w;

	vec4(vec4& in)
	{
		x = in.x;
		y = in.y;
		z = in.z;
		w = in.w;
	}

	vec4(vec3& in, float w)
	{
		x = in.x;
		y = in.y;
		z = in.z;
		this->w = w;
	}

	vec4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};

vec3 add(vec3 a, vec3 b)
{
	return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3 sub(vec3 a, vec3 b)
{
	return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

float dot(vec3 a, vec3 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float dot(vec4 a, vec4 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

u32 vecToColor(vec4& in)
{
	u32 color;
	u8 a, r, g, b;
	a = (u8)(in.w * 255);
	r = (u8)(in.x * 255);
	g = (u8)(in.y * 255);
	b = (u8)(in.z * 255);

	color = a << 24 | r << 16 | g << 8 | b;
	return color;
}

bool init_SDL()
{
	
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow( "Arg_Tracer",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	if ( window == nullptr )
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	screen = SDL_GetWindowSurface( window );

	render_surface = SDL_CreateRGBSurface(
		0,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		32, 0, 0, 0, 0);

	return true;
}

void close_SDL()
{
	//Destroy window
	SDL_DestroyWindow( window );
	window = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}

void set_pixel(SDL_Surface* surface, int x, int y, u32 color)
{
	if (x < 0 || x > SCREEN_WIDTH || y < 0 || y > SCREEN_HEIGHT)
		return;

	u32* pixels = (u32*)surface->pixels;

	pixels[ (y * surface->w) + x ] = color;
}

void render()
{
	float max_x = 5.0;
	float max_y = 5.0;

	for (int i = 0; i < SCREEN_WIDTH; i++)
	{
		for (int j = 0; j < SCREEN_HEIGHT; j++)
		{
			float x = (float(i * 2 - SCREEN_WIDTH) / SCREEN_WIDTH);
			float y = (float(j * 2 - SCREEN_HEIGHT) / SCREEN_HEIGHT);

			vec3 ray_o(x * max_x, y * max_y, 0.0f);
			vec3 ray_d(0.0f, 0.0f, -1.0f);

			vec3 sphere_c(0.0f, 0.0f, -10.0f);
			float sphere_r = 1.0f;

			vec3 omc = sub(ray_o, sphere_c);
			float b = dot(ray_d, omc);
			float c = dot(omc, omc) - sphere_r * sphere_r;
			float bsqmc = b * b - c;

			if (bsqmc >= 0.0f)
			{
				set_pixel(render_surface, i, j, COLOR_RED);// vecToColor(vec4(0.4f, 0.4f, 1.0f, 1.0f)));
			}
		}
	}
	
}

int main(int argc, char** argv)
{
	if ( !init_SDL() )
	{
		printf( "Failed to init SDL!" );
		return 0;
	}

	bool quit = false;
	SDL_Event e;
	// Render loop
	while ( !quit )
	{
		while ( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit
			if ( e.type == SDL_QUIT )
			{
				quit = true;
			}
		}

		render();

		SDL_BlitSurface( render_surface, 0, screen, 0);
		SDL_UpdateWindowSurface( window );
	}
		

	close_SDL();
	return 0;
}