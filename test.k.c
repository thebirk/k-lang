struct Data
{
	value: u32;
	data: u64;
	time: u16;
}

func SDL_CreateWindow(
title: s8*, x: s32, y: s32,w: s32, h: s32,
					  u32 flags
)
: void*;

func main(argc: int, argv: s8**)
{
	if argc == 0 {
		return 1;
	}

	window := SDL_CreateWindow("Hello SDL!", 0, 0, 300, 300, 0);
	if window == 0 {
        return 2;
	}

	return 0;
}
