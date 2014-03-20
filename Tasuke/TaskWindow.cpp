#include <glog/logging.h>
#include <QApplication>
#include <QDesktopWidget>
#include "Tasuke.h"
#include "TaskWindow.h"
#include "TaskEntry.h"


TaskWindow::TaskWindow(QWidget* parent) : QMainWindow(parent) {
	LOG(INFO) << "TaskWindow instance created";

	currentlySelected = 0;
	ui.setupUi(this);

	this->installEventFilter(this);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	//hotkey
	hotKeyThread = new HotKeyThread(this);
	connect(hotKeyThread, SIGNAL(hotKeyPress(int)), this, SLOT(handleHotKeyPress(int)), Qt::QueuedConnection);
	hotKeyThread->start();
}

TaskWindow::~TaskWindow() {
	LOG(INFO) << "TaskWindow instance destroyed";
	clearTasks();
	delete hotKeyThread;
}

//this function highlights the selected entry
//it creates a new taskentry with a distinct background and replaces the old task entry
//by inserting new list item and then deleting the old one.
//it will also dehighlight the previously selected with a similar method
void TaskWindow::highlightCurrentlySelected(){

	QPixmap pxr(QString::fromUtf8("roundedEntryMaskSelect.png")); //highlighted bg image
	QPixmap pxr2(QString::fromUtf8("roundedEntryMask.png")); //normal bg image

	//highlight currently selected
	if((currentlySelected>=0) && (currentlySelected<currentTasks.size())) {
		
		Task t = currentTasks[currentlySelected];
		TaskEntry * entry = new TaskEntry(currentlySelected+1, t.getDescription(), t.getTags(), t.getBegin(), t.getEnd(), this);
		entry->ui.bg->setPixmap(pxr); 

		QListWidgetItem *listItem = new QListWidgetItem();
		listItem->setSizeHint(QSize(TASK_ENTRY_WIDTH, TASK_ENTRY_HEIGHT));
		ui.taskList->insertItem(currentlySelected, listItem);
		ui.taskList->setItemWidget(listItem, entry);

		listItem = ui.taskList->takeItem(currentlySelected+1);
		delete listItem->listWidget();
		delete listItem;
	}

	//dehilight previously selected
	if (previouslySelected<currentTasks.size()) {
		Task t2 = currentTasks[previouslySelected];
		TaskEntry * entry2 = new TaskEntry(previouslySelected+1, t2.getDescription(), t2.getTags(), t2.getBegin(), t2.getEnd(), this);
		entry2->ui.bg->setPixmap(pxr2);

		QListWidgetItem *listItem2 = new QListWidgetItem();
		listItem2->setSizeHint(QSize(TASK_ENTRY_WIDTH, TASK_ENTRY_HEIGHT));
		ui.taskList->insertItem(previouslySelected, listItem2);
		ui.taskList->setItemWidget(listItem2, entry2);

		listItem2 = ui.taskList->takeItem(previouslySelected+1);
		delete listItem2->listWidget();
		delete listItem2;
	}
}

void TaskWindow::showTasks(QList<Task> tasks) {
	currentTasks = tasks;
	clearTasks();
	
	for (int i = 0; i < tasks.size(); i++){

		Task t = tasks[i];

		TaskEntry * entry = new TaskEntry(i+1, t.getDescription(), t.getTags(), t.getBegin(), t.getEnd(), this);

		QListWidgetItem *listItem = new QListWidgetItem();
		listItem->setSizeHint(QSize(TASK_ENTRY_WIDTH, TASK_ENTRY_HEIGHT));
		ui.taskList->addItem(listItem);
		ui.taskList->setItemWidget(listItem, entry);	
	}

	//focus on latest task
	previouslySelected = currentlySelected;
	currentlySelected = tasks.size()-1;
	highlightCurrentlySelected();
	ui.taskList->scrollToItem(ui.taskList->item(currentlySelected));

}

void TaskWindow::clearTasks() {
	while (ui.taskList->count() != 0) {
		QListWidgetItem *listItem = ui.taskList->takeItem(0);
		delete listItem->listWidget();
		delete listItem;
	}
}


void TaskWindow::scrollUp(){
	if (currentlySelected>0) {
		previouslySelected = currentlySelected;
		currentlySelected--;
		ui.taskList->scrollToItem(ui.taskList->item(currentlySelected));
		highlightCurrentlySelected();
	}
}

void TaskWindow::scrollDown(){
	if(currentlySelected < ui.taskList->count()-1){
		previouslySelected = currentlySelected;
		currentlySelected++;
		ui.taskList->scrollToItem(ui.taskList->item(currentlySelected));
		highlightCurrentlySelected();
	}
}

void TaskWindow::pageUp(){
	if (currentlySelected < 1){
		return;
	}

	if (currentlySelected > TASKS_PER_PAGE-1) {
		previouslySelected = currentlySelected;
		currentlySelected -= TASKS_PER_PAGE;
	} else {
		previouslySelected = currentlySelected;
		currentlySelected = 0;
	}

	ui.taskList->scrollToItem(ui.taskList->item(currentlySelected));
	highlightCurrentlySelected();

}

void TaskWindow::pageDown(){
	if (currentlySelected == ui.taskList->count()-1) {
		return;
	}

	if (currentlySelected < ui.taskList->count() - TASKS_PER_PAGE - 1){
		previouslySelected = currentlySelected;
		currentlySelected += TASKS_PER_PAGE;
	} else {
		previouslySelected = currentlySelected;
		currentlySelected = ui.taskList->count()-1;
	}

	ui.taskList->scrollToItem(ui.taskList->item(currentlySelected));
	highlightCurrentlySelected();
}

void TaskWindow::showAndMoveToSide() {
	QPoint center = QApplication::desktop()->screen()->rect().center() - rect().center();
	center.setY(QApplication::desktop()->screen()->rect().height() / 9);

	move(center);

	showNormal();
	raise();
	activateWindow();
}

void TaskWindow::closeEvent(QCloseEvent *event) {
	hide();
	event->ignore();
}

void TaskWindow::mousePressEvent(QMouseEvent *event){
    mpos = event->pos();
}

void TaskWindow::mouseMoveEvent(QMouseEvent *event){
    if (event->buttons() && Qt::LeftButton) {
        QPoint diff = event->pos() - mpos;
        QPoint newpos = this->pos() + diff;

        this->move(newpos);
    }
}


bool TaskWindow::eventFilter(QObject* object, QEvent* event) {
    if(event->type() == QEvent::KeyPress) {
		QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);

		//scroll
		if(eventKey->modifiers() & Qt::CTRL) {
			if(eventKey->key() == Qt::Key_Down) {
				pageDown();
				return true;
			}

			if (eventKey->key() == Qt::Key_Up) {
				pageUp();
				return true;
			}
		}

		if(eventKey->key() == Qt::Key_Down) {
			scrollDown();
			return true;
		}

		if (eventKey->key() == Qt::Key_Up) {
			scrollUp();
			return true;
		}

		//undo and redo shortcuts
		if (eventKey->matches(QKeySequence::Undo)) {
			Tasuke::instance().runCommand(QString("undo"));
		}

		if(eventKey->matches(QKeySequence::Redo)) {
			Tasuke::instance().runCommand(QString("redo"));
		}

    }

    return QObject::eventFilter(object, event);
}


void TaskWindow::handleHotKeyPress(int key) {
	if (isVisible() == true) {
		hide();
	} else {
		showAndMoveToSide();
	}
}