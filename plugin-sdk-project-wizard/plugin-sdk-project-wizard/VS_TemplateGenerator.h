#pragma once
#define QUAZIP_STATIC
#include "VsProjectItemFile.h"
#include "ErrorMessage.h"
#include "JlCompress.h"

struct VS_ProjectGenerationSettings {
    QString vsDocumentsPath;
    QString pluginSdkPath;
    QString d3d8SdkPath;
    QString d3d9SdkPath;
    QString vcCleoSdkPath;
    QString saCleoSdkPath;
    QString iiiCleoSdkPath;
    QString vcAsiOutputPath;
    QString saAsiOutputPath;
    QString iiiAsiOutputPath;
    QString vcCleoOutputPath;
    QString saCleoOutputPath;
    QString iiiCleoOutputPath;
};

enum eGtaGame {
    GTA_GAME_SA,
    GTA_GAME_VC,
    GTA_GAME_III
};

enum eGenerationFlags {
    VSGEN_D3D9 = 1,
    VSGEN_D3D8 = 2,
    VSGEN_CLEO = 4,
    VSGEN_LA   = 8
};

class VS_ProjectTemplateGenerator {
public:
    static bool GenerateTemplate(QString fileName, QString vsVersion, eGtaGame game, unsigned int flags, VS_ProjectGenerationSettings const &settings) {
        if (ProcessMainCppFile(game) && ProcessVSTemplateFile(game, vsVersion, flags) && ProcessVSProjectFile(game, vsVersion, flags, settings)) {
            QStringList fileNames;
            fileNames.append("templates\\temp\\Main.cpp");
            fileNames.append("templates\\temp\\Project.vcxproj");
            fileNames.append("templates\\temp\\MyTemplate.vstemplate");
            fileNames.append("templates\\" + vsVersion + "\\Project.vcxproj.filters");
            if(game == GTA_GAME_SA)
                fileNames.append("templates\\SA.ico");
            else if(game == GTA_GAME_VC)
                fileNames.append("templates\\VC.ico");
            else
                fileNames.append("templates\\III.ico");
            QString path = settings.vsDocumentsPath + "Templates/ProjectTemplates/Plugin-SDK/";
                
            QString subFolder = (flags & VSGEN_CLEO) ? "CLEO/" : "ASI/";
            if (JlCompress::compressFiles(path + subFolder + fileName + ".zip", fileNames))
                return true;
            else
                return MESSAGE_ERROR("VS_ProjectTemplateGenerator::GenerateTemplate(): can't create .zip archive (" + fileName + ".zip)");
        }
        return false;
    }

    static bool ProcessMainCppFile(eGtaGame game) {
        VsProjectItemFile mainCppFile;
        if (!mainCppFile.Read("templates\\Main.cpp"))
            return false;
        if (game == GTA_GAME_SA)
            mainCppFile.SetLinesValue("#include \"plugin.h\"", "#include \"plugin.h\"");
        else if (game == GTA_GAME_VC)
            mainCppFile.SetLinesValue("#include \"plugin.h\"", "#include \"plugin_vc.h\"");
        else
            mainCppFile.SetLinesValue("#include \"plugin.h\"", "#include \"plugin_III.h\"");
        return mainCppFile.Write("templates\\temp\\Main.cpp");
    }

