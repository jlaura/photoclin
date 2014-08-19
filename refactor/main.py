#! /usr/bin/env pythonw

import json
import logging
import logging.handlers
import os
import sys
import time
from functools import partial

#PySide GUI
from PySide import QtCore, QtUiTools, QtGui
from PySide.QtUiTools import QUiLoader
from PySide.QtCore import Slot, QMetaObject
import PySide.QtXml  # Explicit import for PyInstaller

#GDAL
from osgeo import gdal

#Matplotlib
import matplotlib
matplotlib.rcParams['backend.qt4'] = 'PySide'
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg
from matplotlib.figure import Figure

import numpy as np

import fileio

COLORTABLE=[QtGui.qRgb(255,255,255)]
for i in range(255): COLORTABLE.append(QtGui.qRgb(i,i,i))

destination = {'btn_open':'inscroll', 'btn_demopen':'outscroll'}

class Plot(QtGui.QWidget):
    """
    """
    def __init__(self, parent, uielement, title=None, xlabel=None, ylabel=None):
        super(Plot, self).__init__()
        self.parent = parent

        #Set dpi via preference
        self.fig = Figure(dpi=72)
        self.ax = self.fig.add_subplot(111)
        self.ax.grid()
        self.ax.set_xlabel(xlabel)
        self.ax.set_ylabel(ylabel)
        self.ax.set_title(title)
        self.canvas = FigureCanvas(self.fig)

        ntb = NavigationToolbar(self.canvas, self)
        uielement.addWidget(self.canvas, 1)  # Expanding
        uielement.addWidget(ntb)

class MapPlot(QtGui.QWidget):
    """
    """
    def __init__(self, parent, uielement):
        super(MapPlot, self).__init__()
        self.parent = parent
        self.img = None

        #Set dpi via preference
        self.fig = Figure(dpi=72)
        self.ax = self.fig.add_subplot(111)
        self.fig.subplots_adjust(wspace=0.01, hspace=0.01)
        self.ax.grid()
        self.ax.set_xticks([])
        self.ax.set_yticks([])
        self.canvas = FigureCanvas(self.fig)

        combo = QtGui.QComboBox(self)
        [combo.addItem(m) for m in matplotlib.cm.datad]
        combo.activated[str].connect(self.recolor)


        ntb = NavigationToolbar(self.canvas, self)
        ntb.addWidget(combo)
        uielement.addWidget(self.canvas, 1)  # Expanding
        uielement.addWidget(ntb)

    def recolor(self, cbar):
        self.img.set_cmap(cbar)
        self.canvas.draw()

class UiLoader(QUiLoader):
    """
    Subclass :class:`~PySide.QtUiTools.QUiLoader` to create the user interface
    in a base instance.

    Unlike :class:`~PySide.QtUiTools.QUiLoader` itself this class does not
    create a new instance of the top-level widget, but creates the user
    interface in an existing instance of the top-level class.

    This mimics the behaviour of :func:`PyQt4.uic.loadUi`.
    """

    def __init__(self, baseinstance):
        """
        Create a loader for the given ``baseinstance``.

        The user interface is created in ``baseinstance``, which must be an
        instance of the top-level class in the user interface to load, or a
        subclass thereof.

        ``parent`` is the parent object of this loader.
        """
        QUiLoader.__init__(self, baseinstance)
        self.baseinstance = baseinstance

    def createWidget(self, class_name, parent=None, name=''):
        if parent is None and self.baseinstance:
            # supposed to create the top-level widget, return the base instance
            # instead
            return self.baseinstance
        else:
            # create a new widget for child widgets
            widget = QUiLoader.createWidget(self, class_name, parent, name)
            if self.baseinstance:
                # set an attribute for the new child widget on the base
                # instance, just like PyQt4.uic.loadUi does.
                setattr(self.baseinstance, name, widget)
            return widget

class NavigationToolbar(NavigationToolbar2QTAgg):
    # only display the buttons we need
    toolitems = [t for t in NavigationToolbar2QTAgg.toolitems if
                 t[0] in ('Home', 'Pan', 'Zoom')]

