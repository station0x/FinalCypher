// Copyright Pandores Marketplace 2022. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

#if UE_5_0_OR_LATER
using EpicGames.Core;
#else
using Tools.DotNETCommon;
#endif

public class FirebaseFeatures : ModuleRules
{
	/**
	 * If we should statically link against firebase C++ SDK debug libraries (Windows only).
	 * Note that it requires a Source Build and to link the build to the Debug CRT.
	 * You would typically add the following line in your Target.cs file:
	 *		bDebugBuildsActuallyUseDebugCRT = true;
	 *		
	 * /!\ Debug Libraries' binaries are not included by default anymore. /!\
	 **/
	static bool bLinkDebugFirebaseLibrary = false;

	/**
	 * If we want to link libraries compiled with -fPIC instead of the default
	 * ones for Linux.
	 **/
	static bool bLinuxLinkPICLibraries = true;

	/**
	 *	The firebase libraries we statically link. (except for iOS).
	 **/
	string[] FirebaseLibraries = new string[]
	{
		// Don't disable. This is the core library needed by all others.
		// Disabling it will result in link errors.
		"firebase_app",

		/**
		 * You can comment out libraries here to remove completly a module's code 
		 * in the plugin. Calling a function after doing so will result in a no-op.
		 * 
		 * Here is how you should do to disable AdMob for example:
		 * //"firebase_admob",
		 *
		 * If you get compilation errors after doing so, it means the module
		 * wasn't programmed to be disabled. If you want this functionality,
		 * just send an email to pandores.marketplace@gmail.com with the request.
		 * Also note that some modules depend on others. (firestore can't be 
		 * used without auth, ...)
		 **/
		
		"firebase_admob",
		"firebase_analytics",
		"firebase_auth",
		"firebase_database",
		"firebase_dynamic_links",
		"firebase_firestore",
		"firebase_functions",
		"firebase_messaging",
		"firebase_remote_config",
		"firebase_storage",
		//"firebase_instance_id",
	};

	/**
	 *  Firebase's frameworks for IOS.
	 **/
	string[] FirebaseFrameworks = new string[]
	{
		"firebase",

		/**
		 * The same as above applies for iOS with the below frameworks.
		*/

		"firebase_admob",
		"firebase_analytics",
		"firebase_auth",
		"firebase_database",
		"firebase_dynamic_links",
		"firebase_firestore",
		"firebase_functions",
		"firebase_instance_id",
		"firebase_messaging",
		"firebase_remote_config",
		"firebase_storage",
	};

	string FirebaseSdkRoot;
	string ProjectDirectory;

	bool bIsPluginInEngineDir;

	public FirebaseFeatures(ReadOnlyTargetRules Target) : base(Target)
	{ 
		// Path pointing to the SDK's root.
		FirebaseSdkRoot = Path.Combine(PluginDirectory, "Source/ThirdParty/firebase_cpp_sdk/");

		ProjectDirectory = (Target != null && Target.ProjectFile != null ? Path.GetDirectoryName(Target.ProjectFile.ToString()) : "");

		// We need to read from Engine's config for modules' options.
		DirectoryReference ProjectDirRef = (Target != null && Target.ProjectFile != null) ? DirectoryReference.FromFile(Target.ProjectFile) : null;
		EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, ProjectDirRef, Target.Platform);

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// Unreal Engine Dependencies
		PublicDependencyModuleNames .AddRange(new string[] { "Core" });
		PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine", "OpenSSL" });

		// Config values for mediation taken from DefaultEngine.ini.
		string FirebaseModulesSection = "/Script/FirebaseFeatures.FirebaseConfig";
		bEnableAuth			= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableAuth",			true);
		bEnableFirestore	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableFirestore",		true);
		bEnableDatabase		= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableDatabase",		true);
		bEnableAdMob		= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableAdMob",			true);
		bEnableAnalytics	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableAnalytics",		true);
		bEnableFunctions	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableFunctions",		true);
		bEnableMessaging	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableMessaging",		true);
		bEnableRemoteConfig	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableRemoteConfig",	true);
		bEnableStorage		= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableStorage",		true);
		bEnableCrashlytics	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableCrashlytics",	true);
		bEnableDynamicLinks	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableDynamicLinks",	true);
		bEnablePerformance	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnablePerformance",	true);

