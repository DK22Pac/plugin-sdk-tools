using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using Microsoft.Win32;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;

namespace PluginSdkWizard {
    class VsUtility {
        public static String CreateIdentifierSafeName(String strName) {
            int nLen = strName.Length;
            String strSafeName = "";
            for (int nCntr = 0; nCntr < nLen; nCntr++) {
                char cChar = strName[nCntr];
                if ((cChar >= 'A' && cChar <= 'Z') || (cChar >= 'a' && cChar <= 'z') ||
                    (cChar == '_') || (cChar >= '0' && cChar <= '9') || (cChar > '\x7F' && cChar != '\xA7'))
                {
                    if (cChar < '\uD800' || cChar > '\uDFFF') {
                        if (cChar == '\u3000')
                            cChar = '_';
                        strSafeName += cChar;
                    }
                }
            }
            if (strSafeName == "")
                strSafeName = "My";
            else if (strSafeName[0] >= '0' && strSafeName[0] <= '9')
                strSafeName = "My" + strSafeName;
            return strSafeName;
        }

        public static String BrowseForDirectory(IntPtr owner, string initialDirectory = null, string title = null) {
            IVsUIShell uiShell = (IVsUIShell)Package.GetGlobalService(typeof(SVsUIShell));
            if (null == uiShell) {
                using (var ofd = new FolderBrowserDialog()) {
                    ofd.RootFolder = Environment.SpecialFolder.Desktop;
                    ofd.SelectedPath = initialDirectory;
                    ofd.ShowNewFolderButton = false;
                    DialogResult result;
                    if (owner == IntPtr.Zero)
                        result = ofd.ShowDialog();
                    else
                        result = ofd.ShowDialog(NativeWindow.FromHandle(owner));
                    if (result == DialogResult.OK)
                        return ofd.SelectedPath;
                    else
                        return null;
                }
            }
            if (owner == IntPtr.Zero)
                ErrorHandler.ThrowOnFailure(uiShell.GetDialogOwnerHwnd(out owner));
            VSBROWSEINFOW[] browseInfo = new VSBROWSEINFOW[1];
            browseInfo[0].lStructSize = (uint)Marshal.SizeOf(typeof(VSBROWSEINFOW));
            browseInfo[0].pwzInitialDir = initialDirectory;
            browseInfo[0].pwzDlgTitle = title;
            browseInfo[0].hwndOwner = owner;
            browseInfo[0].nMaxDirName = 260;
            IntPtr pDirName = IntPtr.Zero;
            try {
                browseInfo[0].pwzDirName = pDirName = Marshal.AllocCoTaskMem(520);
                int hr = uiShell.GetDirectoryViaBrowseDlg(browseInfo);
                if (hr == VSConstants.OLE_E_PROMPTSAVECANCELLED)
                    return null;
                ErrorHandler.ThrowOnFailure(hr);
                return Marshal.PtrToStringAuto(browseInfo[0].pwzDirName);
            }
            finally {
                if (pDirName != IntPtr.Zero)
                    Marshal.FreeCoTaskMem(pDirName);
            }
        }

        public static bool IsLibraryExtension(string ext) {
            string extLower = ext.ToLower();
            return extLower == "asi" ||
                   extLower == "cleo" ||
                   extLower == "flt" ||
                   extLower == "dll" ||
                   extLower == "sf";
        }

        public static string GetFileNameExtension(string filename) {
            int lastDotPosition = -1;
            for (int i = 0; i < filename.Length; i++) {
                if (filename[i] == '.')
                    lastDotPosition = i;
            }
            if (lastDotPosition != -1) {
                int extensionLength = filename.Length - lastDotPosition - 1;
                if (extensionLength > 0)
                    return filename.Substring(lastDotPosition, extensionLength);
            }
            return "";
        }

        public static string RemoveTrailingSpaces(string str) {
            if (str.Length > 0) {
                int strEndPos = str.Length;
                for (int i = str.Length - 1; i >= 0; i -= 1) {
                    if (str[i] == ' ' || str[i] == '\t')
                        strEndPos = i;
                    else
                        return str.Substring(0, strEndPos);
                }
            }
            return "";
        }

