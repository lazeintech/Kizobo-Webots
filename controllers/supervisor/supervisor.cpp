// File:          supervisor.cpp
// Date:
// Description:
// Author:
// Modifications:

// You may need to add webots include files such as
// <webots/DistanceSensor.hpp>, <webots/Motor.hpp>, etc.
// and/or to add some other includes
#include <webots/Supervisor.hpp>
using namespace webots;

#include <iostream>
#include <fstream>
#include <thread>
using namespace std;

#ifdef WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <process.h>
#endif

void runChildProcess()
{
  const char *exe = "./blocklyServer/blocklyServer.exe";
  const char *args[] = {"blocklyServer.exe", nullptr}; // Program name must be the first argument

  // _P_NOWAIT to run the process asynchronously, or _P_WAIT to wait for it to finish
  int ret = _spawnvp(_P_NOWAIT, exe, args);

  if (ret == -1)
  {
    perror("spawnvp failed"); // Use perror to print the actual error message
  }
}
// This is the main program of your controller.
// It creates an instance of your Robot instance, launches its
// function(s) and destroys it at the end of the execution.
// Note that only one instance of Robot should be created in
// a controller program.
// The arguments of the main function can be specified by the
// "controllerArgs" field of the Robot node
int main(int argc, char **argv)
{
#ifdef WINDOWS
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  char *exe = "./blocklyServer/blocklyServer.exe";
  if (!CreateProcess(NULL, exe, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
  {
    cout << "Could not create process: " << GetLastError() << endl;
    return 1;
  }

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  return 0;

#else
  std::thread childThread;
  try
  {
    // Create a thread to run the child process
    childThread = std::thread(runChildProcess);
    childThread.detach();
  }
  catch (const std::exception &e)
  {
    std::cerr << "Failed to create thread: " << e.what() << std::endl;
    return 0;
  }
#endif

  // create the Robot instance.
  Supervisor *supervisor = new Supervisor();

  Node *robot = supervisor->getFromDef("ROBOT");
  // Field *controller = robot->getField("controller");
  // get the time step of the current world.
  int timeStep = (int)supervisor->getBasicTimeStep();

  // You should insert a getDevice-like function in order to get the
  // instance of a device of the robot. Something like:
  //  Motor *motor = robot->getMotor("motorname");
  //  DistanceSensor *ds = robot->getDistanceSensor("dsname");
  //  ds->enable(timeStep);

  // Main loop:
  // - perform simulation steps until Webots is stopping the controller
  while ((supervisor->step(timeStep)) != -1)
  {
    string message = supervisor->wwiReceiveText();
    if (message.length() != 0)
    {
      supervisor->wwiSendText("okkkkkkkkk");
      // cout << message << endl;

      ofstream out("../my_controller/my_controller.py");

      out << message << endl;

      out.close();

      supervisor->simulationReset();
      robot->restartController();
    }
  }

  // Enter here exit cleanup code.
  childThread.join();
  delete supervisor;
  return 0;
}
