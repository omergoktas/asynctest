/****************************************************************************
**
** Copyright (C) 2019 Ömer Göktaş
** Contact: omergoktas.com
**
** This file is part of the AsyncTest project.
**
** The AsyncTest is free software: you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public License
** version 3 as published by the Free Software Foundation.
**
** The AsyncTest is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with the AsyncTest. If not, see
** <https://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <async.h>
#include <QRandomGenerator>
#include <QApplication>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFutureWatcher>
#include <QTimer>

// Example task state messages
static const QStringList messages = {"Initializing...", "Phase 1/2", "Phase 2/2", "Finalizing..."};

// Example task function
int timeConsumingRandomNumberGenerator(QFutureInterfaceBase* futureInterface, int rangeMin, int rangeMax)
{
    auto future = static_cast<QFutureInterface<int>*>(futureInterface);
    future->setProgressRange(0, 100);
    future->setProgressValue(0);

    int value = QRandomGenerator::global()->bounded(rangeMin, rangeMax);
    for (int i = 1; i <= 100; ++i) {
        if (future->isPaused())
            future->waitForResume();
        if (future->isCanceled())
            return value;
        value = QRandomGenerator::global()->bounded(rangeMin, rangeMax);
        future->setProgressValueAndText(i, QString("Random number: %1").arg(value));
        QThread::msleep(50);
    }

    future->reportResult(value);
    return value;
}

int main(int argc, char* argv[])
{
    //! Some configurations regarding to GUI (nothing to do with Async lib)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);
    QApplication::setStyle("fusion");

    QProgressDialog dialog;
    dialog.setAutoClose(false);
    dialog.setAutoReset(false);
    dialog.setMinimumWidth(250);
    dialog.setWindowTitle("Async Test");
    dialog.setCancelButtonText("Pause");
    dialog.disconnect(&dialog); // Hack: Prevent "rejected" signal to close dialog window
    dialog.show();

    //! Run the task asynchronously
    QFutureWatcher<int> watcher;
    watcher.setFuture(Async::run(timeConsumingRandomNumberGenerator, 100, 999));

    //! Using "canceled" signal of the dialog in order to provide pause/resume functionality
    QObject::connect(&dialog, &QProgressDialog::canceled, [&] {
        if (watcher.isFinished()) {
            dialog.close();
            return;
        }
        watcher.future().togglePaused();
        dialog.setLabelText(watcher.isPaused() ? "Paused" : "Running");
        dialog.setCancelButtonText(watcher.isPaused() ? "Resume" : "Pause");
    });

    //! Catch state changes on the task by connecting appropriate signals to slots
    QObject::connect(&watcher, &QFutureWatcherBase::progressTextChanged, [&]
    { dialog.setLabelText(watcher.progressText()); });
    QObject::connect(&watcher, &QFutureWatcherBase::progressValueChanged, [&]
    { dialog.setValue(watcher.progressValue()); });
    QObject::connect(&watcher, &QFutureWatcherBase::resultReadyAt, [&]
    { QMessageBox::information(0, "Done", "Result: " + QString::number(watcher.result())); });
    QObject::connect(&watcher, &QFutureWatcherBase::finished, [&] {
        dialog.setLabelText("Done!");
        dialog.setCancelButtonText("Close");
    });
    // See QTBUG-12152 for QFutureWatcherBase::paused signal

    //! Cancel the operation after 1 second
    // QTimer::singleShot(1000, &watcher, &QFutureWatcher<int>::cancel);

    return app.exec();
}