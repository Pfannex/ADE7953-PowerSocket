#include "FFS.h"

FFS::FFS(){

  Serial.println("#### CFG ####");
  String CFG_jsonFileBuffer = readJsonString(CFG_FilePath, false);
  DynamicJsonBuffer CFG_JsonBuffer;
  JsonObject& CFG_root = CFG_JsonBuffer.parseObject(CFG_jsonFileBuffer);
  
}

void FFS::TEST(){

  CFG_root.printTo(Serial);Serial.println("");
  
  Serial.println("#### pub ####");
  Serial.println(readJsonString(pub_FilePath, false));
  Serial.println("#### sub ####");
  Serial.println(readJsonString(sub_FilePath, false));
  Serial.println("#### MyFile ####");
  Serial.println(readJsonString(MyFile_FilePath, false));
  Serial.println("#### ADE7953 ####");
  Serial.println(readJsonString(ADE7953reg_FilePath, false));
  Serial.println("#### ARRAY ####");
  Serial.println(readJsonString(testArray_FilePath, true));
  

  //Serial.println("#### CFG ####");
  //parseJsonObject(GetJsonObject(CFG_FilePath));
  //parseJsonObject(MyFile_root);
  //CFG_root.printTo(Serial);

  //String jsonFileBuffer = readJsonString(CFG_FilePath, false);
  //DynamicJsonBuffer JsonBuffer;
  //JsonObject& root = JsonBuffer.parseObject(jsonFileBuffer);
  //root.printTo(Serial);
  
/*  Serial.println("#### pub ####");
  parseJsonObject(GetJsonObject(pub_FilePath));
  Serial.println("#### sub ####");
  parseJsonObject(GetJsonObject(sub_FilePath));
  Serial.println("#### MyFile ####");
  parseJsonObject(GetJsonObject(MyFile_FilePath));
  Serial.println("#### ADE7953 ####");
  parseJsonObject(GetJsonObject(ADE7953reg_FilePath));
*/
  //parseJsonArray(GetJsonArray(testArray_FilePath));
}


//===> Read json DATA <------------------------------------------------
String FFS::readJsonString(String path, bool typ){  //Typ true = ARRAY 
 //if (typ == true) Serial.println("readJsonString Typ = true");
 //if (typ == false) Serial.println("readJsonString Typ = false");
  
 File jsonFile;
 String jsonData = "NIL";
 
  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("");
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system...OK");
    if (SPIFFS.exists(path)) {
      //file exists, reading and loading
      Serial.print("file exists: ");Serial.println(path);
      jsonFile = SPIFFS.open(path, "r");
      if (jsonFile) {
        Serial.println("opened File...OK");
        size_t size = jsonFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        jsonFile.readBytes(buf.get(), size);
        
        if (typ == true){
        //Array 
          DynamicJsonBuffer jsonBufferArray;
          JsonArray& jsonArray = jsonBufferArray.parseArray(buf.get());             
          if (jsonArray.success()) {
            //Serial.println("inside reading");
            //Serial.println(jsonArray[0].asString());
            char buffer[size];
            jsonArray.printTo(buffer, sizeof(buffer));
            jsonData = String(buffer); 
          }else{
            Serial.println("failed to load  ARRAY");
          }
        }else{
        //Object
          DynamicJsonBuffer jsonBuffer;
          JsonObject& json = jsonBuffer.parseObject(buf.get());
          if (json.success()) {         
            char buffer[size];
            json.printTo(buffer, sizeof(buffer));
            jsonData = String(buffer);
          }else{
            Serial.println("failed to load OBJECT");
          }
        }
      }
    }else{
      Serial.println("jsonFile does not exist");
    }
  }else{
    Serial.println("failed to mount FS");
  }
  jsonFile.close();
  //end read    
  //Serial.println(jsonData);    
  return jsonData;  
}




void FFS::parseJsonObject(JsonObject& jsonObject){
  //Serial.println("parseJsonObject");
  for (auto &element : jsonObject){
    String strKey = element.key;
    String strValue = element.value;
    Serial.print(strKey);Serial.print("      ");Serial.println(strValue);
  }
}

void FFS::parseJsonArray(JsonArray& jsonArray){
  //int i=0;
  for (auto &element : jsonArray){
    if (element.is<JsonArray&>()){
      Serial.println("-->>");//Serial.println(i);
      JsonArray& nestedArray = element;
      parseJsonArray(nestedArray);
    }else{
      Serial.println(element.asString()); //Serial.print(" | ");Serial.println(i);
    }
    //i++;      
  }
}


JsonObject& FFS::GetJsonObject(String jsonFileBuffer){
  Serial.println("GetJsonObject");
  //String jsonFileBuffer = readJsonString(path,false);
  DynamicJsonBuffer JsonBuffer;
  JsonObject& root = JsonBuffer.parseObject(jsonFileBuffer);
  root.printTo(Serial);
  return root; 
}


JsonArray& FFS::GetJsonArray(String jsonFileBuffer){
  //Serial.println("GetJsonArray");
  //String jsonFileBuffer = readJsonString(path,true);
  
  //Serial.println("GetJsonArray String read");
  //Serial.println(jsonFileBuffer);
  
 
  DynamicJsonBuffer JsonBuffer;
  JsonArray& root = JsonBuffer.parseArray(jsonFileBuffer);
  //parseJsonArray(root);
  //root.printTo(Serial);

  //Serial.println("print ArrayItems:");
  //Serial.println(root[0].asString());
  //Serial.println(root[1].asString());
  
  return root; 
}
  


//===> getFileSize <---------------------------------------------------
size_t FFS::getFileSize(String path){
  File tmpFile;
  size_t size = 0;
  
  //read configuration from FS json
  Serial.println("");
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists(path)) {
      Serial.println("file exists");
      tmpFile = SPIFFS.open(path, "r");
      if (tmpFile) {
        Serial.println("opened File");
        size = tmpFile.size();
      }
    }
  }
  return size;
}