    static bool ProcessVSTemplateFile(eGtaGame game, QString vsVersion, unsigned int genFlags) {
        VsProjectItemFile vsTemplateFile;
        if (!vsTemplateFile.Read("templates\\" + vsVersion + "\\MyTemplate.vstemplate"))
            return false;
        QString name, description, icon;
        if (game == GTA_GAME_SA) {
            name = "GTA SA ";
            description = "A project for GTA San Andreas ";
            icon = "SA.ico";
        }
        else if (game == GTA_GAME_VC) {
            name = "GTA VC ";
            description = "A project for GTA Vice City ";
            icon = "VC.ico";
        }
        else {
            name = "GTA III ";
            description = "A project for GTA III ";
            icon = "III.ico";
        }
        if (genFlags & VSGEN_CLEO) {
            name += "CLEO plugin";
            description += "CLEO plugin";
        }
        else {
            name += ".ASI plugin";
            description += ".ASI plugin";
        }
        if (game == GTA_GAME_SA) {
            if (genFlags & VSGEN_D3D9) {
                name += " (D3D9";
                description += " with D3D9";
                if (genFlags & VSGEN_LA) {
                    name += "+LA comp.";
                    description += " and Limit Adjuster compatibility";
                }
                name += ')';
                description += ')';
            }
            else if (genFlags & VSGEN_LA) {
                name += " (+LA comp.)";
                description += " with Limit Adjuster compatibility";
            }
        }
        else {
            if (genFlags & VSGEN_D3D8 || genFlags & VSGEN_D3D9) {
                if (genFlags & VSGEN_D3D8) {
                    name += " (D3D8";
                    description += " with D3D8";
                }
                else {
                    name += " (D3D8toD3D9";
                    description += " with D3D8toD3D9";
                }
                if (genFlags & VSGEN_LA) {
                    name += "+LA comp.";
                    description += " and Limit Adjuster compatibility";
                }
                name += ')';
                description += ')';
            }
            else if (genFlags & VSGEN_LA) {
                name += " (+LA comp.)";
                description += " with Limit Adjuster compatibility";
            }
        }
        vsTemplateFile.SetNodesValue("Name", name);
        vsTemplateFile.SetNodesValue("Description", description);
        vsTemplateFile.SetNodesValue("Icon", icon);
        return vsTemplateFile.Write("templates\\temp\\MyTemplate.vstemplate");
    }

