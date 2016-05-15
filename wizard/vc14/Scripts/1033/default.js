/* Plugin SDK - for licensing and copyright see license.txt */

function OnFinish(selProj, selObj)
{
    try {

        // Create Project Name
        var strProjectName = wizard.FindSymbol('PROJECT_NAME');
        strProjectName = strProjectName.replace("Plugin_", ""); // done this way so directory existence checks still works
        strProjectName = strProjectName.replace("Plugin", ""); // further possibilities
        strProjectName = strProjectName.replace("plugin", ""); // further possibilities
        strProjectName = strProjectName.replace(" ", ""); // remove spaces

        // Safe Project Name
        strProjectName = CreateCPPName(CreateSafeName(strProjectName)).replace(/[^\w\s]/gi, '');
        wizard.RemoveSymbol('PROJECT_NAME');
        wizard.AddSymbol('PROJECT_NAME', strProjectName);
        wizard.AddSymbol('PROJECT_NAME_SAFE', strProjectName);
        wizard.RemoveSymbol('VS_SOLUTION_NAME');
        wizard.AddSymbol('VS_SOLUTION_NAME', "Plugin_" + strProjectName);

        var strProjectPath = wizard.FindSymbol('PROJECT_PATH');
        strProjectPath += "\\..\\Plugin_" + strProjectName + "\\project";

        if (!CanUseDrive(strProjectPath))
            return VS_E_WIZARDBACKBUTTONPRESS;

        // Safe Project Name in Uppercase
        wizard.AddSymbol('PROJECT_NAME_SAFE_UPPERCASE', strProjectName.toUpperCase());

        // Current Year for Copyright
        var currentTime = new Date();
        wizard.AddSymbol('CURRENT_YEAR', currentTime.getFullYear());

        // Render Files that require it
        var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');

        // Plugin properties directly used without modification from Plugin SDK (so this isn't required anymore)
        //wizard.RenderTemplate(strTemplatePath + "\\Plugin_Settings.props", "Plugin_Settings.props", true); 
        
        // The project file needs to be rendered
        wizard.RenderTemplate(strTemplatePath + "\\default.vcxproj", strProjectPath + "\\default.vcxproj", false, true);

        selProj = CreateCustomProject(strProjectName, strProjectPath);

        // Delete temp rendered project
        var fso = new ActiveXObject('Scripting.FileSystemObject');
        DelFile(fso, strProjectPath + "\\default.vcxproj"); 

        AddConfig(selProj, strProjectName);
        AddFilters(selProj); //SetupFilters(selProj);

        var InfFile = CreateCustomInfFile();

        AddFilesToCustomProj(selProj, strProjectName, strProjectPath, InfFile);

        PchSettings(selProj);

        InfFile.Delete();

        selProj.Object.Save();

        // Open files
        var rgoFiles = [
            selProj.Object.Files("readme.md").Object,
            selProj.Object.Files("CPlugin" + strProjectName + ".h").Object,
            selProj.Object.Files("CPlugin" + strProjectName + ".cpp").Object,
            selProj.Object.Files("IPlugin" + strProjectName + ".h").Object
         ];

        for (var iFileToOpen = 0; iFileToOpen < rgoFiles.length; iFileToOpen++) {
            var oFileToOpen = rgoFiles[iFileToOpen];

            var vsViewKind = vsViewKindPrimary;

            switch (oFileToOpen.Object.FileType) {
                case eFileTypeCppForm:
                case eFileTypeCppClass:
                case eFileTypeCppControl:
                case eFileTypeCppWebService:
                    vsViewKind = vsViewKindDesigner;
            }

            var window = oFileToOpen.Open(vsViewKind);

            if (window) {
                window.visible = true;
            }
        }
    }
    catch(e)
    {
        if (e.description.length != 0)
            SetErrorInfo(e);

        return e.number
    }
}

