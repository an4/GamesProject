// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class GPProject : ModuleRules
{
    // See https://wiki.unrealengine.com/Linking_Static_Libraries_Using_The_Build_System for details
    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

	public GPProject(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Load OpenCV libraries in build system
        LoadOpenCV(Target);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }
	}

    public bool LoadOpenCV(TargetInfo Target)
    {
        const string cvDir = "opencv3.0.0b";
        // TODO: trim this list
        // TODO: comctl32.lib = windows controls => remove or find a proper way to include!
        //string[] libNames = {
        //    "comctl32.lib",
        //    "ippicvmt.lib",
        //    "IlmImf.lib",
        //    "libjasper.lib",
        //    "libjpeg.lib",
        //    "libpng.lib",
        //    "libtiff.lib",
        //    "libwebp.lib",
        //    "opencv_calib3d300.lib",
        //    "opencv_core300.lib",
        //    "opencv_features2d300.lib",
        //    "opencv_flann300.lib",
        //    "opencv_highgui300.lib",
        //    "opencv_imgcodecs300.lib",
        //    "opencv_imgproc300.lib",
        //    "opencv_ml300.lib",
        //    "opencv_objdetect300.lib",
        //    "opencv_photo300.lib",
        //    "opencv_shape300.lib",
        //    "opencv_stitching300.lib",
        //    "opencv_superres300.lib",
        //    "opencv_ts300.lib",
        //    "opencv_video300.lib",
        //    "opencv_videoio300.lib",
        //    "opencv_videostab300.lib",
        //    "zlib.lib",
        //};
        string[] libNames = {
                                "opencv_world300.lib",
                                "opencv_ts300.lib"
                            };

        bool isLibrarySupported = false;

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, cvDir, "lib", PlatformString);

            foreach (string lib in libNames)
            {
                PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, lib));
            }
        }

        if (isLibrarySupported)
        {
            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, cvDir, "include"));
        }

        Definitions.Add(string.Format("WITH_OPENCV_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }
}