    static bool ProcessVSProjectFile(eGtaGame game, QString vsVersion, unsigned int genFlags, VS_ProjectGenerationSettings const &settings) {
        VsProjectItemFile vsProjectFile;
        if (!vsProjectFile.Read("templates\\" + vsVersion + "\\Project.vcxproj"))
            return false;
        QString extension, outDir, definitions, includeFolders, pluginSdkLibFolder, libFolders, dependencies, vcIncludes, vcLibraries;
        //vcIncludes = "$(IncludePath)";
        includeFolders = settings.pluginSdkPath;
        pluginSdkLibFolder = settings.pluginSdkPath + "output\\lib\\";
        if (game == GTA_GAME_SA) {
            includeFolders += "plugin_sa\\;";
            if (genFlags & VSGEN_CLEO) {
                outDir = settings.saCleoOutputPath;
            }
            else {
                outDir = settings.saAsiOutputPath;
            }
            if (genFlags & VSGEN_D3D9) {
                definitions += "_PLUGIN_D3D_INCLUDE";
                vcIncludes += ";" + settings.d3d9SdkPath + "Include\\";
                vcLibraries += ";" + settings.d3d9SdkPath + "Lib\\x86\\";
                dependencies += "d3d9.lib;d3dx9.lib;";
            }
        }
        else if (game == GTA_GAME_VC) {
            includeFolders += "plugin_vc\\;";
            if (genFlags & VSGEN_CLEO) {
                outDir = settings.vcCleoOutputPath;
            }
            else {
                outDir = settings.vcAsiOutputPath;
            }
            if (genFlags & VSGEN_D3D8) {
                definitions += "_PLUGIN_D3D_INCLUDE;";
                vcIncludes += ";" + settings.d3d8SdkPath + "include\\";
                vcLibraries += ";" + settings.d3d8SdkPath + "lib\\";
                dependencies += "d3d8.lib;d3dx8.lib;";
            }
            else if (genFlags & VSGEN_D3D9) {
                definitions += "_PLUGIN_D3D_INCLUDE;_PLUGIN_D3D8TO9;";
                vcIncludes += ";" + settings.d3d9SdkPath + "Include\\";
                vcLibraries += ";" + settings.d3d9SdkPath + "Lib\\x86\\";
                dependencies += "d3d9.lib;d3dx9.lib;";
            }
        }
        else {
            includeFolders += "plugin_III\\;";
            if (genFlags & VSGEN_CLEO) {
                outDir = settings.iiiCleoOutputPath;
            }
            else {
                outDir = settings.iiiAsiOutputPath;
            }
            if (genFlags & VSGEN_D3D8) {
                definitions += "_PLUGIN_D3D_INCLUDE;";
                vcIncludes += ";" + settings.d3d8SdkPath + "include\\";
                vcLibraries += ";" + settings.d3d8SdkPath + "lib\\";
                dependencies += "d3d8.lib;d3dx8.lib;";
            }
            else if (genFlags & VSGEN_D3D9) {
                definitions += "_PLUGIN_D3D_INCLUDE;_PLUGIN_D3D8TO9;";
                vcIncludes += ";" + settings.d3d9SdkPath + "Include\\";
                vcLibraries += ";" + settings.d3d9SdkPath + "Lib\\x86\\";
                dependencies += "d3d9.lib;d3dx9.lib;";
            }
        }
        if (genFlags & VSGEN_CLEO) {
            extension = ".cleo";
            if (game == GTA_GAME_SA) {
                includeFolders += settings.saCleoSdkPath + ';';
                libFolders += settings.saCleoSdkPath + ';';
            }
            else if (game == GTA_GAME_VC) {
                includeFolders += settings.vcCleoSdkPath + ';';
                libFolders += settings.vcCleoSdkPath + ';';
            }
            else {
                includeFolders += settings.iiiCleoSdkPath + ';';
                libFolders += settings.iiiCleoSdkPath + ';';
            }
            dependencies += "cleo.lib;";
        }
        else {
            extension = ".asi";
        }
        if (genFlags & VSGEN_LA) {
            definitions += "_PLUGIN_LA_COMP;";
        }

        vsProjectFile.SetNodesValue("TargetExt", extension);
        vsProjectFile.SetNodesValue("OutDir", outDir);
        vsProjectFile.SetNodesValue("AdditionalIncludeDirectories", includeFolders + "%(AdditionalIncludeDirectories)");
		vsProjectFile.SetNodesValue("PreprocessorDefinitions", "_USING_V110_SDK71_", true,
			"_USING_V110_SDK71_;_CRT_SECURE_NO_WARNINGS;_CRT_NON_CONFORMING_SWPRINTFS;" + definitions + "%(PreprocessorDefinitions)");
		vsProjectFile.SetNodesValue("PreprocessorDefinitions", "_USING_V110_SDK71_", false,
			"_CRT_SECURE_NO_WARNINGS;_CRT_NON_CONFORMING_SWPRINTFS;" + definitions + "%(PreprocessorDefinitions)");
        vsProjectFile.SetNodesValue("AdditionalLibraryDirectories", pluginSdkLibFolder + ";" + libFolders + " % (AdditionalLibraryDirectories)");
        vsProjectFile.SetNodesValue("IncludePath", "$(IncludePath)" + vcIncludes);
        vsProjectFile.SetNodesValue("LibraryPath", "$(LibraryPath)" + vcLibraries);

		QString pluginLibName = "plugin";
		if (game == GTA_GAME_VC)
			pluginLibName.append("_vc");
		else if (game == GTA_GAME_III)
			pluginLibName.append("_III");

        vsProjectFile.SetNodesValue("AdditionalDependencies", "plugin_d.lib", true,
            pluginLibName + "_d.lib;paths_d.lib;" + dependencies + "%(AdditionalDependencies)");
        vsProjectFile.SetNodesValue("AdditionalDependencies", "plugin.lib", true,
            pluginLibName + ".lib;paths.lib;" + dependencies + "%(AdditionalDependencies)");

        return vsProjectFile.Write("templates\\temp\\Project.vcxproj");
    }
};