def loadUiWidget(uifile, baseinstance=None):
    """
    Dynamically load a user interface from the given ``uifile``.

    ``uifile`` is a string containing a file name of the UI file to load.

    If ``baseinstance`` is ``None``, the a new instance of the top-level widget
    will be created.  Otherwise, the user interface is created within the given
    ``baseinstance``.  In this case ``baseinstance`` must be an instance of the
    top-level widget class in the UI file to load, or a subclass thereof.  In
    other words, if you've created a ``QMainWindow`` interface in the designer,
    ``baseinstance`` must be a ``QMainWindow`` or a subclass thereof, too.  You
    cannot load a ``QMainWindow`` UI file with a plain
    :class:`~PySide.QtGui.QWidget` as ``baseinstance``.

    :method:`~PySide.QtCore.QMetaObject.connectSlotsByName()` is called on the
    created user interface, so you can implemented your slots according to its
    conventions in your widget class.

    Return ``baseinstance``, if ``baseinstance`` is not ``None``.  Otherwise
    return the newly created instance of the user interface.
    """
    loader = UiLoader(baseinstance)
    widget = loader.load(uifile)
    QMetaObject.connectSlotsByName(widget)
    return widget

class MainWindow(QtGui.QMainWindow):

    def __init__(self):
        super(MainWindow, self).__init__()
        self.windows = {}
        self.plots = {}
        self.wcounter = 0
        self.imageloaded = False
        self.dtmloaded = False

        #Show the window and maximize to full screen
        self.ui = loadUiWidget('ui/main.ui', self)
        self.ui.show()
        self.ui.showMaximized()
        self.ui.activateWindow()
        self.ui.raise_()

        self.connectUI()
        self.defineplots()

        #Logger to write INFO to the GUI and DEBUG to a file
        self.logger = logging.getLogger(__name__)
        formatter = logging.Formatter('[%(levelname)s] %(message)s')
        handler = logging.StreamHandler(stream=sys.stdout)
        handler.setFormatter(formatter)
        handler.setLevel(logging.DEBUG)
        h2 = logging.handlers.RotatingFileHandler('Pc2d.log', maxBytes=1000000, backupCount=3)
        h2.setFormatter(formatter)
        h2.setLevel(logging.DEBUG)
        self.logger.addHandler(handler)
        self.logger.addHandler(h2)
        self.logger.setLevel(logging.DEBUG)

        #welcome='PC2D Logfile: {} {}.'.format(time.strftime("%d/%m/%Y"),time.strftime("%H:%M:%S") )
        self.logger.info('Welcome to PC2D {} {}'.format(time.strftime("%m/%d/%Y"),time.strftime("%H:%M:%S") ))

    def defineplots(self):
        for i, pl in enumerate([('Steps V. Residuals', '# of NR Steps', 'Residuals', self.ui.plotavlb),
        ('Residuals V. SOR Step', 'SOR Step', 'Residuals', self.ui.plotbvlb)]):
            self.plots[i] = Plot(self, pl[3], pl[0], pl[1], pl[2])


    def connectUI(self):
        ui = self.ui
        ui.actionAbout.triggered.connect(self.aboutDialog)
        ui.actionOpen.triggered.connect(partial(self.openDialog, 'image'))
        ui.btn_open.clicked.connect(partial(self.openDialog, 'image'))
        ui.actionOpen_DTM.triggered.connect(partial(self.openDialog, 'dtm'))
        ui.btn_demopen.clicked.connect(partial(self.openDialog, 'dtm'))

        ui.link.clicked[bool].connect(self.linkdisplays)

        #Redirect stdout and stderr to the logging window.
        sys.stdout = OutLog(ui.logtext, sys.stdout)
        #sys.stderr = OutLog(ui.logtext, sys.stderr, QtGui.QColor(255,0,0))

    def createImageViewers(self, source):
        """
        Creates the labels within scroll boxs that the input images are rendered to.
        """

        ui = self.ui
        if source == 'image':
            self.deletebutton(source)
            self.plots['image'] = MapPlot(self, self.ui.imagevlb)
        else:
            self.deletebutton(source)
            self.plots['dtm'] = MapPlot(self, self.ui.dtmvlb)


    def linkdisplays(self, active):
        print active
        if active:
            if 'image' in self.plots.keys() and 'dtm' in self.plots.keys():
                self.imglink = self.plots['image'].ax.callbacks.connect('xlim_changed', partial(self.on_xlim_changed, 'image'))
                self.dtmlink = self.plots['dtm'].ax.callbacks.connect('xlim_changed', partial(self.on_xlim_changed, 'dtm'))
            else:
                print 'Unsetting link.'
                self.ui.link.setChecked(False)
        else:
            print "Unchecking"
            self.ui.link.setChecked(False)

            try:
                print "Removing connect"
                self.plots['image'].canvas.mpl_disconnect(self.imglink)
                self.plots['dtm'].canvas.mpl_disconnect(self.dtmlink)
            except:
                print "Failed to remove"

    def on_xlim_changed(self, source, ax):
        self.plots[source].canvas.draw()
        xlim = self.plots[source].ax.get_xlim()
        ylim = self.plots[source].ax.get_ylim()

        if source == 'image':
            dest = 'dtm'
        elif source == 'dtm':
            dest = 'image'

        destax = self.plots[dest].ax
        destax.set_xlim(xlim, emit=False)
        destax.set_ylim(ylim, emit=False)

        self.plots[dest].canvas.draw()

    def deletebutton(self, source):
        """
        Method to remove the UI button for loading an image or DEM.
        """
        if source == 'image':
            #Remove Image
            self.ui.imagevlb.takeAt(1)
            #Remove toolbar
            self.ui.imagevlb.takeAt(1)
        else:
            self.ui.dtmvlb.takeAt(1)
            self.ui.dtmvlb.takeAt(1)

    def aboutDialog(self):
        self.aboutui = loadUiWidget('ui/about.ui', None)
        self.aboutui.show()

    def openDialog(self, source):
        filename = QtGui.QFileDialog.getOpenFileName(self, 'Open file',
                                                     QtCore.QDir.currentPath())

        if len(filename[0]) > 0:
            #Open the image
            self.createImageViewers(source)
            self.ds = gdal.Open(filename[0])
            self.logger.debug('Opened {} from source "{}"'.format(filename[0], source))
            #Read the header into memory
            if source == 'image':
                self.logger.debug("Calling extract header")
                self.imageheader = fileio.getheader(filename[0], self.logger)
                self.logger.debug("Image Header:\n {}".format(json.dumps(self.imageheader, indent=2)))
            else:
                self.dtmheader = fileio.getheader(filename[0], self.logger)
            self.loadinput(source)

    def loadinput(self, source):
        """
        Load a single band from a .cub as an array.
        """
        if source == 'image':
            self.arr = self.ds.GetRasterBand(1).ReadAsArray()
            pltclass = self.plots['image']
            pltclass.img = pltclass.ax.imshow(self.arr, interpolation='nearest')
            self.logger.info('Image Dimensions: {}'.format(self.arr.shape))
            self.imageloaded = True
            self.validate()
        else:
            self.dtm = self.ds.GetRasterBand(1).ReadAsArray()
            pltclass = self.plots['dtm']
            pltclass.img = pltclass.ax.imshow(self.dtm, interpolation='nearest')
            self.logger.info('DTM Dimensions: {}'.format(self.dtm.shape))
            #levels = np.linspace(np.amin(self.dtm), np.amax(self.dtm), num=10)
            #pltclass.ax.contour(self.dtm, levels, origin='lower', linewidth=2)
            self.dtmloaded = True
            self.validate()

    def validate(self):
        """
        Validates the image and DTM.  If successful, instantiates the dispatcher to
        start hitting the API code.
        """
        if not self.imageloaded:
            self.logger.debug('DTM Loaded.  Image Missing.')
            return
        elif not self.dtmloaded:
            self.logger.debug('Image Loaded.  DTM Missing.')
            return
        else:
            try:
                assert(self.arr.shape == self.dtm.shape)
            except:
                self.logger.error('Size Mismatch: Image and DTM must be the same size.')
                return

            nx = self.arr.shape[1]
            ny = self.arr.shape[0]

            self.pc2d = PC2D(nx, ny,logger=self.logger, inputds=self.ds)

