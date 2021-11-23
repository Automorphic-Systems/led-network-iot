# Sunshine: Effects Library

## Setmode  Effects

These are effects that are supported by the setmode operation, and whose pixel values are computed on the node’s hardware itself.  Operations for any setmode effect are applied directly to the pixel array itself.  




### Effect : Rainbow 

 Cycles through all of the colors at max saturation and renders them on the LEDs in rapid succession

 Parameters
	
	"mode", "rainbow"
	"rate", [int] - rate at which the rainbow is cycled, between 0 and 255


### Effect : Streak

 Displays a single streak of LEDs 
 
 Parameters

	"mode", "streak"
	"hue",[int] - sets HSV color value of the streak - no value defaults to a changing rainbow effect 
	- value can be between 1 and 255

### Effect : Chase

 Displays chasing colors of LEDs 

 Parameters

	"mode", "chase"
	"color_count",[int] - sets the number of colors to use from the palette, value can be between 1 and 16 
	"rate", [int] - rate at which the chasing pattern moves, value can be between 0 and 255
	"max_iter", [int] - number of iterations/frames to render before resetting the palette - can be 
	between 1 and 100000 (arbitrarily large)

### Effect : Flicker

 Displays flickering LEDs, using a variation of the Fire 2012 algorithm

 Parameters

	"mode", "flicker"
	"color_count",[int] - sets the number of colors to use from the palette, value can be between 1 and 16 ✔
	"max_iter", [int] - number of iterations/frames to render before resetting the palette - can be between 
	1 and 100000 (arbitrarily large) ✔
	"twink_min", [int] - minimum number of LEDs to fade 
	"twink_max", [int] - maximum number of LEDs to fade
	"boost_thr", [int] - number of LEDs to boost intermittently ✔
	"boost_rate", [int] - frequency at which boosting occurs, value can be between 1 and 16 ✔
	"seed_amt", [int] - number of "sparks" to seed the array ✔
	"fade_rate", [int] - decay rate of the lit LEDs on the array, value can be between 1 and 16 ✔

 Unspecified value ranges above will need to be determined by the number of LEDs in the array. 



### Tentative 2-D setmode effects:

	Perlin noise
	Marquis
	Hexmode
	Gush mode
	2d Fire/Flicker
	Pulse


## SetFrame Effects

These are effects that are supported by the setframe operation, and whose pixel values are computed by hardware outside of the node.  The node simply renders the pixels as instructed from the frame it receives. 

These effects are written externally, so the results are a series of frames that are passed to the node.


### Effect : Automata

 Displays output from a generalized 1-d cellular automata
 
 Parameters

	"mode", "automata"

 Other parameters TBD


### Proposed 2D setframe effects

	Mandala generator
	Lenia / A-life
	Bomb-redux
	Discretized SEIR model 
	Symbols
	Automata
	Boids