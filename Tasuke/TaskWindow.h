#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <glog/logging.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QtWidgets/QMainWindow>
#include <QMouseEvent>
#include <QListView>
#include <QBitmap>
#include <QKeySequence>
#include <QPoint>
#include "Task.h"
#include "ui_TaskWindow.h"
#include "HotKeyThread.h"
#include "TaskEntry.h"
#include "TutorialWidget.h"

class TaskWindow : public QMainWindow {
	Q_OBJECT

public:
	TaskWindow(QWidget *parent = 0);
	virtual ~TaskWindow();

	// Handles task entry creation and addition to list
	bool isInRange(int index);
	TaskEntry* createEntry(Task t, int index);
	void addListItem(TaskEntry* entry, int row, int pixmapID);
	void addListItem(TaskEntry* entry);

	// Handles focusing and highlighting of task entries
	void highlightCurrentlySelected(int prevsize);
	void highlightAndAnimateCurrentlySelected(int prevsize);
	void focusOnNewTask(bool haveAnimation);
	void showTasks(QList<Task> tasks);

	// TutorialWidget initialization
	void initTut();

	// Handles scrolling
	void scrollUp();
	void scrollDown();
	void pageUp();
	void pageDown();

	// Stacked widget functions
	int getScreen(); // Helps decide which key press events to execute
	void changeTutorialWidgetTabs(); // Handles key press "tab"

	
public slots:
	void showListWidget();
	void showTutorialWidget();
	void showAndMoveToSide();

protected:
	void closeEvent(QCloseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	bool eventFilter(QObject* object, QEvent* event);


private:
	static const int TASKS_PER_PAGE = 5;
	static const int TASK_ENTRY_WIDTH = 780;
	static const int TASK_ENTRY_HEIGHT = 65;

	Ui::TaskWindowClass ui;	
	TutorialWidget *tut;
	HotKeyThread *hotKeyThread;
	QPoint mpos;
	QList<Task> currentTasks;
	int currentlySelected; // Represents of the # of the selected entry in the UI. Min is 1. Max is size of the task list
	int previouslySelected;
	int previousSize;

};

#endif // TASKWINDOW_H