function CreateCustomProject(strProjectName, strProjectPath)
{
    try
    {
        //var strProjTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
        var strProjTemplate = '';

        //strProjTemplate = strProjTemplatePath + '\\default.vcxproj';
        strProjTemplate = strProjectPath + '\\default.vcxproj'; 

        var Solution = dte.Solution;
        var strSolutionName = "";
        if (wizard.FindSymbol("CLOSE_SOLUTION"))
        {
            Solution.Close();
            strSolutionName = wizard.FindSymbol("VS_SOLUTION_NAME");
            if (strSolutionName.length)
            {
                //var strSolutionPath = strProjectPath.substr(0, strProjectPath.length - strProjectName.length);
                var strSolutionPath = strProjectPath;
                Solution.Create(strSolutionPath, strSolutionName);
            }
        }

        var strProjectNameWithExt = '';
        strProjectNameWithExt = strProjectName + '.vcxproj';

        var oTarget = wizard.FindSymbol("TARGET");
        var prj;
        if (wizard.FindSymbol("WIZARD_TYPE") == vsWizardAddSubProject)
        {
            var prjItem = oTarget.AddFromTemplate(strProjTemplate, strProjectNameWithExt);
            prj = prjItem.SubProject;
        }
        else
        {
            prj = oTarget.AddFromTemplate(strProjTemplate, strProjectPath, strProjectNameWithExt);
        }

        var fxtarget = wizard.FindSymbol("TARGET_FRAMEWORK_VERSION");
        if (fxtarget != null && fxtarget != "")
        {
            fxtarget = fxtarget.split('.', 2);
            if (fxtarget.length == 2)
            prj.Object.TargetFrameworkVersion = parseInt(fxtarget[0]) * 0x10000 + parseInt(fxtarget[1])
        }

        return prj;
    }
    catch(e)
    {
        throw e;
    }
}

function AddFilters(proj)
{
    try
    {
        // Add the folders to your project
        var L_strSource_Text = "Source Files";
        var group = proj.Object.AddFilter(L_strSource_Text);
        group.Filter = "cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx";
        group.UniqueIdentifier = "{4FC737F1-C7A5-4376-A066-2A32D752A2FF}";

        var L_strHeader_Text = "Header Files";
        group = proj.Object.AddFilter(L_strHeader_Text);
        group.Filter = "h;hpp;hxx;hm;inl;inc;xsd";
        group.UniqueIdentifier = "{93995380-89BD-4b04-88EB-625FBE52EBFB}";

        var L_strResources_Text = "Resource Files";
        group = proj.Object.AddFilter(L_strResources_Text);
        group.Filter = "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav;mfcribbon-ms";
        group.UniqueIdentifier = "{67DA6AB6-F800-4c08-8B7A-83BB121AAD01}";

        group = proj.Object.AddFilter("Flownodes");

        group = proj.Object.AddFilter("Tools");
        group.Filter = "bat;astylerc";

        group = proj.Object.AddFilter("Installer");
        group.Filter = "nsi;nsh";
    }
    catch(e)
    {
        throw e;
    }
}

function AddConfig(proj, strProjectName)
{
    try {

        // Not required everything contained in vcxproj

        /*
        var config = proj.Object.Configurations('Debug');
        config.ConfigurationType = typeDynamicLibrary;

        var CLTool = config.Tools('VCCLCompilerTool');
        // TODO: Add compiler settings

        var LinkTool = config.Tools('VCLinkerTool');
        // TODO: Add linker settings

        config = proj.Object.Configurations('Release');
        config.ConfigurationType = typeDynamicLibrary;

        var CLTool = config.Tools('VCCLCompilerTool');
        // TODO: Add compiler settings

        var LinkTool = config.Tools('VCLinkerTool');
        // TODO: Add linker settings
        */
    }
    catch(e)
    {
        throw e;
    }
}

function PchSettings(proj)
{
    var precompiledheader = proj.Object.Files("StdAfx.cpp");

    for (var iConfig = 1; iConfig <= precompiledheader.FileConfigurations.Count; iConfig++)
    {
        precompiledheader.FileConfigurations(iConfig).Tool.UsePrecompiledHeader = pchCreateUsingSpecific;
    }
}

function DelFile(fso, strWizTempFile)
{
    try
    {
        if (fso.FileExists(strWizTempFile))
        {
            var tmpFile = fso.GetFile(strWizTempFile);
            tmpFile.Delete();
        }
    }
    catch(e)
    {
        throw e;
    }
}

