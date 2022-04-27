The repo at this point in time represents an implementation of a widget-ified Qt3D instance, following the work of Florian Blume. However, this approach (which leveraged `QOpenGLWidget`) proved fundamentally incompatible with OSX (untested on Windows), and would have introduced future technical debt related to Apple's ditching of OpenGL and move to Metal.

It is saved here for posterity!
