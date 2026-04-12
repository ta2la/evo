/**
 * @class Cmd
 * @brief Command Class.
 */
#pragma once

#include <QString>

//=============================================================================
class  EmBrowserFileAccess {
//=============================================================================
public:
//!  Construction
    EmBrowserFileAccess() = delete;
//<METHODS>
    static void pickDir();
    static void pickDirAndList();
    static int  pickDirStatus();
    static QString pickDirName();
    static void listDir();
    static int  fileCount();
    static QString  fileNameGet(int index);
    static void readTestFile(const QString& fileName);
    static QString getTestFile();
    static void    writeFile(const QString& fileName, const QString& content);

    static void startPickAndList();
    static void downloadUrlToDir(const QString& url, const QString& fileName);
    static bool isChromiumBrowser();
//=============================================================================
//protected:
//! @section Data
};