function CreateCustomInfFile()
{
    try
    {
        var fso, TemplatesFolder, TemplateFiles, strTemplate;
        fso = new ActiveXObject('Scripting.FileSystemObject');

        var TemporaryFolder = 2;
        var tfolder = fso.GetSpecialFolder(TemporaryFolder);
        var strTempFolder = tfolder.Drive + '\\' + tfolder.Name;
        
        var strWizTempFile = strTempFolder + "\\" + fso.GetTempName();

        var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
        var strInfFile = strTemplatePath + '\\Templates.inf';
        wizard.RenderTemplate(strInfFile, strWizTempFile);

        var WizTempFile = fso.GetFile(strWizTempFile);
        return WizTempFile;
    }
    catch(e)
    {
        throw e;
    }
}

function GetTargetName(strName, strProjectName)
{
    try
    {
        var strTarget = strName;

        var strProjectNameSafe = wizard.FindSymbol('PROJECT_NAME_SAFE');

        if (strName == "readme.md")
            strTarget = "..\\" + strName;

        if (strName == "changelog.md")
            strTarget = "..\\" + strName;

        if (strName == "license.txt")
            strTarget = "..\\" + strName;

        if (strName == "authors.txt")
            strTarget = "..\\" + strName;

        if (strName == "CPluginSample.h")
            strTarget = "..\\src\\CPlugin" + strProjectNameSafe + ".h";

        if (strName == "CPluginSample.cpp")
            strTarget = "..\\src\\CPlugin" + strProjectNameSafe + ".cpp";

        if (strName == "CPluginSampleModule.cpp")
            strTarget = "..\\src\\CPlugin" + strProjectNameSafe + "Module.cpp";

        if (strName == "IPluginSample.h")
            strTarget = "..\\inc\\IPlugin" + strProjectNameSafe + ".h";

        if (strName == "StdAfx.cpp")
            strTarget = "..\\src\\" + strName;

        if (strName == "StdAfx.h")
            strTarget = "..\\src\\" + strName;

        if (strName == "tools\\build.bat")
            strTarget = "..\\" + strName;
	    
        if (strName == "tools\\AStyle\\AStyle.exe")
            strTarget = "..\\" + strName;
	    
        if (strName == "tools\\AStyle\\license.html")
            strTarget = "..\\" + strName;
	    
        if (strName == "tools\\AStyle\\styles.css")
            strTarget = "..\\" + strName;
	    
        if (strName == "tools\\codestyle.astylerc")
            strTarget = "..\\" + strName;

        if (strName == "tools\\stylecode.bat")
            strTarget = "..\\" + strName;

        if (strName == "tools\\build_installer.bat")
            strTarget = "..\\" + strName;

        if (strName == "tools\\Sample_Installer.nsi")
            strTarget = "..\\tools\\" + strProjectNameSafe + "_Installer.nsi";

        return strTarget;
    }
    catch(e)
    {
        throw e;
    }
}

