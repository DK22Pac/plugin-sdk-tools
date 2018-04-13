using System;
using System.Collections.Generic;
using Microsoft.VisualStudio.TemplateWizard;
using Microsoft.VisualStudio.Shell;
using EnvDTE;
using System.IO;
using System.Windows;
using System.Globalization;

namespace PluginSdkWizard {
    public class WizardImpl : IWizard {
        private WizardMainWindow window;
        private DTE dte;
        private string solutionDirectory;
        private string solutionName;
        private string destinationDirectory;
        private int pluginType;
        private bool bOnlySa;

        public void BeforeOpeningFile(ProjectItem projectItem) { }

        public void ProjectFinishedGenerating(Project project) { }

        public void ProjectItemFinishedGenerating(ProjectItem projectItem) { }

        public void RunFinished() { }

        string GetConfigName(string config, string gameName) {
            string configName = gameName + " ";
            if (config == "Debug")
                configName += "z";
            return configName + config;
        }

        string GetConfigFullName(string config, string gameName) {
            return GetConfigName(config, gameName) + "|Win32";
        }

        private void SetupConfiguration(ref string pcf, ref string pgc, ref string igp, ref string pgd, ref string idg,
            string config, bool isDebug, string gameName, string gameId, string toolset)
        {
            string configName = GetConfigName(config, gameName);
            string configFullName = GetConfigFullName(config, gameName);

            string outDirName = window.GetOutputDirName();

            bool usesD3d = window.cbUseDirectXSdk.IsChecked == true;

            string gameDir = "";
            if (gameId == "SA")
                gameDir = VsUtility.GetName(window.tbSaDir.Text, "$(GTA_SA_DIR)");
            else if (gameId == "VC")
                gameDir = VsUtility.GetName(window.tbVcDir.Text, "$(GTA_VC_DIR)");
            else
                gameDir = VsUtility.GetName(window.tbIIIDir.Text, "$(GTA_III_DIR)");
            string outputDir = VsUtility.FormatName(window.tbOutputFolder.Text, "$(SolutionDir)\\output", window.safeProjectName,
                "$(ProjectDir)", "$(SolutionDir)", gameId, gameDir, outDirName, config);
            outputDir = VsUtility.NormalisePath(outputDir);
            string targetName = VsUtility.FormatName(window.tbTargetName.Text, window.safeProjectName + "." + gameId, window.safeProjectName,
                "", "", gameId, "", "", config);
            string targetExt = "";
            string fileExt = VsUtility.GetFileNameExtension(targetName);
            if (!String.IsNullOrEmpty(fileExt) && VsUtility.IsLibraryExtension(fileExt)) {
                targetExt = fileExt;
                targetName = targetName.Substring(0, targetName.Length - fileExt.Length);
            }
            else {
                if (pluginType == 1) // CLEO Plugin
                    targetExt = "cleo";
                else if (pluginType == 2 || pluginType == 3) // MoonLoader Module or DLL Library
                    targetExt = "dll";
                else // ASI Plugin
                    targetExt = "asi";
            }
            if (isDebug)
                targetName += "_d";

            List<string> includeDirs = new List<string>{ };
            List<string> libraryDirs = new List<string> { };
            List<string> libraries = new List<string> { };
            List<string> defs = new List<string> { };

            string pluginSdkDir = VsUtility.GetName(window.tbPluginSdkDir.Text, "$(PLUGIN_SDK_DIR)");
            pluginSdkDir = VsUtility.NormalisePath(pluginSdkDir);
            includeDirs.Add(pluginSdkDir + "plugin_" + gameId.ToLower() + "\\");
            includeDirs.Add(pluginSdkDir + "plugin_" + gameId.ToLower() + "\\game_" + gameId.ToLower() + "\\");
            includeDirs.Add(pluginSdkDir + "shared\\");
            libraryDirs.Add(pluginSdkDir + "output\\lib\\");
            string pluginLibName = "plugin";
            if (gameId != "SA")
                pluginLibName += "_" + gameId.ToLower();
            if (isDebug)
                pluginLibName += "_d";
            libraries.Add(pluginLibName);

            if (usesD3d) {
                libraries.Add("d3d9");
                libraries.Add("d3dx9");
                if (gameId == "VC" || gameId == "III") {
                    string rwD3D9Dir = VsUtility.GetName(window.tbRwD3d9Dir.Text, "$(RWD3D9_DIR)");
                    rwD3D9Dir = VsUtility.NormalisePath(rwD3D9Dir);
                    includeDirs.Add(rwD3D9Dir + "source\\");
                    libraryDirs.Add(rwD3D9Dir + "libs\\");
                    libraries.Add("rwd3d9");
                }
            }

            if (isDebug)
                defs.Add("_DEBUG");
            else
                defs.Add("_NDEBUG");
            defs.Add("_USING_V110_SDK71_");
            defs.Add("_CRT_SECURE_NO_WARNINGS");
            defs.Add("_CRT_NON_CONFORMING_SWPRINTFS");
            defs.Add(gameName);
            if (gameId == "SA") {
                defs.Add("GTAGAME_NAME=\"San Andreas\"");
                defs.Add("GTAGAME_ABBR=\"SA\"");
                defs.Add("GTAGAME_ABBRLOW=\"sa\"");
                defs.Add("GTAGAME_PROTAGONISTNAME=\"CJ\"");
                defs.Add("GTAGAME_CITYNAME=\"San Andreas\"");
            }
            else if (gameId == "VC") {
                defs.Add("GTAGAME_NAME=\"Vice City\"");
                defs.Add("GTAGAME_ABBR=\"VC\"");
                defs.Add("GTAGAME_ABBRLOW=\"vc\"");
                defs.Add("GTAGAME_PROTAGONISTNAME=\"Tommy\"");
                defs.Add("GTAGAME_CITYNAME=\"Vice City\"");
            }
            else {
                defs.Add("GTAGAME_NAME=\"3\"");
                defs.Add("GTAGAME_ABBR=\"3\"");
                defs.Add("GTAGAME_ABBRLOW=\"3\"");
                defs.Add("GTAGAME_PROTAGONISTNAME=\"Claude\"");
                defs.Add("GTAGAME_CITYNAME=\"Liberty City\"");
            }
            if (pluginType == 1) { // CLEO Plugin
                string cleoSdkDirPath = "";
                if (gameId == "SA") {
                    libraries.Add("cleo");
                    cleoSdkDirPath = window.tbSaCleoSdkDir.Text;
                }
                else {
                    libraries.Add(gameId + ".CLEO");
                    if (gameId == "VC")
                        cleoSdkDirPath = window.tbVcCleoSdkDir.Text;
                    else
                        cleoSdkDirPath = window.tbIIICleoSdkDir.Text;
                }
                string cleoSdkDir = VsUtility.GetName(cleoSdkDirPath, "$(CLEO_SDK_" + gameId + "_DIR)");
                cleoSdkDir = VsUtility.NormalisePath(cleoSdkDir);
                includeDirs.Add(cleoSdkDir);
                libraryDirs.Add(cleoSdkDir);
            }
            else if (pluginType == 2) { // MoonLoader Module
                string moonloaderSdkDir = VsUtility.GetName(window.tbMoonLoaderSdkDir.Text, "$(MOONLOADER_SDK_SA_DIR)");
                moonloaderSdkDir = VsUtility.NormalisePath(moonloaderSdkDir);
                includeDirs.Add(moonloaderSdkDir + "src\\");
                includeDirs.Add(moonloaderSdkDir + "src\\libs\\lua\\");
                includeDirs.Add(moonloaderSdkDir + "src\\libs\\sol2\\");
                libraryDirs.Add(moonloaderSdkDir + "src\\libs\\lua\\");
                libraries.Add("lua51");
                defs.Add("LUA_BUILD_AS_DLL");
                defs.Add("MODULE_NAME=" + window.safeProjectName);
            }
            if (window.cbLimitAdjusterCompatibilityMode.IsChecked == true)
                defs.Add("_LA_SUPPORT");
            if (usesD3d)
                defs.Add("_DX9_SDK_INSTALLED");

            // game versions
            if (gameId == "SA") {
                if (window.cbVersionSA_10US.IsChecked == true)
                    defs.Add("PLUGIN_SGV_10US");
                if (window.cbVersionSA_10EU.IsChecked == true)
                    defs.Add("PLUGIN_SGV_10EU");
                if (window.cbVersionSA_11US.IsChecked == true)
                    defs.Add("PLUGIN_SGV_11US");
                if (window.cbVersionSA_11EU.IsChecked == true)
                    defs.Add("PLUGIN_SGV_11EU");
                if (window.cbVersionSA_Steam.IsChecked == true)
                    defs.Add("PLUGIN_SGV_SR2");
                if (window.cbVersionSA_SteamLV.IsChecked == true)
                    defs.Add("PLUGIN_SGV_SR2LV");
            }
            else if (gameId == "VC") {
                if (window.cbVersionVC_10.IsChecked == true)
                    defs.Add("PLUGIN_SGV_10EN");
                if (window.cbVersionVC_11.IsChecked == true)
                    defs.Add("PLUGIN_SGV_11EN");
                if (window.cbVersionVC_Steam.IsChecked == true)
                    defs.Add("PLUGIN_SGV_STEAM");
            }
            else {
                if (window.cbVersionIII_10.IsChecked == true)
                    defs.Add("PLUGIN_SGV_10EN");
                if (window.cbVersionIII_11.IsChecked == true)
                    defs.Add("PLUGIN_SGV_11EN");
                if (window.cbVersionIII_Steam.IsChecked == true)
                    defs.Add("PLUGIN_SGV_STEAM");
            }

            List<string> additionalIncludeDirs = VsUtility.SplitListLine(isDebug ? window.tbDbgAdditionalIncDirs.Text : window.tbRelAdditionalIncDirs.Text);
            List<string> additionalLibraryDirs = VsUtility.SplitListLine(isDebug ? window.tbDbgAdditionalLibDirs.Text : window.tbRelAdditionalLibDirs.Text);
            List<string> additionalLibraries = VsUtility.SplitListLine(isDebug ? window.tbDbgAdditionalLibs.Text : window.tbRelAdditionalLibs.Text);
            List<string> additionalDefs = VsUtility.SplitListLine(isDebug ? window.tbDbgAdditionalDefs.Text : window.tbRelAdditionalDefs.Text);

            for (int i = 0; i < additionalIncludeDirs.Count; i++) {
                additionalIncludeDirs[i] = VsUtility.FormatName(additionalIncludeDirs[i], "", window.safeProjectName,
                    "$(ProjectDir)", "$(SolutionDir)", gameId, gameDir, outDirName, config);
            }
            for (int i = 0; i < additionalLibraryDirs.Count; i++) {
                additionalLibraryDirs[i] = VsUtility.FormatName(additionalLibraryDirs[i], "", window.safeProjectName,
                    "$(ProjectDir)", "$(SolutionDir)", gameId, gameDir, outDirName, config);
            }
            for (int i = 0; i < additionalLibraries.Count; i++)
                additionalLibraries[i] = VsUtility.FormatName(additionalLibraries[i], "", window.safeProjectName, "", "", gameId, "", "", config);
            for (int i = 0; i < additionalDefs.Count; i++)
                additionalDefs[i] = VsUtility.FormatName(additionalDefs[i], "", window.safeProjectName, "", "", gameId, "", "", config);

            includeDirs.AddRange(additionalIncludeDirs);
            libraryDirs.AddRange(additionalLibraryDirs);
            libraries.AddRange(additionalLibraries);
            defs.AddRange(additionalDefs);

            for (int i = 0; i < libraries.Count; i++) {
                if (!libraries[i].EndsWith(".lib") && !libraries[i].EndsWith(".a") && !libraries[i].EndsWith(".obj"))
                    libraries[i] += ".lib";
            }

            VsUtility.AddLine(ref pcf, "    <ProjectConfiguration Include=\"" + configFullName + "\">");
            VsUtility.AddLine(ref pcf, "      <Configuration>" + configName + "</Configuration>");
            VsUtility.AddLine(ref pcf, "      <Platform>Win32</Platform>");
            VsUtility.AddLine(ref pcf, "    </ProjectConfiguration>");

            VsUtility.AddLine(ref pgc, "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + configFullName + "'\" Label=\"Configuration\">");
            VsUtility.AddLine(ref pgc, "    <ConfigurationType>DynamicLibrary</ConfigurationType>");
            VsUtility.AddLine(ref pgc, "    <UseDebugLibraries>" + (isDebug ? "true" : "false") + "</UseDebugLibraries>");
            VsUtility.AddLine(ref pgc, "    <CharacterSet>MultiByte</CharacterSet>");
            VsUtility.AddLine(ref pgc, "    <PlatformToolset>" + toolset + "</PlatformToolset>");
            if (!isDebug)
                VsUtility.AddLine(ref pgc, "    <WholeProgramOptimization>true</WholeProgramOptimization>");
            VsUtility.AddLine(ref pgc, "  </PropertyGroup>");

            VsUtility.AddLine(ref igp, "  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='" + configFullName + "'\">");
            VsUtility.AddLine(ref igp, "    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />");
            VsUtility.AddLine(ref igp, "  </ImportGroup>");

            VsUtility.AddLine(ref pgd, "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + configFullName + "'\">");
            VsUtility.AddLine(ref pgd, "    <OutDir>" + outputDir + "</OutDir>");
            VsUtility.AddLine(ref pgd, "    <IntDir>$(ProjectDir).obj\\" + gameName + "\\" + config + "\\</IntDir>");
            VsUtility.AddLine(ref pgd, "    <TargetName>" + targetName + "</TargetName>");
            VsUtility.AddLine(ref pgd, "    <TargetExt>." + targetExt + "</TargetExt>");
            if (usesD3d) {
                string dxSdkDir = VsUtility.GetName(window.tbDirectX9SdkDir.Text, "$(DIRECTX9_SDK_DIR)");
                dxSdkDir = VsUtility.NormalisePath(dxSdkDir);
                VsUtility.AddLine(ref pgd, "    <IncludePath>$(IncludePath);" + dxSdkDir + "Include\\" + "</IncludePath>");
                VsUtility.AddLine(ref pgd, "    <LibraryPath>$(LibraryPath);" + dxSdkDir + "Lib\\x86\\" + "</LibraryPath>");
            }
            VsUtility.AddLine(ref pgd, "  </PropertyGroup>");

            VsUtility.AddLine(ref idg, "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='" + configFullName + "'\">");
            VsUtility.AddLine(ref idg, "    <ClCompile>");
            VsUtility.AddLine(ref idg, "      <WarningLevel>Level3</WarningLevel>");
            VsUtility.AddLine(ref idg, "      <Optimization>" + (isDebug ? "Disabled" : "MaxSpeed") + "</Optimization>");
            if (!isDebug) {
                VsUtility.AddLine(ref idg, "      <FunctionLevelLinking>true</FunctionLevelLinking>");
                VsUtility.AddLine(ref idg, "      <IntrinsicFunctions>true</IntrinsicFunctions>");
            }
            VsUtility.AddLine(ref idg, "      <SDLCheck>false</SDLCheck>");
            VsUtility.AddLine(ref idg, "      <RuntimeLibrary>" + (isDebug ? "MultiThreadedDebug" : "MultiThreaded") + "</RuntimeLibrary>");
            VsUtility.AddLine(ref idg, "      <AdditionalIncludeDirectories>" + VsUtility.JoinList(includeDirs, true) + "%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>");
            VsUtility.AddLine(ref idg, "      <PreprocessorDefinitions>" + VsUtility.JoinList(defs, true) + "%(PreprocessorDefinitions)</PreprocessorDefinitions>");
            VsUtility.AddLine(ref idg, "      <LanguageStandard>stdcpplatest</LanguageStandard>");
            VsUtility.AddLine(ref idg, "      <AdditionalOptions>/Zc:threadSafeInit- %(AdditionalOptions)</AdditionalOptions>");
            if (pluginType == 2) {
                VsUtility.AddLine(ref idg, "      <PrecompiledHeader>Create</PrecompiledHeader>");
                VsUtility.AddLine(ref idg, "      <PrecompiledHeaderFile>pch.h</PrecompiledHeaderFile>");
            }
            VsUtility.AddLine(ref idg, "    </ClCompile>");
            VsUtility.AddLine(ref idg, "    <Link>");
            if (!isDebug) {
                VsUtility.AddLine(ref idg, "      <EnableCOMDATFolding>true</EnableCOMDATFolding>");
                VsUtility.AddLine(ref idg, "      <OptimizeReferences>true</OptimizeReferences>");
            }
            VsUtility.AddLine(ref idg, "      <GenerateDebugInformation>" + (isDebug ? "Debug" : "No") + "</GenerateDebugInformation>");
            VsUtility.AddLine(ref idg, "      <LinkTimeCodeGeneration>" + (isDebug ? "Default" : "UseLinkTimeCodeGeneration") + "</LinkTimeCodeGeneration>");
            VsUtility.AddLine(ref idg, "      <AdditionalLibraryDirectories>" + VsUtility.JoinList(libraryDirs, true) + "%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>");
            VsUtility.AddLine(ref idg, "      <AdditionalDependencies>" + VsUtility.JoinList(libraries, true) + "%(AdditionalDependencies)</AdditionalDependencies>");
            VsUtility.AddLine(ref idg, "      <SubSystem>Windows</SubSystem>");
            VsUtility.AddLine(ref idg, "    </Link>");
            VsUtility.AddLine(ref idg, "  </ItemDefinitionGroup>");
        }

        private void GetHeadersAndSources(ref string output, string srcFileName, bool filters, bool addPchForSA) {
            string releaseConfigFullName = GetConfigFullName("GTASA", "Release");
            string debugConfigFullName = GetConfigFullName("GTASA", "Debug");
            VsUtility.AddLine(ref output, "  <ItemGroup>");
            VsUtility.AddLine(ref output, "    <ClCompile Include=\"" + srcFileName + "\" />");
            if (addPchForSA) {
                if (filters)
                    VsUtility.AddLine(ref output, "    <ClCompile Include=\"pch.cpp\" />");
                else {
                    VsUtility.AddLine(ref output, "    <ClCompile Include=\"pch.cpp\">");
                    VsUtility.AddLine(ref output, "      <PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)' == '" + debugConfigFullName + "'\">Create</PrecompiledHeader>");
                    VsUtility.AddLine(ref output, "      <PrecompiledHeaderFile Condition=\"'$(Configuration)|$(Platform)' == '" + debugConfigFullName + "'\">pch.h</PrecompiledHeaderFile>");
                    VsUtility.AddLine(ref output, "      <PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)' == '" + releaseConfigFullName + "'\">Create</PrecompiledHeader>");
                    VsUtility.AddLine(ref output, "      <PrecompiledHeaderFile Condition=\"'$(Configuration)|$(Platform)' == '" + releaseConfigFullName + "'\">pch.h</PrecompiledHeaderFile>");
                    VsUtility.AddLine(ref output, "    </ClCompile>");
                }
            }
            VsUtility.AddLine(ref output, "  </ItemGroup>");
            if (addPchForSA) {
                VsUtility.AddLine(ref output, "  <ItemGroup>");
                VsUtility.AddLine(ref output, "    <ClInclude Include=\"pch.h\" />");
                VsUtility.AddLine(ref output, "  </ItemGroup>");
            }
        }

        public void RunStarted(object automationObject, Dictionary<string, string> replacementsDictionary, WizardRunKind runKind,
            object[] customParams)
        {
            dte = automationObject as DTE;
            destinationDirectory = replacementsDictionary["$destinationdirectory$"];
            solutionName = replacementsDictionary["$specifiedsolutionname$"];
            solutionDirectory = replacementsDictionary["$solutiondirectory$"];

            try {
                window = new WizardMainWindow(VsUtility.CreateIdentifierSafeName(replacementsDictionary["$safeprojectname$"]));

                Nullable<bool> dialogCompleted = window.ShowModal();

                if (dialogCompleted != true || !window.finished)
                    throw new WizardBackoutException();

                string version = dte.Version;
                string toolsetVersion = "v141_xp";
                if (version == "14.0")
                    toolsetVersion = "v140_xp";

                pluginType = window.cmbProjectType.SelectedIndex;

                bOnlySa = window.IsSaSelected() && !window.IsVcOrIIISelected();

                string projectConfigurations = "";
                string propertyGroupConfigurations = "";
                string importGroupPropertySheets = "";
                string propertyGroupDirs = "";
                string itemDefinitionGroups = "";

                if (window.cbSA.IsChecked == true) {
                    SetupConfiguration(ref projectConfigurations, ref propertyGroupConfigurations, ref importGroupPropertySheets,
                        ref propertyGroupDirs, ref itemDefinitionGroups, "Release", false, "GTASA", "SA", toolsetVersion);
                    SetupConfiguration(ref projectConfigurations, ref propertyGroupConfigurations, ref importGroupPropertySheets,
                        ref propertyGroupDirs, ref itemDefinitionGroups, "Debug", true, "GTASA", "SA", toolsetVersion);
                }
                if (window.cbVC.IsChecked == true) {
                    SetupConfiguration(ref projectConfigurations, ref propertyGroupConfigurations, ref importGroupPropertySheets,
                        ref propertyGroupDirs, ref itemDefinitionGroups, "Release", false, "GTAVC", "VC", toolsetVersion);
                    SetupConfiguration(ref projectConfigurations, ref propertyGroupConfigurations, ref importGroupPropertySheets,
                        ref propertyGroupDirs, ref itemDefinitionGroups, "Debug", true, "GTAVC", "VC", toolsetVersion);
                }
                if (window.cbIII.IsChecked == true) {
                    SetupConfiguration(ref projectConfigurations, ref propertyGroupConfigurations, ref importGroupPropertySheets,
                        ref propertyGroupDirs, ref itemDefinitionGroups, "Release", false, "GTA3", "III", toolsetVersion);
                    SetupConfiguration(ref projectConfigurations, ref propertyGroupConfigurations, ref importGroupPropertySheets,
                        ref propertyGroupDirs, ref itemDefinitionGroups, "Debug", true, "GTA3", "III", toolsetVersion);
                }

                replacementsDictionary.Add("$ToolsVersion$", version);
                replacementsDictionary.Add("$PluginSdkGuid$", "{B212DDA4-2A8E-45B2-914D-7BEEB31D06B1}");

                replacementsDictionary.Add("$ProjectConfigurations$", projectConfigurations);
                replacementsDictionary.Add("$PropertyGroupConfigurations$", propertyGroupConfigurations);
                replacementsDictionary.Add("$ImportGroupPropertySheets$", importGroupPropertySheets);
                replacementsDictionary.Add("$PropertyGroupDirs$", propertyGroupDirs);
                replacementsDictionary.Add("$ItemDefinitionGroups$", itemDefinitionGroups);

                string sourceFileName = window.safeProjectName + ".cpp";

                replacementsDictionary.Add("$SourceFileName$", sourceFileName);

                string headersAndSources = "";
                string filtersHeadersAndSources = "";

                GetHeadersAndSources(ref headersAndSources, sourceFileName, false, pluginType == 2);
                GetHeadersAndSources(ref filtersHeadersAndSources, sourceFileName, true, pluginType == 2);

                replacementsDictionary.Add("$HeadersAndSources$", headersAndSources);
                replacementsDictionary.Add("$FiltersHeadersAndSources$", filtersHeadersAndSources);

                string projname = window.safeProjectName;
                string className = VsUtility.FormatName(window.tbBaseClassName.Text, projname, projname, "", "", "", "", "", "");
                className = VsUtility.CreateIdentifierSafeName(className);
                string instanceName = "";
                if (className[0] >= 'A' && className[0] <= 'Z') {
                    string lowerLetter = "";
                    lowerLetter += className[0];
                    lowerLetter = lowerLetter.ToLower();
                    instanceName = lowerLetter + className.Substring(1, className.Length - 1);
                }
                else {
                    if (className[0] == '_')
                        instanceName = className + "_inst";
                    else
                        instanceName = "_" + className;
                }
                replacementsDictionary.Add("$projname$", projname);
                replacementsDictionary.Add("$classname$", className);
                replacementsDictionary.Add("$classinstancename$", instanceName);
            }
            catch (Exception) {
                if (Directory.Exists(destinationDirectory)) {
                    DirectoryInfo parentDir = Directory.GetParent(destinationDirectory);
                    if (parentDir.Exists && parentDir.Name == solutionName)
                        Directory.Delete(parentDir.FullName, true);
                    else
                        Directory.Delete(destinationDirectory, true);
                }
                throw;
            }
        }

        public bool ShouldAddProjectItem(string filePath) {
            if (filePath == "Main.Asi.cpp") {
                if (pluginType == 0 || pluginType == 3)
                    return true;
                if ((pluginType == 1 || pluginType == 2) && !bOnlySa)
                    return true;
                return false;
            }
            else if (filePath == "Main.Cleo.SA.cpp") {
                if (pluginType == 1 && bOnlySa)
                    return true;
                return false;
            }
            else if (filePath == "Main.MoonLoader.SA.cpp" || filePath == "Pch.MoonLoader.h" || filePath == "Pch.MoonLoader.cpp") {
                if (pluginType == 2 && bOnlySa)
                    return true;
                return false;
            }
            return true;
        }
    }
}
