/**
 * @file Cmd.cpp
 * @brief Implementation of Cmd class.
 */

#include "EmBrowserFileAccess.h"
#include "TcCmdTransl.h"
#include <emscripten.h>
#include "T2lActiveFile.h"
#include "T2lGFile.h"
#include "T2lFileRetrieverModel.h"
#include "T2lDisplayCol.h"
#include "T2lCmd_view_fit.h"

#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QFileInfo>

extern "C" {
EMSCRIPTEN_KEEPALIVE
    void onPickDirResult(int result)
{
    if (FileRetrieverModel::instance().retriever()) {
        FileRetrieverModel::instance().retriever()->setContent("");
    }
    XCALL("voidcmd")
}
}

//=============================================================================
EM_JS(void, pickDirEx_, (), {
    window.pickDirStatus = -2;
    window.showDirectoryPicker({ mode: 'readwrite' })
        .then(async dir => {
            window.dir = dir;
            window.pickDirStatus = 1;

            // Read directory contents
            try {
                let dirList = [];
                for await (const entry of dir.values()) {
                    if (entry.kind === 'file' &&
                             entry.name.toLowerCase().endsWith('.t2d')) {
                        dirList.push(entry.name);
                    }
                }
                window.dirFiles = dirList;
                window.listDirComplete = 1;
            } catch (e) {
                console.error("Error reading directory:", e);
                window.dirlist = [];
            }

            _onPickDirResult(1);
        })
        .catch(e  => { console.error(e); window.dir = null; window.pickDirStatus = -1; });

    console.log("listDir_");
});

//=============================================================================
EM_JS(void, pickDir_, (), {
    window.pickDirStatus = -2;
    window.showDirectoryPicker({ mode: 'readwrite' })
        .then(dir => { window.dir = dir; window.pickDirStatus = 1; })
        .catch(e  => { console.error(e); window.dir = null; window.pickDirStatus = -1; });

    console.log("listDir_");
});

//=============================================================================
EM_JS(int, pickDirStatus_, (), {
    return window.pickDirStatus ?? 0;
});

//=============================================================================
EM_JS(void, listDir_, (), {
    console.log("listDir_");

    if (!window.dir) {
        console.error("No directory selected");
        window.dirFiles = null;
        window.listDirComplete = -1;
        return;
    }

    window.listDirComplete = -2;
    window.dirFiles = [];

    console.log("listDir_");

    // Read entries (non-recursive)
    (async function () {
        try {
             for await (const handle of window.dir.values()) {
                console.log("listDir_ HANDLE kind", handle.kind);
                if (handle.kind === 'file') {
                    window.dirFiles.push(handle.name);
                    console.log("listDir_ HANDLE name", handle.name);
                }
             }
            window.listDirComplete = 1;
        } catch (e) {
            console.error(e);
            window.dirFiles = null;
            window.listDirComplete = -1;
        }
    })();

    console.log("listDir_ FINISHED");
});

//=============================================================================
EM_JS(int, listDirStatus_, (), {
    return window.listDirComplete ?? 0;
});

//=============================================================================
EM_JS(int, listDirCount_, (), {
    return window.dirFiles ? window.dirFiles.length : 0;
});

//=============================================================================
EM_JS(const char*, listDirFileName_, (int index), {
    if (!window.dirFiles || index < 0 || index >= window.dirFiles.length)
        return 0;
    var str = window.dirFiles[index];
    var lengthBytes = lengthBytesUTF8(str) + 1;
    var stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(str, stringOnWasmHeap, lengthBytes);
    return stringOnWasmHeap;
});

//=============================================================================
extern "C" {
EMSCRIPTEN_KEEPALIVE
    void readFile_callback(int result)
{
    T2l::ActiveFile* af = T2l::ActiveFile::activeGet();
    QString con = EmBrowserFileAccess::getTestFile();
    qDebug() << con;

    QTextStream ts(&con);
    af->file()->loadStream_(ts);

    T2l::Display* view = T2l::DisplayCol::instance().active();
    if (view) {
        T2l::Box2F box = T2l::Cmd_view_fit::fitBox(*view);
        if (!box.isEmpty()) T2l::Cmd_view_fit::fitDisplay(*view, box);
    }
}
}