function AddFilesToCustomProj(proj, strProjectName, strProjectPath, InfFile)
{
    try
    {
        var projItems = proj.ProjectItems

        var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');

        var strTpl = '';
        var strName = '';

        var strTextStream = InfFile.OpenAsTextStream(1, -2);
        while (!strTextStream.AtEndOfStream)
        {
            strTpl = strTextStream.ReadLine();
            if (strTpl != '')
            {
                strName = strTpl;
                var strTarget = GetTargetName(strName, strProjectName);
                var strTemplate = strTemplatePath + '\\' + strTpl;
                var strFile = strProjectPath + '\\' + strTarget;

                var bCopyOnly = false;  //"true" will only copy the file from strTemplate to strTarget without rendering/adding to the project
                var strExt = strName.substr(strName.lastIndexOf("."));
                if (strExt == ".astylerc" || strExt == ".exe" || strExt == ".bmp" || strExt == ".ico" || strExt == ".gif" || strExt == ".rtf" || strExt == ".css")
                    bCopyOnly = true;

                wizard.RenderTemplate(strTemplate, strFile, bCopyOnly);

                var projFilters = proj.Object.Filters;
                // Filter / add
                if (strFile.indexOf('tools\\AStyle') !== -1) 
                {      
			// dont add Astyle tool itself to project
		} else if (strFile.indexOf('resource.h') !== -1) 
                {      
                    var filterRc = projFilters.Item("Resource Files");
                    filterRc.AddFile(strFile);
                } else if (strFile.indexOf('Installer') !== -1 || strFile.indexOf('installer') !== -1 || strFile.indexOf('.nsi') !== -1) {
                    var filterRc = projFilters.Item("Installer");
                    filterRc.AddFile(strFile);
                } else if (strFile.indexOf('.exe') !== -1 || strFile.indexOf('.bat') !== -1 || strFile.indexOf('.astyle') !== -1) {
                    var filterRc = projFilters.Item("Tools");
                    filterRc.AddFile(strFile);
                } else {
                    proj.Object.AddFile(strFile);
                }
            }
        }
        strTextStream.Close();
    }
    catch(e)
    {
        throw e;
    }
}

