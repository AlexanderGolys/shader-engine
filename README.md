## How to build and run:
- to build the project use the command ".\premake5 --{filename-without-cpp-suffix} cmake" in the main directory terminal
- you may need to install premake, to do so run the executable premake file from main directory
- you can also run the bash script "rebuild.bat" (after modifying the target file if needed)
- currently only Windows system is supported 
- your main file should be placed in the "src/render-projects" directory
- the executable file will be in one of the build directories "cmake-build-{BUILD_TYPE}"
- your IDE should also recognise them as CMake projects automatically giving you run configuration
- you don't need to run premake each time, the modified files should recompile automatically

## Render gallery: 
https://www.behance.net/alexandergolys


## Implemented features:
- using vertex, fragment, geometry and compute shaders with OpenGL API
- shading with Phong material model, both vertex and face based triangular meshes
- hyperbolic geometry, conformal metric pullbacks
- Kleinian groups and hyperbolic tesselations
- Rendering circular triangles used for triangulations in hyperbolic plane \
- Pullbacking non-standard Riemannian metrics by conformal maps 
- mean, Gaussian and normal curvatures, principal directions, torsion, fundamental forms, shape operator 
- 1-parameter families of surfaces/curves realising animated deformation of leaves
- 2D rigid bodies and their basic dynamics
- 2D diffusion schemes
- Bezier and spline interpolations, Coons patches, tensor splines for surfaces
- Algebra of real functions on mesh/manifold and vector fields or differential forms as its modules
- Runge-Kutta ODE solver
- Second order partial differential equations solver 
- Integral transforms (Fourier, Laplace)
- Discrete transforms (DFT, generalised Fourier series, convolutions)
- Initial surface deformation encoded in PDE solution (or any non-homogeneous flow field)
- SDF based renderer 
- GLSL shader code generator realising SDF functions of primitives composed from C++ code level 
- Automatic shader code generating and manipulating code of function definitions allowing their realisation 
as elements of the function field on C++ level, modular shader code management and code macros substitutions 
- Multivariable functions allowing functorial composition of differential for elementary functions provided by user
- Rendering space curves as associated pipe surface with mesh generated on the GPU level so that curve can be deformed freely 
with no side effects in any moment within the code
- FFT in dimensions 1 and 2, fast convolutions of discrete functions, FFT-based rendering 
- SPH fluid simulations 
- Reading and processing audio signals, Gabor transforms, spectral power density 
- Foliated parametric surfaces
- differential operators of sampled data with FFT based implementation, FFT-based curve renderer 
- config file storing paths to directories used implicitly in the code, so that no hardcoding is needed
- Time dependent flows 
- basic windows filesystem operations and file buffering 

# Next objectives:
- divide into API and specific renderers
- add Vulkan 
- make profiling 
- scripting language interpreted to SDF shader code


## Fixes and improvements TODO list:
- more debug logging
