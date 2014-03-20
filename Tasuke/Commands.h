#ifndef COMMANDS_H
#define COMMANDS_H

#include "Task.h"

// This is an interface for all user commands. The intended method to intialize
// a ICommand instance is through the Interpreter.
class ICommand {
private:
	bool hasRun;
public:
	ICommand();
	virtual ~ICommand();
	
	virtual void run();
	virtual void undo();
};

class AddCommand : public ICommand {
private:
	Task task;
public:
	AddCommand(Task& _task);
	~AddCommand();
	
	void run();
	void undo();
};

class RemoveCommand : public ICommand {
private:
	int id;
	Task task;
public:
	RemoveCommand(int _id);
	~RemoveCommand();
	
	void run();
	void undo();
};

class EditCommand : public ICommand {
private:
	int id;
	Task old;
	Task task;
public:
	EditCommand(int _id, Task& _task);
	~EditCommand();
	
	void run();
	void undo();
};

#endif
