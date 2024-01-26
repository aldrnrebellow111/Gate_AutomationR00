#ifndef JSON_H
#define JSON_H

//#define RFID_READ_DELAY 10


#define CMD_COMMAND String("CM")
#define CMD_PORT_CONNECTION_OK_STATUS String("COK")
#define CMD_SET_RECORD String("SR")
#define CMD_READ_RECORD String("RR")
#define CMD_SET_CLOCK String("SC")
#define CMD_RECIEVE_CLOCK String("RC")
#define CMD_ERROR String("ER")
#define CMD_STATUS String("ST")
#define CMD_CONN_INIT String("CON")
#define CMD_STATUS_OK String("OK")
#define JSON_DATA_NAME String("NA")
#define JSON_DATA_ID String("ID")
#define JSON_DATA_DATE String("DA")
#define JSON_DATA_TIME String("TI")
#define CMD_STATUS_DUPLICATE_ENTRY String("DU")
#define CMD_STATUS_MEMORY_FULL String("MF")
#define CMD_CARD_NEW_LOG String("LO")

bool parseJson(void);
String jsonExtract(String json, String name);
void SendPcStatusReplyData(String Comamnd , String Status);
void SendCardLogToPc(uint8_t RelayId , String cardId , String CardName);
#endif
