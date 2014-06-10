from PySide import QtCore, QtGui

COLORTABLE=[QtGui.qRgb(255,255,255)]
for i in range(255): COLORTABLE.append(QtGui.qRgb(i,i,i))

class ImageViewer(QtGui.QWidget):
    def __init__(self, parent):
        super(ImageViewer, self).__init__()

        self.parent = parent

        self.ui = loadUiWidget('ui/display.ui', self.parent)
        self.connectUI()
        #Show the window and maximize to full screen
        self.ui.show()


        self.printer = QtGui.QPrinter()
        self.scaleFactor = 0.0
