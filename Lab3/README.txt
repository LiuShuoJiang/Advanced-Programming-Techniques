If you'd link to run the code:

Because the code is mainly debugged in Visual Studio, I need to explain the method of replacing the files to prevent Visual Studio from not finding the file correctly.

1. Replace the "common" folder in the submission with the "common" folder of the original tutorial https://github.com/opengl-tutorials/ogl/tree/2.1_branch (you can also only replace the two files controls.cpp and controls.hpp, because these are the files I modified).
2. Replace the original StandardShading.fragmentshader file with the corresponding file I submitted.
3. Run the tutorial9_AssImp_Modified.cpp file (replace the original tutorial09.cpp file in the source file).