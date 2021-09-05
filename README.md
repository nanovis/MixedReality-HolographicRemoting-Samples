# Opengl-on-Hololens2

This code is a sample program for streaming OpenGL API on Hololens2.

We modified  [microsoft/MixedReality-HolographicRemoting-Samples](https://github.com/microsoft/MixedReality-HolographicRemoting-Samples). microsoft/MixedReality-HolographicRemoting-Samples to enable OpenGL.

## Setup
1- Open the .sln file.

2- On first use, right-click the solution and select Restore NuGet Packages.
#### Running  using Visual studio:
 Debug Propeties -> Configuration poperities -> Debugging -> command Arguments -> ``` <Hololens2 Ip Address>  ```

#### Command line 
  ```
./SampleRemoteOpenXr.exe <Hololens2 Ip Address.> 
  ```
 ## Output
  The below Windows show the rendered scene from Both OpenGL and Hololens2

<img src="https://github.com/nanovis/Opengl-on-Hololens2/blob/main/Images/OpenGL%20win.png" width="300" height="200"> <img src="https://github.com/nanovis/Opengl-on-Hololens2/blob/main/Images/DirectX%20Win.png" width="300" height="200">

### Hololens2 View 

<img src="https://github.com/nanovis/Opengl-on-Hololens2/blob/main/Images/Hololens.png" width="300" height="100">



### 3D Manipulation 

The user can manipulate the triangle with  translation, rotation, and scaling.

Right hand -> translation.

Left hand-> rotation. 

Both hands -> scaling. 

Close your thumbs and your index finger to start the interaction. Follow the hand gesture in the following image:
  
<img src="https://github.com/nanovis/Opengl-on-Hololens2/blob/main/Images/movment.jpg" width="400" height="400">

