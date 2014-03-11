#include "Tasuke.h"
#include "Constants.h"
#include "Commands.h"

AddCommand::AddCommand(Task& _task) {
	task = _task;
}

AddCommand::~AddCommand() {

}
	
void AddCommand::run() {
	Tasuke::instance().getStorage().addTask(task);
	Tasuke::instance().showMessage(QString("Added \"%1\"").arg(task.getDescription()));
}
void AddCommand::undo() {
	Tasuke::instance().getStorage().popTask();
	Tasuke::instance().showMessage(QString("Undone add \"%1\"").arg(task.getDescription()));
}

RemoveCommand::RemoveCommand(int _id) {
	id = _id;
	task = Tasuke::instance().getStorage().getTask(id);
}

RemoveCommand::~RemoveCommand() {

}
	
void RemoveCommand::run() {
	Tasuke::instance().getStorage().removeTask(id);
	Tasuke::instance().showMessage(QString("Removed \"%1\"").arg(task.getDescription()));
}
void RemoveCommand::undo() {
	Tasuke::instance().getStorage().addTask(task, id);
	Tasuke::instance().showMessage(QString("Undone remove \"%1\"").arg(task.getDescription()));
}