class OutLog():
    def __init__(self, edit, out=None, color=None):
        """(edit, out=None, color=None) -> can write stdout, stderr to a
        QTextEdit.
        edit = QTextEdit
        out = alternate stream ( can be the original sys.stdout )
        color = alternate color (i.e. color stderr a different color)
        """
        self.edit = edit
        self.out = None
        self.color = color

    def write(self, m):
        if self.color:
            tc = self.edit.textColor()
            self.edit.setTextColor(self.color)

        self.edit.moveCursor(QtGui.QTextCursor.End)
        self.edit.insertPlainText( m )

        if self.color:
            self.edit.setTextColor(tc)

        if self.out:
            self.out.write(m)

class PC2D():
    """
    Attributes
    ----------
    xpixelcount     (int) Number of 'columns'
    ypixelcount     (int) Number of 'rows'
    npixels         (int) columns * rows
    maxzoomlevel    (int) Maximum number of zoom levels to support
    nx              (ndarray) X dimensions for each zoom level
    ny              (ndarray) Y dimensions for each zoom level
    """

    def __init__(self,xpixelcount, ypixelcount,currentzoom=1, logger=None, inputds=None):
        self.ds = inputds
        self.xpixelcount = xpixelcount
        self.ypixelcount = ypixelcount
        self.npixels = xpixelcount * ypixelcount
        self._logger = logger
        self.maxzoomlevel=10
        self.typelong = 'Altitudes Scaled'
        self.typeshort = 'STO'
        self.currentzoom = currentzoom  # Was N10
        self.generatedatastructures()

    def __repr__(self):
        return """
PC2D
-----
x pixel count: {}
y pixel count: {}
total pixel couny: {}
max zoom level: {}
erms: {}
taurms: {}
nx: {}
ny: {}
nz: {}
n1: {}
n2: {}
n3: {}
n4: {}
n5: {}
n6: {}
n7: {}
n8: {}
n9: {}
n8d: {}
n9d: {}\n""".format(self.xpixelcount, self.ypixelcount, self.npixels,
                   self.maxzoomlevel,
                   self.erms, self.taurms, self.nx, self.ny, self.nz,
                   self.n1, self.n2, self.n3, self.n4, self.n5, self.n6,
                    self.n7, self.n8, self.n9, self.n8d, self.n9d)

    def generatedatastructures(self):
        self.erms = np.zeros(self.maxzoomlevel)
        self.taurms = np.zeros(self.maxzoomlevel)

        self.nx = np.zeros(self.maxzoomlevel, dtype=np.int)
        self.ny = np.zeros(self.maxzoomlevel, dtype=np.int)
        self.nx[0] = self.xpixelcount
        self.ny[0] = self.ypixelcount

        self.computezoomdimesions()

        self.nz = (self.xpixelcount + 1) * (self.ypixelcount + 1)

        self.n1 = np.empty(self.maxzoomlevel)
        self.n1[0] = self.currentzoom

        self.n2 = np.empty(self.maxzoomlevel)
        self.n3 = np.empty(self.maxzoomlevel)
        self.n2[0] = self.n1[0] + self.npixels
        self.n3[0] = self.n2[0] + self.nz

        self.n4 = int(self.n3[0] + self.nz)
        self.n5 = int(self.n4 + self.nz)
        self.n6 = int(self.n5 + self.nz)
        self.n7 = int(self.n6 + 5 * self.nz)
        self.n8 = int(self.n7 + 5 * self.nz)
        self.n9 = int(self.n8 + self.nz)
        self.n8d = int(self.n7 + self.nz * (2 * (self.xpixelcount + 1) + 3))
        self.n9d = int(self.n8d + self.nz * ((self.xpixelcount + 1) + 1))
        self._logger.debug('Generating Dataset Structures\n{}'.format(self.__repr__()))

    def computezoomdimesions(self):
        """
        Compute the zoom level dimesions to populate the nx and ny vectors
        """
        idx = 0
        while self.nx[idx] > 32 and self.ny[idx] > 32:
            if idx + 1 > len(self.nx):
                break
            idx += 1
            self.nx[idx] = self.nx[idx-1] / 2
            self.ny[idx] = self.ny[idx-1] / 2

    def get_phase_inc_emi(self):
        self._logger.debug('Extracting Phase, Incidence, and Emission from Cube')
        try:
            pass
        except:
            self._logger.error('Unable to extract Phase, Incidence, and Emission.  Please run campt on the input image.')
if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    mainwindow = MainWindow()
    sys.exit(app.exec_())
