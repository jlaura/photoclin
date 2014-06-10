#! /usr/bin/env pythonw

import os
import sys
from PySide import QtCore, QtUiTools, QtGui

from osgeo import gdal

COLORTABLE=[QtGui.qRgb(255,255,255)]
for i in range(255): COLORTABLE.append(QtGui.qRgb(i,i,i))


def loadUiWidget(uifilename, parent=None):
    """
    Function to load a QtDesigned .ui file
    """
    uifilename =  os.path.dirname(os.path.abspath(__file__)) + '/' + uifilename
    loader = QtUiTools.QUiLoader()
    uifile = QtCore.QFile(uifilename)
    uifile.open(QtCore.QFile.ReadOnly)
    ui = loader.load(uifile, parent)
    uifile.close()
    return ui

class MainWindow(QtGui.QMainWindow):

    def __init__(self):
        super(MainWindow, self).__init__()
        self.windows = []

        self.ui = loadUiWidget('ui/main.ui', None)
        self.connectUI()
        #Show the window and maximize to full screen
        self.ui.show()
        self.ui.showMaximized()

    def connectUI(self):
        ui = self.ui
        ui.actionAbout.triggered.connect(self.aboutDialog)
        ui.actionOpen.triggered.connect(self.openDialog)

    def aboutDialog(self):
        self.aboutui = loadUiWidget('ui/about.ui', self)
        self.aboutui.show()

    def openDialog(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self, 'Open file',
                                                     QtCore.QDir.currentPath())
        if fileName:
            #Open the image
            ds = gdal.Open(fileName[0])
            image = ImageViewer(self, ds)
            self.ui.mdiarea.addSubWindow(image)
            self.windows.append(image)
            image.show()
            #return fname, fpath, dirpath

class ImageViewer(QtGui.QWidget):
    def __init__(self, parent, datasource):
        super(ImageViewer, self).__init__()

        self.parent = parent
        self.ds = datasource

        self.ui = loadUiWidget('ui/display.ui', self.parent)

        #Load the image array into
        self.loadArray()
        self.connectUI()

        self.loadimg1()

    def connectUI(self):
        img1 = self.ui.img1
        img1.setBackgroundRole(QtGui.QPalette.Base)
        img1.setSizePolicy(QtGui.QSizePolicy.Ignored,
                           QtGui.QSizePolicy.Ignored)
        img1.setScaledContents(True)


    def loadArray(self):
        self.arr = self.ds.GetRasterBand(1).ReadAsArray()
        print self.arr
        self.shape = self.arr.shape

    def loadimg1(self):
        image = QtGui.QImage(self.arr.data, self.shape[1], self.shape[0], QtGui.QImage.Format_Indexed8)
        image.setColorTable(COLORTABLE)
        self.ui.img1.setPixmap(QtGui.QPixmap.fromImage(image))
        self.scaleFactor = 1.0

    def zoomIn(self):
        self.scaleImage(1.25)

    def zoomOut(self):
        self.scaleImage(0.8)

    def fullsize(self):
        self.ui.img1.adjustSize()
        self.scaleFactor = 1.0

    def fitToWindow(self):
        fitToWindow = self.ui.home.isChecked()
        self.ui.scrollarea.setWidgetResizabl(fitToWindow)
        if not fitToWindow:
            self.fullsize()

    def scaleImage(self, factor):
        self.scaleFactor *= factor
        self.ui.img1.resize(self.scaleFactor * self.ui.img1.pixmap().size())

        self.adjustScrollBar(self.scrollArea.horizontalScrollBar(), factor)
        self.adjustScrollBar(self.scrollArea.verticalScrollBar(), factor)

        #self.zoomInAct.setEnabled(self.scaleFactor < 3.0)
        #self.zoomOutAct.setEnabled(seld.scaleFactor > 0.333)

    def adjustScrollBar(self, scrollBar, factor):
        scrollBar.setValue(int(factor * scrollBar.value() + ((factor - 1) * scrollBar.pageStep() / 2)))

if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    mainwindow = MainWindow()
    sys.exit(app.exec_())
