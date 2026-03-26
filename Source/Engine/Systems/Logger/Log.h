/*
/
// filename: Log.h
// author: Callen Betts
// brief: defines log class
/
*/

#pragma once

namespace Logger
{

  // define the file to trace to
  static std::ofstream file("Log.txt");
  static std::vector<std::string> messages;
  static bool addedNewMessage = false;

  // retrieve our message vector
  std::vector<std::string>& getMessages();

  // if we've added a new message
  bool AddedNewMessage();
  void SetNewMessage(bool val);

  // write a string to the log and console
  void write(const std::string text);
  // used to write errors to the log and console
  void error(const std::string text);
  // add a message to our list
  void addMessage(const std::string text);

  // write a value to the logger given any type
  template <typename T>
  void write(std::string text, const T value)
  {
    if (file.is_open())
    {
      // write text 
      std::cout << text << value << std::endl;
      file << text << value << std::endl;
      addMessage(text);
    }
  }

} // Logger