//=============================================================================
EM_JS(int, readFileToWindowBuff_, (const char* filename, int maxBytes), {
    if (!window.dir) {
        console.error("No directory selected");
        window.readFileStatus = -1;
        return -1;
    }

    const name = UTF8ToString(filename);
    const limit = (maxBytes|0) > 0 ? (maxBytes|0) : 10000;

    window.readFileStatus = -2; // in progress
    window.buff = "";           // clear old value
    window.readFileSize = 0;

    (async () => {
        try {
            const fileHandle  = await window.dir.getFileHandle(name);
            const file        = await fileHandle.getFile();
            const slice       = file.slice(0, limit);
            const arrayBuffer = await slice.arrayBuffer();
            const bytes       = new Uint8Array(arrayBuffer);

            // Decode to text (UTF-8)
            const text = new TextDecoder("utf-8", { fatal: false }).decode(bytes);

            window.buff = text;
            window.readFileSize = bytes.length;
            window.readFileStatus = 1;

            // Optional debug preview
            const hex = Array.from(bytes.slice(0, Math.min(bytes.length, 64)))
                            .map(b => b.toString(16).padStart(2, "0")).join(" ");
            console.log(`readFileToWindowBuff_: read ${bytes.length} bytes from ${name}`);
            console.log("Data (first 64 bytes hex):", hex);
            console.log("Data (as text preview):", text.slice(0, 200));

            _readFile_callback(1);
        } catch (e) {
            console.error("readFileToWindowBuff_ error:", e);
            window.buff = "";
            window.readFileSize = 0;
            window.readFileStatus = -1;
        }
    })();

    return -2; // started
});

//=============================================================================
EM_JS(char*, getWindowBuffText_, (), {
    if (typeof window.buff !== 'string') return 0;
    var str = window.buff;
    var lengthBytes = lengthBytesUTF8(str) + 1;
    var p = _malloc(lengthBytes);
    stringToUTF8(str, p, lengthBytes);
    return p;
});

//=============================================================================
EM_JS(int, writeWindowBuffToFile_, (const char* filename), {
    if (!window.dir) {
        console.error("No directory selected");
        window.writeFileStatus = -1;
        return -1;
    }

    const name = UTF8ToString(filename);
    window.writeFileStatus = -2; // in progress

    (async () => {
        try {
            const fileHandle = await window.dir.getFileHandle(name, { create: true });
            const writable   = await fileHandle.createWritable();

            // Ensure window.buff is a string
            const text = (typeof window.buff === "string") ? window.buff : String(window.buff);

            // Encode and write
            const encoder = new TextEncoder();
            await writable.write(encoder.encode(text));
            await writable.close();

            console.log(`writeWindowBuffToFile_: wrote ${text.length} chars to ${name}`);
            console.log("Data (first 200 chars):", text.slice(0, 200));

            window.writeFileStatus = 1; // success
        } catch (e) {
            console.error("writeWindowBuffToFile_ error:", e);
            window.writeFileStatus = -1; // error
        }
    })();

    return -2; // started
});

//=============================================================================
EM_JS(int, readFile_, (const char* filename), {
    if (!window.dir) {
        console.error("No directory selected");
        window.writeFileStatus = -1;
        return -1;
    }

    const name = UTF8ToString(filename);
    window.writeFileStatus = -2; // in progress

    (async () => {
        try {
            const fileHandle = await window.dir.getFileHandle(name, { create: true });
            const writable   = await fileHandle.createWritable();

            // Ensure window.buff is a string
            const text = (typeof window.buff === "string") ? window.buff : String(window.buff);

            // Encode and write
            const encoder = new TextEncoder();
            await writable.write(encoder.encode(text));
            await writable.close();

            console.log(`writeWindowBuffToFile_: wrote ${text.length} chars to ${name}`);
            console.log("Data (first 200 chars):", text.slice(0, 200));

            window.writeFileStatus = 1; // success

            //readFile_callback();
        } catch (e) {
            console.error("writeWindowBuffToFile_ error:", e);
            window.writeFileStatus = -1; // error
        }
    })();

    return -2; // started
});

