#include <QtWidgets>

#include "mainwindow.h"
#include "drawboard.h"

MainWindow::MainWindow()
{
    drawboardArea = new DrawBoardArea;
    setCentralWidget(drawboardArea);

    createActions();
    createMenus();

    setWindowTitle(tr("DrawBoard"));
    resize(500, 500);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}


void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                   tr("Open File"), QDir::currentPath());
        if (!fileName.isEmpty())
            drawboardArea->openImage(fileName);
    }
}


void MainWindow::save()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

/*
 * The save() slot is called when the users choose the Save As menu entry,
 * and then choose an entry from the format menu.
 */

void MainWindow::penColor()
{
    QColor newColor = QColorDialog::getColor(drawboardArea->penColor());
    if (newColor.isValid())
        drawboardArea->setPenColor(newColor);
}

void MainWindow::penWidth()
{
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("DrawBoard"),
                                        tr("Select pen width:"),
                                        drawboardArea->penWidth(),
                                        1, 50, 1, &ok);
    if (ok)
        drawboardArea->setPenWidth(newWidth);
}

/*
 * The QInputDialog class provides a simple convenience dialog to get a single value from the user.
 * We use the static QInputDialog::getInt() function, which combines a QLabel and a QSpinBox.
 * The QSpinBox is initialized with the drawboard area's pen width, allows a range from 1 to 50,
 * a step of 1 (meaning that the up and down arrow increment or decrement the value by 1).
 */



void MainWindow::about()
{
    QMessageBox::about(this, tr("About DrawBoard"),
            tr("<p>The <b>DrawBoard</b> is an app made by Team Tuci for some "
               "of the finest scrubs.</p> <p><b>Enjoy!</b></p>"));
}

void MainWindow::createActions()
{

    /*
     * TO CODE : THE NEW IMAGE BUTTON   !!!!!
     *
     */

    newImageAct = new QAction(tr("&New Image..."), this);
    newImageAct->setShortcut(tr("Ctrl+N"));
    connect(newImageAct, SIGNAL(triggered()),
            drawboardArea, SLOT(clearImage()));


    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());

        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
    }

    printAct = new QAction(tr("&Print..."), this);
    connect(printAct, SIGNAL(triggered()), drawboardArea, SLOT(print()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    penColorAct = new QAction(tr("Pen &Color..."), this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

    penWidthAct = new QAction(tr("Pen &Width..."), this);
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

    clearScreenAct = new QAction(tr("&Clear Screen"), this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct, SIGNAL(triggered()),
            drawboardArea, SLOT(clearImage()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

}

/*
 * createAction() creates the actions for the menu entries
 */

void MainWindow::createMenus()
{
    saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);

    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(newImageAct);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);
}

/*
 * The QMenu class provides a menu widget for use in menu bars, context menus, and other popup menus.
 * The QMenuBar class provides a horizontal menu bar with a list of pull-down QMenus.
 */

bool MainWindow::maybeSave()
{
    if (drawboardArea->isModified()) {
       QMessageBox::StandardButton ret;
       ret = QMessageBox::warning(this, tr("DrawBoard"),
                          tr("The image has been modified.\n"
                             "Do you want to save your changes?"),
                          QMessageBox::Save | QMessageBox::Discard
                          | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            return saveFile("png");
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}


bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()))
                               .arg(QString::fromLatin1(fileFormat)));
    if (fileName.isEmpty()) {
        return false;
    } else {
        return drawboardArea->saveImage(fileName, fileFormat.constData());
    }
}

/*
 *
 * In saveFile(), we pop up a file dialog with a file name suggestion.
 * The static QFileDialog::getSaveFileName() function returns a file name selected by the user.
 * The file does not have to exist.
 */