// SIG // Begin signature block
// SIG // MIIXPgYJKoZIhvcNAQcCoIIXLzCCFysCAQExCzAJBgUr
// SIG // DgMCGgUAMGcGCisGAQQBgjcCAQSgWTBXMDIGCisGAQQB
// SIG // gjcCAR4wJAIBAQQQEODJBs441BGiowAQS9NQkAIBAAIB
// SIG // AAIBAAIBAAIBADAhMAkGBSsOAwIaBQAEFP9REkc7iEsy
// SIG // MTaCMZhRfDelVKKzoIISMTCCBGAwggNMoAMCAQICCi6r
// SIG // EdxQ/1ydy8AwCQYFKw4DAh0FADBwMSswKQYDVQQLEyJD
// SIG // b3B5cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENvcnAu
// SIG // MR4wHAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRpb24x
// SIG // ITAfBgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhvcml0
// SIG // eTAeFw0wNzA4MjIyMjMxMDJaFw0xMjA4MjUwNzAwMDBa
// SIG // MHkxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5n
// SIG // dG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVN
// SIG // aWNyb3NvZnQgQ29ycG9yYXRpb24xIzAhBgNVBAMTGk1p
// SIG // Y3Jvc29mdCBDb2RlIFNpZ25pbmcgUENBMIIBIjANBgkq
// SIG // hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAt3l91l2zRTmo
// SIG // NKwx2vklNUl3wPsfnsdFce/RRujUjMNrTFJi9JkCw03Y
// SIG // SWwvJD5lv84jtwtIt3913UW9qo8OUMUlK/Kg5w0jH9FB
// SIG // JPpimc8ZRaWTSh+ZzbMvIsNKLXxv2RUeO4w5EDndvSn0
// SIG // ZjstATL//idIprVsAYec+7qyY3+C+VyggYSFjrDyuJSj
// SIG // zzimUIUXJ4dO3TD2AD30xvk9gb6G7Ww5py409rQurwp9
// SIG // YpF4ZpyYcw2Gr/LE8yC5TxKNY8ss2TJFGe67SpY7UFMY
// SIG // zmZReaqth8hWPp+CUIhuBbE1wXskvVJmPZlOzCt+M26E
// SIG // RwbRntBKhgJuhgCkwIffUwIDAQABo4H6MIH3MBMGA1Ud
// SIG // JQQMMAoGCCsGAQUFBwMDMIGiBgNVHQEEgZowgZeAEFvQ
// SIG // cO9pcp4jUX4Usk2O/8uhcjBwMSswKQYDVQQLEyJDb3B5
// SIG // cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENvcnAuMR4w
// SIG // HAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xITAf
// SIG // BgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhvcml0eYIP
// SIG // AMEAizw8iBHRPvZj7N9AMA8GA1UdEwEB/wQFMAMBAf8w
// SIG // HQYDVR0OBBYEFMwdznYAcFuv8drETppRRC6jRGPwMAsG
// SIG // A1UdDwQEAwIBhjAJBgUrDgMCHQUAA4IBAQB7q65+Siby
// SIG // zrxOdKJYJ3QqdbOG/atMlHgATenK6xjcacUOonzzAkPG
// SIG // yofM+FPMwp+9Vm/wY0SpRADulsia1Ry4C58ZDZTX2h6t
// SIG // KX3v7aZzrI/eOY49mGq8OG3SiK8j/d/p1mkJkYi9/uEA
// SIG // uzTz93z5EBIuBesplpNCayhxtziP4AcNyV1ozb2AQWtm
// SIG // qLu3u440yvIDEHx69dLgQt97/uHhrP7239UNs3DWkuNP
// SIG // tjiifC3UPds0C2I3Ap+BaiOJ9lxjj7BauznXYIxVhBoz
// SIG // 9TuYoIIMol+Lsyy3oaXLq9ogtr8wGYUgFA0qvFL0QeBe
// SIG // MOOSKGmHwXDi86erzoBCcnYOMIIEejCCA2KgAwIBAgIK
// SIG // YQHPPgAAAAAADzANBgkqhkiG9w0BAQUFADB5MQswCQYD
// SIG // VQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4G
// SIG // A1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0
// SIG // IENvcnBvcmF0aW9uMSMwIQYDVQQDExpNaWNyb3NvZnQg
// SIG // Q29kZSBTaWduaW5nIFBDQTAeFw0wOTEyMDcyMjQwMjla
// SIG // Fw0xMTAzMDcyMjQwMjlaMIGDMQswCQYDVQQGEwJVUzET
// SIG // MBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVk
// SIG // bW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0
// SIG // aW9uMQ0wCwYDVQQLEwRNT1BSMR4wHAYDVQQDExVNaWNy
// SIG // b3NvZnQgQ29ycG9yYXRpb24wggEiMA0GCSqGSIb3DQEB
// SIG // AQUAA4IBDwAwggEKAoIBAQC9MIn7RXKoU2ueiU8AI8C+
// SIG // 1B09sVlAOPNzkYIm5pYSAFPZHIIOPM4du733Qo2X1Pw4
// SIG // GuS5+ePs02EDv6DT1nVNXEap7V7w0uJpWxpz6rMcjQTN
// SIG // KUSgZFkvHphdbserGDmCZcSnvKt1iBnqh5cUJrN/Jnak
// SIG // 1Dg5hOOzJtUY+Svp0skWWlQh8peNh4Yp/vRJLOaL+AQ/
// SIG // fc3NlpKGDXED4tD+DEI1/9e4P92ORQp99tdLrVvwdnId
// SIG // dyN9iTXEHF2yUANLR20Hp1WImAaApoGtVE7Ygdb6v0LA
// SIG // Mb5VDZnVU0kSMOvlpYh8XsR6WhSHCLQ3aaDrMiSMCOv5
// SIG // 1BS64PzN6qQVAgMBAAGjgfgwgfUwEwYDVR0lBAwwCgYI
// SIG // KwYBBQUHAwMwHQYDVR0OBBYEFDh4BXPIGzKbX5KGVa+J
// SIG // usaZsXSOMA4GA1UdDwEB/wQEAwIHgDAfBgNVHSMEGDAW
// SIG // gBTMHc52AHBbr/HaxE6aUUQuo0Rj8DBEBgNVHR8EPTA7
// SIG // MDmgN6A1hjNodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20v
// SIG // cGtpL2NybC9wcm9kdWN0cy9DU1BDQS5jcmwwSAYIKwYB
// SIG // BQUHAQEEPDA6MDgGCCsGAQUFBzAChixodHRwOi8vd3d3
// SIG // Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL0NTUENBLmNy
// SIG // dDANBgkqhkiG9w0BAQUFAAOCAQEAKAODqxMN8f4Rb0J2
// SIG // 2EOruMZC+iRlNK51sHEwjpa2g/py5P7NN+c6cJhRIA66
// SIG // cbTJ9NXkiugocHPV7eHCe+7xVjRagILrENdyA+oSTuzd
// SIG // DYx7RE8MYXX9bpwH3c4rWhgNObBg/dr/BKoCo9j6jqO7
// SIG // vcFqVDsxX+QsbsvxTSoc8h52e4avxofWsSrtrMwOwOSf
// SIG // f+jP6IRyVIIYbirInpW0Gh7Bb5PbYqbBS2utye09kuOy
// SIG // L6t6dzlnagB7gp0DEN5jlUkmQt6VIsGHC9AUo1/cczJy
// SIG // Nh7/yCnFJFJPZkjJHR2pxSY5aVBOp+zCBmwuchvxIdpt
// SIG // JEiAgRVAfJ/MdDhKTzCCBJ0wggOFoAMCAQICEGoLmU/A
// SIG // ACWrEdtFH1h6Z6IwDQYJKoZIhvcNAQEFBQAwcDErMCkG
// SIG // A1UECxMiQ29weXJpZ2h0IChjKSAxOTk3IE1pY3Jvc29m
// SIG // dCBDb3JwLjEeMBwGA1UECxMVTWljcm9zb2Z0IENvcnBv
// SIG // cmF0aW9uMSEwHwYDVQQDExhNaWNyb3NvZnQgUm9vdCBB
// SIG // dXRob3JpdHkwHhcNMDYwOTE2MDEwNDQ3WhcNMTkwOTE1
// SIG // MDcwMDAwWjB5MQswCQYDVQQGEwJVUzETMBEGA1UECBMK
// SIG // V2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwG
// SIG // A1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSMwIQYD
// SIG // VQQDExpNaWNyb3NvZnQgVGltZXN0YW1waW5nIFBDQTCC
// SIG // ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANw3
// SIG // bvuvyEJKcRjIzkg+U8D6qxS6LDK7Ek9SyIPtPjPZSTGS
// SIG // KLaRZOAfUIS6wkvRfwX473W+i8eo1a5pcGZ4J2botrfv
// SIG // hbnN7qr9EqQLWSIpL89A2VYEG3a1bWRtSlTb3fHev5+D
// SIG // x4Dff0wCN5T1wJ4IVh5oR83ZwHZcL322JQS0VltqHGP/
// SIG // gHw87tUEJU05d3QHXcJc2IY3LHXJDuoeOQl8dv6dbG56
// SIG // 4Ow+j5eecQ5fKk8YYmAyntKDTisiXGhFi94vhBBQsvm1
// SIG // Go1s7iWbE/jLENeFDvSCdnM2xpV6osxgBuwFsIYzt/iU
// SIG // W4RBhFiFlG6wHyxIzG+cQ+Bq6H8mjmsCAwEAAaOCASgw
// SIG // ggEkMBMGA1UdJQQMMAoGCCsGAQUFBwMIMIGiBgNVHQEE
// SIG // gZowgZeAEFvQcO9pcp4jUX4Usk2O/8uhcjBwMSswKQYD
// SIG // VQQLEyJDb3B5cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0
// SIG // IENvcnAuMR4wHAYDVQQLExVNaWNyb3NvZnQgQ29ycG9y
// SIG // YXRpb24xITAfBgNVBAMTGE1pY3Jvc29mdCBSb290IEF1
// SIG // dGhvcml0eYIPAMEAizw8iBHRPvZj7N9AMBAGCSsGAQQB
// SIG // gjcVAQQDAgEAMB0GA1UdDgQWBBRv6E4/l7k0q0uGj7yc
// SIG // 6qw7QUPG0DAZBgkrBgEEAYI3FAIEDB4KAFMAdQBiAEMA
// SIG // QTALBgNVHQ8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAN
// SIG // BgkqhkiG9w0BAQUFAAOCAQEAlE0RMcJ8ULsRjqFhBwEO
// SIG // jHBFje9zVL0/CQUt/7hRU4Uc7TmRt6NWC96Mtjsb0fus
// SIG // p8m3sVEhG28IaX5rA6IiRu1stG18IrhG04TzjQ++B4o2
// SIG // wet+6XBdRZ+S0szO3Y7A4b8qzXzsya4y1Ye5y2PENtEY
// SIG // Ib923juasxtzniGI2LS0ElSM9JzCZUqaKCacYIoPO8cT
// SIG // ZXhIu8+tgzpPsGJY3jDp6Tkd44ny2jmB+RMhjGSAYwYE
// SIG // lvKaAkMve0aIuv8C2WX5St7aA3STswVuDMyd3ChhfEjx
// SIG // F5wRITgCHIesBsWWMrjlQMZTPb2pid7oZjeN9CKWnMyw
// SIG // d1RROtZyRLIj9jCCBKowggOSoAMCAQICCmEFojAAAAAA
// SIG // AAgwDQYJKoZIhvcNAQEFBQAweTELMAkGA1UEBhMCVVMx
// SIG // EzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1Jl
// SIG // ZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3Jh
// SIG // dGlvbjEjMCEGA1UEAxMaTWljcm9zb2Z0IFRpbWVzdGFt
// SIG // cGluZyBQQ0EwHhcNMDgwNzI1MTkwMTE1WhcNMTMwNzI1
// SIG // MTkxMTE1WjCBszELMAkGA1UEBhMCVVMxEzARBgNVBAgT
// SIG // Cldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAc
// SIG // BgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjENMAsG
// SIG // A1UECxMETU9QUjEnMCUGA1UECxMebkNpcGhlciBEU0Ug
// SIG // RVNOOjg1RDMtMzA1Qy01QkNGMSUwIwYDVQQDExxNaWNy
// SIG // b3NvZnQgVGltZS1TdGFtcCBTZXJ2aWNlMIIBIjANBgkq
// SIG // hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA8AQtspbAGoFn
// SIG // JbEmYrMTS84wusASOPyBZTQHxDayJGj2BwTAB5f0t/F7
// SIG // HmIsRtlLpFE0t9Ns7Vo7tIOhRz0RCC41a0XmwjyMAmYC
// SIG // qRhp60rtJyzuPHdbpNRwmUtXhBDQry34iR3m6im058+e
// SIG // BmKnclTCO8bPP7jhsFgQbOWl18PCdTe99IXhgego2Bvx
// SIG // 8q7xgqPW1wOinxWE+z36q+G2MsigAmTz5v8aJnEIU4oV
// SIG // AvKDJ3ZJgnGn760yeMbXbBZPImWXYk1GL/8jr4XspnC9
// SIG // A8va2DIFxSuQQLae1SyGbLfLEzJ9jcZ+rhcvMvxmux2w
// SIG // RVX4rfotZ4NnKZOE0lqhIwIDAQABo4H4MIH1MB0GA1Ud
// SIG // DgQWBBTol/b374zx5mnjWWhO95iKet2bLjAfBgNVHSME
// SIG // GDAWgBRv6E4/l7k0q0uGj7yc6qw7QUPG0DBEBgNVHR8E
// SIG // PTA7MDmgN6A1hjNodHRwOi8vY3JsLm1pY3Jvc29mdC5j
// SIG // b20vcGtpL2NybC9wcm9kdWN0cy90c3BjYS5jcmwwSAYI
// SIG // KwYBBQUHAQEEPDA6MDgGCCsGAQUFBzAChixodHRwOi8v
// SIG // d3d3Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL3RzcGNh
// SIG // LmNydDATBgNVHSUEDDAKBggrBgEFBQcDCDAOBgNVHQ8B
// SIG // Af8EBAMCBsAwDQYJKoZIhvcNAQEFBQADggEBAA0/d1+R
// SIG // PL6lNaTbBQWEH1by75mmxwiNL7PNP3HVhnx3H93rF7K9
// SIG // fOP5mfIKRUitFLtpLPI+Z2JU8u5/JxGSOezO2YdOiPdg
// SIG // RyN7JxVACJ+/DTEEgtg1tgycANOLqnhhxbWIQZ0+NtxY
// SIG // pCebOtq9Bl0UprIPTMGOPIvyYpn4Zu3V8xwosDLbyjEJ
// SIG // vPsiaEZM+tNzIucpjiIA+1a/Bq6BoBW6NPkojh9KYgWh
// SIG // ifWBR+kNkQjXWDuPHmsJaanASHxVgj9fADhDnAbMP9gv
// SIG // v09zCT39ul70x+w3wmRhoE3UPXDMW7ATgcHUozEavWTW
// SIG // ltJ6PypbRlMJPM0D+T9ZAMyJU2ExggR5MIIEdQIBATCB
// SIG // hzB5MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGlu
// SIG // Z3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMV
// SIG // TWljcm9zb2Z0IENvcnBvcmF0aW9uMSMwIQYDVQQDExpN
// SIG // aWNyb3NvZnQgQ29kZSBTaWduaW5nIFBDQQIKYQHPPgAA
// SIG // AAAADzAJBgUrDgMCGgUAoIGkMBkGCSqGSIb3DQEJAzEM
// SIG // BgorBgEEAYI3AgEEMBwGCisGAQQBgjcCAQsxDjAMBgor
// SIG // BgEEAYI3AgEVMCMGCSqGSIb3DQEJBDEWBBRD573odGht
// SIG // DuvZD+9oREp/ubzpsDBEBgorBgEEAYI3AgEMMTYwNKAa
// SIG // gBgAYwB1AHMAdABvAG0AdwBpAHoALgBqAHOhFoAUaHR0
// SIG // cDovL21pY3Jvc29mdC5jb20wDQYJKoZIhvcNAQEBBQAE
// SIG // ggEARB6u45hpHcw9dJ1kVqrdxQKIDXpHqE5ug51VwhkY
// SIG // KUgWTmI5ra0+ufLmKXYQOLNPSnNgWonKeG0w8K5QD4UW
// SIG // J9C0JG5nAqlyfJIaaq6htLw2cz5Ke453xiNP1LNuwWtP
// SIG // VdKQLmdPul5VW/SYXkxJ4VKZ/eUM6WoXanYD+NiLvcWj
// SIG // uSoQhohdvXnN8f6z6Xxzta7xQ+GdVShJFPa1a9IQDmLd
// SIG // g3KbK8pvBzCWZLp7XuF9OsxlNd6iFxZU78FPbN5YtGnR
// SIG // cLxR4MzBxavVh9pyEHscP4EwPkeaetMYIh2ufrbEr98R
// SIG // SL/gEjT4hsxwiwE04QkgQqNcRKTPC047gbeYqqGCAh8w
// SIG // ggIbBgkqhkiG9w0BCQYxggIMMIICCAIBATCBhzB5MQsw
// SIG // CQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQ
// SIG // MA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9z
// SIG // b2Z0IENvcnBvcmF0aW9uMSMwIQYDVQQDExpNaWNyb3Nv
// SIG // ZnQgVGltZXN0YW1waW5nIFBDQQIKYQWiMAAAAAAACDAH
// SIG // BgUrDgMCGqBdMBgGCSqGSIb3DQEJAzELBgkqhkiG9w0B
// SIG // BwEwHAYJKoZIhvcNAQkFMQ8XDTEwMDMxOTEzMDA1OFow
// SIG // IwYJKoZIhvcNAQkEMRYEFI2my6oEaZuQ5u31IF2xpaLt
// SIG // 0zomMA0GCSqGSIb3DQEBBQUABIIBABZaWeWNnLDWxt8t
// SIG // t0w1E1e+LMEzC1oELO1CyyPJ/z4mYrbtJkEjVO4n9Qrh
// SIG // WRCwiFVMEbBnb0fqvXncDMwBvuu7ODjLhXDAsaZgHOof
// SIG // IWhvUTgFb+iJmq3E9wd/9xAwppwHemW8xXy76cT1BH3G
// SIG // O5F/PlW/DVeMau1E0QmCdWGR3Y648/p9p2v2cwPWGy/z
// SIG // Sry8HYjS2ZfSapOtqJ3WQnEmu5YEhwq6++o9IWROK76I
// SIG // cT3oZoK/097drHGC/LTC+L4NRxRv22MgH16QtBhCWVOd
// SIG // cu0pTmQwz2DjlkcxKF2/pBGnNAWs05oEqr4WLasds3IH
// SIG // pD24xF79NyE/6vam6LQ=
// SIG // End signature block
