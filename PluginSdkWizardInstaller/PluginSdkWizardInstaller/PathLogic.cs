using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace PluginSdkWizardInstaller
{
    class PathLogic
    {
        public class FoundFolderException : Exception {
            public FoundFolderException(string theFolder) {
                this.theFolder = theFolder;
            }

            public string GetFolder() {
                return theFolder;
            }

            private string theFolder;
        };

        // Translated code from C++ config tool.
        static private bool checkFolderConfiguration(
            string dirLoc,
            string[] requiredFiles, string[] requiredFolders
        )
        {
            if ( Directory.Exists( dirLoc ) == false )
                return false;

            string baseDir = dirLoc + "\\";

            if ( requiredFiles != null )
            {
                foreach ( string checkFile in requiredFiles )
                {
                    if ( File.Exists( baseDir + checkFile ) == false )
                    {
                        return false;
                    }
                }
            }

            if ( requiredFolders != null )
            {
                foreach ( string checkDir in requiredFolders )
                {
                    if ( Directory.Exists( baseDir + checkDir ) == false )
                    {
                        return false;
                    }
                }
            }

            // Alright
            return true;
        }

        static public bool IsPluginSDKDirectory(string dirLoc)
        {
            string[] requiredFiles =
            {
                "installer-launcher.exe", "LICENSE", "README.md"
            };

            string[] requiredFolders =
            {
                "examples", "injector", "plugin_iii", "plugin_vc", "plugin_sa", "shared", "tools"
            };

            return checkFolderConfiguration(dirLoc, requiredFiles, requiredFolders);
        }

        static public bool IsDirectX9Directory(string dirLoc)
        {
            string[] requiredFiles =
            {
                "Include\\d3d9.h",
                "Include\\d3dx9.h",
                "Lib\\x86\\d3d9.lib",
                "Lib\\x86\\d3dx9.lib"
            };

            string[] requiredFolders =
            {
                "Include",
                "Lib",
                "Lib\\x86"
            };

            return checkFolderConfiguration(dirLoc, requiredFiles, requiredFolders);
        }

        static public bool IsRWD3D9Directory(string dirLoc)
        {
            string[] requiredFiles =
            {
                "libs\\rwd3d9.lib",
                "source\\rwd3d9.h"
            };

            string[] requiredFolders =
            {
                "libs",
                "source"
            };

            return checkFolderConfiguration(dirLoc, requiredFiles, requiredFolders);
        }

        static public bool IsCLEO_SDK_Directory(string dirLoc)
        {
            string[] requiredFiles =
            {
                "cleo.h",
                "cleo.lib"
            };

            return checkFolderConfiguration(dirLoc, requiredFiles, null);
        }

        static public bool IsMoonLoaderSdkDirectory(string dirLoc)
        {
            string[] requiredFiles =
            {
                "src\\lua_module.h",
                "src\\pch.h",
                "src\\pch.cpp"
            };

            string[] requiredFolders =
            {
                "src",
                "src\\libs",
                "src\\libs\\lua",
                "src\\libs\\sol2"
            };

            return checkFolderConfiguration(dirLoc, requiredFiles, requiredFolders);
        }

        static public bool IsGTA3Directory(string dirLoc)
        {
            string[] requiredFiles =
            {
                "gta3.exe",
                "anim\\cuts.dir",
                "anim\\cuts.img",
                "anim\\ped.ifp",
                "data\\object.dat",
                "data\\main.scm",
                "models\\gta3.dir",
                "models\\gta3.img",
                "models\\generic.txd",
                "models\\Generic\\peds.dff",
                "models\\Coll\\peds.col",
                "txd\\mainsc1.txd",
                "txd\\mainsc2.txd"
            };

            string[] requiredFolders =
            {
                "anim",
                "data",
                "models",
                "mss",
                "txd"
            };

            return checkFolderConfiguration(dirLoc, requiredFiles, requiredFolders);
        }

        static public bool IsGTAVCDirectory(string dirLoc)
        {
            string[] requiredFiles =
            {
                "gta-vc.exe",
                "anim\\cuts.dir",
                "anim\\cuts.img",
                "anim\\ped.ifp",
                "data\\gta_vc.dat",
                "data\\main.scm",
                "models\\gta3.img",
                "models\\gta3.dir",
                "models\\generic.txd",
                "models\\coll\\generic.col",
                "txd\\outro.txd"
            };

            string[] requiredFolders =
            {
                "anim",
                "data",
                "models",
                "mss",
                "TEXT",
                "txd"
            };

            return checkFolderConfiguration(dirLoc, requiredFiles, requiredFolders);
        }

        static public bool IsGTASADirectory(string dirLoc)
        {
            string[] requiredFiles =
            {
                "stream.ini",
                "anim\\anim.img",
                "anim\\cuts.img",
                "anim\\ped.ifp",
                "data\\object.dat",
                "data\\surface.dat",
                "models\\cutscene.img",
                "models\\gta3.img",
                "models\\gta_int.img",
                "models\\player.img",
                "models\\particle.txd",
                "models\\generic\\vehicle.txd",
                "models\\grass\\plant1.txd",
                "models\\txd\\LOADSCS.txd"
            };

            string[] requiredFolders =
            {
                "anim",
                "data",
                "models",
                "models\\coll",
                "models\\generic",
                "models\\grass",
                "models\\txd",
                "text"
            };

            if ( !checkFolderConfiguration(dirLoc, requiredFiles, requiredFolders) )
                return false;

            // Check for existance of the GTA:SA executable.
            string baseDir = dirLoc + "\\";

            if ( !File.Exists( baseDir + "GTA_SA.EXE" ) && !File.Exists( baseDir + "gta-sa.exe" ) )
                return false;

            // Looking like a real GTA SA folder.
            return true;
        }

        public delegate void FolderCallback( string fullFileName );

        static public void ForAllFolders( string dirLoc, bool recursive, FolderCallback cb )
        {
            try
            {
                foreach ( string dirName in Directory.EnumerateDirectories(dirLoc) )
                {
                    // Notify.
                    cb( dirName );

                    if ( recursive )
                    {
                        // Go into that directory.
                        DirectoryInfo dir = new DirectoryInfo(dirName);
                        if ((dir.Attributes & FileAttributes.Hidden) == 0)
                        {
                            ForAllFolders(dirName, recursive, cb);
                        }
                    }
                }
            }
            catch( Exception )
            {
                return;
            }
        }

        public delegate bool IsGameFolderCallback( string fullPath );

        static public string ScanForGTAGameFolder( IsGameFolderCallback cb )
        {
            string theFolder = null;

            FolderCallback findIterCB = (findLoc) =>
            {
                if ( cb( findLoc ) )
                {
                    theFolder = findLoc;
                    throw new FoundFolderException(findLoc);
                }
            };

            string programFilesFolder = Environment.GetFolderPath( Environment.SpecialFolder.ProgramFilesX86 );

            ForAllFolders( programFilesFolder, false, findIterCB );

            if ( theFolder == null )
            {
                // Check the Rockstar Games folder.
                string rockstarGamesFolder = programFilesFolder + "\\Rockstar Games";

                ForAllFolders( rockstarGamesFolder, false, findIterCB );
            }

            if ( theFolder == null )
            {
                // At last, try the default steam games directory.
                string steamGamesFolder = programFilesFolder + "\\Steam\\steamapps\\common";

                ForAllFolders( steamGamesFolder, false, findIterCB );
            }

            if ( theFolder == null )
            {
                // Should also scan inside the D: drive.
                ForAllFolders( "D:\\", true, findIterCB );
            }

            // Return anything we found.
            return theFolder;
        }

        public static string ScanGTASAGameDirectory()
        {
            return ScanForGTAGameFolder( IsGTASADirectory );
        }

        public static string ScanGTAVCGameDirectory()
        {
            return ScanForGTAGameFolder( IsGTAVCDirectory );
        }

        public static string ScanGTA3GameDirectory()
        {
            return ScanForGTAGameFolder( IsGTA3Directory );
        }

        public static bool EnsureFolder(string folderPath)
        {
            try
            {
                return ( Directory.CreateDirectory( folderPath ) != null );
            }
            catch( Exception )
            {
                return false;
            }
        }

        static public string GetOsVariable(string varName) {
            // Check user environment variables.
            {
                string userVar = Environment.GetEnvironmentVariable(varName, EnvironmentVariableTarget.User);

                if (userVar != null)
                    return userVar;
            }

            // Check system environment variables.
            {
                string globVar = Environment.GetEnvironmentVariable(varName, EnvironmentVariableTarget.Machine);

                if (globVar != null)
                    return globVar;
            }

            // Not found.
            return "";
        }
    }
}
