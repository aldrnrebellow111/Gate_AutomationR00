#include <Arduino.h>
#include "nvm.hpp"
#include "JsonParser.hpp"

void SendPcStatusReplyData(String Comamnd , String Status)
{
  Serial.println("{\"" + (CMD_COMMAND) + "\":\"" + Comamnd + "\",\"" +
                 (CMD_STATUS) + "\":\"" + Status + "\"}");
}

bool parseJson(void)
{
  static bool stringComplete = false;
  static String inputString = "";
  uint8_t ret_add = 0;

  if (Serial.available())
  {
    char inChar = (char)Serial.read();
    if (inChar == '\n')
    {
      stringComplete = true;
    }
    else if (inChar != '\r')
    {
      inputString += inChar;
    }

  }
  if (stringComplete)
  {

    if ((CMD_CONN_INIT) == jsonExtract(inputString, CMD_COMMAND))
    {
      SendPcStatusReplyData(CMD_CONN_INIT , CMD_PORT_CONNECTION_OK_STATUS);//Serial.println(("{\"CMD\":\"CONN_OK\"}"));
    }
    else if ((CMD_SET_RECORD) == jsonExtract(inputString, CMD_COMMAND))
    {
      String sName = jsonExtract(inputString, JSON_DATA_NAME);
      String sRfid = jsonExtract(inputString, JSON_DATA_ID);
      inputString = ""; //Dont remove this line
      uint32_t nRfidval = sRfid.toInt();
      uint32_t u32RecIdx = 0;
      if(false == checkForDuplicateCardData(nRfidval , &u32RecIdx))
      {
        bool bRecStatus = SaveNewRecord(sName , nRfidval);
        if (true == bRecStatus)
        {
          SendPcStatusReplyData(CMD_SET_RECORD , CMD_STATUS_OK);
        }
        else if (false == bRecStatus)
        {
          SendPcStatusReplyData(CMD_SET_RECORD , CMD_STATUS_MEMORY_FULL);
        }
      }
      else/*Duplicate entry*/
      {
        SendPcStatusReplyData(CMD_SET_RECORD , CMD_STATUS_DUPLICATE_ENTRY);
      }
    }
    else if ((CMD_RECIEVE_CLOCK) == jsonExtract(inputString, CMD_COMMAND))
    {
      {
        SendPcStatusReplyData(jsonExtract(inputString, CMD_COMMAND) , CMD_ERROR);
      }
    }
    inputString = "";
    stringComplete = false;
    return true;
  }
  return false;
}


String jsonExtract(String json, String name)
{
  char next;
  int start, stop;
  name = "\"" + name + "\"";
  start = json.indexOf(name) + name.length() + 1;
  next = json.charAt(start);
  if (next == '\"')
  {
    //Serial.println(".. a string");
    start = start + 1;
    stop = json.indexOf('"', start + 1);
  }
  else if (next == '[') {
    //Serial.println(".. a list");
    int count = 1;
    int i = start;
    while (count > 0 && i++ < json.length())
    {
      if (json.charAt(i) == ']') {
        count--;
      }
      else if (json.charAt(i) == '[') {
        count++;
      }
    }
    stop = i + 1;
  }
  else if (next == '{')
  {
    //Serial.println(".. a struct");
    int count = 1;
    int i = start;
    while (count > 0 && i++ < json.length()) {
      if (json.charAt(i) == '}') {
        count--;
      }
      else if (json.charAt(i) == '{') {
        count++;
      }
    }
    stop = i + 1;
  }
  else if (next == '.' || next == '-' || ('0' <= next  && next <= '9'))
  {
    //Serial.println(".. a number");
    int i = start;
    while (i++ < json.length() && json.charAt(i) == '.' || ('0' <= json.charAt(i)  && json.charAt(i) <= '9'))
    {
    }
    stop = i;
  }
  return json.substring(start, stop);
}

void SendCardLogToPc(uint8_t RelayId , String cardId , String CardName)
{
  String Msg = String("{\"RELAY\":\"") + String(RelayId) + String("\",\"ID\":\"") + String(cardId) + String("\",\"Name\":\"")+ String(CardName) + String("\"}");
  SendPcStatusReplyData(CMD_CARD_NEW_LOG , Msg);
}