//=============================================================================
EM_JS(int, setWindowBuffText_, (const char* utf8), {
    try {
        window.buff = utf8 ? UTF8ToString(utf8) : "";
        return 1;
    } catch (e) {
        console.error("setWindowBuffText_ error:", e);
        return -1;
    }
});

//=============================================================================
EM_JS(int, downloadUrlToDir_, (const char* url, const char* filename), {
    if (!window.dir) {
        console.error("No directory selected");
        return -1;
    }
    const urlStr  = UTF8ToString(url);
    const nameStr = UTF8ToString(filename);

    (async () => {
        try {
            const response = await fetch(urlStr);
            if (!response.ok) throw new Error("HTTP " + response.status);
            const text = await response.text();

            const fileHandle = await window.dir.getFileHandle(nameStr, { create: true });
            const writable   = await fileHandle.createWritable();
            await writable.write(new TextEncoder().encode(text));
            await writable.close();

            if (!window.dirFiles.includes(nameStr)) window.dirFiles.push(nameStr);

            console.log("downloadUrlToDir_: saved " + nameStr + " (" + text.length + " chars)");
            _onPickDirResult(1);
        } catch (e) {
            console.error("downloadUrlToDir_ error:", e);
        }
    })();
    return 0;
});

//=============================================================================
EM_JS(bool, isChromiumBrowser_, (), {
    // JavaScript code runs in browser context
    var ua = navigator.userAgent;
    // Return true if Chrome or Edge (both Chromium-based)
    return ua.includes("Chrome/") || ua.includes("Edg/");
});

//=============================================================================
void EmBrowserFileAccess::downloadUrlToDir(const QString& url, const QString& fileName)
{
    downloadUrlToDir_(url.toUtf8().constData(), fileName.toUtf8().constData());
}

//=============================================================================
bool EmBrowserFileAccess::isChromiumBrowser()
{
    return isChromiumBrowser_();
}

//=============================================================================
void EmBrowserFileAccess::writeFile(const QString& fileName, const QString& content)
{
    const QByteArray textUtf8 = content.toUtf8();
    setWindowBuffText_(textUtf8.constData());

    const QByteArray nameUtf8 = fileName.toUtf8();
    writeWindowBuffToFile_(nameUtf8.constData());
}

//=============================================================================
void EmBrowserFileAccess::readTestFile(const QString& fileName)
{
    QString cmd = QString("cad_create_file_direct /fake ") + QFileInfo(fileName).completeBaseName();
    std::string cmdStr = cmd.toStdString();
    XCALL(cmdStr.c_str());

    readFileToWindowBuff_(fileName.toStdString().c_str(), 10000);
}

//=============================================================================
QString EmBrowserFileAccess::getTestFile()
{
    char* textPtr = getWindowBuffText_();
    QString text;
    if (textPtr) {
        text = QString::fromUtf8(textPtr);
        free(textPtr);
    }

    if (text.isEmpty()) text = "EMPTY";
    return text;
}

//=============================================================================
void EmBrowserFileAccess::pickDir()
{
    pickDir_();
}

//=============================================================================
int EmBrowserFileAccess::pickDirStatus()
{
    return pickDirStatus_();
}

//=============================================================================
void EmBrowserFileAccess::listDir()
{
    listDir_();
}

//=============================================================================
int EmBrowserFileAccess::fileCount()
{
    return listDirCount_();
}

//=============================================================================
QString  EmBrowserFileAccess::fileNameGet(int index)
{
    const char* nameP = listDirFileName_(index);
    QString name(nameP);
    free((void*)nameP);
    return name;
}

//=============================================================================
void EmBrowserFileAccess::startPickAndList() {
    pickDirEx_();
}

//=============================================================================