        public static bool IsQuotesStartOrEnd(string line, int pos) {
            if (line[pos] == '"') {
                if (pos > 0) {
                    int slashesCount = 0;
                    for (int i = pos - 1; i >= 0; i -= 1) {
                        if (line[i] == '\\')
                            slashesCount += 1;
                        else
                            return slashesCount % 2 == 0;
                    }
                    return slashesCount % 2 == 0;
                }
                return true;
            }
            return false;
        }
        public static List<string> SplitListLine(string line) {
            string entry = "";
            List<string> ary = new List<string> { };
            bool insideQuotes = false;
            for (int i = 0; i < line.Length; i += 1) {
                char c = line[i];
                if (!String.IsNullOrEmpty(entry)) {
                    if (insideQuotes) {
                        if (IsQuotesStartOrEnd(line, i)) {
                            insideQuotes = false;
                            entry += '"';
                        }
                        else
                            entry += c;
                    }
                    else {
                        if (c == ';' || c == ',' || c == '\n' || c == '\r') {
                            ary.Add(RemoveTrailingSpaces(entry));
                            entry = "";
                        }
                        else {
                            if (IsQuotesStartOrEnd(line, i)) {
                                insideQuotes = true;
                                entry += '"';
                            }
                            else
                                entry += c;
                        }
                    }
                }
                else {
                    if (c != ';' && c != ',' && c != '\n' && c != '\r' && c != '\t' && c != ' ') {
                        if (IsQuotesStartOrEnd(line, i)) {
                            insideQuotes = true;
                            entry += '"';
                        }
                        else
                            entry += c;
                    }
                }
            }
            if (!String.IsNullOrEmpty(entry))
                ary.Add(RemoveTrailingSpaces(entry));
            return ary;
        }

        public static string JoinList(List<string> list, bool addSepAtTheEnd = false) {
            string result = "";
            if (list.Count > 0) {
                for(int i = 0; i < list.Count; i++) {
                    if (i != 0)
                        result += ';';
                    result += list[i];
                }
                if (addSepAtTheEnd)
                    result += ';';
            }
            return result;
        }
        
        public static string NormalisePath(string path, bool fixProjSolutionDirs = true) {
            string newPath = path;
            StringReplace(ref newPath, "/", "\\");
            if (newPath.Length > 0) {
                if (newPath[newPath.Length - 1] != '\\')
                    newPath += '\\';
            }
            if (fixProjSolutionDirs) {
                StringReplace(ref newPath, "$(SolutionDir)\\", "$(SolutionDir)");
                StringReplace(ref newPath, "$(ProjectDir)\\", "$(ProjectDir)");
            }
            return newPath;
        }

        public static string GetName(string name, string defaultValue) {
            string newLine = name;
            if (String.IsNullOrEmpty(newLine))
                return defaultValue;
            return newLine;
        }

        public static void StringReplace(ref string str, string what, string to) {
            str = str.Replace(what, to);
            str = str.Replace(what.ToUpper(), to);
            str = str.Replace(what.ToLower(), to);
        }

