This directory contents script files for building installers (installation programs) for zNotes.
The directory "is" contents scripts for building an installer with "Inno Setup Compiler".
The directory "qtifw" contents scripts for building an installer with "Qt Installer Framework".
To build an installer with "Inno Setup Compiler" it needs to run command:
  "<path to Inno Setup directory>\iscc.exe" script.iss 
in "is" directory.

To build an installer with "Qt Installer Framework" it needs to run command:
  <path to Qt Installer Framework bin directory>\binarycreator.exe --offline-only -p packages -c config\config.xml zNotesInstaller.exe
in "qtifw" directory.