		// Prints useful information about the environment of the user.
		CheckEnvironment(Target);

		// Editor-only config.
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] 
			{ 
				"Slate", 
				"SlateCore", 
				"UnrealEd", 
				"PropertyEditor", 
				"Settings", 
				"Projects" 
			});
		}

		// Android Build configuration.
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			// Engine dependencies.
			PrivateDependencyModuleNames.Add("Launch");

			// Firebase includes.
			PublicIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "8.9.0/inc/"));

			// Android's UPL file.
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "FirebaseSdk.android.upl.xml"));

			LinkFirebaseLibraries();
			
			AddRuntimeDependency(Path.Combine(GetServicesDir(Target), "google-services.json"));
		}

		// IOS Build Configuration.
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			// Required system frameworks.
			PublicFrameworks.AddRange(new string[]
			{
				//"UniformTypeIdentifiers",
				"UserNotifications",
				"CoreFoundation",
				"Foundation",
				"Security",
				"GSS",
				"SystemConfiguration",
				"JavaScriptCore",
				"LocalAuthentication"
			});

			// Firebase includes. Switched back to libs for the cpp sdk.
			PublicIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "include/ios"));

			// iOS UPL
			AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(ModuleDirectory, "FirebaseSdk.ios.upl.xml"));

			// Objective-C Frameworks

			if (bEnableCrashlytics)
            {
				AddFrameworkChecked("FirebaseCrashlytics");
            }

			if (bEnablePerformance)
            { 
				AddFrameworkChecked("FirebasePerformance");
				AddFrameworkChecked("GTMSessionFetcher");
				AddFrameworkChecked("FirebaseABTesting");
				AddFrameworkChecked("FirebaseRemoteConfig");
			}

			if (System.Array.IndexOf(FirebaseFrameworks, "firebase") >= 0)
			{
				AddFirebaseFramework("firebase");
				AddFrameworkChecked("FirebaseCore");
				AddFrameworkChecked("FirebaseCoreDiagnostics");
				AddFrameworkChecked("GoogleUtilities");
				AddFrameworkChecked("GoogleDataTransport");
				AddFrameworkChecked("FirebaseInstallations");
				AddFrameworkChecked("PromisesObjC");
				AddFrameworkChecked("nanopb");
				AddFrameworkChecked("leveldb-library");
			}

			if (bEnableDatabase && System.Array.IndexOf(FirebaseFrameworks, "firebase_database") >= 0)
			{
				AddFirebaseFramework("firebase_database");
				AddFrameworkChecked("FirebaseDatabase");
				AddFrameworkChecked("leveldb-library");
			}

			if (bEnableStorage && System.Array.IndexOf(FirebaseFrameworks, "firebase_storage") >= 0)
			{
				AddFirebaseFramework("firebase_storage");
				AddFrameworkChecked("FirebaseStorage");
				AddFrameworkChecked("GTMSessionFetcher");
			}

			if (bEnableDynamicLinks && System.Array.IndexOf(FirebaseFrameworks, "firebase_dynamic_links") >= 0)
			{
				AddFirebaseFramework("firebase_dynamic_links");
				AddFrameworkChecked("FirebaseDynamicLinks");
			}

			if (bEnableFunctions && System.Array.IndexOf(FirebaseFrameworks, "firebase_functions") >= 0)
			{
				AddFirebaseFramework("firebase_functions");
				AddFrameworkChecked("FirebaseFunctions");
				AddFrameworkChecked("GTMSessionFetcher");
			}

			if (bEnableFirestore && System.Array.IndexOf(FirebaseFrameworks, "firebase_firestore") >= 0)
			{
				AddFirebaseFramework("firebase_firestore");
				AddFrameworkChecked("FirebaseFirestore");
				AddFrameworkChecked("BoringSSL-GRPC");
				AddFrameworkChecked("abseil");
				AddFrameworkChecked("gRPC-C++", "Resources/gRPCCertificates-Cpp.bundle");
				AddFrameworkChecked("gRPC-Core");
				AddFrameworkChecked("Libuv-gRPC");
				AddFrameworkChecked("leveldb-library");
			}

			// Auth Objective-C dependencies.
			if (bEnableAuth && System.Array.IndexOf(FirebaseFrameworks, "firebase_auth") >= 0)
			{
				AddFirebaseFramework("firebase_auth");
				AddFrameworkChecked("AppAuth");
				AddFrameworkChecked("GoogleSignIn");
				AddFrameworkChecked("FirebaseAuth");
				AddFrameworkChecked("GTMSessionFetcher");
			}

			// Messaging Objective-C dependencies.
			if (bEnableMessaging && System.Array.IndexOf(FirebaseFrameworks, "firebase_messaging") >= 0)
			{
				AddFirebaseFramework("firebase_messaging");
				AddFrameworkChecked("FirebaseMessaging");
				AddFrameworkChecked("FirebaseABTesting");
				AddFrameworkChecked("FirebaseInAppMessaging", "Resources/InAppMessagingDisplayResources.bundle");

				PublicFrameworks.Add("UserNotifications");
			}

			// AdMob Objective-C dependencies.
			if (bEnableAdMob && System.Array.IndexOf(FirebaseFrameworks, "firebase_admob") >= 0)
			{
				AddFirebaseFramework("firebase_admob");
				AddFrameworkChecked("GoogleMobileAds");
				AddFrameworkChecked("AppAuth");
				AddFrameworkChecked("GTMAppAuth");
				AddFrameworkChecked("GTMSessionFetcher");
				AddFrameworkChecked("UserMessagingPlatform");
			}

			// Analytics Objective-C dependencies.
			if (bEnableAnalytics && System.Array.IndexOf(FirebaseFrameworks, "firebase_analytics") >= 0)
			{
				AddFirebaseFramework("firebase_analytics");
				AddFrameworkChecked("FirebaseAnalytics");
				AddFrameworkChecked("FirebaseCore");
				AddFrameworkChecked("FirebaseCoreDiagnostics");
				AddFrameworkChecked("FirebaseInstallations");
				AddFrameworkChecked("GoogleAppMeasurement");
				AddFrameworkChecked("GoogleAppMeasurementIdentitySupport");
				AddFrameworkChecked("GoogleDataTransport");
				AddFrameworkChecked("GoogleUtilities");
				AddFrameworkChecked("nanopb");
				AddFrameworkChecked("PromisesObjC");
			}

			if (bEnableRemoteConfig && System.Array.IndexOf(FirebaseFrameworks, "firebase_remote_config") >= 0)
			{
				AddFirebaseFramework("firebase_remote_config");
				AddFrameworkChecked("FirebaseRemoteConfig");
				AddFrameworkChecked("FirebaseABTesting");
			}

			// Native SDK include
			PublicSystemIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "include/ios/"));

			// Required plist file.
			if (!File.Exists(Path.Combine(ProjectDirectory, "Services/GoogleService-Info.plist")) &&
				!File.Exists(Path.Combine(PluginDirectory, "Config/GoogleService-Info.plist")))
			{
				System.Console.WriteLine("\n\t /!\\ Failed to find GoogleService-Info.plist (you can ignore this message in remote builds). /!\\\n ");
			}
			else
			{
				System.Console.WriteLine("GoogleService-Info.plist found.");
			}
		}

		// Win64 Build Configuration.
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			// Required system frameworks.
			PublicFrameworks.AddRange(new string[]
			{
				//"UniformTypeIdentifiers",
				"UserNotifications",
				"CoreFoundation",
				"Foundation",
				"Security",
				"Kerberos",
				"GSS",
				"SystemConfiguration",
				"JavaScriptCore",
				"LocalAuthentication"
			});

			// Firebase includes.
			PublicIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "include"));

			LinkFirebaseLibraries();

			AddRuntimeDependency(Path.Combine(GetServicesDir(Target), "google-services.json"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// System libraries.
			PublicSystemLibraries.Add("userenv.lib");
			PublicSystemLibraries.Add("psapi.lib");

			// Firebase includes.
			PublicIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "include"));
			
			LinkFirebaseLibraries();

			AddRuntimeDependency(Path.Combine(GetServicesDir(Target), "google-services.json"));
		}

		// Linux Build Configuration.
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			// System libraries.
			PublicSystemLibraries.AddRange(new string[] { "pthread", "c" });

			// Engine's Libcurl
			AddEngineThirdPartyPrivateStaticDependencies(Target, "libcurl");

			// Linux's includes.
			PublicIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "include/linux"));

			// Libraries.
			if (bLinuxLinkPICLibraries)
			{
				string DepRoot = Path.Combine(FirebaseSdkRoot, "libs/linux/x86_64_PIC/dependencies/");

				string[] AdditionalDependencies = Directory.GetFiles(DepRoot);
				foreach (string Dependency in AdditionalDependencies)
				{
					PublicAdditionalLibraries.Add(Dependency);
				}
			}
			else
			{
				string DepRoot = Path.Combine(FirebaseSdkRoot, "libs/linux/x86_64/dependencies/");

				PublicAdditionalLibraries.AddRange(new string[]
				{
					Path.Combine(DepRoot, "libfirestore_dep.a"),
					Path.Combine(DepRoot, "libglib-2.0.so"),
					Path.Combine(DepRoot, "libgrpc.a"),
					Path.Combine(DepRoot, "libsecret-1.so")
				});
			}

			// Firebase includes.
			PublicIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "include/linux"));

			LinkFirebaseLibraries();

			AddFirebaseLibrary("firebase_instance_id");
		}
		else
        {
			System.Console.Error.WriteLine("This platform is not supported by Firebase Features.");
        }
		   
		// Module includes
		PublicIncludePaths .Add(Path.Combine(ModuleDirectory, "Public"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
		
		AddRuntimeDependency(Path.Combine(GetServicesDir(Target), "google-services.json"));

		// Firebase Performance doesn't have a C++ library.
		// We manually define it here.
		if (bEnablePerformance)
        {
			PublicDefinitions.Add("WITH_FIREBASE_PERFORMANCE=1");
		}
		if (bEnableCrashlytics)
        {
			PublicDefinitions.Add("WITH_FIREBASE_CRASHLYTICS=1");
        }
	}

	private void LinkFirebaseLibraries()
	{ 
		AddFirebaseLibrary("firebase_app");
		if (bEnableAdMob && System.Array.IndexOf(FirebaseLibraries, "firebase_admob") >= 0)
		{
			AddFirebaseLibrary("firebase_admob");
		}
		if (bEnableAnalytics && System.Array.IndexOf(FirebaseLibraries, "firebase_analytics") >= 0)
		{ 
			AddFirebaseLibrary("firebase_analytics");
		}
		if (bEnableAuth && System.Array.IndexOf(FirebaseLibraries, "firebase_auth") >= 0)
        {
			AddFirebaseLibrary("firebase_auth");
        }
		if (bEnableDatabase && System.Array.IndexOf(FirebaseLibraries, "firebase_database") >= 0)
        {
			AddFirebaseLibrary("firebase_database");
        }
		if (bEnableDynamicLinks && System.Array.IndexOf(FirebaseLibraries, "firebase_dynamic_links") >= 0)
        {
			AddFirebaseLibrary("firebase_dynamic_links");
        } 
		if (bEnableFirestore && System.Array.IndexOf(FirebaseLibraries, "firebase_firestore") >= 0)
        {
			AddFirebaseLibrary("firebase_firestore");
        }
		if (bEnableFunctions && System.Array.IndexOf(FirebaseLibraries, "firebase_functions") >= 0)
        {
			AddFirebaseLibrary("firebase_functions");
        }
		if (bEnableMessaging && System.Array.IndexOf(FirebaseLibraries, "firebase_messaging") >= 0)
        {
			AddFirebaseLibrary("firebase_messaging");
        }
		if (bEnableRemoteConfig && System.Array.IndexOf(FirebaseLibraries, "firebase_remote_config") >= 0)
        {
			AddFirebaseLibrary("firebase_remote_config");
        }
		if (bEnableStorage && System.Array.IndexOf(FirebaseLibraries, "firebase_storage") >= 0)
        {
			AddFirebaseLibrary("firebase_storage");
        }
	}

	private string[] GetFirebaseSdkRelativeLibsDir(ReadOnlyTargetRules Target)
	{
		const string LibsDir = "libs/";

		bool bIsDebugBuild = IsDebugBuild();

		// Removed for UE5 support.
		//if (Target.Platform == UnrealTargetPlatform.Win32)
		//{
		//	return new string[]
		//	{
		//		LibsDir + "/windows/VS2015/MD/x86/" 
		//			+ (bIsDebugBuild ? "Debug" : "Release")
		//	};
		//}

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			return new string[]
			{
				LibsDir + "windows/VS2015/MD/x64/"
					+ (bIsDebugBuild ? "Debug" : "Release")
			};
		}

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			return new string[]
			{
				LibsDir + "darwin/universal",
			};
		}

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			return new string[]
			{
				LibsDir + "../8.9.0/libs/android/armeabi-v7a/c++",
				LibsDir + "../8.9.0/libs/android/arm64-v8a/c++",

				// Uncomment these libs if you target these architectures.
				// Note that Unreal Engine doesn't let you build x86 without a Source Build.
				//LibsDir + "android/x86/c++",
				//LibsDir + "android/x86_64/c++",
			};
		}

		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			return new string[]
			{
				LibsDir + "ios/universal/",
				
				// Uncomment these libs if you target these architectures.
				//LibsDir + "ios/arm64/",
				//LibsDir + "ios/armv7/",
				//LibsDir + "ios/i386/",
				//LibsDir + "ios/x86_64/",
			};
		}

		if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			if (bLinuxLinkPICLibraries)
            {
				return new string[]
				{
					LibsDir + "linux/x86_64_PIC/"
			   };
			}

			else return new string[]
			{
				LibsDir + "linux/x86_64/"
			};
		}

		throw new System.Exception("The plugin doesn't contain the C++ libraries for this platform.");
	}

	private string GetPlatformLibPrefix()
	{
		return
			Target.Platform == UnrealTargetPlatform.Win64 ?
				"" : "lib";
	}

	private string GetPlatformLibPostfix()
	{
		return
			Target.Platform == UnrealTargetPlatform.Win64 ?
				"lib" : "a";
	}

	private bool IsDebugBuild()
    {
		if (bLinkDebugFirebaseLibrary)
		{
			switch (Target.Configuration)
			{
				case UnrealTargetConfiguration.DebugGame:
				case UnrealTargetConfiguration.Debug:
					return true;
				default:
					return false;
			}
		}
		return false;
	}

	private Framework CreateFramework(string Name, string Path, string Bundle)
    {
		return new Framework
		(
			Name, Path, Bundle
		);
	}

	private void AddFrameworkChecked(string Name, string Bundle = null, bool bCppFramework = false)
    {
		string FrameworkRoot	 = Path.Combine(FirebaseSdkRoot, "frameworks/ios/universal");
		string FrameworkLocation = FrameworkRoot + "/" + (bCppFramework ? "cpp" : "objc") + "/" + Name + ".framework.zip";

		if (!File.Exists(FrameworkLocation))
        {
			System.Console.Error.WriteLine("Failed to find iOS framework \"" + Name + "\" for Firebase as file " + FrameworkLocation + " doesn't exist.");
			return;
		}

		PublicAdditionalFrameworks.Add(CreateFramework
		(
			Name, FrameworkLocation, Bundle
		));
	}

	private void AddRuntimeDependency(string Path, string Target = null, StagedFileType Type = StagedFileType.UFS)
    {
		if (Target == null)
        {
			RuntimeDependencies.Add(Path, Type);
		}
		else
        {
			RuntimeDependencies.Add(Target, Path, Type);
		}
    }

	private string GetServicesDir(ReadOnlyTargetRules Target)
    {
		return "$(ProjectDir)/Services/";
	}

	private bool GetFeaturesEnabledStatus(string Section, string Key, bool bDefault = false)
	{
		// We want to link everything when the plugin is not built directly.
		if (bIsPluginInEngineDir)
        {
			return true;
        }
		
		bool bValue = bDefault;
		if (EngineConfig.TryGetValue(Section, Key, out bValue))
		{
			return bValue;
		}
		return bDefault;
	}

	private void AddFirebaseLibrary(string LibraryName)
    {
		string[] LibrariesDir = GetFirebaseSdkRelativeLibsDir(Target);
		foreach (string LibraryDir in LibrariesDir)
        {
			string LibLocation = Path.Combine(FirebaseSdkRoot, LibraryDir, GetPlatformLibPrefix() + LibraryName + "." + GetPlatformLibPostfix());
			if (!File.Exists(LibLocation))
			{
				System.Console.Error.WriteLine("Required library " + LibLocation + " doesn't exist.");
			}
			else
			{
				PublicAdditionalLibraries.Add(LibLocation);
				PublicDefinitions.Add("WITH_" + LibraryName.ToUpper() + "=1");
			}
		}
	}

	private void AddFirebaseFramework(string FrameworkName)
    {
		PublicAdditionalLibraries.Add(Path.Combine(FirebaseSdkRoot, "frameworks/ios/universal/cpp/libs/lib" + FrameworkName + ".a"));
		PublicDefinitions.Add("WITH_" + FrameworkName.ToUpper() + "=1");
	}

	private void CheckEnvironment(ReadOnlyTargetRules Target)
    {
		// Engine plugin's binaries aren't rebuilt. If we don't link frameworks here, we'll get unresolved
		// external symbols. We just notice the users that they need to actually rebuild the plugin to disable 
		// the module.

		System.Console.WriteLine("Firebase Features plugin path is " + PluginDirectory);

		bIsPluginInEngineDir = PluginDirectory.Contains("Engine/Plugins") || PluginDirectory.Contains("Engine\\Plugins");

		// Change true to false if you don't want these messages to appear anymore.
		if (true)
		{ 
			if (bIsPluginInEngineDir)
			{
					System.Console.WriteLine("\n\t /!\\ Firebase Features is being included in a build as an Engine plugin. " +
						"It means that its binaries won't be rebuilt and thus, C++ changes of disabling or enabling " +
						"features won't take effect. If you want to remove C++ libraries from the build, you have to " +
						"rebuild the plugin.\n");
			}
			else
			{
				// Summary of what's linked or not to help troubleshooting.
				System.Console.WriteLine("Firebase Features is being included in a build as a Project plugin. Linkage will follow the plugin's configuration: ");
				PrintModuleLinkageStatus("Auth",			bEnableAuth);
				PrintModuleLinkageStatus("Firestore",		bEnableFirestore);
				PrintModuleLinkageStatus("Database",		bEnableDatabase);
				PrintModuleLinkageStatus("AdMob",			bEnableAdMob);
				PrintModuleLinkageStatus("Analytics",		bEnableAnalytics);
				PrintModuleLinkageStatus("Functions",		bEnableFunctions);
				PrintModuleLinkageStatus("Messaging",		bEnableMessaging);
				PrintModuleLinkageStatus("Remote Config",	bEnableRemoteConfig);
				PrintModuleLinkageStatus("Storage",			bEnableStorage);
				PrintModuleLinkageStatus("Crashlytics",		bEnableCrashlytics);
				PrintModuleLinkageStatus("Dynamic Links",	bEnableDynamicLinks);
				PrintModuleLinkageStatus("Performance",		bEnablePerformance);
			}
		}

		// Checks for the directory of iOS frameworks.
		if (!Directory.Exists(Path.Combine(FirebaseSdkRoot, "frameworks/ios/universal")))
        {
			if (Target.Platform == UnrealTargetPlatform.IOS)
            {
				System.Console.WriteLine("\n\n /!\\ iOS Firebase framework directory does not exist. Build will fail. /!\\\n ");
            }
			else
            {
				System.Console.WriteLine("Failed to find iOS Firebase frameworks directory.");
			}
        }

	}

	private void PrintModuleLinkageStatus(string Module, bool bLinked)
    { 
		System.Console.WriteLine(("  * Firebase " + Module + ": ").PadRight(30) + (bLinked ? "Linked" : "Disabled"));
	}

	private ConfigHierarchy EngineConfig;

	private bool bEnableAuth;
	private bool bEnableFirestore;
	private bool bEnableDatabase;
	private bool bEnableAdMob;
	private bool bEnableAnalytics;
	private bool bEnableFunctions;
	private bool bEnableMessaging;
	private bool bEnableRemoteConfig;
	private bool bEnableStorage;
	private bool bEnableCrashlytics;
	private bool bEnableDynamicLinks;
	private bool bEnablePerformance;

}