        public static string FormatName(string name, string defaultValue, string projectName, string projectDir, string solutionDir,
            string gameId, string gameDir, string outDirName, string configName)
        {
            string newLine = name;
            if (String.IsNullOrEmpty(newLine))
                return defaultValue;
            StringReplace(ref newLine, "$ProjectName$", projectName);
            StringReplace(ref newLine, "$ProjectDir$", projectDir);
            StringReplace(ref newLine, "$SolutionDir$", solutionDir);
            StringReplace(ref newLine, "$Config$", configName);
            StringReplace(ref newLine, "$Game$", gameId);
            StringReplace(ref newLine, "$GameLow$", gameId.ToLower());
            if (gameId == "SA") {
                StringReplace(ref newLine, "$GameName$", "San Andreas");
                StringReplace(ref newLine, "$GameAlt$", "SA");
                StringReplace(ref newLine, "$GameAltLow$", "sa");
            }
            else if (gameId == "VC") {
                StringReplace(ref newLine, "$GameName$", "Vice City");
                StringReplace(ref newLine, "$GameAlt$", "VC");
                StringReplace(ref newLine, "$GameAltLow$", "vc");
            }
            else if (gameId == "III") {
                StringReplace(ref newLine, "$GameName$", "3");
                StringReplace(ref newLine, "$GameAlt$", "3");
                StringReplace(ref newLine, "$GameAltLow$", "3");
            }
            else {
                StringReplace(ref newLine, "$GameName$", "");
                StringReplace(ref newLine, "$GameAlt$", "");
                StringReplace(ref newLine, "$GameAltLow$", "");
            }
            StringReplace(ref newLine, "$GameDir$", gameDir);
            StringReplace(ref newLine, "$OutDirName$", outDirName);
            return newLine;
        }

        public static void AddNewLineIfNeeded(ref string buf) {
            if (!String.IsNullOrEmpty(buf) && buf[buf.Length - 1] != '\n')
                buf += "\n";
        }

        public static void AddLine(ref string buf, string line) {
            AddNewLineIfNeeded(ref buf);
            buf += line;
        }

        public static bool IsVsVariable(string varName) {
            return varName == "RemoteMachine"
            || varName == "Configuration"
            || varName == "Platform"
            || varName == "ParentName"
            || varName == "RootNameSpace"
            || varName == "IntDir"
            || varName == "OutDir"
            || varName == "DevEnvDir"
            || varName == "InputDir"
            || varName == "InputPath"
            || varName == "InputName"
            || varName == "InputFileName"
            || varName == "InputExt"
            || varName == "ProjectDir"
            || varName == "ProjectPath"
            || varName == "ProjectName"
            || varName == "ProjectFileName"
            || varName == "ProjectExt"
            || varName == "SolutionDir"
            || varName == "SolutionPath"
            || varName == "SolutionName"
            || varName == "SolutionFileName"
            || varName == "SolutionExt"
            || varName == "TargetDir"
            || varName == "TargetPath"
            || varName == "TargetName"
            || varName == "TargetFileName"
            || varName == "TargetExt"
            || varName == "VSInstallDir"
            || varName == "VCInstallDir"
            || varName == "FrameworkDir"
            || varName == "FrameworkVersion"
            || varName == "FrameworkSDKDir"
            || varName == "WebDeployPath"
            || varName == "WebDeployRoot"
            || varName == "SafeParentName"
            || varName == "SafeInputName"
            || varName == "SafeRootNamespace"
            || varName == "FxCopDir";
        }

        public static List<string> ResolveEnvironmentVariables(string input, ref string output) {
            bool readingEnvVar = false;
            List<string> errList = new List<string>();
            string currEnvVar = "";
            output = "";
            for (int i = 0; i < input.Length; i++) {
                if (readingEnvVar) {
                    if (input[i] == ')') {
                        readingEnvVar = false;
                        string envVar = Environment.GetEnvironmentVariable(currEnvVar);
                        if (envVar != null)
                            output += envVar;
                        else {
                            output += "$(" + currEnvVar + ")";
                            if (!IsVsVariable(currEnvVar))
                                errList.Add(String.Format(Properties.Resources.strEnvVarNotFound, currEnvVar));
                        }
                        continue;
                    }
                    currEnvVar += input[i];
                }
                else {
                    if (i + 1 < input.Length) {
                        if (input[i] == '$' && input[i + 1] == '(') {
                            readingEnvVar = true;
                            i += 1;
                            continue;
                        }
                    }
                    output += input[i];
                }
            }
            if (readingEnvVar) {
                output += currEnvVar;
                errList.Add(String.Format(Properties.Resources.strWarnUnfinishedVar, currEnvVar));
            }
            return errList;
        }
    }
}
