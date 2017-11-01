using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PluginSdkWizardInstaller
{
    class SDKFolders
    {
        private class FoundFolderException : Exception
        {
            public FoundFolderException(string theFolder)
            {
                this.theFolder = theFolder;
            }

            public string GetFolder()
            {
                return theFolder;
            }

            private string theFolder;
        };

        static public string FindPluginSdkDir()
        {
            try
            {
                string desktopFolder = Environment.GetFolderPath( Environment.SpecialFolder.Desktop );

                // Scan common plugin-sdk locations.
                {
                    string[] likelyFolders =
                    {
                        desktopFolder + "\\plugin-sdk",
                        "D:\\plugin-sdk",
                        "D:\\Projects\\plugin-sdk",
                        Environment.GetFolderPath( Environment.SpecialFolder.ProgramFiles ) + "\\plugin-sdk"
                    };

                    foreach ( string tryLoc in likelyFolders )
                    {
                        if ( PathLogic.IsPluginSDKDirectory( tryLoc ) )
                        {
                            throw new FoundFolderException( tryLoc );
                        }
                    }
                }

                // If not found in likely folders, we scan the desktop ones next.
                PathLogic.ForAllFolders(desktopFolder, true,
                    (tryLoc) => {
                        if ( PathLogic.IsPluginSDKDirectory( tryLoc ) )
                        {
                            throw new FoundFolderException( tryLoc );
                        }
                    }
                );
            }
            catch( FoundFolderException except )
            {
                return except.GetFolder();
            }
            
            // Could not find anything.
            return null;
        }

        static public string FindDirectX9SdkDir()
        {
            try
            {
                string progFilesPath = Environment.GetFolderPath( Environment.SpecialFolder.ProgramFilesX86 );

                // Check common folders first.
                {
                    string[] likelyFolders =
                    {
                        "D:\\Projects\\DXSDK\\9.0",
                        progFilesPath + "\\Microsoft DirectX SDK (June 2010)"
                    };

                    foreach ( string tryLoc in likelyFolders )
                    {
                        if ( PathLogic.IsDirectX9Directory( tryLoc ) )
                        {
                            throw new FoundFolderException( tryLoc );
                        }
                    }
                }

                // Try scanning the entire program files folder (x86) for the DX9 SDK.
                PathLogic.ForAllFolders(progFilesPath, false,
                    (tryLoc) =>
                    {
                        if ( PathLogic.IsDirectX9Directory(tryLoc) )
                        {
                            throw new FoundFolderException( tryLoc );
                        }
                    }
                );
            }
            catch( FoundFolderException except )
            {
                return except.GetFolder();
            }

            return null;
        }

        static public string FindRWD3D9SdkDir()
        {
            try
            {
                //TODO: we have no idea how the RWD3D9 sdk looks like, yet. Please adjust.

                string desktopFolder = Environment.GetFolderPath( Environment.SpecialFolder.Desktop );

                // Try common locations of RWD3D9 first.
                {
                    string[] likelyFolders =
                    {
                        desktopFolder + "\\rwd3d9",
                        "D:\\Projects\\rwd3d9"
                    };

                    foreach ( string tryLoc in likelyFolders )
                    {
                        if ( PathLogic.IsRWD3D9Directory( tryLoc ) )
                        {
                            throw new FoundFolderException( tryLoc );
                        }
                    }
                }
            }
            catch( FoundFolderException except )
            {
                return except.GetFolder();
            }

            return null;
        }
    }
}
