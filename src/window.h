/////////////////////////////////////////////////////////////////////////////////////
//
//This file is part of benImage
//
//benImage is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////////////


#ifndef WINDOW_H
#define WINDOW_H


#include "iostream"
#include "fstream"
#include "vector"
#include "sstream"
#include "string"
#include "list"

#include "QApplication"
#include "QMenuBar"
#include "QMenu"
#include "QWidget"
#include "QVBoxLayout"
#include "QFileDialog"
#include "QGraphicsScene"
#include "QGraphicsView"
#include "QGraphicsPixmapItem"
#include "QPixmap"
#include "QSize"
#include "QImage"
#include "QMainWindow"
#include "QPainter"
#include "QLabel"
#include "QFile"
#include "QRgb"
#include "QTextStream"
#include "QMessageBox"
#include "QFileInfo"


using namespace std;

struct benHeaderFile
{
    char type;
    unsigned byteSize;
};

struct benHeaderInfo
{
    unsigned byteSize;
    unsigned height;
    unsigned width;
};

struct colorStruct
{
    unsigned char r,g,b;
    colorStruct(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b)
    {

    }
};

class CWindow : public QMainWindow
{
    //define qobject
    Q_OBJECT
public:
    //constructor
    CWindow();

    //destructor
    ~CWindow();

    //conversions
    //convertToRGB: takes a string RRGGBB and converts it into a QColor
    QColor convertToRGB(const string& hex);
    //convertToHex: legacy
    string convertToHex(const QColor& color);
    unsigned char convertToHex(int r, int g, int b);
    //convert qcolor to a char array
    char getCharFromColor(const QColor &color);

    //if opened with windows thing
    void OpenedWithFile(const std::string &file);
    void DisplayFile();

    QString OpenedFile;
    QLabel *label;
    QPainter *p;
public slots:
    //openFile: opens a .ben file and displays it on the screen
    void OpenFile();

    //openbmpfile : opens a .bmp and displays it on the screen
    void OpenBMPFile();

    //encodes image on screen into a .ben file
    void SaveFile();

    //displays simple messagebox for credit info
    void OpenAbout();

private:


    QPixmap *mainpix;
    //necessary stuff
    QMenu *file;
    QMenu *help;

    QAction *open;
    QAction *open_bmp;
    QAction *save;
    QAction *quit;
    QAction *about;
};

#endif // WINDOW_H
