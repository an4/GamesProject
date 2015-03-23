Cloning, Compiling, and Contributing
====================================

1. Clone repository (should work outside of the Unreal Projects directory)
2. Inside the game directory, run ```git submodule update --init```. This will clone in the required API files for the Kinect connection. You may need to setup SSH keys in your git client.
3. Navigate to the directory, right click the .uproject file and select "Generate Visual Studio project files"
4. Open the new .sln file in Visual Studio
5. Rebuild the project
6. Run the project, and the editor should open and the game should be playable!
