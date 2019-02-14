## Wilson's IAP
Graphical programming demo, used to simulate water wave motion and others, extended from LearnOpenGL by JoeyDeVries. 
Submitted as IAP project for Wilson's School.

## Tech/framework used
<b>Built with</b>
- [OpenGL](https://www.opengl.org/)
- [GLEW](http://glew.sourceforge.net/)
- [GLFW](https://www.glfw.org/)

## Implementation of Wave Equation
	float a = dot(position.x, normalize(waveDir));  //float a = position.x;
	float b = peak - 1.0f; 
	float k = (2.0f * pi) / wavelength;
	float c = sqrt(9.81f/k);
	
	float wave_dx = (exp(k * b)/ k) * sin(k * (a + c * time));
	float wave_dy = -(exp(k * b)/ k) * cos(k * (a + c * time));

## How to use?
1) Run the program's .exe
2) Use the mouse to edit the camera (viewport)
3) End the program's running by pressing ESC key.

## Credits
Thanks to [Joey DeVries](http://www.learnopengl.com)

## License
MIT © [cjntifo]()
