/*
/
// filename: Log.cpp
// author: Callen Betts
// brief: implements Log.h
/
*/

#include "stdafx.h"
#include "Log.h"

std::vector<std::string>& Logger::getMessages()
{
  return messages;
}

bool Logger::AddedNewMessage()
{
  return addedNewMessage;
}

void Logger::SetNewMessage(bool val)
{
  addedNewMessage = val;
}

// write text to the trace log file
void Logger::write(const std::string text)
{
  if (file.is_open())
  {
    // write text 
    std::cout << text << std::endl;
    file << "Console: " << text << std::endl;
    addMessage(text);
  }
}

// write text to the trace log file with error context
void Logger::error(const std::string text)
{
  if (file.is_open())
  {
    // write text 
    std::cerr << text << std::endl;
    file << "ERROR: " << text << std::endl;
    addMessage(text);
  }
}

void Logger::addMessage(const std::string text)
{
  messages.push_back(text);
  addedNewMessage = true;
}
